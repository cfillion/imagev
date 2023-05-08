#include "comparators.hpp"

#include <memory>
#include <unicode/coll.h>

struct NameComparator::Private {
  std::unique_ptr<icu::Collator> collator;
};

NameComparator::NameComparator() : m_p { std::make_unique<Private>() }
{
  UErrorCode status { U_ZERO_ERROR };
  m_p->collator.reset(icu::Collator::createInstance(icu::Locale::getDefault(), status));

  if(U_FAILURE(status)) {
    fprintf(stderr, "NameComparator: Collator::createInstance failed: %d\n", status);
    return;
  }

  status = U_ZERO_ERROR;
  m_p->collator->setAttribute(UCOL_NORMALIZATION_MODE, UCOL_ON, status);
  if(U_FAILURE(status))
    fprintf(stderr, "NameComparator::setAttribute: normalization mode failed: %d\n", status);

  status = U_ZERO_ERROR;
  m_p->collator->setAttribute(UCOL_STRENGTH, UCOL_SECONDARY, status);
  if(U_FAILURE(status))
    fprintf(stderr, "NameComparator::setAttribute: case insensitive mode failed: %d\n", status);

  status = U_ZERO_ERROR;
  m_p->collator->setAttribute(UCOL_NUMERIC_COLLATION, UCOL_ON, status);
  if(U_FAILURE(status))
    fprintf(stderr, "NameComparator: numeric mode failed: %d\n", status);

  // treat punctuation and symbols as significant (non ignorable)
  status = U_ZERO_ERROR;
  m_p->collator->setAttribute(UCOL_ALTERNATE_HANDLING, UCOL_NON_IGNORABLE, status);
  if (U_FAILURE(status))
    fprintf(stderr, "NameComparator: alternate handling failed: %d\n", status);
}

NameComparator::~NameComparator()
{
}

bool NameComparator::operator()(const std::string *a8, const std::string *b8)
const
{
  if(!m_p->collator)
    return a8 < b8;

  // TODO: cache the UTF-16 strings like in the macOS version
  const icu::UnicodeString &a16 { icu::UnicodeString::fromUTF8(a8->c_str()) },
                           &b16 { icu::UnicodeString::fromUTF8(b8->c_str()) };

  return m_p->collator->compare(a16, b16) == icu::Collator::LESS;
}
