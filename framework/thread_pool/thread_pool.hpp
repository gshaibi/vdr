#ifndef thread_pool_hpp
#define thread_pool_hpp

#include <boost/atomic.hpp>
#include <boost/chrono.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <map>
#include <queue>

// #include "routines.hpp" //noexcept
#include "waitable_q.hpp"

namespace ilrd
{

class ThreadPool : private boost::noncopyable
{
public:
	explicit ThreadPool(int numThreads_);
	~ThreadPool();

	typedef boost::function<void()> TaskFunc;
	enum Priority
	{
		LOW = 0,
		MEDIUM,
		HIGH
	};
	void Add(const TaskFunc& task_, Priority p_ = MEDIUM);
	void SetNum(int numThreads_); //For reducing number of threads
	void Start();
	// returns if threadpool successfully stopped before the duration passed
	bool Stop(const boost::chrono::steady_clock::duration& duration_ =
				  boost::chrono::steady_clock::duration::min());

	bool IsRunning() const;

private:
	typedef boost::shared_ptr<boost::thread> ThreadPtr;

	class Task // TODO: Maybe just pass compare function to m_tasks?
	{
	public:
		explicit Task(TaskFunc taskFunc_ = TaskFunc(0), int priority_ = 0);
		// generated dtor cctor and op=.

		bool operator<(const Task& o_) const;
		bool IsPoisonous() const;

		void Do();

		static Task CreatePoisonousTask();

	private:
		TaskFunc m_taskFunc;
		int m_priority;
	};

	class PriorityQSuitableForWaitableQ : private std::priority_queue<Task>
	{
	public:
		using priority_queue::empty;
		using priority_queue::pop;
		using priority_queue::push;
		using priority_queue::value_type;

		const Task& front() { return top(); }
	};

	static void ThreadActionImp(ThreadPool* tp_);
	static bool FinishedExecutionForImp(std::pair<boost::thread::id, std::pair<ThreadPtr , bool> >,
										boost::chrono::steady_clock::duration);
	static bool
		FinishedExecutionUntilImp(std::pair<boost::thread::id, std::pair<ThreadPtr , bool> >,
								  boost::chrono::steady_clock::time_point);
	static void CancelJoinThreadImp(std::pair<boost::thread::id, std::pair<ThreadPtr , bool> >);

	void CleanThreadsIfImp(
		boost::function<bool(std::pair<boost::thread::id, std::pair<ThreadPtr , bool> >)> pred_);
	void InsertCleanupTaskImp();
	void CleanupTaskImp();
	void AddThreadImp();
	void AddTaskImp(const Task& task_);
	void SetThreadDoneImp(boost::thread::id id_);

	static const int POISON_TASK_PRIORITY = 5;//TODO: Maybe Task class responsability
	static const int CLEAN_TASK_PRIORITY = 4;
	
	mutable boost::mutex m_threadsMutex;
	boost::atomic<int> m_numThreadsShouldExist; // TODO: Atomic?
	WaitableQ<Task, PriorityQSuitableForWaitableQ> m_tasks;
	std::map<boost::thread::id, std::pair<ThreadPtr, bool> > m_threads;
};

} // namespace ilrd

#endif // thread_pool_hpp
