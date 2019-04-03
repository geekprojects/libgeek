#ifndef __LIBGEEK_CORE_TIMERS_H_
#define __LIBGEEK_CORE_TIMERS_H_

#include <geek/core-thread.h>

#include <sigc++/sigc++.h>

namespace Geek
{
namespace Core
{

enum TimerType
{
    TIMER_ONE_SHOT,
    TIMER_PERIODIC,
};

class Timer
{
 private:
    TimerType m_type;
    uint64_t m_period;
    sigc::signal<void, Timer*> m_signal;
    void* m_data;

    bool m_active;
    uint64_t m_nextRun;

 public:

    Timer(TimerType type, uint64_t period)
    {
        m_type = type;
        m_period = period;
        m_data = NULL;
        m_active = false;
    }

    TimerType getType() { return m_type; }
    uint64_t getPeriod() { return m_period; }
    void setPeriod(uint64_t period) { m_period = period; }
    sigc::signal<void, Timer*> signal() { return m_signal; }

    void setData(void* data) { m_data = data; }
    void* getData() { return m_data; }

    void setNextRun(uint64_t next) { m_nextRun = next; } 
    uint64_t getNextRun() { return m_nextRun; }

    void setActive(bool active) { m_active = active; }
    bool isActive() { return m_active; }
};

class TimerManager : public Geek::Thread
{
 private:
    Geek::CondVar* m_condVar;
    Geek::Mutex* m_timersMutex;
    std::vector<Timer*> m_timers;

 public:
    TimerManager();
    virtual ~TimerManager();

    /**
     * Add a timer to be scheduled.
     */
    void addTimer(Timer* timer);
    void resetTimer(Timer* timer);
    void cancelTimer(Timer* timer);
    bool isScheduled(Timer* timer);

    virtual bool main();
};

};
};

#endif
