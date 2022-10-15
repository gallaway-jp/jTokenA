//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <fstream>
#include <climits>
#include "connector.h"
#include "context_id.h"
#include "char_property.h"
#include "common.h"
#include "dictionary.h"
#include "dictionary_rewriter.h"
#include "feature_index.h"
#include "iconv_utils.h"
#include "mmap.h"
#include "param.h"
#include "scoped_ptr.h"
#include "utils.h"
#include "writer.h"

const unsigned int DictionaryMagicID = 0xef718f77u;

int toInt(const char *str) {
    if (!str || std::strlen(str) == 0) {
        return INT_MAX;
    }
    return std::atoi(str);
}

int calcCost(const std::string &w, const std::string &feature,
             int factor,
             DecoderFeatureIndex *fi, DictionaryRewriter *rewriter,
             CharProperty *property) {
    CHECK_DIE(fi);
    CHECK_DIE(rewriter);
    CHECK_DIE(property);

    LearnerPath path;
    LearnerNode rnode;
    LearnerNode lnode;
    rnode.stat  = lnode.stat = MECAB_NOR_NODE;
    rnode.rpath = &path;
    lnode.lpath = &path;
    path.lnode  = &lnode;
    path.rnode  = &rnode;

    size_t mblen = 0;
    const CharInfo cinfo = property->getCharInfo(w.c_str(),
                                                 w.c_str() + w.size(),
                                                 &mblen);
    path.rnode->char_type = cinfo.default_type;
    std::string ufeature, lfeature, rfeature;
    rewriter->rewrite2(feature, &ufeature, &lfeature, &rfeature);
    fi->buildUnigramFeature(&path, ufeature.c_str());
    fi->calcCost(&rnode);
    return tocost(rnode.wcost, factor);
}

int progress_bar_darts(size_t current, size_t total) {
    return progress_bar("emitting double-array", current, total);
}

template <typename T1, typename T2>
struct pair_1st_cmp: public std::binary_function<bool, T1, T2> {
    bool operator()(const std::pair<T1, T2> &x1,
                    const std::pair<T1, T2> &x2)  {
        return x1.first < x2.first;
    }
};

bool Dictionary::open(const char *file, const char *mode) {
    close();
    filename_.assign(file);
    CHECK_FALSE(dmmap_->open(file, mode))
                << "no such file or directory: " << file;

    CHECK_FALSE(dmmap_->size() >= 100)
                << "dictionary file is broken: " << file;

    const char *ptr = dmmap_->begin();

    unsigned int dsize;
    unsigned int tsize;
    unsigned int fsize;
    unsigned int magic;
    unsigned int dummy;

    read_static<unsigned int>(&ptr, magic);
    CHECK_FALSE((magic ^ DictionaryMagicID) == dmmap_->size())
                << "dictionary file is broken: " << file;

    read_static<unsigned int>(&ptr, version_);
    CHECK_FALSE(version_ == DIC_VERSION)
                << "incompatible version: " << version_;

    read_static<unsigned int>(&ptr, type_);
    read_static<unsigned int>(&ptr, lexsize_);
    read_static<unsigned int>(&ptr, lsize_);
    read_static<unsigned int>(&ptr, rsize_);
    read_static<unsigned int>(&ptr, dsize);
    read_static<unsigned int>(&ptr, tsize);
    read_static<unsigned int>(&ptr, fsize);
    read_static<unsigned int>(&ptr, dummy);

    charset_ = ptr;
    ptr += 32;
    da_.set_array(reinterpret_cast<void *>(const_cast<char*>(ptr)));

    ptr += dsize;

    token_ = reinterpret_cast<const Token *>(ptr);
    ptr += tsize;

    feature_ = ptr;
    ptr += fsize;

    CHECK_FALSE(ptr == dmmap_->end())
                << "dictionary file is broken: " << file;

    return true;
}

void Dictionary::close() {
    dmmap_->close();
}

#define DCONF(file) create_filename(dicdir, std::string(file));

bool Dictionary::assignUserDictionaryCosts(
        const Param &param,
        const std::vector<std::string> &dics,
        const char *output) {
    Connector matrix;
    DictionaryRewriter rewriter;
    DecoderFeatureIndex fi;
    ContextID cid;
    CharProperty property;

    const std::string dicdir = param.get<std::string>("dicdir");

    const std::string matrix_file     = DCONF(MATRIX_DEF_FILE);
    const std::string matrix_bin_file = DCONF(MATRIX_FILE);
    const std::string left_id_file    = DCONF(LEFT_ID_FILE);
    const std::string right_id_file   = DCONF(RIGHT_ID_FILE);
    const std::string rewrite_file    = DCONF(REWRITE_FILE);

    const std::string from = param.get<std::string>("dictionary-charset");

    const int factor = param.get<int>("cost-factor");
    CHECK_DIE(factor > 0)   << "cost factor needs to be positive value";

    std::string config_charset = param.get<std::string>("config-charset");
    if (config_charset.empty()) {
        config_charset = from;
    }

    CHECK_DIE(!from.empty()) << "input dictionary charset is empty";

    Iconv config_iconv;
    CHECK_DIE(config_iconv.open(config_charset.c_str(), from.c_str()))
    << "iconv_open() failed with from=" << config_charset << " to=" << from;

    rewriter.open(rewrite_file.c_str(), &config_iconv);
    CHECK_DIE(fi.open(param)) << "cannot open feature index";

    CHECK_DIE(property.open(param));
    property.set_charset(from.c_str());

    if (!matrix.openText(matrix_file.c_str()) &&
        !matrix.open(matrix_bin_file.c_str())) {
        matrix.set_left_size(1);
        matrix.set_right_size(1);
    }

    cid.open(left_id_file.c_str(),
             right_id_file.c_str(), &config_iconv);
    CHECK_DIE(cid.left_size()  == matrix.left_size() &&
              cid.right_size() == matrix.right_size())
    << "Context ID files("
    << left_id_file
    << " or "
    << right_id_file << " may be broken: "
    << cid.left_size() << " " << matrix.left_size() << " "
    << cid.right_size() << " " << matrix.right_size();

    std::ofstream ofs(output);
    CHECK_DIE(ofs) << "permission denied: " << output;

    for (size_t i = 0; i < dics.size(); ++i) {
        std::ifstream ifs(WPATH(dics[i].c_str()));
        CHECK_DIE(ifs) << "no such file or directory: " << dics[i];
        std::cout << "reading " << dics[i] << " ... ";
        scoped_fixed_array<char, BUF_SIZE> line;
        while (ifs.getline(line.get(), line.size())) {
            char *col[8];
            const size_t n = tokenizeCSV(line.get(), col, 5);
            CHECK_DIE(n == 5) << "format error: " << line.get();
            std::string w = col[0];
            const std::string feature = col[4];
            const int cost = calcCost(w, feature, factor,
                                      &fi, &rewriter, &property);
            std::string ufeature, lfeature, rfeature;
            CHECK_DIE(rewriter.rewrite(feature, &ufeature, &lfeature, &rfeature))
            << "rewrite failed: " << feature;
            const int lid = cid.lid(lfeature.c_str());
            const int rid = cid.rid(rfeature.c_str());
            CHECK_DIE(lid >= 0 && rid >= 0 && matrix.is_valid(lid, rid))
            << "invalid ids are found lid=" << lid << " rid=" << rid;
            escape_csv_element(&w);
            ofs << w << ',' << lid << ',' << rid << ','
                << cost << ',' << feature << '\n';
        }
    }

    return true;
}