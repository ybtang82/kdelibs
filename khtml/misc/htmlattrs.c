/* ANSI-C code produced by gperf version 2.7 */
/* Command-line: gperf -a -L ANSI-C -E -C -c -o -t -k * -NfindAttr -Hhash_attr -Wwordlist_attr -D -s 2 htmlattrs.gperf  */
/* This file is automatically generated from
#khtmlattrs.in by makeattrs, do not edit */
/* Copyright 1999 Lars Knoll */
#include "khtmlattrs.h"
struct attrs {
    const char *name;
    int id;
};
/* maximum key range = 929, duplicates = 0 */

#ifdef __GNUC__
__inline
#endif
static unsigned int
hash_attr (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943,   0, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943,   5,  50,   0,
       10,   0,  90, 215,  65,  60,   0, 190,   0, 115,
        0,   0, 165,   0,  85,  30,  45,  10, 210,   0,
       35,   0,   0, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943, 943, 943, 943, 943,
      943, 943, 943, 943, 943, 943
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      case 13:
        hval += asso_values[(unsigned char)str[12]];
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#endif
const struct attrs *
findAttr (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 124,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 14,
      MIN_HASH_VALUE = 14,
      MAX_HASH_VALUE = 942
    };

  static const struct attrs wordlist_attr[] =
    {
      {"code", ATTR_CODE},
      {"onload", ATTR_ONLOAD},
      {"onunload", ATTR_ONUNLOAD},
      {"cols", ATTR_COLS},
      {"alt", ATTR_ALT},
      {"label", ATTR_LABEL},
      {"data", ATTR_DATA},
      {"class", ATTR_CLASS},
      {"id", ATTR_ID},
      {"style", ATTR_STYLE},
      {"onselect", ATTR_ONSELECT},
      {"rel", ATTR_REL},
      {"color", ATTR_COLOR},
      {"selected", ATTR_SELECTED},
      {"size", ATTR_SIZE},
      {"clear", ATTR_CLEAR},
      {"content", ATTR_CONTENT},
      {"face", ATTR_FACE},
      {"object", ATTR_OBJECT},
      {"codebase", ATTR_CODEBASE},
      {"declare", ATTR_DECLARE},
      {"readonly", ATTR_READONLY},
      {"cite", ATTR_CITE},
      {"action", ATTR_ACTION},
      {"noshade", ATTR_NOSHADE},
      {"src", ATTR_SRC},
      {"rows", ATTR_ROWS},
      {"name", ATTR_NAME},
      {"text", ATTR_TEXT},
      {"rules", ATTR_RULES},
      {"coords", ATTR_COORDS},
      {"axis", ATTR_AXIS},
      {"onfocus", ATTR_ONFOCUS},
      {"classid", ATTR_CLASSID},
      {"standby", ATTR_STANDBY},
      {"onblur", ATTR_ONBLUR},
      {"title", ATTR_TITLE},
      {"dir", ATTR_DIR},
      {"char", ATTR_CHAR},
      {"onreset", ATTR_ONRESET},
      {"disabled", ATTR_DISABLED},
      {"onmousedown", ATTR_ONMOUSEDOWN},
      {"for", ATTR_FOR},
      {"noresize", ATTR_NORESIZE},
      {"width", ATTR_WIDTH},
      {"defer", ATTR_DEFER},
      {"abbr", ATTR_ABBR},
      {"media", ATTR_MEDIA},
      {"scope", ATTR_SCOPE},
      {"headers", ATTR_HEADERS},
      {"span", ATTR_SPAN},
      {"colspan", ATTR_COLSPAN},
      {"onkeydown", ATTR_ONKEYDOWN},
      {"tabindex", ATTR_TABINDEX},
      {"type", ATTR_TYPE},
      {"start", ATTR_START},
      {"scheme", ATTR_SCHEME},
      {"enctype", ATTR_ENCTYPE},
      {"onmouseout", ATTR_ONMOUSEOUT},
      {"accept", ATTR_ACCEPT},
      {"lang", ATTR_LANG},
      {"codetype", ATTR_CODETYPE},
      {"html", ATTR_HTML},
      {"value", ATTR_VALUE},
      {"plain", ATTR_PLAIN},
      {"border", ATTR_BORDER},
      {"charset", ATTR_CHARSET},
      {"method", ATTR_METHOD},
      {"href", ATTR_HREF},
      {"nohref", ATTR_NOHREF},
      {"link", ATTR_LINK},
      {"onclick", ATTR_ONCLICK},
      {"wrap", ATTR_WRAP},
      {"alink", ATTR_ALINK},
      {"nowrap", ATTR_NOWRAP},
      {"longdesc", ATTR_LONGDESC},
      {"accesskey", ATTR_ACCESSKEY},
      {"shape", ATTR_SHAPE},
      {"hspace", ATTR_HSPACE},
      {"checked", ATTR_CHECKED},
      {"align", ATTR_ALIGN},
      {"datetime", ATTR_DATETIME},
      {"rowspan", ATTR_ROWSPAN},
      {"onchange", ATTR_ONCHANGE},
      {"rev", ATTR_REV},
      {"frame", ATTR_FRAME},
      {"onsubmit", ATTR_ONSUBMIT},
      {"ondblclick", ATTR_ONDBLCLICK},
      {"bordercolor", ATTR_BORDERCOLOR},
      {"usemap", ATTR_USEMAP},
      {"compact", ATTR_COMPACT},
      {"onmouseup", ATTR_ONMOUSEUP},
      {"charoff", ATTR_CHAROFF},
      {"bgcolor", ATTR_BGCOLOR},
      {"summary", ATTR_SUMMARY},
      {"onkeyup", ATTR_ONKEYUP},
      {"ismap", ATTR_ISMAP},
      {"version", ATTR_VERSION},
      {"scrolling", ATTR_SCROLLING},
      {"target", ATTR_TARGET},
      {"multiple", ATTR_MULTIPLE},
      {"profile", ATTR_PROFILE},
      {"vspace", ATTR_VSPACE},
      {"oversrc", ATTR_OVERSRC},
      {"archive", ATTR_ARCHIVE},
      {"valuetype", ATTR_VALUETYPE},
      {"height", ATTR_HEIGHT},
      {"language", ATTR_LANGUAGE},
      {"accept-charset", ATTR_ACCEPT_CHARSET},
      {"onmouseover", ATTR_ONMOUSEOVER},
      {"vlink", ATTR_VLINK},
      {"hreflang", ATTR_HREFLANG},
      {"cellpadding", ATTR_CELLPADDING},
      {"cellspacing", ATTR_CELLSPACING},
      {"maxlength", ATTR_MAXLENGTH},
      {"onmousemove", ATTR_ONMOUSEMOVE},
      {"valign", ATTR_VALIGN},
      {"onkeypress", ATTR_ONKEYPRESS},
      {"frameborder", ATTR_FRAMEBORDER},
      {"background", ATTR_BACKGROUND},
      {"prompt", ATTR_PROMPT},
      {"http-equiv", ATTR_HTTP_EQUIV},
      {"marginwidth", ATTR_MARGINWIDTH},
      {"marginheight", ATTR_MARGINHEIGHT}
    };

  static const signed char lookup[] =
    {
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,   0,  -1,  -1,  -1,  -1,  -1,
       -1,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,   2,   3,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,   4,  -1,  -1,  -1,  -1,  -1,  -1,
        5,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   6,
        7,  -1,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
        9,  -1,  -1,  10,  -1,  -1,  -1,  -1,  11,  -1,
       12,  -1,  -1,  13,  14,  15,  -1,  16,  -1,  17,
       -1,  18,  -1,  19,  -1,  -1,  -1,  20,  21,  22,
       -1,  -1,  -1,  -1,  -1,  -1,  23,  24,  25,  26,
       -1,  -1,  -1,  -1,  27,  -1,  -1,  -1,  -1,  28,
       29,  30,  -1,  -1,  31,  -1,  -1,  32,  -1,  -1,
       -1,  -1,  33,  -1,  -1,  -1,  -1,  34,  -1,  -1,
       -1,  35,  -1,  -1,  -1,  36,  -1,  -1,  37,  38,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  39,  -1,  -1,
       -1,  -1,  -1,  40,  -1,  -1,  41,  -1,  42,  -1,
       -1,  -1,  -1,  43,  -1,  44,  -1,  -1,  -1,  -1,
       45,  -1,  -1,  -1,  46,  47,  -1,  -1,  -1,  -1,
       48,  -1,  49,  -1,  50,  -1,  -1,  51,  -1,  52,
       -1,  -1,  -1,  53,  54,  55,  56,  57,  -1,  -1,
       58,  59,  -1,  -1,  60,  -1,  -1,  -1,  61,  62,
       63,  -1,  -1,  -1,  -1,  64,  65,  66,  -1,  -1,
       -1,  67,  -1,  -1,  68,  -1,  69,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  70,  -1,  -1,  71,  -1,  72,
       73,  74,  -1,  75,  76,  -1,  -1,  -1,  -1,  -1,
       77,  78,  79,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  80,  -1,  -1,  81,  -1,
       -1,  -1,  82,  83,  -1,  -1,  -1,  -1,  84,  -1,
       85,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  86,  -1,
       87,  -1,  -1,  -1,  -1,  -1,  88,  -1,  -1,  -1,
       -1,  89,  -1,  -1,  -1,  -1,  -1,  90,  -1,  91,
       -1,  -1,  92,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  93,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  94,  -1,  -1,
       -1,  -1,  95,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       96,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  97,  -1,  -1,  -1,  -1,  -1,  -1,  98,
       -1,  99,  -1, 100,  -1,  -1,  -1, 101,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 102, 103,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 104,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 105,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 106,  -1, 107, 108,
       -1, 109,  -1,  -1,  -1, 110,  -1,  -1, 111,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 112,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 113,  -1,  -1, 114,
       -1, 115,  -1,  -1,  -1,  -1, 116,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      117,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 118,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 119,  -1,  -1,  -1,  -1,
       -1, 120,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      121,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 122,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 123
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash_attr (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist_attr[index].name;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1))
                return &wordlist_attr[index];
            }
          else if (index < -TOTAL_KEYWORDS)
            {
              register int offset = - 1 - TOTAL_KEYWORDS - index;
              register const struct attrs *wordptr = &wordlist_attr[TOTAL_KEYWORDS + lookup[offset]];
              register const struct attrs *wordendptr = wordptr + -lookup[offset + 1];

              while (wordptr < wordendptr)
                {
                  register const char *s = wordptr->name;

                  if (*str == *s && !strncmp (str + 1, s + 1, len - 1))
                    return wordptr;
                  wordptr++;
                }
            }
        }
    }
  return 0;
}


