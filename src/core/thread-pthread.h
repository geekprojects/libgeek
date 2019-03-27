#ifndef __LIBGEEK_THREAD_PTHREAD_H_
#define __LIBGEEK_THREAD_PTHREAD_H_

#include <geek/core-thread.h>

#include <pthread.h>

namespace Geek {

class PThreadMutex : public Geek::Mutex
{
 private:
    pthread_mutex_t m_mutex;

 public:
    PThreadMutex();
    virtual ~PThreadMutex();

    bool lock();
    void unlock();
};

class PThreadCondVar : public Geek::CondVar
{
 private:
    pthread_cond_t m_cond;
    pthread_mutex_t m_condMutex;

 public:
    PThreadCondVar();
    virtual ~PThreadCondVar();

    virtual bool wait(uint64_t timeoutms);
    virtual bool signal();
};

class PThreadThread : public Geek::ThreadImpl
{
 private:
    pthread_t m_pthread;

 public:
    PThreadThread(Geek::Thread* thread);
    virtual ~PThreadThread();

    void start();
    void wait();

    Geek::Mutex* createMutex();
    Geek::CondVar* createCondVar();
};

};

#endif
