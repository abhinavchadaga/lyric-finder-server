#ifndef BACKEND_INCLUDE_SONG_HPP_
#define BACKEND_INCLUDE_SONG_HPP_

#include <string>

class song {
 public:
  song(std::string title, std::string artist, std::string album,
       std::string lyrics);
  [[nodiscard]] const std::string &get_title() const;
  [[nodiscard]] const std::string &get_artist() const;
  [[nodiscard]] const std::string &get_album() const;
  [[nodiscard]] const std::string &get_lyrics() const;

 protected:
  const std::string m_title;
  const std::string m_artist;
  const std::string m_album;
  const std::string m_lyrics;
};

std::ostream &operator<<(std::ostream &os, const song &s);

std::string get_first_ten_words(const std::string &s);

#endif  // BACKEND_INCLUDE_SONG_HPP_
