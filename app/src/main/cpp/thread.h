// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_ANDROID_THREAD_H
#define MECAB_ANDROID_THREAD_H

#include <pthread.h>
#include <sched.h>

#define MECAB_USE_THREAD 1
#undef atomic_add
#undef compare_and_swap
#undef yield_processor
#define atomic_add(a, b) __sync_add_and_fetch(a, b)
#define compare_and_swap(a, b, c)  __sync_val_compare_and_swap(a, b, c)
#define yield_processor() sched_yield()
#define HAVE_ATOMIC_OPS 1

class read_write_mutex {
public:
    inline void write_lock() {
        atomic_add(&write_pending_, 1);
        while (compare_and_swap(&l_, 0, kWaFlag)) {
            yield_processor();
        }
    }
    inline void read_lock() {
        while (write_pending_ > 0) {
            yield_processor();
        }
        atomic_add(&l_, kRcIncr);
        while ((l_ & kWaFlag) != 0) {
            yield_processor();
        }
    }
    inline void write_unlock() {
        atomic_add(&l_, -kWaFlag);
        atomic_add(&write_pending_, -1);
    }
    inline void read_unlock() {
        atomic_add(&l_, -kRcIncr);
    }

    read_write_mutex(): l_(0), write_pending_(0) {}

private:
    static const int kWaFlag = 0x1;
    static const int kRcIncr = 0x2;
    long l_;
    long write_pending_;
};

class scoped_writer_lock {
public:
    scoped_writer_lock(read_write_mutex *mutex) : mutex_(mutex) {
        mutex_->write_lock();
    }
    ~scoped_writer_lock() {
        mutex_->write_unlock();
    }
private:
    read_write_mutex *mutex_;
};

class scoped_reader_lock {
public:
    scoped_reader_lock(read_write_mutex *mutex) : mutex_(mutex) {
        mutex_->read_lock();
    }
    ~scoped_reader_lock() {
        mutex_->read_unlock();
    }
private:
    read_write_mutex *mutex_;
};

class thread {
private:
    pthread_t hnd;

public:
    static void* wrapper(void *ptr) {
        thread *p = static_cast<thread *>(ptr);
        p->run();
        return 0;
    }

    virtual void run() {}

    void start() {
        pthread_create(&hnd, 0, &thread::wrapper,
                   static_cast<void *>(this));
    }

    void join() {
        pthread_join(hnd, 0);
    }

    virtual ~thread() {}
};

#endif //MECAB_ANDROID_THREAD_H
