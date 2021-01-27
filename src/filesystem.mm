#include "filesystem.hpp"

#include <Foundation/Foundation.h>

bool fs::moveToTrash(const fs::path &path)
{
  NSString *fn { [NSString stringWithUTF8String:path.c_str()] };

  return [[NSFileManager defaultManager]
      trashItemAtURL: [NSURL fileURLWithPath:fn]
    resultingItemURL: nil
               error: nil
  ];
}
