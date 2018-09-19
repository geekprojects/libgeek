#include "thread-pthread.h"

using namespace std;
using namespace Geek;

PThreadMutex::PThreadMutex()
{
    pthread_mutex_init(&m_mutex, NULL);
}

PThreadMutex::~PThreadMutex()
{
    pthread_mutex_destroy(&m_mutex);
}

bool PThreadMutex::lock()
{
    pthread_mutex_lock(&m_mutex);
    return true;
}

void PThreadMutex::unlock()
{
    pthread_mutex_unlock(&m_mutex);
}

PThreadCondVar::PThreadCondVar()
{
    pthread_cond_init(&m_cond, NULL);
    pthread_mutex_init(&m_condMutex, NULL);
}

PThreadCondVar::~PThreadCondVar()
{
}

bool PThreadCondVar::wait()
{
    pthread_cond_wait(&m_cond, &m_condMutex);

    return true;
}

bool PThreadCondVar::signal()
{
    pthread_cond_signal(&m_cond);

    return true;
}

static void* pthreadentry(void* args)
{
    Thread* t = (Thread*)args;
    t->entry();
    return NULL;
}

PThreadThread::PThreadThread(Thread* thread)
    : ThreadImpl(thread)
{
    m_thread = thread;
}

PThreadThread::~PThreadThread()
{
}

void PThreadThread::start()
{
    pthread_create(&m_pthread, NULL, pthreadentry, m_thread);
}

void PThreadThread::wait()
{
    pthread_join(m_pthread, NULL);
}

Mutex* PThreadThread::createMutex()
{
    return new PThreadMutex();
}

CondVar* PThreadThread::createCondVar()
{
    return new PThreadCondVar();
}

