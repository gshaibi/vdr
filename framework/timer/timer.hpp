#ifndef TIMER_HPP
#define TIMER_HPP

#include <boost/noncopyable.hpp>
#include <boost/chrono.hpp>
#include <boost/function.hpp>

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

	Handle Set(const boost::chrono::steady_clock::duration& duration_, boost::function<void ()> callback_);
	void Cancel(Handle handle_);

private:
	
	Reactor& m_r;
};

} // ilrd




#endif // TIMER_HPP
