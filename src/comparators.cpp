#include "comparators.hpp"

NameComparator::NameComparator()
{
  m_collator.setCaseSensitivity(Qt::CaseInsensitive);
  m_collator.setNumericMode(true);
}

bool NameComparator::operator()(const QFileInfo *a, const QFileInfo *b) const
{
  return m_collator.compare(a->absoluteFilePath(), b->absoluteFilePath()) < 0;
}
