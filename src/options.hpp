#ifndef IMAGEV_OPTIONS_HPP
#define IMAGEV_OPTIONS_HPP

#include <unordered_map>
#include <variant>
#include <vector>

enum class Option {
  All,
  Help,
  Recursive,
  Skip,
};

class Options {
public:
  using value_t = std::variant<bool>;

  Options(char *argv[]);
  const std::vector<const char *> &files() const { return m_files; }
  void printHelp() const;

  template<typename T>
  const T &get(Option opt) const { return std::get<T>(m_options.at(opt)); }

private:
  value_t *findByName(const char *);
  value_t *findByFlag(char);
  std::unordered_map<Option, value_t> m_options;
  std::vector<const char *> m_files;
};

#endif
