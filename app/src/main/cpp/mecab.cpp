// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include "mecab.h"
#include <string>

const size_t kErrorBufferSize = 256;
char kErrorBuffer[kErrorBufferSize];

const char *getGlobalError() {
    return kErrorBuffer;
}

void setGlobalError(const char *str) {
    strncpy(kErrorBuffer, str, kErrorBufferSize - 1);
    kErrorBuffer[kErrorBufferSize - 1] = '\0';
}