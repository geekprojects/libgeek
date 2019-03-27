
#include <geek/core-timers.h>

#include <sys/time.h>

using namespace std;
using namespace Geek;
using namespace Geek::Core;

static uint64_t getTimestamp()
{
    timeval tv;
    gettimeofday(&tv, NULL);

    uint64_t millis;
    millis = tv.tv_sec * 1000l;
    millis += tv.tv_usec / 1000l;

    return millis;
}

TimerManager::TimerManager()
{
    m_condVar = Thread::createCondVar();
    m_timersMutex = Thread::createMutex();
}

TimerManager::~TimerManager()
{
}

void TimerManager::addTimer(Timer* timer)
{
    uint64_t now = getTimestamp();
    timer->setNextRun(now + timer->getPeriod());

    m_timersMutex->lock();
    m_timers.push_back(timer);
    m_timersMutex->unlock();

    m_condVar->signal();
}

void TimerManager::resetTimer(Timer* timer)
{
    m_timersMutex->lock();
    uint64_t now = getTimestamp();
    timer->setNextRun(now + timer->getPeriod());
    m_timersMutex->unlock();

    m_condVar->signal();
}


void TimerManager::cancelTimer(Timer* timer)
{
    m_timersMutex->lock();
    vector<Timer*>::iterator it;
    for (it = m_timers.begin(); it != m_timers.end(); it++)
    {
        if (timer == *it)
        {
            m_timers.erase(it);
            break;
        }
    }
    m_timersMutex->unlock();
}

bool TimerManager::main()
{
    while (true)
    {
        m_timersMutex->lock();
        vector<Timer*>::iterator it;
        vector<vector<Timer*>::iterator> removeIts;

        uint64_t next = getTimestamp() * (60 * 1000);
        for (it = m_timers.begin(); it != m_timers.end(); it++)
        {
            Timer* timer = *it;
            uint64_t now = getTimestamp();
            if (timer->getNextRun() <= now)
            {
                // Fire!
                timer->signal().emit(timer);
                if (timer->getType() == TIMER_PERIODIC)
                {
                    timer->setNextRun(now + timer->getPeriod());
                }
                else
                {
                    removeIts.push_back(it);
                    continue; // Don't process next run time
                }
            }

            // Next run is imminent?
            if (timer->getNextRun() < next)
            {
                next = timer->getNextRun();
            }
        }

        // Clean up finished ONE SHOT timers
        for (vector<Timer*>::iterator removeIt : removeIts)
        {
            m_timers.erase(removeIt);
        }
        m_timersMutex->unlock();

        uint64_t now = getTimestamp();
        uint64_t wait = next - now;
        if (wait > 0)
        {
#if 0
            printf("TimerManager::main: Waiting %llu ms\n", wait);
#endif
            m_condVar->wait(wait);
        }
    }

    return false;
}

