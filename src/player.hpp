#ifndef IMAGEV_PLAYER_HPP
#define IMAGEV_PLAYER_HPP

#include "playlist.hpp"

struct mpv_event_command;
struct mpv_event_end_file;
struct mpv_handle;
struct mpv_render_context;

class Options;
class Window;

class Player {
public:
  Player(const Options *);
  Player(const Player &) = delete;
  ~Player();

  void attach(Window *);
  void detach();
  void processEvents();
  void render(int w, int h);

  // these functions may be called from the playlist builder thread
  void setFile(const char *);
  void addFile(const char *);
  void unloadFile();
  void playlistReady();

  Playlist *playlist() { return &m_playlist; }
  const Options *options() const { return m_options; }
  const char *path() { return m_path; }

private:
  static void update(void *self);

  void fileLoaded();
  void fileEnded(const mpv_event_end_file *);
  void playlistReady(bool idle);

  mpv_handle *m_mpv;
  mpv_render_context *m_mpv_gl;
  const Options *m_options;
  Window *m_window;

  Playlist m_playlist;
  std::atomic_bool m_skipOnReady;
  const char *m_path;
};

#endif
