//
// Created by Abhinav Chadaga on 11/24/23.
//

#ifndef LYRIC_FINDER_BACKEND_INCLUDE_SEARCH_ENGINE_GPU_HPP_
#define LYRIC_FINDER_BACKEND_INCLUDE_SEARCH_ENGINE_GPU_HPP_

#include <cuda_runtime.h>

#include <ISearchEngine.hpp>

#include "song.hpp"

#define gpuErrchk(ans) \
  { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line,
                      bool abort = true) {
  if (code != cudaSuccess) {
    fprintf(stderr, "GPUassert: %s %s %d\n", cudaGetErrorString(code), file,
            line);
    std::cout << "ERROR" << std::endl;
    if (abort) exit(code);
  }
}

class search_engine_gpu final : public ISearchEngine {
 public:
  explicit search_engine_gpu(const char *path_to_db);
  ~search_engine_gpu() override;
  el::Logger *get_logger() override;

 private:
  std::vector<song_metadata> m_db;
  // host copies
  char *h_lyrics;
  int *h_song_indices;
  // device copies
  char *d_lyrics;
  int *d_song_indices;

  el::Logger *m_logger;

  std::vector<song> query_internal(const std::string &lyric_to_find) override;
};

#endif  // LYRIC_FINDER_BACKEND_INCLUDE_SEARCH_ENGINE_GPU_HPP_
