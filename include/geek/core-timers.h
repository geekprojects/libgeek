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

struct Timer
{
    TimerType type;
    uint64_t period;
    sigc::signal<void, Timer*> signal;
    void* data;

    uint64_t next;

    Timer()
    {
        type = TIMER_PERIODIC;
        period = 0;
        data = NULL;
    }

    Timer(TimerType _type, uint64_t _period)
    {
        type = _type;
        period = _period;
        data = NULL;
    }
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

    void addTimer(Timer* timer);
    void cancelTimer(Timer* timer);

    virtual bool main();
};

};
};

#endif
