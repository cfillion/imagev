#include "comparators.hpp"

#include <vector>

#include <CoreServices/CoreServices.h>
#include <Foundation/Foundation.h>

struct NameComparator::Private {
  CollatorRef collator;
};

static std::vector<unichar> unistring(const std::string &utf8, UniCharCount *length)
{
  const NSString *str { [NSString stringWithUTF8String:utf8.c_str()] };
  *length = [str length];

  std::vector<unichar> buffer;
  buffer.reserve(*length);
  [str getCharacters:buffer.data() range:NSMakeRange(0, *length)];

  return buffer;
}

NameComparator::NameComparator() : m_p { std::make_unique<Private>() }
{
  const UCCollateOptions options {
    kUCCollateCaseInsensitiveMask | kUCCollatePunctuationSignificantMask |
    kUCCollateDigitsAsNumberMask | kUCCollateDigitsOverrideMask
  };

  const OSStatus status = UCCreateCollator(nullptr, 0, options, &m_p->collator);
  if(status != 0)
    fprintf(stderr, "UCCreateCollator failed");
}

NameComparator::~NameComparator()
{
  if(m_p->collator)
    UCDisposeCollator(&m_p->collator);
}

bool NameComparator::operator()(const std::string &a, const std::string &b) const
{
  if(!m_p->collator)
    return a < b;

  UniCharCount aLength, bLength;

  SInt32 order;
  UCCompareText(m_p->collator,
    unistring(a, &aLength).data(), aLength,
    unistring(b, &bLength).data(), bLength,
    nullptr, &order);

  return order < 0;
}
