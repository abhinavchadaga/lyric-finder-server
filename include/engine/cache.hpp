//
// Created by Abhinav Chadaga on 12/21/23.
//

#ifndef CACHE_HPP_
#define CACHE_HPP_

#include <string>
#include <vector>

#include "engine/song.hpp"

class cache {
 public:
  cache() = default;
  ~cache() = default;

  std::string get_key() const;
  const std::vector<song>& get_value() const;
  void insert(std::string key, std::vector<song>&& value);

 private:
  std::string m_key;
  std::vector<song> m_value;
};

#endif  //_CACHE_HPP_
