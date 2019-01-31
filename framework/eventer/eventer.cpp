#include <cassert> // assert
#include <unistd.h> // using pipe , write , read
#include <fcntl.h> // using O_NONBLOCK

#include <algorithm> //using remove
#include <sstream> //using stingstream

#include <boost/bind.hpp> //using boost bind 

#include "eventer.hpp" // header file
#include "logger.hpp" //using logger

namespace ilrd
{
/**********************************Eventer*************************************/
//ctors & dtors//
Eventer::Eventer(Reactor& reactor_):
									m_reactor(reactor_),
									m_events(),
									m_handleCounter(0),
									m_eventsLock()
{
	//create pipe
	//save both ends of the pipe 
	if(-1 == pipe2(m_pipe, O_NONBLOCK))
	{
		ilrd::Log("[Eventer] create pipe failed");
		throw std::runtime_error(strerror(errno));
	}
}

Eventer::~Eventer()
{
	ilrd::Log("[Eventer]  ~Eventer");

	if(m_events.empty() == false)
	{
		{
		std::stringstream msg;
		msg << "[Eventer] removing fd " << m_pipe[READ]<<" to reactor" <<std::endl;
		ilrd::Log(msg.str());
		}
		m_reactor.RemFD(m_pipe[READ], Reactor::READ);
	}

	//close pipes resource
	close(m_pipe[READ]);
	close(m_pipe[WRITE]);
}

//methods//
Eventer::Handle Eventer::SetEvent(boost::function<void(void)> cb_)
{
	assert(cb_.empty() == true);
	
	//Lock before set new event
	boost::unique_lock<boost::mutex> lock(m_eventsLock);

	ilrd::Log("[Eventer] set event");

	//register to reactor with the READ side of pipe 
	if(m_events.empty() == true)
	{
		{
		std::stringstream msg;
		msg << "[Eventer] adding fd " << m_pipe[READ]<<" to reactor" <<std::endl;
		ilrd::Log(msg.str());
		}
		m_reactor.AddFD(m_pipe[READ], Reactor::READ, boost::bind(&Eventer::OnEventFinishedCB, this, _1));
	}
	
	//create a key 
	Handle usrHandle = m_handleCounter++;
	
	//insert to key to m_events along with the callback as value
	
		//Concurrently accessing other elements is safe.
		//concurrently iterating ranges in the container is not safe.
	m_events[usrHandle] = cb_;
	
	{
		std::stringstream msg;
		msg << "[Eventer] event id is " << usrHandle <<std::endl;
		ilrd::Log(msg.str());
	}

	//return the key as handle to the user
	return usrHandle;
}

void Eventer::SignalEvent(Handle handle_) const
{
	//TODO:Lock before write
	boost::unique_lock<boost::mutex> lock(m_eventsLock);

	ilrd::Log("[Eventer] Signal event");
	//write the events key number (handle_) into side b of the pipe
	
	if(-1 == write(m_pipe[WRITE], &handle_, sizeof(Handle)))
	{
		ilrd::Log("[Eventer] write to pipe failed");
		throw std::runtime_error(strerror(errno));
	}
	
	{
		std::stringstream msg;
		msg << "[Eventer] event id " << handle_ <<" signaled"<< std::endl;
		ilrd::Log(msg.str());
	}
}

//private methods//
void Eventer::OnEventFinishedCB(int readFd_) //callback function
{
	assert(readFd_ == m_pipe[READ]);
	
	ilrd::Log("[Eventer] on event finished");
	Handle usrHandle = 0;
	
	// read the key from the pipe 
	if(-1 == read(readFd_, &usrHandle, sizeof(Handle)))
	{
		ilrd::Log("[Eventer] read from pipe failed");
		throw std::runtime_error(strerror(errno));
	}

	{
		std::stringstream msg;
		msg << "[Eventer] invoking event id " << usrHandle <<" callback"<< std::endl;
		ilrd::Log(msg.str());
	}

	//invoke the event callback
	//Concurrently accessing other elements is safe.
	//concurrently iterating ranges in the container is not safe.
	assert(m_events.find(usrHandle) != m_events.end());
	m_events[usrHandle]();

	//Lock before remove
	boost::unique_lock<boost::mutex> lock(m_eventsLock);

	// remove from reactor in case of last event
	if(m_events.size() == 1)
	{
		{
		std::stringstream msg;
		msg << "[Eventer] removing fd " << m_pipe[READ]<<" to reactor" <<std::endl;
		ilrd::Log(msg.str());
		}
		m_reactor.RemFD(m_pipe[READ], Reactor::READ);
	}

	//remove the event element from the map
	m_events.erase(usrHandle);
	
	{
		std::stringstream msg;
		msg << "[Eventer] event id " << usrHandle <<" removed"<< std::endl;
		ilrd::Log(msg.str());
	}
}

} //namespace ilrd

