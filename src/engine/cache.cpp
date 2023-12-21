#include "engine/cache.hpp"

std::string cache::get_key() const { return m_key; }
const std::vector<song> &cache::get_value() const { return m_value; }
void cache::insert(std::string key, std::vector<song> &&value) {
  m_key = key;
  m_value = std::move(value);
}