static const DOMString attrList[] = {
    "ABBR",
    "ACCEPT-CHARSET",
    "ACCEPT",
    "ACCESSKEY",
    "ACTION",
    "ALIGN",
    "ALINK",
    "ALT",
    "ARCHIVE",
    "AXIS",
    "BACKGROUND",
    "BGCOLOR",
    "BORDER",
    "BORDERCOLOR",
    "CELLPADDING",
    "CELLSPACING",
    "CHAR",
    "CHAROFF",
    "CHARSET",
    "CHECKED",
    "CITE",
    "CLASS",
    "CLASSID",
    "CLEAR",
    "CODE",
    "CODEBASE",
    "CODETYPE",
    "COLOR",
    "COLS",
    "COLSPAN",
    "COMPACT",
    "CONTENT",
    "COORDS",
    "DATA",
    "DATETIME",
    "DECLARE",
    "DEFER",
    "DIR",
    "DISABLED",
    "ENCTYPE",
    "FACE",
    "FOR",
    "FRAME",
    "FRAMEBORDER",
    "HEADERS",
    "HEIGHT",
    "HREF",
    "HREFLANG",
    "HSPACE",
    "HTML",
    "HTTP-EQUIV",
    "ID",
    "ISMAP",
    "LABEL",
    "LANG",
    "LANGUAGE",
    "LINK",
    "LONGDESC",
    "MARGINHEIGHT",
    "MARGINWIDTH",
    "MAXLENGTH",
    "MEDIA",
    "METHOD",
    "MULTIPLE",
    "NAME",
    "NOHREF",
    "NORESIZE",
    "NOSHADE",
    "NOWRAP",
    "OBJECT",
    "ONBLUR",
    "ONCHANGE",
    "ONCLICK",
    "ONDBLCLICK",
    "ONFOCUS",
    "ONKEYDOWN",
    "ONKEYPRESS",
    "ONKEYUP",
    "ONLOAD",
    "ONMOUSEDOWN",
    "ONMOUSEMOVE",
    "ONMOUSEOUT",
    "ONMOUSEOVER",
    "ONMOUSEUP",
    "ONRESET",
    "ONSELECT",
    "ONSUBMIT",
    "ONUNLOAD",
    "OVERSRC",
    "PLAIN",
    "PROFILE",
    "PROMPT",
    "READONLY",
    "REL",
    "REV",
    "ROWS",
    "ROWSPAN",
    "RULES",
    "SCHEME",
    "SCOPE",
    "SCROLLING",
    "SELECTED",
    "SHAPE",
    "SIZE",
    "SPAN",
    "SRC",
    "STANDBY",
    "START",
    "STYLE",
    "SUMMARY",
    "TABINDEX",
    "TARGET",
    "TEXT",
    "TITLE",
    "TYPE",
    "USEMAP",
    "VALIGN",
    "VALUE",
    "VALUETYPE",
    "VERSION",
    "VLINK",
    "VSPACE",
    "WIDTH",
    "WRAP",
    0
};
const DOMString &getAttrName(unsigned short id)
{
    return attrList[id-1];
};
