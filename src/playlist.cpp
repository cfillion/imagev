#include "playlist.hpp"

#include "comparators.hpp"
#include "player.hpp"

#include <algorithm>
#include <random>

Playlist::Playlist(Player *player)
  : m_player { player }, m_ready { false }, m_position { 0 },
    m_lastSeek { 1, false }
{
}

void Playlist::build(const std::vector<const char *> &args)
{
  bool keepCurrentFile { false };

  for(const char *arg : args) {
    const fs::path fn { arg };

    if(fs::is_directory(fn))
      appendDirectory(fn);
    else {
      if(!fs::exists(fn)) {
        fprintf(stderr, "%s: No such file or directory\n", fn.c_str());
        continue;
      }

      appendFile(fn);
      keepCurrentFile = true;

      // open the first file fast!
      if(m_files.size() == 1)
        m_player->setFile(m_files.front().c_str());

      if(args.size() == 1)
        appendDirectory(fn.parent_path(), &fn);
    }
  }

  m_shuffled.resize(m_files.size());
  std::iota(m_shuffled.begin(), m_shuffled.end(), 0);

  if(!m_files.empty())
    sort(keepCurrentFile);

  m_ready = true;

  if(!keepCurrentFile) {
    // Force seek (absoluteSeek checks for m_position != newPos).
    m_position = -1;
    absoluteSeek(0);
  }

  m_player->playlistReady();
}

void Playlist::appendDirectory(const fs::path &dn, const fs::path *skip)
{
  fs::error_code ec;
  for(const fs::path &fn : fs::directory_iterator(dn, ec)) {
    if(fs::is_directory(fn) || (skip && *skip == fn))
      continue;

    appendFile(fn);
  }

  if(ec) {
    fprintf(stderr, "%s: Directory enumeration failed: %s\n",
      dn.c_str(), ec.message().c_str());
  }
}

void Playlist::appendFile(const fs::path &path, const bool)
{
  fs::error_code ec;
  const fs::path &absPath { fs::canonical(path, ec) };

  if(ec) {
    fprintf(stderr, "%s: canonical failed: %s\n",
      path.c_str(), ec.message().c_str());
    return;
  }

  const std::string &fn {
#ifdef HAVE_STD_FILESYSTEM
    absPath.u8string()
#else
    absPath.string()
#endif
  };

  m_files.push_back(fn);
}

void Playlist::sort(const bool keepCurrentFile)
{
  const std::string firstFile { m_files.front() };

  // TODO: more sorting options (eg. modification date)
  NameComparator comp;
  std::sort(m_files.begin(), m_files.end(), std::ref(comp));

  std::random_device randomDevice;
  std::mt19937 randomGenerator { randomDevice() };
  std::shuffle(m_shuffled.begin(), m_shuffled.end(), randomGenerator);

  if(keepCurrentFile) {
    const auto &match { std::find(m_files.begin(), m_files.end(), firstFile) };
    m_position = std::distance(m_files.begin(), match);
  }
}

void Playlist::absoluteSeek(size_t pos)
{
  if(!m_ready || m_files.empty() || pos >= m_files.size() || pos == m_position)
    return;

  m_position = pos;
  m_player->setFile(m_files[pos].c_str());
}

void Playlist::relativeSeek(const int delta, const bool shuffle)
{
  if(!m_ready || m_files.size() < 2)
    return;

  size_t pos;

  if(shuffle) {
    const auto &match { std::find(m_shuffled.begin(), m_shuffled.end(), m_position) };
    auto shuffledIndex { std::distance(m_shuffled.begin(), match) };
    shuffledIndex += delta;
    shuffledIndex %= m_shuffled.size();
    pos = m_shuffled[shuffledIndex];
  }
  else
    pos = m_position + delta;

  m_lastSeek = { delta, shuffle };

  absoluteSeek(pos);
}

void Playlist::skip()
{
  if(m_ready)
    relativeSeek(m_lastSeek.delta, m_lastSeek.shuffle);
}

void Playlist::deleteCurrent()
{
  if(!m_ready || m_files.empty())
    return;

  const std::string &fn { m_files[m_position] };

  if(!fs::moveToTrash(fn)) {
    fprintf(stderr, "%s: could not move to trash\n", fn.c_str());
    return;
  }

  m_player->unloadFile(); // invalidate uses of the path that we'll delete

  const auto &match { std::find(m_shuffled.begin(), m_shuffled.end(), m_position) };
  m_shuffled.erase(match);

  for(size_t &shuffledPos : m_shuffled) {
    if(shuffledPos > m_position)
      --shuffledPos;
  }

  m_files.erase(m_files.begin() + m_position);

  if(m_position == m_files.size() && m_position > 0)
    m_position--;

  if(!m_files.empty())
    m_player->setFile(m_files[m_position].c_str());
}
