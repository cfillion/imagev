#include "player.hpp"

#include <cstring>
#include <stdexcept>

#include <mpv/client.h>
#include <mpv/render_gl.h>

#include "options.hpp"
#include "window.hpp"

static auto mpv_set_property_flag(mpv_handle *mpv, const char *name, int flag)
{
  mpv_set_property(mpv, name, MPV_FORMAT_FLAG, &flag);
}

Player::Player(const Options *opts)
  : m_mpv_gl { nullptr }, m_options { opts }, m_window { nullptr },
    m_playlist { this }, m_skipOnReady { false }, m_path { nullptr }
{
  m_mpv = mpv_create();

  if(!m_mpv)
    throw std::runtime_error("mpv_create failed");

  mpv_set_property_flag(m_mpv, "access-references", false);
  mpv_set_property_flag(m_mpv, "audio", false);
  mpv_set_property_flag(m_mpv, "keep-open", true);
  mpv_set_property_flag(m_mpv, "loop-file", -1);
  mpv_set_property_flag(m_mpv, "prefetch-playlist", true);
  mpv_set_property_flag(m_mpv, "terminal", false);

  if(mpv_initialize(m_mpv) < 0)
    throw std::runtime_error("mpv_initialize failed");
}

Player::~Player()
{
  if(m_mpv_gl)
    mpv_render_context_free(m_mpv_gl);

  mpv_terminate_destroy(m_mpv);
}

void Player::processEvents()
{
  while(true) {
    const mpv_event *event { mpv_wait_event(m_mpv, 0) };

    switch(event->event_id) {
    case MPV_EVENT_NONE:
      return;
    case MPV_EVENT_IDLE:
      m_window->update();
      m_window->updateTitle();
      break;
    case MPV_EVENT_END_FILE:
      fileEnded(static_cast<mpv_event_end_file *>(event->data));
      break;
    case MPV_EVENT_FILE_LOADED:
      fileLoaded();
    default:
      break;
    };
  }
}

void Player::attach(Window *window)
{
  m_window = window;

  mpv_set_wakeup_callback(m_mpv, [](void *window) {
    static_cast<Window *>(window)->playerEvent();
  }, window);

  int advCtrl { 1 };

  mpv_opengl_init_params glParams { Window::getProcAddress, nullptr, nullptr };
  mpv_render_param params[] {
    { MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL) },
    { MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &glParams },
    { MPV_RENDER_PARAM_ADVANCED_CONTROL, &advCtrl },
    { MPV_RENDER_PARAM_INVALID, nullptr },
  };

  if(mpv_render_context_create(&m_mpv_gl, m_mpv, params) < 0)
    throw std::runtime_error("failed to initialize mpv GL context");

  mpv_render_context_set_update_callback(m_mpv_gl,
    &Player::update, static_cast<void *>(this));
}

void Player::detach()
{
  m_window = nullptr;
  mpv_set_wakeup_callback(m_mpv, nullptr, nullptr);
  mpv_render_context_set_update_callback(m_mpv_gl, nullptr, nullptr);

  const char *cmd[] { "stop", nullptr };
  mpv_command_async(m_mpv, 0, cmd);
}

void Player::update(void *instance)
{
  Player *self { static_cast<Player *>(instance) };
  const uint64_t flags { mpv_render_context_update(self->m_mpv_gl) };

  if(flags & MPV_RENDER_UPDATE_FRAME)
    self->m_window->update();
}

void Player::render(const int width, const int height)
{
  int flip { 1 };

  mpv_opengl_fbo fbo { 0, width, height, 0 };
  mpv_render_param params[] {
    { MPV_RENDER_PARAM_OPENGL_FBO, &fbo },
    { MPV_RENDER_PARAM_FLIP_Y, &flip },
    { MPV_RENDER_PARAM_INVALID, nullptr }
  };
  mpv_render_context_render(m_mpv_gl, params);
}

void Player::fileLoaded()
{
  m_window->updateTitle();
}

void Player::fileEnded(const mpv_event_end_file *event)
{
  if(event->reason != MPV_END_FILE_REASON_ERROR)
    return;

  fprintf(stderr, "error reading %s: %s\n", m_path, mpv_error_string(event->error));

  if(m_options->get<bool>(Option::Skip)) {
    if(m_playlist.ready())
      m_playlist.skip();
    else
      m_skipOnReady = true;
  }
  else {
    // the current file cannot be read
    // clear the previous file from the screen and update the title
    const char *cmd[] { "stop", nullptr };
    mpv_command_async(m_mpv, 0, cmd);
  }
}

void Player::playlistReady() // called from the playlist building thread
{
  if(m_skipOnReady)
    m_playlist.skip();
  else
    m_window->updateTitle();
}

void Player::setFile(const char *fn)
{
  // Used for displaying in the titlebar (sometimes before the playlist is ready).
  // fn's memory belong to the playlist
  m_path = fn;

  const char *cmd[] { "loadfile", fn, "replace", nullptr };
  mpv_command_async(m_mpv, 0, cmd);
}

void Player::addFile(const char *fn)
{
  const char *cmd[] { "loadfile", fn, "append", nullptr };
  mpv_command_async(m_mpv, 0, cmd);
}

void Player::unloadFile()
{
  m_path = nullptr; // m_path would be invalid since it belongs to the playlist

  const char *cmd[] { "stop", nullptr };
  mpv_command_async(m_mpv, 0, cmd);
}
