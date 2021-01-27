#include "options.hpp"
#include "player.hpp"
#include "window.hpp"

#include <stdexcept>
#include <thread>

int main(int, char *argv[])
{
  const Options options { argv };

  if(options.get<bool>(Option::Help)) {
    options.printHelp();
    return 0;
  }

  try {
    Player player { &options };
    Window window { &player };
    std::thread playlistThread {
      &Playlist::build, player.playlist(), options.files()
    };
    window.loop();
    playlistThread.join();
  }
  catch(const std::runtime_error &e) {
    fprintf(stderr, "%s\n", e.what());
    return 1;
  }

  return 0;
}
