
#include <geek/core-tasks.h>
#include <geek/core-random.h>

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <wchar.h>

using namespace std;
using namespace Geek::Core;

class TestTask : public Task
{
    int m_i;
    int m_time;

 public:
    TestTask(wstring title, int i, int time) : Task(title)
    {
        m_i = i;
        m_time = time;
    }

    ~TestTask()
    {
    }

    void run()
    {
        printf("TestTask::run: Starting: m_i=%d, sleeping for %d seconds\n", m_i, m_time);
        sleep(m_time);
        printf("TestTask::run: Done!: m_i=%d\n", m_i);
    }
};

int main(int argc, char** argv)
{
    Random r;
    int run;

    int tasksPerRun = 5;

    for (run = 0; run < 3; run++)
    {
        int workers = 1;
        if (run > 0)
        {
            workers = run * 5;
        }
        printf("tests: Starting run %d: workers=%d\n", run, workers);
        TaskExecutor executor(workers);

        int i;
        for (i = 0; i < tasksPerRun; i++)
        {
            wchar_t title[100];
            swprintf(title, 100, L"Test Task %d-%d", run, i);
            executor.addTask(new TestTask(wstring(title), (run * tasksPerRun) + i, r.range(1, 3)));
        }

        vector<TaskInfo> tasks = executor.getTaskInfo();
        for (TaskInfo info : tasks)
        {
            printf("%ls: %d\n", info.title.c_str(), info.state);
        }

        executor.wait();
    }
    return 0;
}

