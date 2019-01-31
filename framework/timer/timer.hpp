#ifndef TIMER_HPP
#define TIMER_HPP

#include <boost/noncopyable.hpp>
#include <boost/chrono.hpp>						// its a timer 
#include <boost/function.hpp>					// callbacks

#include <map>									// manage callbacks
#include <sys/timerfd.h>						// linux timer object

#include "logger.hpp"							// log events
#include "reactor.hpp"							// time callbacks

namespace ilrd
{

// P - parameter to callback. should be copyable.
class Timer : boost::noncopyable
{
public:
	explicit Timer(Reactor& r_);
	//generated dtor.

	typedef int Handle;
	typedef boost::chrono::steady_clock::time_point timePoint_type;
	typedef boost::chrono::steady_clock::duration duraton_type;
	typedef boost::function<void ()> CallBack_type;

	//returns handle to be used in Cancel.
	Handle Set(duraton_type& duration_, CallBack_type callback_);
	void Cancel(Handle handle_);

private:
	Handle m_handleCounter; 
	Reactor& m_reactor;
	int m_timerFd;
	std::map<timePoint_type, std::pair<Handle, CallBack_type> > m_callBacks;

	void SetTimerIMP(duraton_type& duration_, CallBack_type callback_);
	void CallBackWrapper(CallBack_type cb_);
};

} // ilrd




#endif // TIMER_HPP
