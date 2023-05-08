#ifndef IMAGEV_FILESYSTEM_HPP
#define IMAGEV_FILESYSTEM_HPP

#include "config.hpp"

#ifdef HAVE_STD_FILESYSTEM
  #include <filesystem>
  namespace fs {
    using namespace std::filesystem;
    using error_code = std::error_code;
  }
#else
  #include <boost/filesystem.hpp>
  namespace fs {
    using namespace boost::filesystem;
    using error_code = boost::system::error_code;
  }
#endif

namespace fs {
  bool moveToTrash(const fs::path &);
}

#endif
