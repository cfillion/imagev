#ifndef IMAGEV_PLAYLIST_HPP
#define IMAGEV_PLAYLIST_HPP

#include "filesystem.hpp"

#include <atomic>
#include <vector>

class Player;

class Playlist {
public:
  Playlist(Player *);
  void build(const std::vector<const char *> &args);
  bool ready() const { return m_ready; }

  void absoluteSeek(size_t);
  void relativeSeek(int, bool shuffle = false);
  void skip();
  void deleteCurrent();

  bool empty() const { return m_files.empty(); }
  size_t size() const { return m_files.size(); }
  size_t position() const { return m_position; }

private:
  void appendDirectory(const fs::path &, const fs::path *skip = nullptr);
  void appendFile(const fs::path &, bool checkType = false);
  void sort(bool keepCurrentFile);

  Player *m_player;

  std::atomic_bool m_ready;
  std::vector<std::string> m_files;
  std::vector<size_t> m_shuffled;
  size_t m_position;

  struct {
    int delta;
    bool shuffle;
  } m_lastSeek;
};

#endif
