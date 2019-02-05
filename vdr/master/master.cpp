#include <cassert>	// assert

#include "logger.hpp"
#include "master.hpp"
#include "routines.hpp"
#include "singleton.hpp" // singleton
#include <libconfig.h++> // Config

namespace ilrd
{

using namespace std; 			 //STL libraries
using namespace boost;  	 //boost libraries
using namespace protocols; //os/minion reply/request protocols

/***********************************Master*************************************/
//static data definition//
boost::chrono::steady_clock::duration Master::TIMEOUT(TIMEOUT_IN_NANOSECONDS); 

//ctor//
Master::Master(size_t nMinions_, Reactor& r_, const sockaddr_in& minionAddr_)
: m_osPtr(NULL),
	m_minionProxy(0, *this, r_, minionAddr_),
	m_blockTable(BLOCK_SIZE),
	m_timer(r_),
	m_eventer(r_),
	m_tpool(0),
	m_readRequests(),
	m_writeRequests()
{
	// Get config instance
	Singleton<libconfig::Config> cfg;
	//TODO: init numthreads with Config.lockup
	int numThreads = 0;
	try
	{
		numThreads = cfg.GetInstance().lookup("numThreads");
	}
	catch (const libconfig::SettingNotFoundException& nfex)
    {
        std::cerr << "Setting '" << nfex.getPath() << "' is missing from conf file." << std::endl;
    }
	catch (const libconfig::SettingTypeException& tex)
	{
		std::cout << "Setting '" <<  tex.getPath() << "' doesnt match it's type." << std::endl;
	}
	
	m_tpool.SetNum(numThreads);
	
	//TODO: init the ThreadPool member (Start)
	// write to log
	stringstream str;
	str << "[Master] ctor: numMinions = " << nMinions_ << " blockSize = " << BLOCK_SIZE;
	Log(str.str());
}

/*******************************public methods*********************************/
void Master::SetOsProxy(OsProxy *os_)
{
	Log("[Master] SetOsProxy");

	assert(NULL == m_osPtr); //prevent double set
	assert(os_ != NULL); 

	m_osPtr = os_;
}

void Master::Read(protocols::os::ReadRequest req_)
{
	Log("[Master] Read");

	assert(m_osPtr != NULL);
	// assert that request isn't already in map
	assert(m_readRequests.end() == m_readRequests.find(req_.GetID()));
	assert(m_writeRequests.end() == m_writeRequests.find(req_.GetID()));

	// Process request - Translate, set Timer 
	RequestData data = ProcessRequestIMP(req_.GetOffset(), req_.GetID());

	// Insert request to map
	// m_sentRequests[id_] = make_pair(requests, make_pair(handle, NBD_AWAITING_REPLY));
	MappedReadRequest to_insert = {data, req_};
	m_readRequests.insert(make_pair(req_.GetID(), to_insert));

	// pass the requests to minion proxys
	SendReadRequestsIMP(data.blockLocations, req_);
}


// TODO: need to unite all the duplicate code between Read & Write
void Master::Write(protocols::os::WriteRequest req_)
{
	Log("[Master] Write");

	assert(m_osPtr != NULL);
	// assert that request isn't already in map
	assert(m_readRequests.end() == m_readRequests.find(req_.GetID()));
	assert(m_writeRequests.end() == m_writeRequests.find(req_.GetID()));

	// Process request - Translate, set Timer 
	RequestData data = ProcessRequestIMP(req_.GetOffset(), req_.GetID());

	// Insert request to map
	MappedWriteRequest to_insert = {data, req_};
	m_writeRequests.insert(make_pair(req_.GetID(), to_insert));

	// pass the requests to minion proxys
	//TODO: encrypt data - use SendWriteRequestsIMP as a call back
	SendWriteRequestsIMP(data.blockLocations, req_);
}

/*******************************private methods********************************/
void Master::ReplyReadIMP(protocols::minion::ReadReply rep_)
{ 
	// write to log
	stringstream str;
	str << "[Master] ReplyReadIMP | status = " << rep_.GetStatus() << \
	" | minionID = " << rep_.GetMinionID();
	Log(str.str());

	assert(m_osPtr != NULL);

	RequestStatus status = ProcessReadReplyIMP(rep_.GetID(), rep_.GetMinionID());
	
	// write to log
	str.str(""); 
	str << "[Master] RequestStatus = " << status; 
	Log(str.str());

	//reply to nbd if haven't replied yet
	if (NBD_AWAITING_REPLY == status)
	{
		os::ReadReply ospReply(rep_.GetID(), rep_.GetStatus(), rep_.GetData());

		// write to log
		stringstream str;
		str << "[Master] calling OsProxy::ReplyRead | status = " << rep_.GetStatus() \
		<< " buffer = " << rep_.GetData();
		Log(str.str());

		m_osPtr->ReplyRead(ospReply);
	}
}

void Master::ReplyWriteIMP(protocols::minion::WriteReply rep_) 
{ 
	// write to log
	stringstream str;
	str << "[Master] ReplyWriteIMP | status = " << rep_.GetStatus() << \
	" | minionID = " << rep_.GetMinionID();
	Log(str.str());

	assert(m_osPtr != NULL);

	RequestStatus status = ProcessWriteReplyIMP(rep_.GetID(), rep_.GetMinionID());

	// write to log
	str.str("");
	str << "[Master] RequestStatus = " << status;
	Log(str.str());

	//reply to nbd if haven't replied yet
	if (NBD_AWAITING_REPLY == status)
	{
		//TODO: decrypt befor send reply to os

		//TODO wrap the below code in cb func to be used by encryptor(decrypt)
		//TODO bind with WriteReply rep_

		os::WriteReply ospReply(rep_.GetID(), rep_.GetStatus()); 

		// write to log
		stringstream str;
		str << "[Master] calling OsProxy::ReplyWrite | status = " << rep_.GetStatus();
		Log(str.str());

		m_osPtr->ReplyWrite(ospReply);
	}
}

//TODO: ReadReplyToOsProxyCB(protocols::minion::WriteReply rep_)
// {
// 	os::WriteReply ospReply(rep_.GetID(), rep_.GetStatus()); 

// 		// write to log
// 		stringstream str;
// 		str << "[Master] calling OsProxy::ReplyWrite | status = " << rep_.GetStatus();
// 		Log(str.str());

// 		m_osPtr->ReplyWrite(ospReply);
// }

// TODO: don't need this func?
//used in Master::Read & Master::Write
// Master::RequestStatus Master::ProcessReplyIMP(protocols::ID id_, size_t minionID_) 

Master::RequestStatus Master::ProcessReadReplyIMP(protocols::ID id_, size_t minionID_) 
{
	// write to log
	stringstream str;
	str << "[Master] ProcessReadReplyIMP | minionID = " << minionID_;
	str << " ID = " << *(size_t *)(&id_);
	Log(str.str());

	// if request id isn't in map - ignore the reply
	ReadIterator found(m_readRequests.find(id_));
	
	if (m_readRequests.end() == found)
	{
		Log("[Master] ERROR: request ID is not in ReadMap - ignoring the reply");
		return REPLIED_TO_NBD;
	}

	MappedReadRequest request = (*found).second;
	BlockLocations bl = request.data.blockLocations;

	// find minionID_ in vector
	for (Iterator iter(bl.begin()); iter != bl.end(); ++iter)
	{
		// if found minion - remove it 
		if (minionID_ == (*iter).minionID)
		{
			Log("[Master] removing minion's BlockLocation from mapped vector");

			// erase minion from the vector
			bl.erase(iter);

			// if vector is empty - remove ID from map & cancel timer
			if (bl.empty())
			{
				Log("[Master] vector empty - removing ID from map & canceling timer");
				m_readRequests.erase(id_);
				m_timer.Cancel(request.data.handle);
			}

			break;
		}
	}

	// get RequestStatus
	RequestStatus status = request.data.status;

	// write to log
	str.str("");
	str << "[Master] RequestStatus = " << status;
	Log(str.str());

	// update RequestStatus
	request.data.status = REPLIED_TO_NBD;

	return status; 
}

Master::RequestStatus Master::ProcessWriteReplyIMP(protocols::ID id_, size_t minionID_) 
{
	// write to log
	stringstream str;
	str << "[Master] ProcessWriteReplyIMP | minionID = " << minionID_;
	str << " ID = " << *(size_t *)(&id_);
	Log(str.str());

	// if request id isn't in map - ignore the reply
	WriteIterator found(m_writeRequests.find(id_));

	if (m_writeRequests.end() == found)
	{
		Log("[Master] ERROR: request ID is not in WriteMap - ignoring the reply");
		return REPLIED_TO_NBD;
	}

	MappedWriteRequest request = (*found).second;
	BlockLocations bl = request.data.blockLocations;

	// find minionID_ in vector
	for (Iterator iter(bl.begin()); iter != bl.end(); ++iter)
	{
		// if found minion - remove it 
		if (minionID_ == (*iter).minionID)
		{
			Log("[Master] removing minion's BlockLocation from mapped vector");

			// erase minion from the vector
			bl.erase(iter);

			// if vector is empty - remove ID from map & cancel timer
			if (bl.empty())
			{
				Log("[Master] vector empty - removing ID from map & canceling timer");
				m_writeRequests.erase(id_);
				m_timer.Cancel(request.data.handle);
			}

			break;
		}
	}

	// get RequestStatus
	RequestStatus status = request.data.status;

	// write to log
	str.str("");
	str << "[Master] RequestStatus = " << status;
	Log(str.str());

	// update RequestStatus
	request.data.status = REPLIED_TO_NBD;

	return status; 
}

Timer::Handle Master::SetTimerIMP(protocols::ID id_)
{
	Log("[Master] setting timer");
	Timer::Handle handle = m_timer.Set(TIMEOUT, GetTimerCbIMP(id_));
	stringstream str;
	str << "[Master] timer set | Timer handle = " << handle;
	str << " ID = " << *(size_t *)(&id_);
	Log(str.str());

	return handle;
}

// TODO: make inline?
Timer::CallBack Master::GetTimerCbIMP(protocols::ID id_)
{
	// TODO: bind used correctly?
	return bind(&Master::OnTimerIMP, this, id_);
}

void Master::OnTimerIMP(protocols::ID id_) //callback passed to Timer::Set
{
	// write to log
	stringstream str;
	str << "[Master] OnTimerIMP | ID = " << *(size_t *)(&id_);
	Log(str.str());

	if (m_readRequests.find(id_) != m_readRequests.end())
	{
		Log("[Master] ID found in ReadRequests map");
		OnTimerReadIMP(id_);
	}
	else if (m_writeRequests.find(id_) != m_writeRequests.end())
	{
		Log("[Master] ID found in WriteRequests map");
		OnTimerWriteIMP(id_);
	}
	else 
	{
		// if ID doesn't exist in any map - ignore it
		Log("[Master] ERROR: ID wasn't found in map - ignoring it");
	}
}

void Master::OnTimerReadIMP(protocols::ID id_) 
{
	// write to log
	Log("[Master] OnTimerReadIMP");

	assert(m_writeRequests.end() == m_writeRequests.find(id_));

	// find id_ in ReadRequests map
	ReadIterator found(m_readRequests.find(id_));

	BlockLocations requests = (*found).second.data.blockLocations;
	protocols::os::ReadRequest ospRequest = (*found).second.ospRequest;

	// resend the requests to all minion proxys that haven't replied yet
	SendReadRequestsIMP(requests, ospRequest); // TODO: can also reset timer?

	//TODO: remove SetTimeIMP - SendWriteRequestsIMP responsibilty
	// reset the Timer
	(*found).second.data.handle = SetTimerIMP(id_);
}

void Master::OnTimerWriteIMP(protocols::ID id_) 
{
	// write to log
	Log("[Master] OnTimerWriteIMP");

	assert(m_readRequests.end() == m_readRequests.find(id_));

	// find id_ in WriteRequests map
	WriteIterator found(m_writeRequests.find(id_));

	BlockLocations requests = (*found).second.data.blockLocations;
	protocols::os::WriteRequest ospRequest = (*found).second.ospRequest;

	// resend the requests to all minion proxys that haven't replied yet
	SendWriteRequestsIMP(requests, ospRequest); 
	

	//TODO: remove SetTimeIMP - SendWriteRequestsIMP responsibilty
	// reset the Timer
	(*found).second.data.handle = SetTimerIMP(id_);
}

Master::RequestData Master::ProcessRequestIMP(size_t offset_, 
                                              protocols::ID id_)
{
	stringstream str;
	str << "[Master] ProcessRequestIMP | ID = " << *(size_t *)(&id_) << " offset = " << offset_;
	Log(str.str());

	// Translate the request with BlockTable
	Log("[Master] calling BlockTable::Translate");
	BlockLocations requests(m_blockTable.Translate(offset_));

	// Set timer
	//TODO: move SetTimer to SendWriteRequestsIMP 
	Timer::Handle handle = SetTimerIMP(id_);

	// create RequestData to insert in map
	RequestData data = {handle, NBD_AWAITING_REPLY, requests};

	return data;
}

// TODO: change requests_ to reference& for for SendWriteRequestsIMP & SendReadRequestsIMP
void Master::SendWriteRequestsIMP(BlockLocations requests_, 
                                  protocols::os::WriteRequest ospRequest_)
{
	//TODO: this function will be used as the encryptor cb

	//TODO: SetTimer and insert to m_writeRequests map before sending
	//1. Timer::Handle handle = SetTimerIMP(id_);
	//insert time handle to map:
	//2.							m_writeRequests[id].data.handle = handle

	//TODO: 3. get map element at m_writeRequests[id]

	//m_writeRequests[id].data.blockLocations == requests_
	//m_writeRequests[id].OspRequest  = ospRequest_

	// send the requests to minion proxys
	for (size_t i = 0; i < requests_.size(); ++i)
	{
		BlockTable::BlockLocation curr = requests_[i];
		minion::WriteRequest minionRequest(ospRequest_, curr.blockOffset);

		// write to log
		stringstream str;
		str << "[Master] calling MinionProxy::WriteReq | minionID = " << \
		curr.minionID << " block = " << curr.blockOffset;
		Log(str.str()); 

		m_minionProxy.WriteReq(minionRequest);
	}
}

void Master::SendReadRequestsIMP(BlockLocations requests_, 
                                 protocols::os::ReadRequest ospRequest_)
{
	//TODO: SetTimer and insert to map before sending

	// send the requests to minion proxys
	for (size_t i = 0; i < requests_.size(); ++i)
	{
		BlockTable::BlockLocation curr = requests_[i];
		minion::ReadRequest minionRequest(ospRequest_, curr.blockOffset);

		// write to log
		stringstream str;
		str << "[Master] calling MinionProxy::ReadReq | minionID = " << \
		curr.minionID << " block = " << curr.blockOffset;
		Log(str.str()); 
		
		m_minionProxy.ReadReq(minionRequest);
	}
}

} // namespace ilrd
