#ifndef EVENTER
#define EVENTER

#include <map> //using map

#include <boost/noncopyable.hpp>  // using boost noncopyable
#include <boost/function.hpp> //using boost function
#include <boost/thread/mutex.hpp> //using mutex

#include "reactor.hpp" //using Reactor
#include "routines.hpp"

namespace ilrd
{

//TODO: comment on thread saftey guarantees

class Eventer : private boost::noncopyable
{
public:
  //constructors & destructors//
  explicit Eventer(Reactor& reactor_); 
	~Eventer();

	// Blocked CCtor & op=

  //methods//
	typedef unsigned int Handle;
	Handle SetEvent(boost::function<void(void)> cb_);
	void SignalEvent(Handle handle_) const;

private:
	enum PipeType{READ = 0, WRITE = 1};

	typedef boost::function<void(void)> CallBack;
	void OnEventFinishedCB(int fd_); //callback function

	Reactor& m_reactor;
	std::map<Handle, CallBack> m_events;
	int m_pipe[2];
	Handle m_handleCounter;
	mutable boost::mutex m_eventsLock;

};//class Eventer

} //namespae ilrd

#endif // EVENTER

