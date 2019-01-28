#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream> //for log

#include "logger.hpp"
#include "routines.hpp"
#include "thread_pool.hpp"

namespace ilrd
{

ThreadPool::ThreadPool(int numThreads_)
    : m_numThreadsShouldExist(numThreads_), m_tasks(), m_threads()
{
}

void ThreadPool::Add(const TaskFunc& taskFunc_, Priority p_)
{
    // Create Task with func and priority, and insert it to m_tasks.
    AddTaskImp(Task(taskFunc_, p_));
}

void ThreadPool::SetNum(int numThreads_)
{
    if (!IsRunning())
    {
        m_numThreadsShouldExist =
            numThreads_; // If not running, no atomic operation needed.
        return;
    }
    // Else, if running,
    int threadsDifference = numThreads_ - m_numThreadsShouldExist.load();
    bool shouldCleanThreads = threadsDifference < 0;
    // If new threads needed, create them and insert to m_threads.
    while (threadsDifference > 0)
    {
        AddThreadImp();
        --threadsDifference;
    }
    // Else, Add poisonous apples.
    while (threadsDifference < 0)
    {
        AddTaskImp(Task::CreatePoisonousTask());
        ++threadsDifference;
    }
    if (shouldCleanThreads)
    {
        InsertCleanupTaskImp();
    }

    m_numThreadsShouldExist.store(
        numThreads_); // Even if the threads haven't eaten the apples yet. (So
                      // another SetNum won't add unnecessary apples.)
}

bool ThreadPool::IsRunning() const
{
    boost::unique_lock<boost::mutex> lock(m_threadsMutex);
    return !m_threads.empty();
}

void ThreadPool::Start()
{
    assert(m_threads.empty());

    int numThreadsShouldCreate(m_numThreadsShouldExist.load());
    for (int i = 0; i < numThreadsShouldCreate; ++i)
    {
        AddThreadImp();
    }
}

bool ThreadPool::Stop(const boost::chrono::steady_clock::duration& duration_)
{
    using namespace boost::chrono;
    time_point<steady_clock> deadLine(steady_clock::now() + duration_);

    SetNum(0);

    CleanThreadsIfImp(
        boost::bind(&ThreadPool::FinishedExecutionUntilImp, _1, deadLine));

    while (steady_clock::now() < deadLine && IsRunning())
    {
        boost::this_thread::yield();
    }

    return !IsRunning();
}

ThreadPool::~ThreadPool()
{
    // Try join the threads in m_threads for 0 duration.
    Stop(boost::chrono::steady_clock::duration::min());
    // Cancel what's left.
    boost::unique_lock<boost::mutex> lock(m_threadsMutex);
    std::for_each(m_threads.begin(), m_threads.end(), CancelJoinThreadImp);
}

void ThreadPool::AddTaskImp(const Task& task_) { m_tasks.Enqueue(task_); }

void ThreadPool::AddThreadImp()
{
    ThreadPtr t(new boost::thread(ThreadActionImp, this));
    boost::unique_lock<boost::mutex> lock(m_threadsMutex);

    m_threads.insert(std::make_pair(t->get_id(), std::make_pair(t, false)));
}

void ThreadPool::InsertCleanupTaskImp()
{
    AddTaskImp(Task(boost::bind(&ThreadPool::CleanupTaskImp, this),
                    CLEAN_TASK_PRIORITY));
}

bool ThreadPool::FinishedExecutionForImp(
    std::pair<boost::thread::id, std::pair<ThreadPtr, bool> > threadPair_,
    boost::chrono::steady_clock::duration duration_)
{
    return (boost::this_thread::get_id() != threadPair_.first) &&
           (threadPair_.second.second);
}

bool ThreadPool::FinishedExecutionUntilImp(
    std::pair<boost::thread::id, std::pair<ThreadPtr, bool> > threadPair_,
    boost::chrono::steady_clock::time_point deadLine_)
{
    return threadPair_.second.second;
}

void ThreadPool::CleanThreadsIfImp(
    boost::function<
        bool(std::pair<boost::thread::id, std::pair<ThreadPtr, bool> >)>
        pred_)
{
    Log("Cleaning thread map...");

    boost::unique_lock<boost::mutex> lock(m_threadsMutex);
    for (std::map<boost::thread::id, std::pair<ThreadPtr, bool> >::iterator i(
             m_threads.begin());
         i != m_threads.end();)
    {
        if (pred_(*i))
        {
            std::stringstream str;
            str << "Thread " << boost::this_thread::get_id()
                << " cleaning thread " << i->first;
            Log(str.str());

            i->second.first->join();
            m_threads.erase(i++);
        }
        else
        {
            ++i;
        }
    }
}

void ThreadPool::CleanupTaskImp()
{
    CleanThreadsIfImp(
        boost::bind(&FinishedExecutionForImp, _1,
                    boost::chrono::steady_clock::duration::min()));
}

void ThreadPool::CancelJoinThreadImp(
    std::pair<boost::thread::id, std::pair<ThreadPtr, bool> > threadPair_)
{
    // assert(0 == pthread_cancel(threadPair_.second.first->native_handle()));
    pthread_cancel(threadPair_.second.first->native_handle());
    // assert(0 == pthread_join(threadPair_.second.first->native_handle(),
    // NULL));
    pthread_join(threadPair_.second.first->native_handle(), NULL);
    {
        std::stringstream str;
        str << "Killed thread " << threadPair_.first;
        Log(str.str());
    }
}

void ThreadPool::SetThreadDoneImp(boost::thread::id id_)
{
    // boost::unique_lock<boost::mutex> lock(m_threadsMutex);
    m_threads[id_].second = true; // Sets the done flag.
}

void ThreadPool::ThreadActionImp(ThreadPool* tp_)
{
    while (1)
    {
        Task t;
        // pop a function from m_tp.m_waitableQ.
        tp_->m_tasks.Dequeue(t);
        if (t.IsPoisonous())
        {
            tp_->SetThreadDoneImp(boost::this_thread::get_id());
            return;
        }
        else
        {
            try
            {
                assert(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,
                                             NULL) == 0);
                t.Do();
                assert(pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL) ==
                       0);
            }
            catch (const boost::thread_interrupted& i)
            {
                Log("Thread interrupted");
                throw;
            }
            catch (const std::exception& e)
            {
                Log(e.what());
                throw;
            }
            catch (...)
            {
                throw;
            }
        }
    }
}

//*******************ThreadPool::Task**********************//
ThreadPool::Task::Task(TaskFunc taskFunc_, int priority_)
    : m_taskFunc(taskFunc_), m_priority(priority_)
{
}

bool ThreadPool::Task::operator<(const Task& o_) const
{
    return m_priority < o_.m_priority;
}

bool ThreadPool::Task::IsPoisonous() const
{
    return POISON_TASK_PRIORITY == m_priority;
}

void ThreadPool::Task::Do() { m_taskFunc(); }

ThreadPool::Task ThreadPool::Task::CreatePoisonousTask()
{
    return Task(TaskFunc(0), POISON_TASK_PRIORITY);
} // TODO: Relate to the priority or the function?

} // namespace ilrd
