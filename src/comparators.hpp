#ifndef IMAGEV_COMPARATORS_HPP
#define IMAGEV_COMPARATORS_HPP

#include <QCollator>
#include <QFileInfo>

class NameComparator {
public:
  NameComparator();
  bool operator()(const QFileInfo *, const QFileInfo *) const;

private:
  QCollator m_collator;
};

#endif
