#include "playlist.hpp"

#include "comparators.hpp"
#include "options.hpp"
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
    fs::error_code ec;
    const fs::path &fn { fs::canonical(arg, ec) };

    if(ec) {
      fprintf(stderr, "%s: %s\n", arg, ec.message().c_str());
      continue;
    }

    if(fs::is_directory(fn))
      appendDirectory(fn);
    else {
      appendFile(fn);
      keepCurrentFile = true;

      // open the first file fast!
      if(m_files.size() == 1)
        m_player->setFile(m_ordered.front()->c_str());

      if(args.size() == 1)
        appendDirectory(fn.parent_path());
    }
  }

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

void Playlist::appendDirectory(const fs::path &dn)
{
  fs::error_code ec;
  for(const fs::path &fn : fs::directory_iterator(dn, ec)) {
    if(fs::is_directory(fn))
      continue;
    else if(!m_player->options()->get<bool>(Option::All) &&
        fn.filename().string()[0] == '.')
      continue;

    appendFile(fn);
  }

  if(ec) {
    fprintf(stderr, "%s: directory enumeration failed: %s\n",
      dn.c_str(), ec.message().c_str());
  }
}

void Playlist::appendFile(const fs::path &fn)
{
  const auto &[it, inserted] = m_files.emplace(
#ifdef HAVE_STD_FILESYSTEM
    fn.u8string()
#else
    fn.string()
#endif
  );

  if(inserted) {
    m_ordered.emplace_back(&*it);
    m_shuffled.emplace_back(m_shuffled.size());
  }
}

void Playlist::sort(const bool keepCurrentFile)
{
  const std::string *firstFile { m_ordered.front() };

  // TODO: more sorting options (eg. modification date)
  NameComparator comp;
  std::sort(m_ordered.begin(), m_ordered.end(), std::ref(comp));

  std::random_device randomDevice;
  std::mt19937 randomGenerator { randomDevice() };
  std::shuffle(m_shuffled.begin(), m_shuffled.end(), randomGenerator);

  if(keepCurrentFile) {
    const auto &match { std::find(m_ordered.begin(), m_ordered.end(), firstFile) };
    m_position = std::distance(m_ordered.begin(), match);
  }
}

size_t Playlist::advanceShuffled(const int delta) const
{
  const auto &match { std::find(m_shuffled.begin(), m_shuffled.end(), m_position) };
  auto shuffledIndex { std::distance(m_shuffled.begin(), match) };
  shuffledIndex += delta;
  shuffledIndex %= m_shuffled.size();
  return m_shuffled[shuffledIndex];
}

void Playlist::absoluteSeek(size_t pos)
{
  if(!m_ready || m_files.empty() || pos >= m_files.size() || pos == m_position)
    return;

  m_position = pos;
  m_player->setFile(m_ordered[pos]->c_str());

  prefetchNext();
}

void Playlist::relativeSeek(const int delta, const bool shuffle)
{
  if(!m_ready || m_files.size() < 2)
    return;

  size_t pos;

  if(shuffle)
    pos = advanceShuffled(delta);
  else
    pos = m_position + delta;

  m_lastSeek = { delta, shuffle };

  absoluteSeek(pos);
}

void Playlist::prefetchNext()
{
  if(m_files.size() < 2)
    return;

  size_t pos;

  if(m_lastSeek.shuffle)
    pos = advanceShuffled(m_lastSeek.delta);
  else {
    pos = m_position + m_lastSeek.delta;
    pos %= m_files.size();
  }

  m_player->addFile(m_ordered[pos]->c_str());
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

  const std::string &fn { *m_ordered[m_position] };

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

  m_files.erase(*m_ordered[m_position]);
  m_ordered.erase(m_ordered.begin() + m_position);

  if(m_position == m_files.size() && m_position > 0)
    m_position--;

  if(!m_files.empty())
    m_player->setFile(m_ordered[m_position]->c_str());
}
