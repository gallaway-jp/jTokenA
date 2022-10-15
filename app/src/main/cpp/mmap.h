// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_ANDROID_MMAP_H
#define MECAB_ANDROID_MMAP_H

#include <errno.h>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "common.h"
#include "utils.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef HAVE_MMAP
#define HAVE_MMAP 1
#endif

template <class T> class Mmap {
private:
    T            *text;
    size_t       length;
    std::string  fileName;
    whatlog what_;

    int    fd;
    int    flag;

public:
    T&       operator[](size_t n)       { return *(text + n); }
    const T& operator[](size_t n) const { return *(text + n); }
    T*       begin()           { return text; }
    const T* begin()    const  { return text; }
    T*       end()           { return text + size(); }
    const T* end()    const  { return text + size(); }
    size_t size()               { return length/sizeof(T); }
    const char *what()          { return what_.str(); }
    const char *file_name()     { return fileName.c_str(); }
    size_t file_size()          { return length; }
    bool empty()                { return(length == 0); }

    bool open(const char *filename, const char *mode = "r") {
        this->close();
        struct stat st;
        fileName = std::string(filename);

        if      (std::strcmp(mode, "r") == 0)
            flag = O_RDONLY;
        else if (std::strcmp(mode, "r+") == 0)
            flag = O_RDWR;
        else
        CHECK_FALSE(false) << "unknown open mode: " << filename;

        CHECK_FALSE((fd = ::open(filename, flag | O_BINARY)) >= 0)
                    << "open failed: " << filename;

        CHECK_FALSE(::fstat(fd, &st) >= 0)
                    << "failed to get file size: " << filename;

        length = st.st_size;

        int prot = PROT_READ;
    if (flag == O_RDWR) prot |= PROT_WRITE;
    char *p;
    CHECK_FALSE((p = reinterpret_cast<char *>
                 (::mmap(0, length, prot, MAP_SHARED, fd, 0)))
                != MAP_FAILED)
        << "mmap() failed: " << filename;

    text = reinterpret_cast<T *>(p);
        ::close(fd);
        fd = -1;

        return true;
    }

    void close() {
        if (fd >= 0) {
            ::close(fd);
            fd = -1;
        }

        if (text) {
            ::munmap(reinterpret_cast<char *>(text), length);
            text = 0;
        }

        text = 0;
    }

    Mmap() : text(0), fd(-1) {}

    virtual ~Mmap() { this->close(); }
};

#endif //MECAB_ANDROID_MMAP_H
