#ifndef __LIBGEEK_THREAD_H_
#define __LIBGEEK_THREAD_H_

#include <vector>
#include <stdint.h>

namespace Geek {

enum thread_state_t
{
    THREAD_INIT,
    THREAD_RUNNING,
    THREAD_COMPLETE
};

class Mutex
{
 protected:
    Mutex();

 public:
    virtual ~Mutex();

    virtual bool lock() = 0;
    virtual void unlock() = 0;
};

class CondVar
{
 protected:

 public:
    virtual ~CondVar();

    virtual bool wait();
    virtual bool wait(uint64_t timeoutms);
    virtual bool signal();
};

class Thread;
class ThreadImpl
{
 protected:
    Thread* m_thread;

    ThreadImpl(Thread* thread) { m_thread = thread; }

 public:
    ThreadImpl() {}
    virtual ~ThreadImpl() {}

    virtual void start() = 0;
    virtual void wait() = 0;

    virtual Mutex* createMutex() = 0;
    virtual CondVar* createCondVar() = 0;
};

class Thread
{
 private:
    static std::vector<Thread*> threads;

    ThreadImpl* m_impl;
    thread_state_t m_state;

    //std::vector<Object*> m_listeners;

 public:
    Thread();
    virtual ~Thread();

    void start();
    void wait();

    bool entry();
    virtual bool main();

    //void addListener(Object* w);

    thread_state_t getState() {return m_state;}
    bool isComplete() {return m_state == THREAD_COMPLETE;}

    static Mutex* createMutex();// { return m_impl->createMutex(); }
    static CondVar* createCondVar();// { return m_impl->createCondVar(); }
};

};

#endif
