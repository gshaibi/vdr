#include <cassert>	// assert

#include "logger.hpp"
#include "master.hpp"
#include "routines.hpp"

namespace ilrd
{

using namespace std; 			 //STL
using namespace protocols; //os/minion reply/request protocols

/***********************************Master*************************************/
//ctor//
Master::Master(size_t nMinions_, Reactor& r_)
: m_osPtr(NULL),
	m_minionProxy(0, *this, r_),
	m_blockTable(BLOCK_SIZE) 
{
	// write to log
	stringstream str;
	str << "[Master] ctor: numMinions=" << nMinions_ << " blockSize=" << BLOCK_SIZE;
	Log(str.str());
}

//public methods//
void Master::SetOsProxy(OsProxy *os_)
{
	assert(NULL == m_osPtr); //prevent double set

	Log("[Master] SetOsProxy");
	m_osPtr = os_;
}

void Master::Read(protocols::os::ReadRequest request_)
{
	assert(m_osPtr != NULL);

	Log("[Master] Read");

	// Translate the request with BlockTable
	std::vector<BlockTable::BlockLocation> 
		requests(m_blockTable.Translate(request_.GetOffset()));
	
	// pass the requests to minion proxys
	for (size_t i = 0; i < requests.size(); ++i)
	{
		BlockTable::BlockLocation curr = requests[i];
		minion::ReadRequest minionRequest(request_, curr.blockOffset);

		// write to log
		stringstream str;
		str << "[Master] calling MinionProxy::ReadRequest | minionID=" << \
		curr.minionID << " block=" << curr.blockOffset;
	 	Log(str.str()); 
		
		m_minionProxy.ReadReq(minionRequest);
	}
}

void Master::Write(protocols::os::WriteRequest request_)
{
	assert(m_osPtr != NULL);

	Log("[Master] WriteReq");

	// Translate the request with BlockTable
	std::vector<BlockTable::BlockLocation> 
		requests(m_blockTable.Translate(request_.GetOffset()));

	// pass the requests to minion proxys
	for (size_t i = 0; i < requests.size(); ++i)
	{
		BlockTable::BlockLocation curr = requests[i];
		minion::WriteRequest minionRequest(request_, curr.blockOffset);

		// write to log
		stringstream str;
		str << "[Master] calling MinionProxy::WriteRequest | minionID=" << \
		curr.minionID << " block=" << curr.blockOffset;
	 	Log(str.str()); 
	
		m_minionProxy.WriteReq(minionRequest);
	}
}

//private methods//
void Master::ReplyReadIMP(protocols::minion::ReadReply reply_)
{ 
	assert(m_osPtr != NULL);

	os::ReadReply ospReply(reply_.GetID(), reply_.GetStatus(), reply_.GetData());
	
	// write to log
	stringstream str;
	str << "[Master] calling OsProxy::ReplyRead | status=" << reply_.GetStatus() \
	<< " buffer=" << reply_.GetData();
	Log(str.str());

	m_osPtr->ReplyRead(ospReply);
}

void Master::ReplyWriteIMP(protocols::minion::WriteReply reply_) 
{ 
	assert(m_osPtr != NULL);

	os::WriteReply ospReply(reply_.GetID(), reply_.GetStatus());

	// write to log
	stringstream str;
	str << "[Master] calling OsProxy::ReplyWrite | status=" << reply_.GetStatus();
	Log(str.str());

	m_osPtr->ReplyWrite(ospReply);
}

} // namespace ilrd
