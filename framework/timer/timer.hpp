#ifndef TIMER_HPP
#define TIMER_HPP

#include <boost/noncopyable.hpp>
#include <boost/chrono.hpp>
#include <boost/function.hpp>
#include <queue>

#include "reactor.hpp"

namespace ilrd
{

// P - parameter to callback. should be copyable.
class Timer : boost::noncopyable
{
public:
	explicit Timer(Reactor& r_);
	//generated dtor.

	//returns handle to be used in Cancel.
	typedef int Handle;
	typedef const boost::chrono::steady_clock::duration& duration_type;
	typedef boost::function<void ()> CallBack_type;

	Handle Set(duration_type duration_, CallBack_type callback_);
	void Cancel(Handle handle_);

private:
	Reactor& m_r;
	
	class TimedCallBack; // foreward declaration
	std::priority_queue<TimedCallBack> m_pq;

	void ThreadFunc(int fd);

	class TimedCallBack
	{
	public:
		explicit TimedCallBack(duration_type duration_, CallBack_type callback_);
		// generated dtor cctor and op=.

		bool operator<(const TimedCallBack& o_) const;
		void Do();
		duration_type GetTime() const;

	private:
		duration_type m_dur;
		CallBack_type m_cb;
	};
};

} // ilrd




#endif // TIMER_HPP
