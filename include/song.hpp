#ifndef BACKEND_INCLUDE_SONG_HPP_
#define BACKEND_INCLUDE_SONG_HPP_

#include <string>

class song_metadata {
 public:
  song_metadata (std::string title, std::string artist, std::string album);
  [[nodiscard]] const std::string &get_title () const;
  [[nodiscard]] const std::string &get_artist () const;
  [[nodiscard]] const std::string &get_album () const;

 protected:
  const std::string m_title;
  const std::string m_artist;
  const std::string m_album;
};

class song final : public song_metadata {
  const std::string m_lyrics;

 public:
  [[nodiscard]] const std::string &get_lyrics () const;
  song (std::string title, std::string artist, std::string album, std::string lyrics);
};

std::ostream &operator<< (std::ostream &os, const song_metadata &s);

std::string get_first_ten_words (const std::string &s);

#endif// BACKEND_INCLUDE_SONG_HPP_
