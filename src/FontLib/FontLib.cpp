#include "FontLib.h"

std::vector<unsigned char> Font::operator[](char ch) const
{
    const unsigned char *res;
    if (ch >= '0' && ch <= '9')
        res = FONTARR[ch - '0'];
    else if (ch >= 'a' && ch <= 'z')
        res = FONTARR[ch - 'a' + 10];
    else if (ch >= 'A' && ch <= 'Z')
        res = FONTARR[ch - 'A' + 10 + 26];
    else if (ch == '.')
        res = FONTARR[62];
    else if (ch == ':')
        res = FONTARR[63];
    else if (ch == '~')
        res = FONTARR[64];
    else
        res = FONTARR[65];
    return vector<unsigned char>(res, res + 12);
}

Font fontMap;