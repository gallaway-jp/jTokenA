// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <fstream>
#include "context_id.h"
#include "iconv_utils.h"
#include "utils.h"

bool open_map(const char *filename,
              std::map<std::string, int> *cmap,
              Iconv *iconv) {
    std::ifstream ifs(WPATH(filename));
    CHECK_DIE(ifs) << "no such file or directory: " << filename;
    cmap->clear();
    char *col[2];
    std::string line;
    while (std::getline(ifs, line)) {
        CHECK_DIE(2 == tokenize2(const_cast<char *>(line.c_str()),
                                 " \t", col, 2))
        << "format error: " << line;
        std::string pos = col[1];
        if (iconv) {
            iconv->convert(&pos);
        }
        cmap->insert(std::pair<std::string, int>
                             (pos, std::atoi(col[0])));
    }
    return true;
}

void ContextID::clear() {
    left_.clear();
    right_.clear();
    left_bos_.clear();
    right_bos_.clear();
}

bool ContextID::open(const char *lfile,
                     const char *rfile,
                     Iconv *iconv) {
    return (::open_map(lfile, &left_, iconv) &&
            ::open_map(rfile, &right_, iconv));
}
