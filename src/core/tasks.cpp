
#include <geek/core-tasks.h>

#include <thread>

using namespace std;
using namespace Geek::Core;

TaskExecutor::TaskExecutor()
{
    init(0);
}

TaskExecutor::TaskExecutor(int maxWorkers)
{
    init(maxWorkers);
}

void TaskExecutor::init(int maxWorkers)
{
    if (maxWorkers == 0)
    {
        int cpus = std::thread::hardware_concurrency();
        printf("TaskExecutor::init: cpus=%d\n", cpus);
        maxWorkers = cpus / 2;
        if (maxWorkers < 1)
        {
            maxWorkers = 1;
        }
    }

    m_maxWorkers = maxWorkers;
    m_tasksMutex = Thread::createMutex();
    m_queueMutex = Thread::createMutex();
    m_workersMutex = Thread::createMutex();
    m_queueEmpty = Thread::createCondVar();
}

TaskExecutor::~TaskExecutor()
{
}

bool TaskExecutor::addTask(Task* task)
{
    unsigned int size;

    m_workersMutex->lock();
    size = m_workers.size();
    m_workersMutex->unlock();

    m_tasksMutex->lock();
    m_tasks.push_back(task);
    m_tasksMutex->unlock();

    if (size < m_maxWorkers)
    {
        startTask(task);
    }
    else
    {
        m_queueMutex->lock();
        task->setState(TASK_QUEUED);
        m_queue.push_back(task);
        m_queueMutex->unlock();
    }

    return true;
}

void TaskExecutor::wait()
{
    while (true)
    {
        m_queueMutex->lock();
        bool queueEmpty = m_queue.empty();
        m_queueMutex->unlock();
        if (queueEmpty)
        {
            m_workersMutex->lock();
            bool workersEmpty = m_workers.empty();
            m_workersMutex->unlock();
            if (workersEmpty)
            {
                break;
            }
        }
        m_queueEmpty->wait();
    }
}

void TaskExecutor::startTask(Task* task)
{
    TaskWorker* taskWorker = new TaskWorker(this, task);

    m_workersMutex->lock();
    m_workers.push_back(taskWorker);
    m_workersMutex->unlock();

    taskWorker->start();
}

void TaskExecutor::taskComplete(TaskWorker* worker)
{
    m_workersMutex->lock();
    printf("TaskExecutor::taskComplete: Clearing worker...\n");

    m_tasksMutex->lock();
vector<Task*>::iterator taskIt;
for (taskIt = m_tasks.begin(); taskIt != m_tasks.end(); taskIt++)
{
if (*taskIt == worker->getTask())
{
m_tasks.erase(taskIt);
break;
}
}
    m_tasksMutex->unlock();

    // Remove the completed worker from our list
    vector<TaskWorker*>::iterator it;
    for (it = m_workers.begin(); it != m_workers.end(); it++)
    {
        if (*it == worker)
        {
            m_workers.erase(it);
            break;
        }
    }

    m_queueMutex->lock();
    if (!m_queue.empty())
    {
        printf("TaskExecutor::taskComplete: Queue is not empty! Workers size=%lu\n", m_workers.size());
        if (m_workers.size() < m_maxWorkers)
        {
            m_workersMutex->unlock();

            printf("TaskExecutor::taskComplete: Starting another task...\n");
            Task* next = m_queue.front();
            m_queue.pop_front();
            m_queueMutex->unlock();
            startTask(next);
        }
        else
        {
            m_workersMutex->unlock();
            m_queueMutex->unlock();
        }
    }
    else
    {
        printf("TaskExecutor::taskComplete: Queue is empty!\n");
        bool workersEmpty = m_workers.empty();
        m_workersMutex->unlock();
        m_queueMutex->unlock();

        if (workersEmpty)
        {
            // No other workers, either!
            printf("TaskExecutor::taskComplete: ... and no more workers!\n");
            m_queueEmpty->signal();
        }
    }
    printf("TaskExecutor::taskComplete: Done!\n");
}

unsigned int TaskExecutor::getTaskCount()
{
    unsigned int count = 0;

    m_tasksMutex->lock();
    count = m_tasks.size();
    m_tasksMutex->unlock();

    return count;
}

vector<TaskInfo> TaskExecutor::getTaskInfo()
{
    vector<TaskInfo> results;

    m_tasksMutex->lock();
    for (Task* task : m_tasks)
    {
        TaskInfo info;
        info.title = task->getTitle();
        info.state = task->getState();
        results.push_back(info);
    }
    m_tasksMutex->unlock();

    return results;
}

TaskWorker::TaskWorker(TaskExecutor* executor, Task* task)
{
    m_executor = executor;
    m_task = task;
}

TaskWorker::~TaskWorker()
{
}

bool TaskWorker::main()
{
    printf("TaskWorker::main: Running task...\n");
    m_task->setState(TASK_RUNNING);
    m_task->run();

    delete m_task;

    printf("TaskWorker::main: Task complete...\n");

    // Tell our TaskExecutor that we're done
    m_executor->taskComplete(this);
    printf("TaskWorker::main: Done!\n");

    return true;
}

