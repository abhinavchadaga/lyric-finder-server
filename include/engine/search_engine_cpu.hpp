#ifndef SERVER_INCLUDE_SEARCH_ENGINE_CPU_HPP_
#define SERVER_INCLUDE_SEARCH_ENGINE_CPU_HPP_

#include "ISearchEngine.hpp"
#include "easylogging++.h"

class search_engine_cpu final : public ISearchEngine {
 public:
  explicit search_engine_cpu(const char *path_to_db, size_t nthreads = 0);
  ~search_engine_cpu() override = default;
  el::Logger *get_logger() override;

 private:
  std::vector<song> m_db;
  el::Logger *m_logger;
  size_t m_nthreads;

  // internal implementation of query()
  std::vector<song> query_internal(const std::string &lyric_to_find) override;
  std::vector<song> query_seq(const std::string &lyric_to_find);
  std::vector<song> query_par(const std::string &lyric_to_find);
  void search_worker(const std::string &lyric_to_find, size_t thread_id,
                     std::vector<song> &results) const;
};

#endif  // SERVER_INCLUDE_SEARCH_ENGINE_CPU_HPP_
