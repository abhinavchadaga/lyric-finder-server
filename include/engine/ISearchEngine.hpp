#ifndef SERVER_ISEARCHENGINE_HPP
#define SERVER_ISEARCHENGINE_HPP

#include <string>
#include <vector>

#include "easylogging++.h"
#include "song.hpp"

class ISearchEngine {
 public:
  ISearchEngine() = default;
  virtual ~ISearchEngine() = default;
  std::vector<song> query(const std::string &lyric_to_find);

 protected:
  virtual std::vector<song> query_internal(
      const std::string &lyric_to_find) = 0;
  virtual el::Logger *get_logger() = 0;
};

void print_results(const std::vector<song> &results);
#endif  // SERVER_ISEARCHENGINE_HPP
