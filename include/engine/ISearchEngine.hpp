#ifndef SERVER_ISEARCHENGINE_HPP
#define SERVER_ISEARCHENGINE_HPP

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "cache.hpp"
#include "easylogging++.h"
#include "song.hpp"

class ISearchEngine {
 public:
  using db = std::vector<song>;
  ISearchEngine() = default;
  virtual ~ISearchEngine() = default;
  const db &query(const std::string &lyric_to_find);
  [[nodiscard]] std::pair<db::const_iterator, db::const_iterator> get_page_range(
      size_t page_number, size_t page_size) const;

 protected:
  virtual db query_internal(const std::string &lyric_to_find) = 0;
  virtual el::Logger *get_logger() = 0;

  cache m_cache;
};

void print_results(const std::vector<song> &results);
#endif  // SERVER_ISEARCHENGINE_HPP
