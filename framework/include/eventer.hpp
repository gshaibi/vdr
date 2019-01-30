#ifndef EVENTER
#define EVENTER

#include <boost/noncopyable.hpp> 

#include "routines.hpp"
#include "reactor.hpp" //using Reactor

namespace ilrd
{

// TODO: API  NOT FINISHED
class Eventer : private boost::noncopyable
{
public:
  //constructors & destructors//
  explicit Eventer(Reactor& reactor_); 
	// using generated Dtor. Blocked CCtor & op=

  //methods//
	typedef int Handle;
	Handle SetEvent(boost::function<void(void)> cb_);
	void SignalEvent(Handle handle_);
private:
	Reactor& m_reactor;
};//class Eventer
/******************************************************************************/

} //namespae ilrd

#endif // EVENTER

