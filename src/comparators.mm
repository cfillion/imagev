#include "comparators.hpp"

#include <unordered_map>

#include <CoreServices/CoreServices.h>
#include <Foundation/Foundation.h>

struct UniString {
  std::unique_ptr<UniChar[]> data;
  UniCharCount size;
};

struct NameComparator::Private {
  CollatorRef collator;
  std::unordered_map<const std::string *, UniString> cache;

  const UniString &to_unistring(const std::string *u8);
};

const UniString &NameComparator::Private::to_unistring(const std::string *u8)
{
  const auto &[it, inserted] = cache.emplace(u8, UniString{});
  UniString &u16 { it->second };

  if(!inserted)
    return u16;

  const NSString *str { [NSString stringWithUTF8String:u8->c_str()] };
  u16.size = [str length];
  u16.data = std::unique_ptr<UniChar[]>(new UniChar[u16.size]);
  [str getCharacters:u16.data.get() range:NSMakeRange(0, u16.size)];

  return u16;
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

bool NameComparator::operator()(const std::string *a8, const std::string *b8) const
{
  if(!m_p->collator)
    return a8 < b8;

  const UniString &a16 { m_p->to_unistring(a8) }, &b16 { m_p->to_unistring(b8) };

  SInt32 order;
  UCCompareText(m_p->collator,
    a16.data.get(), a16.size, b16.data.get(), b16.size, nullptr, &order);

  return order < 0;
}
