#include <chrono>
#include <string>
#include <thread>

#include "easylogging++.h"
#include "json.hpp"
#include "search_engine_cpu.hpp"

using json = nlohmann::json;

search_engine_cpu::search_engine_cpu (const char *path_to_db, size_t nthreads) : ISearchEngine (), m_nthreads (nthreads)
{
  std::string identity{ "search_engine_cpu_" + std::to_string (m_nthreads) + "_threads" };
  m_logger = el::Loggers::getLogger (identity);
  CLOG (INFO, m_logger->id ().c_str ()) << "creating search_engine_cpu (using " << m_nthreads << " threads)";
  CLOG (INFO, m_logger->id ().c_str ()) << "loading database from " << path_to_db;
  std::ifstream json_db (path_to_db);
  if (!json_db.is_open ())
  {
	throw std::runtime_error ("Failed to open JSON database");
  }

  std::string line;
  while (std::getline (json_db, line))
  {
	try
	{
	  auto obj = json::parse (line);
	  std::string lyrics = obj.at ("lyrics");
	  m_db.emplace_back (obj.at ("title"), obj.at ("artist"), obj.at ("album"), lyrics);
	}
	catch (const std::exception &e)
	{
	  std::cerr << "Failed to parse line: " << line << std::endl;
	  std::cerr << "Error: " << e.what () << std::endl;
	}
  }

  json_db.close ();
  CLOG (INFO, m_logger->id ().c_str ()) << "LOADED " << m_db.size () << " songs";
}

std::vector<song>
search_engine_cpu::query_internal (const std::string &lyric_to_find)
{
  if (m_nthreads == 0)
  {
	return query_seq (lyric_to_find);
  }
  else
  {
	return query_par (lyric_to_find);
  }
}

el::Logger *
search_engine_cpu::get_logger ()
{
  return m_logger;
}

std::vector<song>
search_engine_cpu::query_seq (const std::string &lyric_to_find)
{
  std::vector<song> result;
  const auto start = std::chrono::high_resolution_clock::now ();

  std::copy_if (m_db.begin (), m_db.end (), std::back_inserter (result), [&lyric_to_find] (const song &s) {
	return s.get_lyrics ().find (lyric_to_find) != std::string::npos;
  });

  const auto end = std::chrono::high_resolution_clock::now ();
  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);

  CLOG (INFO, m_logger->id ().c_str ()) << "Search completed in " << duration.count () << " milliseconds";

  return result;
}

std::vector<song>
search_engine_cpu::query_par (const std::string &lyric_to_find)
{
  std::vector<std::thread> threads (m_nthreads);
  std::vector<std::vector<song> > t_results (m_nthreads);

  const auto start = std::chrono::high_resolution_clock::now ();
  for (size_t tid = 0; tid < m_nthreads; ++tid)
  {
	threads[tid] = std::thread (&search_engine_cpu::search_worker, this, std::ref (lyric_to_find), tid, std::ref (t_results[tid]));
  }

  for (auto &t : threads)
  {
	t.join ();
  }

  const auto end = std::chrono::high_resolution_clock::now ();

  // collect results into one vector
  std::vector<song> result;
  for (auto &t_result : t_results)
  {
	std::copy (t_result.begin (), t_result.end (), std::back_inserter (result));
  }

  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);

  CLOG (INFO, m_logger->id ().c_str ()) << "Search completed in " << duration.count () << " milliseconds";

  return result;
}

void
search_engine_cpu::search_worker (const std::string &lyric_to_find, size_t thread_id, std::vector<song> &results) const
{
  const size_t chunk_size = m_db.size () / m_nthreads;
  const auto start = static_cast<ptrdiff_t> (thread_id * chunk_size);
  const auto end = static_cast<std::ptrdiff_t> (
	  thread_id == m_nthreads - 1 ? m_db.size () : start + chunk_size);

  auto search_pred = [&lyric_to_find] (const song &s) {
	return s.get_lyrics ().find (lyric_to_find) != std::string::npos;
  };
  std::copy_if (m_db.begin () + start, m_db.begin () + end, std::back_inserter (results), search_pred);
}
