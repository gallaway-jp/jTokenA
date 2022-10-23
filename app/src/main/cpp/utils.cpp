//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <stdint.h>
#include "common.h"
#include "mecab.h"
#include "param.h"
#include "utils.h"

int decode_charset(const char *charset) {
    std::string tmp = charset;
    toLower(&tmp);
    if (tmp == "sjis"  || tmp == "shift-jis" ||
        tmp == "shift_jis" || tmp == "cp932")
        return CP932;
    else if (tmp == "euc"   || tmp == "euc_jp" ||
             tmp == "euc-jp")
        return EUC_JP;
    else if (tmp == "utf8" || tmp == "utf_8" ||
             tmp == "utf-8")
        return UTF8;
    else if (tmp == "utf16" || tmp == "utf_16" ||
             tmp == "utf-16")
        return UTF16;
    else if (tmp == "utf16be" || tmp == "utf_16be" ||
             tmp == "utf-16be")
        return UTF16BE;
    else if (tmp == "utf16le" || tmp == "utf_16le" ||
             tmp == "utf-16le")
        return UTF16LE;
    else if (tmp == "ascii")
        return ASCII;

    return UTF8;  // default is UTF8
}

std::string create_filename(const std::string &path,
                            const std::string &file) {
    std::string s = path;
    if (s.size() && s[s.size()-1] != '/') s += '/';
    s += file;
    return s;
}

void remove_filename(std::string *s) {
    int len = static_cast<int>(s->size()) - 1;
    bool ok = false;
    for (; len >= 0; --len) {
        if ((*s)[len] == '/')  {
            ok = true;
            break;
        }
    }
    if (ok)
        *s = s->substr(0, len);
    else
        *s = ".";
}

void replace_string(std::string *s,
                    const std::string &src,
                    const std::string &dst) {
    const std::string::size_type pos = s->find(src);
    if (pos != std::string::npos) {
        s->replace(pos, src.size(), dst);
    }
}

bool toLower(std::string *s) {
    for (size_t i = 0; i < s->size(); ++i) {
        char c = (*s)[i];
        if ((c >= 'A') && (c <= 'Z')) {
            c += 'a' - 'A';
            (*s)[i] = c;
        }
    }
    return true;
}

int load_request_type(const Param &param) {
    int request_type = MECAB_ONE_BEST;

    if (param.get<bool>("allocate-sentence")) {
        request_type |= MECAB_ALLOCATE_SENTENCE;
    }

    if (param.get<bool>("partial")) {
        request_type |= MECAB_PARTIAL;
    }

    if (param.get<bool>("all-morphs")) {
        request_type |= MECAB_ALL_MORPHS;
    }

    if (param.get<bool>("marginal")) {
        request_type |= MECAB_MARGINAL_PROB;
    }

    const int nbest = param.get<int>("nbest");
    if (nbest >= 2) {
        request_type |= MECAB_NBEST;
    }

    // DEPRECATED:
    const int lattice_level = param.get<int>("lattice-level");
    if (lattice_level >= 1) {
        request_type |= MECAB_NBEST;
    }

    if (lattice_level >= 2) {
        request_type |= MECAB_MARGINAL_PROB;
    }

    return request_type;
}

bool load_dictionary_resource(Param *param) {
    std::string rcfile = param->get<std::string>("rcfile");

    std::string dicdir = param->get<std::string>("dicdir");
    if (dicdir.empty()) {
        dicdir = ".";  // current
    }
    remove_filename(&rcfile);
    replace_string(&dicdir, "$(rcpath)", rcfile);
    param->set<std::string>("dicdir", dicdir, true);
    dicdir = create_filename(dicdir, DICRC);

    if (!param->load(dicdir.c_str())) {
        return false;
    }

    return true;
}

bool load_dictionary_resource2(Param *param) {
    std::string rcfile = param->get<std::string>("rcfile");

    std::string dicdir = param->get<std::string>("dicdir");
    if (dicdir.empty()) {
        dicdir = ".";  // current
    }
    remove_filename(&rcfile);
    replace_string(&dicdir, "$(rcpath)", rcfile);
    param->set<std::string>("dicdir", dicdir, true);
    dicdir = create_filename(dicdir, DICRC);

    if (!param->load2(dicdir.c_str(), param->env, param->jAssetManager)) {
        return false;
    }

    return true;
}