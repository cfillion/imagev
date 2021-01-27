#include "options.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>

struct OptionInfo {
  const char *name;
  char flag;
  Options::value_t defaultValue;
  const char *description;
};

static const std::map<Option, OptionInfo> g_options {
  { Option::Help, { "help", 'h', false, "show this help and exit" } },
  { Option::Skip, { "skip", 's', false, "skip files that cannot be loaded" } },
};

Options::Options(char *argv[])
{
  for(const auto &pair : g_options)
    m_options.insert({pair.first, pair.second.defaultValue});

  bool enableOptions { true };

  while(*++argv) {
    if(enableOptions) {
      if(!strcmp("--", *argv)) {
        enableOptions = false;
        continue;
      }
      else if(**argv == '-') {
        const char *flag = *argv;

        if(flag[1] == '-') {
          if(value_t *value { findByName(&flag[2]) })
            *value = true;
          else
            fprintf(stderr, "imagev: unrecognized option: %s\n", flag);

          continue;
        }

        while(*++flag) {
          if(value_t *value { findByFlag(*flag) })
            *value = true;
          else
            fprintf(stderr, "imagev: unrecognized option: -%c\n", *flag);
        }

        continue;
      }
    }

    m_files.push_back(*argv);
  }
}

Options::value_t *Options::findByName(const char *name)
{
  const auto it {
    std::find_if(std::begin(g_options), std::end(g_options),
      [name](const auto &pair) { return !strcmp(pair.second.name, name); })
  };

  return it == std::end(g_options) ? nullptr : &m_options[it->first];
}

Options::value_t *Options::findByFlag(const char flag)
{
  const auto it {
    std::find_if(std::begin(g_options), std::end(g_options),
      [flag](const auto &pair) { return pair.second.flag == flag; })
  };

  return it == std::end(g_options) ? nullptr : &m_options[it->first];
}

void Options::printHelp() const
{
  printf("Usage: imagev [options] <files or directories...>\n");
  printf("Viewer for directories of images and animations.\n\n");
  printf("Options:\n");

  for(const auto &pair : g_options) {
    const OptionInfo &opt { pair.second };

    char flags[255];
    snprintf(flags, sizeof(flags), "-%c, --%s", opt.flag, opt.name);
    printf("  %-32s%s\n", flags, opt.description);
  }
}
