// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_ANDROID_SCOPED_PTR_H
#define MECAB_ANDROID_SCOPED_PTR_H

#include <cstring>
#include <string>

template<class T> class scoped_ptr {
private:
    T * ptr_;
    scoped_ptr(scoped_ptr const &);
    scoped_ptr & operator= (scoped_ptr const &);

public:
    explicit scoped_ptr(T * p = 0): ptr_(p) {}
    virtual ~scoped_ptr() { delete ptr_; }
    void reset(T * p = 0) {
        delete ptr_;
        ptr_ = p;
    }
    T & operator*() const   { return *ptr_; }
    T * operator->() const  { return ptr_;  }
    T * get() const         { return ptr_;  }
};

template<class T> class scoped_array {
private:
    T * ptr_;
    scoped_array(scoped_array const &);
    scoped_array & operator= (scoped_array const &);

public:
    explicit scoped_array(T * p = 0): ptr_(p) {}
    virtual ~scoped_array() { delete [] ptr_; }
    void reset(T * p = 0) {
        delete [] ptr_;
        ptr_ = p;
    }
    T & operator*() const   { return *ptr_; }
    T * operator->() const  { return ptr_;  }
    T * get() const         { return ptr_;  }
    T & operator[](size_t i) const   { return ptr_[i]; }
};

template<class T, int N> class scoped_fixed_array {
private:
    T * ptr_;
    size_t size_;
    scoped_fixed_array(scoped_fixed_array const &);
    scoped_fixed_array & operator= (scoped_fixed_array const &);

public:
    explicit scoped_fixed_array()
            : ptr_(new T[N]), size_(N) {}
    virtual ~scoped_fixed_array() { delete [] ptr_; }
    size_t size() const { return size_; }
    T & operator*() const   { return *ptr_; }
    T * operator->() const  { return ptr_;  }
    T * get() const         { return ptr_;  }
    T & operator[](size_t i) const   { return ptr_[i]; }
};

class scoped_string: public scoped_array<char> {
public:
    explicit scoped_string() { reset_string(""); }

    void reset_string(const std::string &str) {
        char *p = new char[str.size() + 1];
        std::strcpy(p, str.c_str());
        reset(p);
    }

    void reset_string(const char *str) {
        char *p = new char[std::strlen(str) + 1];
        std::strcpy(p, str);
        reset(p);
    }
};

#endif //MECAB_ANDROID_SCOPED_PTR_H
