#include "engine/song.hpp"

#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

song::song(std::string title, std::string artist, std::string album,
           std::string lyrics)
    : m_title(std::move(title)),
      m_artist(std::move(artist)),
      m_album(std::move(album)),
      m_lyrics(std::move(lyrics)) {}

const std::string &song::get_title() const { return m_title; }

const std::string &song::get_artist() const { return m_artist; }

const std::string &song::get_album() const { return m_album; }

const std::string &song::get_lyrics() const { return m_lyrics; }

std::ostream &operator<<(std::ostream &os, const song &s) {
  os << "Title: " << s.get_title() << std::endl;
  os << "Artist: " << s.get_artist() << std::endl;
  os << "Album: " << s.get_album() << std::endl;
  os << "Lyrics: " << get_first_ten_words(s.get_lyrics()) << std::endl;
  return os;
}

std::string get_first_ten_words(const std::string &s) {
  std::istringstream iss(s);
  const std::vector<std::string> words(std::istream_iterator<std::string>{iss},
                                       std::istream_iterator<std::string>());
  std::string result;
  const bool less_than_ten = words.size() < 10;
  const size_t limit = less_than_ten ? words.size() : 10;
  for (size_t i = 0; i < limit; i++) {
    if (i != 0) result += " ";
    result += words[i];
  }

  if (!less_than_ten) {
    result += "...";
  }

  return result;
}
