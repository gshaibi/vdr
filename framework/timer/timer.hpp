#ifndef TIMER_HPP
#define TIMER_HPP

#include <boost/noncopyable.hpp>
#include <boost/chrono.hpp>						// its a timer 
#include <boost/function.hpp>					// callbacks
#include <boost/shared_array.hpp>				// RAII - file descriptor
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

	typedef unsigned int Handle;
	typedef boost::chrono::steady_clock::time_point TimePoint;
	typedef boost::chrono::steady_clock::duration Duration;
	typedef boost::function<void ()> CallBack;

	//returns handle to be used in Cancel.
	Handle Set(Duration& duration_, CallBack callback_);
	void Cancel(Handle handle_);

private:
	typedef std::map<TimePoint, std::pair<Handle, CallBack> >::iterator TimerIter;
	static const int NANOS_IN_SECOND = 1000000000;

	Handle m_handleCounter; 
	Reactor& m_reactor;
	std::map<TimePoint, std::pair<Handle, CallBack> > m_callBacks;
	
	typedef boost::shared_ptr<int> sharedFD;
	boost::shared_ptr<int> m_timerFd;

	void SetTimerIMP(Duration duration_) const;
	void CallBackWrapper();
	void SetNextTimer(TimerIter iter_) const;
	static void CloseFD(int* fd_);

}; // timer

} // ilrd




#endif // TIMER_HPP
