#ifndef __GEEK_CORE_TASKS_H_
#define __GEEK_CORE_TASKS_H_

#include <deque>
#include <string>

#include <geek/core-thread.h>

#include <sigc++/sigc++.h>

namespace Geek
{
namespace Core
{

class TaskExecutor;

enum TaskState
{
    TASK_CREATED,
    TASK_QUEUED,
    TASK_RUNNING,
};

class Process
{
 protected:

 public:
    Process();
    ~Process();
};

class Task
{
 protected:
    std::wstring m_title;
    TaskState m_state;

    sigc::signal<void, Task*> m_startedSignal;
    sigc::signal<void, Task*> m_completeSignal;

    void setTitle(std::wstring title) { m_title = title; }

 public:
    Task()
    {
        m_title = L"";
        m_state = TASK_CREATED;
    }

    Task(std::wstring title)
    {
        m_title = title;
        m_state = TASK_CREATED;
    }

    virtual ~Task() {}

    virtual void run() {}

    std::wstring getTitle() { return m_title; }
    TaskState getState() { return m_state; }
    void setState(TaskState state) { m_state = state; }

    sigc::signal<void, Task*> startedSignal() { return m_startedSignal; }
    sigc::signal<void, Task*> completeSignal() { return m_completeSignal; }
};

class TaskWorker : public Thread
{
 private:
    TaskExecutor* m_executor;
    Task* m_task;

 public:
    TaskWorker(TaskExecutor* executor, Task* task);
    ~TaskWorker();

    Task* getTask() { return m_task; }

    virtual bool main();
};

struct TaskInfo
{
    std::wstring title;
    TaskState state;
};

class TaskExecutor
{
 private:
    Mutex* m_tasksMutex;
    std::vector<Task*> m_tasks;

    Mutex* m_queueMutex;
    std::deque<Task*> m_queue;
    CondVar* m_queueEmpty;

    Mutex* m_workersMutex;
    std::vector<TaskWorker*> m_workers;

    unsigned int m_maxWorkers;

    void init(int maxWorkers);
    void startTask(Task* task);

 public:
    TaskExecutor();
    TaskExecutor(int maxWorkers);
    ~TaskExecutor();

    bool addTask(Task* task);

    void taskComplete(TaskWorker* worker);

    void wait();

    unsigned int getTaskCount();
    std::vector<TaskInfo> getTaskInfo();
};

};
};

#endif
