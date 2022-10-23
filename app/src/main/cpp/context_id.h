// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_ANDROID_CONTEXT_ID_H
#define MECAB_ANDROID_CONTEXT_ID_H

#include <map>
#include <string>
#include <vector>

class Param;
class Iconv;

class ContextID {
private:
    std::map<std::string, int>  left_;
    std::map<std::string, int>  right_;
    std::string                 left_bos_;
    std::string                 right_bos_;

public:
    void clear();
    bool open(const char *lfile,
              const char *rfile,
              Iconv *iconv = 0);
    int  lid(const char *l) const;
    int  rid(const char *r) const;

    size_t left_size() const { return left_.size(); }
    size_t right_size() const { return right_.size(); }

    const std::map<std::string, int>& left_ids()  const { return left_; }
    const std::map<std::string, int>& right_ids() const { return right_; }

    bool is_valid(size_t lid, size_t rid) {
        return (lid >= 0 && lid < left_size() &&
                rid >= 0 && rid < right_size());
    }
};


#endif //MECAB_ANDROID_CONTEXT_ID_H
