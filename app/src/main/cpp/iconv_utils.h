// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_ANDROID_ICONV_UTILS_H
#define MECAB_ANDROID_ICONV_UTILS_H

#include <iconv.h>

class Iconv {
private:
    iconv_t ic_;

public:
    explicit Iconv();
    virtual ~Iconv();
    bool open(const char *from, const char *to);
    bool convert(std::string *);
};

#endif //MECAB_ANDROID_ICONV_UTILS_H
