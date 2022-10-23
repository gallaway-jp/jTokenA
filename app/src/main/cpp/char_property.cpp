// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <fstream>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include "char_property.h"
#include "common.h"
#include "mmap.h"
#include "param.h"
#include "utils.h"

struct Range {
    int low;
    int high;
    std::vector<std::string> c;
};

int atohex(const char *s) {
    int n = 0;

    CHECK_DIE(std::strlen(s) >= 3
              && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
    << "no hex value: " << s;

    const char *p = s;
    s += 2;
    while (*s) {
        int r = 0;
        if (*s >= '0' && *s <= '9')
            r = *s - '0';
        else if (*s >= 'A' && *s <= 'F')
            r = *s - 'A' + 10;
        else if (*s >= 'a' && *s <= 'f')
            r = *s - 'a' + 10;
        else
            CHECK_DIE(false) << "no hex value: " << p;

        n = 16 * n + r;
        s++;
    }

    return n;
}

CharInfo encode(const std::vector<std::string> &c,
                std::map<std::string, CharInfo> *category) {
    CHECK_DIE(c.size()) << "category size is empty";

    std::map<std::string, CharInfo>::const_iterator it = category->find(c[0]);
    CHECK_DIE(it != category->end())
    << "category [" << c[0] << "] is undefined";

    CharInfo base = it->second;
    for (size_t i = 0; i < c.size(); ++i) {
        std::map<std::string, CharInfo>::const_iterator it =
                category->find(c[i]);
        CHECK_DIE(it != category->end())
        << "category [" << c[i] << "] is undefined";
        base.type += (1 << it->second.default_type);
    }

    return base;
}

bool CharProperty::open(const Param &param) {
    const std::string prefix   = param.get<std::string>("dicdir");
    const std::string filename = create_filename(prefix, CHAR_PROPERTY_FILE);
    return open(filename.c_str());
}

bool CharProperty::open2(const Param &param) {
    const std::string prefix   = param.get<std::string>("dicdir");
    const std::string filename = create_filename(prefix, CHAR_PROPERTY_FILE);
    return open2(filename.c_str(), param.env, param.jAssetManager);
}

bool CharProperty::open(const char *filename) {
    std::ostringstream error;
    CHECK_FALSE(cmmap_->open(filename, "r"));

    const char *ptr = cmmap_->begin();
    unsigned int csize;
    read_static<unsigned int>(&ptr, csize);

    size_t fsize = sizeof(unsigned int) +
                   (32 * csize) + sizeof(unsigned int) * 0xffff;

    CHECK_FALSE(fsize == cmmap_->size())
                << "invalid file size: " << filename;

    clist_.clear();
    for (unsigned int i = 0; i < csize; ++i) {
        const char *s = read_ptr(&ptr, 32);
        clist_.push_back(s);
    }

    map_ = reinterpret_cast<const CharInfo *>(ptr);

    return true;
}

bool CharProperty::open2(const char *filename, void *env, void *jAssetManager) {
    std::ostringstream error;
    CHECK_FALSE(cmmap_->open2(filename, env, jAssetManager));

    const char *ptr = cmmap_->begin();
    unsigned int csize;
    read_static<unsigned int>(&ptr, csize);

    size_t fsize = sizeof(unsigned int) +
                   (32 * csize) + sizeof(unsigned int) * 0xffff;

    CHECK_FALSE(fsize == cmmap_->size())
                << "invalid file size: " << filename;

    clist_.clear();
    for (unsigned int i = 0; i < csize; ++i) {
        const char *s = read_ptr(&ptr, 32);
        clist_.push_back(s);
    }

    map_ = reinterpret_cast<const CharInfo *>(ptr);

    return true;
}

void CharProperty::close() {
    cmmap_->close();
}

size_t CharProperty::size() const { return clist_.size(); }

const char *CharProperty::name(size_t i) const {
    return const_cast<const char*>(clist_[i]);
}

// this function must be rewritten.
void CharProperty::set_charset(const char *ct) {
    charset_ = decode_charset(ct);
}

int CharProperty::id(const char *key) const {
    for (int i = 0; i < static_cast<long>(clist_.size()); ++i) {
        if (std::strcmp(key, clist_[i]) == 0) {
            return i;
        }
    }
    return -1;
}