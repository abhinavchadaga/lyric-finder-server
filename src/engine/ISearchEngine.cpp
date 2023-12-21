#include "engine/ISearchEngine.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>
#include <string>

#include "easylogging++.h"
#include "json.hpp"

using json = nlohmann::json;
using db = ISearchEngine::db;

const db &ISearchEngine::query(const std::string &lyric_to_find) {
  auto *logger = get_logger();
  CLOG(INFO, logger->id().c_str())
      << "Searching for " << get_first_ten_words(lyric_to_find);
  std::string cleaned = lyric_to_find;
  cleaned = std::regex_replace(cleaned, std::regex("[^a-zA-Z]"), "");
  std::transform(cleaned.begin(), cleaned.end(), cleaned.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  if (m_cache.get_key() == cleaned) {
    CLOG(INFO, logger->id().c_str()) << "using cache";
    return m_cache.get_value();
  } else {
    CLOG(INFO, logger->id().c_str()) << "not using cache";
    auto results = query_internal(cleaned);
    m_cache.insert(cleaned, std::move(results));
    return m_cache.get_value();
  }
}

std::pair<db::const_iterator, db::const_iterator> ISearchEngine::get_page_range(
    size_t page_number, size_t page_size) const {
  auto start_idx =
      std::min(static_cast<db::difference_type>((page_number - 1) * page_size),
               static_cast<db::difference_type>(m_cache.get_value().size()));
  auto end_idx =
      std::min(static_cast<db::difference_type>(page_number * page_size),
               static_cast<db::difference_type>(m_cache.get_value().size()));
  return std::make_pair(m_cache.get_value().cbegin() + start_idx,
                        m_cache.get_value().cbegin() + end_idx);
}

void print_results(const db &results) {
  std::cout << "Found " << results.size() << " result(s)" << std::endl;
  for (const auto &song : results) {
    std::cout << song << std::endl;
  }
}
