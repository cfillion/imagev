#ifndef IMAGEV_WINDOW_HPP
#define IMAGEV_WINDOW_HPP

#include <atomic>

#include "filesystem.hpp"

class Player;
struct GLFWwindow;

class Window {
public:
  static void *getProcAddress(void *ctx, const char *name);

  Window(Player *);
  Window(const Window &) = delete;
  ~Window();

  void loop();

  // these functions are thread safe
  void playerEvent() { postEvent(PlayerEvents); }
  void update() { postEvent(Draw); }
  void updateTitle() { postEvent(SetTitle); }

private:
  enum EventFlag {
    PlayerEvents = 1<<0,
    Draw         = 1<<1,
    SetTitle     = 1<<2,
  };

  void draw();
  void setTitle();
#ifdef __APPLE__
  void setFilePath(const char *);
#endif
  void postEvent(EventFlag);
  void handleKey(unsigned int key, int mods);
  void handleChar(unsigned int codepoint);
  bool isFullscreen() const;
  void setFullscreen(bool);

  struct {
    int x, y;
    int w, h;
  } m_prevRect;

  GLFWwindow *m_handle;
  Player *m_player;
  std::atomic_uint m_eventFlags;
};

#endif
