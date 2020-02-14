#include <geek/core-thread.h>

#include "thread-pthread.h"

using namespace std;
using namespace Geek;

Mutex::Mutex()
{
}

Mutex::~Mutex()
{
}

CondVar::~CondVar()
{
}

bool CondVar::wait()
{
    return wait(0);
}

bool CondVar::wait(uint64_t timeoutms)
{
    return false;
}

bool CondVar::signal()
{
    return false;
}


Thread::Thread()
{
    m_state = THREAD_INIT;
    m_impl = new PThreadThread(this);
}

Thread::~Thread()
{
    delete m_impl;
}

#if 0
void Thread::addListener(FObject* w)
{
    m_listeners.push_back(w);
}
#endif

void Thread::start()
{
    m_impl->start();
}

void Thread::wait()
{
    m_impl->wait();
}

bool Thread::entry()
{
    bool res;
    m_state = THREAD_RUNNING;
    res = main();
    m_state = THREAD_COMPLETE;

#if 0
    FThreadEvent* te = new FThreadEvent();
    te->type = THREAD_EVENT;

    vector<FObject*>::iterator it;
    for (it = m_listeners.begin(); it != m_listeners.end(); it++)
    {
        (*it)->postEvent(te);
    }
    delete te;
#endif

    return res;
}

bool Thread::main()
{
    return false;
}


Mutex* Thread::createMutex()
{
    return new PThreadMutex();
}

CondVar* Thread::createCondVar()
{
    return new PThreadCondVar();
}

