#include "engine/ISearchEngine.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>
#include <string>

#include "easylogging++.h"
#include "json.hpp"

using json = nlohmann::json;

std::vector<song> ISearchEngine::query(const std::string &lyric_to_find) {
  auto *logger = get_logger();
  CLOG(INFO, logger->id().c_str())
      << "Searching for " << get_first_ten_words(lyric_to_find);
  std::string cleaned = lyric_to_find;
  cleaned = std::regex_replace(cleaned, std::regex("[^a-zA-Z]"), "");
  std::transform(cleaned.begin(), cleaned.end(), cleaned.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return query_internal(cleaned);
}

void print_results(const std::vector<song> &results) {
  std::cout << "Found " << results.size() << " result(s)" << std::endl;
  for (const auto &song : results) {
    std::cout << song << std::endl;
  }
}
