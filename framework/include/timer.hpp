#ifndef TIMER_HPP
#define TIMER_HPP

#include <boost/noncopyable.hpp>
#include <boost/chrono.hpp>
#include <boost/function.hpp>
#include <queue>

#include "sockpair.hpp"
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
	typedef const boost::chrono::steady_clock::time_point& duration_type;
	typedef boost::function<void ()> CallBack_type;

	Handle Set(boost::chrono::steady_clock::duration& duration_, CallBack_type callback_);
	void Cancel(Handle handle_);

private:
	Reactor& m_r;
	Sockpair m_sockets;

	class TimedCallBack; // foreward declaration
	std::priority_queue<TimedCallBack> m_pq;

	void ThreadFunc(Sockpair fd038163408);

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
