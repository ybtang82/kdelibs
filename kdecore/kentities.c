/* ANSI-C code produced by gperf version 2.7.2 */
/* Command-line: gperf -a -L ANSI-C -E -C -c -o -t -k '*' -NfindEntity -Hhash_Entity -Wwordlist_Entity -D -s 2 kentities.gperf  */
/*   This file is part of the KDE libraries
  
     Copyright (C) 1999 Lars Knoll (knoll@mpi-hd.mpg.de)
  
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
  
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
  
     You should have received a copy of the GNU Library General Public License
     along with this library; see the file COPYING.LIB.  If not, write to
     the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
     Boston, MA 02111-1307, USA.
  
  ----------------------------------------------------------------------------
  
    kentities.gperf: input file to generate a hash table for entities
    kentities.c: DO NOT EDIT! generated by the command
    "gperf -a -L "ANSI-C" -C -G -c -o -t -k '*' -NfindEntity -D -s 2 khtmlentities.gperf > entities.c"   
    from kentities.gperf 

    $Id$     
*/  

#include <string.h>

struct entity {
    const char *name;
    int code;
};
/* maximum key range = 1735, duplicates = 1 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash_Entity (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,    0,
        15,   10,    0, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737,  165,    5,    0,    5,   35,
      1737,    5,    0,   75, 1737,    5,    0,    0,    5,   45,
         5,    0,    5,    0,    0,   25, 1737, 1737,   20,   40,
         0, 1737, 1737, 1737, 1737, 1737, 1737,    0,  305,  115,
       460,    5,    0,  410,  315,   15,    5,    0,   20,  380,
       150,  380,  105,   75,    0,  205,   35,   10,   15,   20,
        15,  105,   25, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737, 1737,
      1737, 1737, 1737, 1737, 1737, 1737
    };
  register int hval = len;

  switch (hval)
    {
      default:
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
const struct entity *
findEntity (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 258,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 8,
      MIN_HASH_VALUE = 2,
      MAX_HASH_VALUE = 1736
    };

  static const struct entity wordlist_Entity[] =
    {
      {"LT", 60},
      {"rarr", 0x2192},
      {"GT", 62},
      {"Mu", 0x039c},
      {"Tau", 0x03a4},
      {"uarr", 0x2191},
      {"Nu", 0x039d},
      {"Pi", 0x03a0},
      {"larr", 0x2190},
      {"le", 0x2264},
      {"real", 0x211c},
      {"xi", 0x03be},
      {"Xi", 0x039e},
      {"ETH", 0x00d0},
      {"eta", 0x03b7},
      {"Zeta", 0x0396},
      {"tau", 0x03c4},
      {"Beta", 0x0392},
      {"zwj", 0x200d},
      {"lt", 60},
      {"THORN", 0x00de},
      {"zeta", 0x03b6},
      {"Delta", 0x0394},
      {"Eta", 0x0397},
      {"QUOT", 34},
      {"para", 0x00b6},
      {"crarr", 0x21b5},
      {"frac14", 0x00bc},
      {"pi", 0x03c0},
      {"equiv", 0x2261},
      {"frac34", 0x00be},
      {"frac12", 0x00bd},
      {"piv", 0x03d6},
      {"part", 0x2202},
      {"weierp", 0x2118},
      {"ne", 0x2260},
      {"rceil", 0x2309},
      {"nu", 0x03bd},
      {"ni", 0x220b},
      {"rArr", 0x21d2},
      {"acute", 0x00b4},
      {"aacute", 0x00e1},
      {"AMP", 38},
      {"iexcl", 0x00a1},
      {"eacute", 0x00e9},
      {"uArr", 0x21d1},
      {"lceil", 0x2308},
      {"uacute", 0x00fa},
      {"iacute", 0x00ed},
      {"lArr", 0x21d0},
      {"Uacute", 0x00da},
      {"int", 0x222b},
      {"zwnj", 0x200c},
      {"Eacute", 0x00c9},
      {"Yacute", 0x00dd},
      {"kappa", 0x03ba},
      {"Oacute", 0x00d3},
      {"perp", 0x22a5},
      {"Kappa", 0x039a},
      {"cap", 0x2229},
      {"Psi", 0x03a8},
      {"frasl", 0x2044},
      {"cup", 0x222a},
      {"Iacute", 0x00cd},
      {"circ", 0x02c6},
      {"acirc", 0x00e2},
      {"ecirc", 0x00ea},
      {"ucirc", 0x00fb},
      {"yen", 0x00a5},
      {"icirc", 0x00ee},
      {"Ucirc", 0x00db},
      {"yacute", 0x00fd},
      {"exist", 0x2203},
      {"Ecirc", 0x00ca},
      {"curren", 0x00a4},
      {"Ocirc", 0x00d4},
      {"cent", 0x00a2},
      {"harr", 0x2194},
      {"sup", 0x2283},
      {"sup1", 0x00b9},
      {"Icirc", 0x00ce},
      {"psi", 0x03c8},
      {"supe", 0x2287},
      {"Chi", 0x03a7},
      {"sup3", 0x00b3},
      {"infin", 0x221e},
      {"Aacute", 0x00c1},
      {"Phi", 0x03a6},
      {"sup2", 0x00b2},
      {"beta", 0x03b2},
      {"iquest", 0x00bf},
      {"eth", 0x00f0},
      {"bull", 0x2022},
      {"Theta", 0x0398},
      {"sect", 0x00a7},
      {"there4", 0x2234},
      {"or", 0x22a6},
      {"isin", 0x2208},
      {"mu", 0x03bc},
      {"theta", 0x03b8},
      {"euro", 0x20ac},
      {"lrm", 0x200e},
      {"rlm", 0x200f},
      {"Prime", 0x2033},
      {"uml", 0x00a8},
      {"auml", 0x00e4},
      {"Acirc", 0x00c2},
      {"percnt", 0x0025},
      {"ge", 0x2265},
      {"reg", 0x00ae},
      {"euml", 0x00eb},
      {"uuml", 0x00fc},
      {"forall", 0x2200},
      {"loz", 0x25ca},
      {"iuml", 0x00ef},
      {"iota", 0x03b9},
      {"agrave", 0x00e0},
      {"phi", 0x03c6},
      {"Uuml", 0x00dc},
      {"egrave", 0x00e8},
      {"alpha", 0x03b1},
      {"ugrave", 0x00f9},
      {"gt", 62},
      {"chi", 0x03c7},
      {"Euml", 0x00cb},
      {"igrave", 0x00ec},
      {"Yuml", 0x0178},
      {"aelig", 0x00e6},
      {"Ouml", 0x00d6},
      {"Ugrave", 0x00d9},
      {"darr", 0x2193},
      {"ensp", 0x2002},
      {"raquo", 0x00bb},
      {"Egrave", 0x00c8},
      {"nabla", 0x2207},
      {"Ograve", 0x00d2},
      {"hArr", 0x21d4},
      {"hellip", 0x2026},
      {"amp", 38},
      {"Iuml", 0x00cf},
      {"laquo", 0x00ab},
      {"Iota", 0x0399},
      {"macr", 0x00af},
      {"quot", 34},
      {"trade", 0x2122},
      {"prime", 0x2032},
      {"Igrave", 0x00cc},
      {"yuml", 0x00ff},
      {"sub", 0x2282},
      {"delta", 0x03b4},
      {"sube", 0x2286},
      {"OElig", 0x0152},
      {"permil", 0x2030},
      {"fnof", 0x0192},
      {"tilde", 0x02dc},
      {"atilde", 0x00e3},
      {"Ntilde", 0x00d1},
      {"oacute", 0x00f3},
      {"ang", 0x2220},
      {"rang", 0x232a},
      {"hearts", 0x2665},
      {"not", 0x00ac},
      {"oline", 0x203e},
      {"Auml", 0x00c4},
      {"aring", 0x00e5},
      {"lang", 0x2329},
      {"Otilde", 0x00d5},
      {"prop", 0x221d},
      {"radic", 0x221a},
      {"sum", 0x2211},
      {"Agrave", 0x00c0},
      {"sim", 0x223c},
      {"Alpha", 0x0391},
      {"and", 0x22a5},
      {"cedil", 0x00b8},
      {"Ccedil", 0x00c7},
      {"shy", 0x00ad},
      {"dArr", 0x21d3},
      {"ocirc", 0x00f4},
      {"brvbar", 0x00a6},
      {"empty", 0x2205},
      {"times", 0x00d7},
      {"AElig", 0x00c6},
      {"Scaron", 0x0160},
      {"upsih", 0x03d2},
      {"clubs", 0x2663},
      {"lowast", 0x2217},
      {"nsub", 0x2284},
      {"rsquo", 0x2019},
      {"rsaquo", 0x203a},
      {"szlig", 0x00df},
      {"ntilde", 0x00f1},
      {"apos", 0x0027},
      {"lsquo", 0x2018},
      {"lsaquo", 0x2039},
      {"rho", 0x03c1},
      {"emsp", 0x2003},
      {"Rho", 0x03a1},
      {"Atilde", 0x00c3},
      {"copy", 0x00a9},
      {"alefsym", 0x2135},
      {"oplus", 0x2295},
      {"notin", 0x2209},
      {"ccedil", 0x00e7},
      {"Aring", 0x00c5},
      {"minus", 0x2212},
      {"nbsp", 0x00a0},
      {"Gamma", 0x0393},
      {"rfloor", 0x230b},
      {"ouml", 0x00f6},
      {"Oslash", 0x00d8},
      {"asymp", 0x2248},
      {"lfloor", 0x230a},
      {"Sigma", 0x03a3},
      {"image", 0x2111},
      {"ograve", 0x00f2},
      {"thinsp", 0x2009},
      {"oelig", 0x0153},
      {"Dagger", 0x2021},
      {"ordf", 0x00aa},
      {"Omega", 0x03a9},
      {"scaron", 0x0161},
      {"plusmn", 0x00b1},
      {"deg", 0x00b0},
      {"thorn", 0x00fe},
      {"epsilon", 0x03b5},
      {"upsilon", 0x03c5},
      {"micro", 0x00b5},
      {"Upsilon", 0x03a5},
      {"Epsilon", 0x0395},
      {"otilde", 0x00f5},
      {"rdquo", 0x201d},
      {"prod", 0x220f},
      {"ldquo", 0x201c},
      {"divide", 0x00f7},
      {"sbquo", 0x201a},
      {"spades", 0x2660},
      {"sigma", 0x03c3},
      {"sigmaf", 0x03c2},
      {"otimes", 0x2297},
      {"cong", 0x2245},
      {"diams", 0x2666},
      {"sdot", 0x22c5},
      {"thetasym", 0x03d1},
      {"Omicron", 0x039f},
      {"pound", 0x00a3},
      {"oslash", 0x00f8},
      {"ndash", 0x2013},
      {"Lambda", 0x039b},
      {"lambda", 0x03bb},
      {"gamma", 0x03b3},
      {"omega", 0x03c9},
      {"ordm", 0x00ba},
      {"bdquo", 0x201e},
      {"dagger", 0x2020},
      {"mdash", 0x2014},
      {"omicron", 0x03bf},
      {"middot", 0x00b7}
    };

  static const short lookup[] =
    {
        -1,   -1,    0,   -1,    1,   -1,   -1,    2,
        -1,   -1,   -1,   -1,    3,    4,    5,   -1,
        -1,    6,   -1,   -1,   -1,   -1,    7,   -1,
         8,   -1,   -1,    9,   -1,   10,   -1,   -1,
        11,   -1,   -1,   -1,   -1,   12,   13,   -1,
        -1,   -1,   -1,   14,   15,   -1,   -1,   -1,
        16,   17,   -1,   -1,   -1,   18,   -1,   -1,
        -1,   19,   -1,   -1,   20,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   21,   22,   -1,
        -1,   23,   24,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   25,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        26,   27,   28,   -1,   -1,   29,   -1,   -1,
        -1,   -1,   -1,   30,   -1,   -1,   -1,   -1,
        31,   -1,   32,   -1,   -1,   -1,   -1,   -1,
        33,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   34,   35,   -1,   -1,
        36,   -1,   37,   -1,   -1,   -1,   -1,   38,
        -1,   39,   40,   41,   -1,   42,   -1,   43,
        44,   -1,   -1,   45,   46,   47,   -1,   -1,
        -1,   -1,   48,   -1,   -1,   49,   -1,   -1,
        -1,   -1,   -1,   -1,   50,   -1,   -1,   -1,
        -1,   -1,   -1,   51,   52,   -1,   53,   -1,
        -1,   -1,   -1,   54,   -1,   -1,   -1,   55,
        56,   -1,   -1,   57,   58,   -1,   -1,   59,
        -1,   -1,   -1,   -1,   60,   -1,   61,   -1,
        -1,   62,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   63,   -1,
        -1,   64,   65,   -1,   -1,   -1,   -1,   66,
        -1,   -1,   -1,   -1,   67,   -1,   -1,   68,
        -1,   69,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   70,   71,   -1,   -1,   -1,
        72,   -1,   -1,   -1,   -1,   73,   74,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   75,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   76,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   77,
        -1,   -1,   -1,   78,   79,   80,   -1,   -1,
        81,   82,   -1,   -1,   -1,   83,   84,   85,
        86,   -1,   87,   88,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   89,   -1,   90,
        -1,   -1,   -1,   -1,   -1,   -1,   91,   92,
        93,   -1,   -1,   -1,   94,   -1,   95,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   96,   -1,
        -1,   -1,   -1,   -1,   -1,   97,   -1,   -1,
        98,   -1,   -1,   99,   -1,   -1,   -1,  100,
        -1,   -1,   -1, -663, -157,   -2,   -1,   -1,
        -1,   -1,  103,   -1,   -1,  104,  105,  106,
       107,  108,  109,  110,   -1,   -1,   -1,   -1,
       111,   -1,  112,   -1,  113,  114,   -1,   -1,
        -1,   -1,  115,   -1,  116,   -1,  117,  118,
        -1,  119,   -1,   -1,   -1,  120,  121,  122,
       123,  124,   -1,  125,   -1,   -1,  126,  127,
        -1,   -1,   -1,  128,   -1,  129,   -1,   -1,
       130,   -1,   -1,   -1,   -1,  131,  132,  133,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       134,  135,   -1,   -1,  136,   -1,  137,   -1,
       138,  139,  140,   -1,   -1,   -1,  141,   -1,
        -1,   -1,   -1,  142,   -1,   -1,   -1,   -1,
       143,  144,   -1,   -1,   -1,   -1,  145,  146,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,  147,
        -1,   -1,   -1,  148,   -1,  149,   -1,   -1,
        -1,  150,  151,  152,   -1,   -1,  153,   -1,
        -1,   -1,   -1,   -1,  154,  155,   -1,   -1,
        -1,   -1,  156,   -1,   -1,   -1,   -1,  157,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,  158,  159,   -1,  160,   -1,
       161,   -1,   -1,   -1,   -1,   -1,   -1,  162,
        -1,   -1,   -1,  163,  164,   -1,   -1,   -1,
       165,   -1,  166,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  167,  168,   -1,   -1,  169,   -1,
        -1,  170,   -1,  171,   -1,   -1,   -1,   -1,
        -1,   -1,  172,   -1,   -1,  173,   -1,   -1,
        -1,   -1,   -1,   -1,  174,  175,   -1,   -1,
        -1,   -1,   -1,   -1,  176,  177,  178,  179,
        -1,   -1,   -1,  180,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,  181,   -1,   -1,
        -1,   -1,  182,  183,   -1,   -1,   -1,  184,
        -1,   -1,   -1,   -1,  185,   -1,   -1,   -1,
        -1,   -1,  186,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  187,  188,  189,   -1,   -1,   -1,
       190,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,  191,   -1,   -1,  192,  193,
       194,   -1,  195,  196,   -1,   -1,   -1,  197,
        -1,   -1,  198,   -1,   -1,  199,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  200,   -1,   -1,  201,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,  202,
       203,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,  204,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,  205,   -1,   -1,
        -1,  206,  207,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  208,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  209,   -1,  210,   -1,   -1,   -1,
       211,   -1,   -1,   -1,   -1,   -1,  212,   -1,
        -1,   -1,  213,   -1,   -1,   -1,   -1,  214,
       215,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,  216,
        -1,   -1,   -1,  217,  218,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,  219,  220,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       221,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,  222,   -1,  223,   -1,
        -1,   -1,   -1,   -1,   -1,  224,   -1,  225,
        -1,   -1,   -1,   -1,  226,   -1,   -1,  227,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,  228,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,  229,   -1,   -1,
        -1,  230,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  231,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,  232,  233,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       234,   -1,   -1,   -1,  235,   -1,   -1,   -1,
        -1,   -1,  236,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,  237,
       238,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  239,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,  240,   -1,   -1,   -1,   -1,
        -1,  241,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,  242,   -1,   -1,   -1,
       243,   -1,   -1,   -1,  244,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,  245,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,  246,   -1,   -1,   -1,  247,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,  248,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,  249,   -1,   -1,   -1,  250,
        -1,   -1,   -1,   -1,  251,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       252,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,  253,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,  254,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,  255,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,  256,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       257
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash_Entity (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist_Entity[index].name;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
                return &wordlist_Entity[index];
            }
          else if (index < -TOTAL_KEYWORDS)
            {
              register int offset = - 1 - TOTAL_KEYWORDS - index;
              register const struct entity *wordptr = &wordlist_Entity[TOTAL_KEYWORDS + lookup[offset]];
              register const struct entity *wordendptr = wordptr + -lookup[offset + 1];

              while (wordptr < wordendptr)
                {
                  register const char *s = wordptr->name;

                  if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
                    return wordptr;
                  wordptr++;
                }
            }
        }
    }
  return 0;
}

