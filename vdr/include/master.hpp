#ifndef MASTER_HPP
#define MASTER_HPP

#include <boost/noncopyable.hpp> //boost::noncopyable
#include <boost/shared_ptr.hpp>  //boost::shared_ptr
#include <boost/chrono.hpp>  		 //boost::chrono

#include <vector> //std::vector
#include <map> 		//std::map

#include <netinet/in.h> // sockaddr_in

#include "protocols.hpp"    //master-minion & master-OSP message protocols
#include "os_proxy.hpp"     //class OsProxy
#include "block_table.hpp"  //class BlockTable
#include "minion_proxy.hpp" //clsas MinionProxy
#include "timer.hpp" 				//class Timer
#include "thread_pool.hpp" // class thread_pool
#include "eventer.hpp" // class eventer
#include "encryptor.hpp" //class encryptor

// TODO: exception safety
// TODO: add timer to Makefile

namespace ilrd
{

class Master : boost::noncopyable
{
public:
	Master(size_t numMinions_, Reactor& r_, const sockaddr_in&); 
	// NOTE: Object is incomplete before calling SetOsProxy.
	// using generated dtor. Blocked cctor & op=

	void SetOsProxy(OsProxy *osp_);

	void Read(protocols::os::ReadRequest request_);
	void Write(protocols::os::WriteRequest request_);

private:

	//enum & typedefs//
	enum RequestStatus 
	{ 
		NBD_AWAITING_REPLY,
		REPLIED_TO_NBD
	};

	typedef std::vector<BlockTable::BlockLocation> BlockLocations;

	//nested structs//
	struct RequestData
	{
		Timer::Handle handle;
		RequestStatus status;
		BlockLocations blockLocations;
	};

	struct MappedReadRequest
	{
		RequestData data;
		protocols::os::ReadRequest ospRequest;
	};
	
	struct MappedWriteRequest
	{
		RequestData data;
		protocols::os::WriteRequest ospRequest;
	};

	//typedefs//
	typedef BlockLocations::iterator Iterator;
	typedef std::map<protocols::ID, MappedWriteRequest>::iterator WriteIterator;
	typedef std::map<protocols::ID, MappedReadRequest>::iterator ReadIterator;
	
	//data members//
	OsProxy *m_osPtr;
	MinionProxy m_minionProxy;
	BlockTable m_blockTable;
	Timer m_timer;
	
	Eventer m_eventer;
	ThreadPool m_tpool;
	Encryptor m_encryptor;

	// std::map<protocols::ID, MappedRequest> m_sentRequests;
	std::map<protocols::ID, MappedReadRequest> m_readRequests;
	std::map<protocols::ID, MappedWriteRequest> m_writeRequests;

	//static data members//
	static const size_t BLOCK_SIZE = 4096; //TODO: remove this when have Config?
	static const size_t TIMEOUT_IN_NANOSECONDS = 100000000; //used to initialize TIMEOUT
	static boost::chrono::steady_clock::duration TIMEOUT; 

	//friend class//
	friend class MinionProxy; //for using ReplyReadIMP & ReplyWriteIMP

	//private methods//
	void ReplyReadIMP(protocols::minion::ReadReply reply_);
	void ReplyWriteIMP(protocols::minion::WriteReply reply_);
	Timer::Handle SetTimerIMP(protocols::ID id_);
	Timer::CallBack GetTimerCbIMP(protocols::ID); //binds OnTimerTMP
	// Timer::CallBack GetTimerWriteCbIMP(protocols::ID); //binds OnTimerReadTMP
	// Timer::CallBack GetTimerCbIMP(protocols::ID); 			//binds OnTimerWriteTMP
	void OnTimerIMP(protocols::ID); //cb passed to Timer
	void OnTimerReadIMP(protocols::ID);  //cb passed to Timer
	void OnTimerWriteIMP(protocols::ID); //cb passed to Timer
	RequestData ProcessRequestIMP(size_t offset_, protocols::ID id_);	  //used in Read & Write
	RequestStatus ProcessReplyIMP(protocols::ID id_, size_t minionID_); //used in ReplyReadIMP & ReplyWriteIMP
	void SendWriteRequestsIMP(protocols::ID id_);
	void SendReadRequestsIMP(BlockLocations, protocols::os::ReadRequest);

	void ReadReplyToOsProxyCB(protocols::minion::ReadReply rep_); // callback sent to encryptor

	// TODO: these instead of ProcessReplyIMP? need to unite duplicate code...
	RequestStatus ProcessReadReplyIMP(protocols::ID id_, size_t minionID_);
	RequestStatus ProcessWriteReplyIMP(protocols::ID id_, size_t minionID_);

};//class Master

} // namespace ilrd

#endif // master_hpp
