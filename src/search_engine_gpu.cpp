#include <algorithm>
#include <cuda_runtime.h>
#include <iostream>

#include "json.hpp"
#include "query_kernel.h"
#include "search_engine_gpu.hpp"

using json = nlohmann::json;

search_engine_gpu::search_engine_gpu (const char *path_to_db)
	: ISearchEngine (), m_logger (el::Loggers::getLogger ("search_engine_gpu"))
{
  CLOG (INFO, "search_engine_gpu") << "Initializing search engine (using gpu)";
  CLOG (INFO, "search_engine_gpu") << "Loading database from " << path_to_db;

  std::ifstream json_db (path_to_db);
  if (!json_db.is_open ())
  {
	throw std::runtime_error ("Failed to open JSON database");
  }

  // LOAD METADATA
  size_t total_chars = 0;
  std::string line;
  while (std::getline (json_db, line))
  {
	try
	{
	  auto obj = json::parse (line);
	  std::string lyrics = obj.at ("lyrics");
	  m_db.emplace_back (obj.at ("title"), obj.at ("artist"), obj.at ("album"));
	  total_chars += lyrics.size () + 1;
	}
	catch (const std::exception &e)
	{
	  std::cerr << "Failed to parse line: " << line << std::endl;
	  std::cerr << "Error: " << e.what () << std::endl;
	}
  }

  // LOAD LYRICS INTO HOST MEMORY
  h_lyrics = new char[total_chars];
  h_song_indices = new int[m_db.size ()];
  size_t offset = 0;
  size_t song_index = 0;

  // Reset file stream to beginning
  json_db.clear ();
  json_db.seekg (0, std::ios::beg);

  while (std::getline (json_db, line))
  {
	try
	{
	  auto obj = json::parse (line);
	  std::string lyrics = obj.at ("lyrics");
	  std::copy (lyrics.begin (), lyrics.end (), h_lyrics + offset);
	  h_song_indices[song_index] = offset;
	  offset += lyrics.size ();
	  h_lyrics[offset++] = '\0';
	  song_index++;
	}
	catch (const std::exception &e)
	{
	  std::cerr << "Failed to parse line: " << line << std::endl;
	  std::cerr << "Error: " << e.what () << std::endl;
	}
  }

  json_db.close ();

  CLOG (INFO, "search_engine_gpu") << "transferring data to device";
  gpuErrchk (cudaMalloc ((void **)&d_lyrics, total_chars * sizeof (char)));
  gpuErrchk (cudaMalloc ((void **)&d_song_indices, m_db.size () * sizeof (int)));

  gpuErrchk (cudaMemcpy (d_lyrics, h_lyrics, total_chars * sizeof (char),
						 cudaMemcpyHostToDevice));
  gpuErrchk (cudaMemcpy (d_song_indices, h_song_indices,
						 m_db.size () * sizeof (int), cudaMemcpyHostToDevice));

  CLOG (INFO, "search_engine_gpu") << "LOADED " << m_db.size () << " songs";
}

std::vector<song>
search_engine_gpu::query_internal (
	const std::string &lyric_to_find)
{
  int *result_indices;
  int num_results;
  float kernel_time_ms = 0.0, data_transfer_time_ms = 0.0;
  CLOG (INFO, "search_engine_gpu") << "Launching query kernel";
  launch_query_kernel ((const char *)lyric_to_find.c_str (),
					   (int)lyric_to_find.size (), d_lyrics, d_song_indices,
					   (int)m_db.size (), &result_indices, &num_results,
					   &kernel_time_ms, &data_transfer_time_ms);
  std::vector<song> results;
  results.reserve (num_results);
  for (int i = 0; i < num_results; i++)
  {
	song_metadata &m = m_db[result_indices[i]];
	const char *lyrics = &h_lyrics[h_song_indices[result_indices[i]]];
	results.emplace_back (m.get_title (), m.get_artist (), m.get_album (), lyrics);
  }
  free (result_indices);
  CLOG (INFO, "search_engine_gpu")
	  << "Search completed in " << kernel_time_ms + data_transfer_time_ms
	  << " milliseconds";
  CLOG (INFO, "search_engine_gpu")
	  << "Kernel time: " << kernel_time_ms
	  << " milliseconds, Data transfer time: " << data_transfer_time_ms
	  << " milliseconds";
  return results;
}

el::Logger *
search_engine_gpu::get_logger ()
{
  return m_logger;
}

search_engine_gpu::~search_engine_gpu ()
{
  CLOG (INFO, "search_engine_gpu") << "Destroying search engine";
  gpuErrchk (cudaFree (d_lyrics));
  gpuErrchk (cudaFree (d_song_indices));
  delete[] h_lyrics;
  delete[] h_song_indices;
}
