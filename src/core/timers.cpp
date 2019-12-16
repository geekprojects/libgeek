
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
    timer->setActive(true);

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
    timer->setActive(false);

    m_timersMutex->lock();
    vector<Timer*>::iterator it;
    for (it = m_timers.begin(); it != m_timers.end(); ++it)
    {
        if (timer == *it)
        {
            m_timers.erase(it);
            break;
        }
    }
    m_timersMutex->unlock();
}

Timer* TimerManager::findTimer(std::string id)
{
    Timer* result = NULL;
    m_timersMutex->lock();
    for (Timer* timer : m_timers)
    {
        if (timer->getId() == id)
        {
            result = timer;
            break;
        }
    }
    m_timersMutex->unlock();
    return result;
}

class TimerThread : public Thread
{
 private:
    Timer* m_timer;

 public:
    TimerThread(Timer* timer)
    {
        m_timer = timer;
    }

    virtual ~TimerThread()
    {
    }

    virtual bool main()
    {
        m_timer->signal().emit(m_timer);
        return true;
    }
};

bool TimerManager::main()
{
    while (true)
    {
        m_timersMutex->lock();
        vector<Timer*>::iterator it;
        vector<Timer*> removeTimers;
        vector<Timer*> emitSignals;

        uint64_t next = getTimestamp() + (60 * 1000);
        for (it = m_timers.begin(); it != m_timers.end(); ++it)
        {
            Timer* timer = *it;
            uint64_t now = getTimestamp();
            bool remove = false;
            if (timer->getNextRun() <= now)
            {
                // Fire!
                emitSignals.push_back(timer);

                if (timer->getType() == TIMER_PERIODIC)
                {
                    timer->setNextRun(now + timer->getPeriod());
                }
                else
                {
                    timer->setActive(false);
                    removeTimers.push_back(timer);
                    remove = true;
                    //continue; // Don't process next run time
                }
            }

            // Next run is imminent?
            if (!remove && timer->getNextRun() < next)
            {
                next = timer->getNextRun();
            }
        }

        // Clean up finished ONE SHOT timers
        for (Timer* timer : removeTimers)
        {
            vector<Timer*>::iterator removeIt;
            for (removeIt = m_timers.begin(); removeIt != m_timers.end(); ++removeIt)
            {
                if (*removeIt == timer)
                {
                    m_timers.erase(removeIt);
                    break;
                }
            }
        }
        m_timersMutex->unlock();

        for (Timer* timer : emitSignals)
        {
#if 0
            TimerThread* thread = new TimerThread(timer);
            thread->start();
#else
            timer->signal().emit(timer);
            if (timer->getType() == TIMER_ONE_SHOT)
            {
                //delete timer;
            }
#endif
        }

        uint64_t now = getTimestamp();
        uint64_t wait = next - now;

        if (emitSignals.empty() && wait > 0)
        {
#if 0
            printf("TimerManager::main: Waiting %llu ms\n", wait);
#endif
            m_condVar->wait(wait);
        }
    }
}

