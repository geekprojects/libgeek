
#include <stdio.h>
#include <sys/time.h>

#include <cinttypes>

#include <geek/core-timers.h>

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

void timer1Func(Timer* timer)
{
    printf("timer1Func: now=%" PRId64 "\n", getTimestamp());
}

void timer2Func(Timer* timer)
{
    printf("timer2Func: now=%" PRId64 "\n", getTimestamp());
}

void timer3Func(Timer* timer)
{
    printf("timer3Func: now=%" PRId64 "\n", getTimestamp());
}

int main(int argc, char** argv)
{
    TimerManager* timerManager = new TimerManager();

    Timer* timer1 = new Timer(TIMER_PERIODIC, 1000);
    timer1->signal().connect(sigc::ptr_fun(&timer1Func));
    timerManager->addTimer(timer1);

    Timer* timer2 = new Timer(TIMER_PERIODIC, 500);
    timer2->signal().connect(sigc::ptr_fun(&timer2Func));
    timerManager->addTimer(timer2);

    Timer* timer3 = new Timer(TIMER_ONE_SHOT, 5000);
    timer3->signal().connect(sigc::ptr_fun(&timer3Func));
    timerManager->addTimer(timer3);

    timerManager->start();
    timerManager->wait();

    return 0;
}

