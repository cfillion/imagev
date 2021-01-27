#ifndef IMAGEV_COMPARATORS_HPP
#define IMAGEV_COMPARATORS_HPP

#include <memory>
#include <string>

class NameComparator {
public:
  NameComparator();
  NameComparator(const NameComparator &) = delete;
  ~NameComparator();

  bool operator()(const std::string &, const std::string &) const;

private:
  struct Private;
  std::unique_ptr<Private> m_p;
};

#endif
