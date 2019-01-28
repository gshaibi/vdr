#ifndef waitable_q_hpp
#define waitable_q_hpp

#include <boost/noncopyable.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <climits> //LONG_MAX
#include <queue>

namespace ilrd
{

template <typename T, typename Q=std::queue<T> > //Q should implement pop, push, empty and front functions, and value_type which must be assignable
class WaitableQ : boost::noncopyable
{
public:
	//generated ctor and dtor

	void Enqueue(const typename Q::value_type& data_);

	static const long INFINITE = LONG_MAX;
	//TODO: Maybe return boost::cv_status?
	bool DequeueFor(typename Q::value_type &ret_, boost::chrono::milliseconds duration_=boost::chrono::milliseconds(INFINITE)); 
	void Dequeue(typename Q::value_type &ret_);

	bool IsEmpty() const;

private:
	Q m_q;
	mutable boost::mutex m_lock;
	boost::condition_variable m_cond;
};

template <typename T, typename Q>
void WaitableQ<T, Q>::Enqueue(const typename Q::value_type& data_)
{
	boost::unique_lock<boost::mutex> ulock(m_lock);
	m_q.push(data_);
	m_cond.notify_one();
}

template <typename T, typename Q>
bool WaitableQ<T, Q>::DequeueFor(typename Q::value_type &ret_, boost::chrono::milliseconds duration_)
{
	using namespace boost::chrono;

	system_clock::time_point bailTime(system_clock::now() + duration_);
	boost::unique_lock<boost::mutex> ulock(m_lock);
	while (m_q.empty())
	{
		if (boost::cv_status::timeout == m_cond.wait_until(ulock, bailTime))
		{
			return false;
		}
	}
	ret_ = m_q.front();
	m_q.pop();
	return true;
}

template <typename T, typename Q>
void WaitableQ<T, Q>::Dequeue(typename Q::value_type &ret_)
{
	boost::unique_lock<boost::mutex> ulock(m_lock);
	while (m_q.empty())
	{
		m_cond.wait(ulock);
	}
	ret_ = m_q.front();
	m_q.pop();
}

template <typename T, typename Q>
bool WaitableQ<T, Q>::IsEmpty() const
{
	boost::unique_lock<boost::mutex> ulock(m_lock);
	return m_q.empty();
}

} // ilrd


#endif // waitable_q_hpp
