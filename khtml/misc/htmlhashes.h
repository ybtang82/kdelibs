#ifndef HTMLHASHES_H
#define HTMLHASHES_H

#include "htmlattrs.h"
#include "htmltags.h"

namespace khtml
{
  int getTagID(const char *tagStr, int len);
  int getAttrID(const char *tagStr, int len);
};

#endif
