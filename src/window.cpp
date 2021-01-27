#include "window.hpp"

#include <stdexcept>

#include <GLFW/glfw3.h>

#include "player.hpp"

constexpr const char *DEFAULT_TITLE { "imagev" };

static Window *getWindow(GLFWwindow *handle)
{
  return static_cast<Window *>(glfwGetWindowUserPointer(handle));
}

static std::string basename(const std::string &fn)
{
  const size_t lastSep { fn.find_last_of("/\\") };

  if(lastSep == std::string::npos)
    return fn;
  else
    return fn.substr(lastSep + 1);
}

void *Window::getProcAddress(void *, const char *name)
{
  return reinterpret_cast<void *>(glfwGetProcAddress(name));
}

Window::Window(Player *player) : m_player(player), m_eventFlags { 0 }
{
  glfwSetErrorCallback([](const int code, const char *description) {
    fprintf(stderr, "GLFW Error %d: %s\n", code, description);
  });

  if(!glfwInit())
    throw std::runtime_error("glfwInit failed");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWmonitor *monitor { glfwGetPrimaryMonitor() };
  const GLFWvidmode* mode { glfwGetVideoMode(monitor) };
  const int width = mode->width * 0.8, height = mode->height * 0.8;

  m_handle = glfwCreateWindow(width, height, DEFAULT_TITLE, nullptr, nullptr);

  if(!m_handle)
    throw std::runtime_error("glfwCreateGUIWindow failed");

  glfwMakeContextCurrent(m_handle);
  m_player->attach(this);

  glfwSetWindowUserPointer(m_handle, static_cast<void *>(this));

  glfwSetWindowRefreshCallback(m_handle, [](GLFWwindow *handle) {
    getWindow(handle)->draw();
  });

  glfwSetKeyCallback(m_handle, [](GLFWwindow *handle, const int key,
      int, const int action, const int mods) {
    if(action == GLFW_PRESS || action == GLFW_REPEAT)
      getWindow(handle)->handleKey(key, mods);
  });

  glfwSetCharCallback(m_handle, [](GLFWwindow *handle,
      const unsigned int codepoint) {
    getWindow(handle)->handleChar(codepoint);
  });
}

Window::~Window()
{
  m_player->detach();
  glfwDestroyWindow(m_handle);
  glfwTerminate();
}

void Window::loop()
{
  while(!glfwWindowShouldClose(m_handle)) {
    const unsigned int eventFlags { m_eventFlags.exchange(0) };

    if(eventFlags & PlayerEvents)
      m_player->processEvents();
    if(eventFlags & Draw)
      draw();
    if(eventFlags & SetTitle)
      setTitle();

#ifdef __linux__
    glfwWaitEventsTimeout(1);
#else
    glfwWaitEvents();
#endif
  }
}

void Window::postEvent(const EventFlag flag)
{
  m_eventFlags |= flag;
  glfwPostEmptyEvent();
}

void Window::draw()
{
  int width, height;
  glfwGetFramebufferSize(m_handle, &width, &height);

  glfwMakeContextCurrent(m_handle);
  m_player->render(width, height);
  glfwSwapBuffers(m_handle);
}

void Window::setTitle()
{
  std::string fn;
  if(const char *path = m_player->path()) {
    fn = basename(path);
#ifdef __APPLE__
    setFilePath(path);
#endif
  }
  else
    fn = DEFAULT_TITLE;

  Playlist *playlist { m_player->playlist() };

  char title[255];
  if(m_player->playlist()->ready())
    snprintf(title, sizeof(title), "%s [%zu/%zu]", fn.c_str(),
      playlist->empty() ? 0 : playlist->position() + 1,
      playlist->size());
  else
    snprintf(title, sizeof(title), "%s [?/?]", fn.c_str());

  glfwSetWindowTitle(m_handle, title);
}

void Window::handleKey(unsigned int key, int mods)
{
  switch(key) {
  case GLFW_KEY_LEFT:
  case GLFW_KEY_UP:
  case GLFW_KEY_PAGE_UP:
  case GLFW_KEY_COMMA:
    if(!mods)
      m_player->playlist()->relativeSeek(-1);
    break;
  case GLFW_KEY_RIGHT:
  case GLFW_KEY_DOWN:
  case GLFW_KEY_PAGE_DOWN:
  case GLFW_KEY_PERIOD:
    if(!mods)
      m_player->playlist()->relativeSeek(+1);
    break;
  case GLFW_KEY_HOME:
    if(!mods)
      m_player->playlist()->absoluteSeek(0);
    break;
  case GLFW_KEY_END:
    if(!mods)
      m_player->playlist()->absoluteSeek(m_player->playlist()->size() - 1);
    break;
  case GLFW_KEY_DELETE:
    if(!mods)
      m_player->playlist()->deleteCurrent();
    break;
  }
}

void Window::handleChar(const unsigned int codepoint) // obeys keyboard layout
{
  switch(codepoint) {
  case 'f':
    setFullscreen(!isFullscreen());
    break;
  case 'q':
    glfwSetWindowShouldClose(m_handle, GLFW_TRUE);
    break;
  case 'R':
    m_player->playlist()->relativeSeek(-1, true);
    break;
  case 'r':
    m_player->playlist()->relativeSeek(+1, true);
    break;
  }
}

bool Window::isFullscreen() const
{
  return glfwGetWindowMonitor(m_handle) != nullptr;
}

void Window::setFullscreen(const bool fullscreen)
{
  if(!fullscreen) {
    glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowMonitor(m_handle, nullptr,
      m_prevRect.x, m_prevRect.y, m_prevRect.w, m_prevRect.h, GLFW_DONT_CARE);
    return;
  }

  glfwGetWindowPos(m_handle, &m_prevRect.x, &m_prevRect.y);
  glfwGetWindowSize(m_handle, &m_prevRect.w, &m_prevRect.h);

  GLFWmonitor *monitor { glfwGetPrimaryMonitor() };
  const GLFWvidmode* mode { glfwGetVideoMode(monitor) };

  glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetWindowMonitor(m_handle, monitor,
    0, 0, mode->width, mode->height, GLFW_DONT_CARE);
}
