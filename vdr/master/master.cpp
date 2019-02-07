#include <cassert>	// assert
#include <arpa/inet.h>	// inet_ntop
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
Master::Master(size_t nMinions_, Reactor& r_, const std::vector<sockaddr_in> minionAddr_)
: m_osPtr(NULL),
	m_minionProxies(),
	m_blockTable(BLOCK_SIZE),
	m_timer(r_),
	m_eventer(r_),
	m_tpool(0),
	m_encryptor(m_eventer, m_tpool),
	m_readRequests(),
	m_writeRequests()
{
	// initialize the proxy minions
	for (int i = 0; i < nMinions_; ++i)
	{
		m_minionProxies.push_back(boost::shared_ptr<MinionProxy>(
									new MinionProxy(i, *this, r_, minionAddr_[i])));
	}
	
	
	// Get config instance
	Singleton<libconfig::Config> cfg;

	// get numthreads from config file
	int numThreads = 0;
	try
	{
		numThreads = cfg.GetInstance().lookup("numThreads");
	}
	catch (const libconfig::SettingNotFoundException& nfex)
    {
		//TODO: write error to log
        std::cerr << "Setting '" << nfex.getPath() << "' is missing from conf file." << std::endl;
    }
	catch (const libconfig::SettingTypeException& tex)
	{
		std::cout << "Setting '" <<  tex.getPath() << "' doesnt match it's type." << std::endl;
	}
	
	//set num threads 
	m_tpool.SetNum(numThreads);

	//start threadpool
	m_tpool.Start();
	
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
	// m_sentRequests[id_] = make_pair(requests, make_pair(handle, SOULD_REPLY_TO_OSP));
	MappedReadRequest to_insert = {data, req_};
	m_readRequests.insert(make_pair(req_.GetID(), to_insert));

	// pass the requests to minion proxys
	SendReadRequestsIMP(req_.GetID());
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

	// encrypt data then SendWriteRequestsIMP
	m_encryptor.Encrypt(boost::bind(&Master::SendWriteRequestsIMP,this, req_.GetID()), 
						const_cast<char*>(req_.GetData()), BLOCK_SIZE);
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

	ReplyStatus status = ProcessReadReplyIMP(rep_.GetID(), rep_.GetMinionID());
	
	// write to log
	str.str(""); 
	str << "[Master] ReplyStatus = " << status; 
	Log(str.str());

	//reply to nbd if haven't replied yet
	if (SOULD_REPLY_TO_OSP == status)
	{
		//Decrypt befor send reply to os
		Log("[Master] calling Decrypt");

		m_encryptor.Decrypt(boost::bind(&Master::ReadReplyToOsProxyIMP, this, rep_),
							const_cast<char*>(&rep_.GetData()->at(0)), BLOCK_SIZE);
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

	ReplyStatus status = ProcessWriteReplyIMP(rep_.GetID(), rep_.GetMinionID());

	// write to log
	str.str("");
	str << "[Master] ReplyStatus = " << status;
	Log(str.str());

	//reply to nbd if haven't replied yet
	if (SOULD_REPLY_TO_OSP == status)
	{
		os::WriteReply ospReply(rep_.GetID(), rep_.GetStatus()); 

		// write to log
		stringstream str;
		str << "[Master] calling OsProxy::ReplyWrite | status = " << rep_.GetStatus();
		Log(str.str());

		m_osPtr->ReplyWrite(ospReply);
	}
}

////ReadReplyToOsProxyIMP is passed as a callback to Decrypt
void Master::ReadReplyToOsProxyIMP(protocols::minion::ReadReply rep_)
{
	os::ReadReply ospReply(rep_.GetID(), rep_.GetStatus(), rep_.GetData());

	// write to log
	stringstream str;
	str << "[Master] calling OsProxy::ReplyRead | status = " << rep_.GetStatus();
	Log(str.str());

	m_osPtr->ReplyRead(ospReply);
}

// TODO: don't need this func?
//used in Master::Read & Master::Write
// Master::ReplyStatus Master::ProcessReplyIMP(protocols::ID id_, size_t minionID_) 

Master::ReplyStatus Master::ProcessReadReplyIMP(protocols::ID id_, size_t minionID_) 
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
		return DONT_REPLY_YET;
	}

	MappedReadRequest request = (*found).second;
	BlockLocations bl = request.data.blockLocations;

	// find minionID_ in vector
	for (Iterator iter(bl.begin()); iter != bl.end(); ++iter)
	{
		// if found minion - remove from map & return SOULD_REPLY_TO_OSP
		if (minionID_ == (*iter).minionID)
		{
			Log("[Master] removing ID from map & canceling timer");

			// remove from map
			m_readRequests.erase(id_);
			
			// write to log
			{
				stringstream str;
				str << "[Master] cancel timer handle no.  " << request.data.handle;
				Log(str.str());
			}
		
			m_timer.Cancel(request.data.handle);

			Log("[Master] returning SOULD_REPLY_TO_OSP");

			return SOULD_REPLY_TO_OSP;
		}
	}

	// if didn't find minion in vector - ignore it
	Log("[Master] ERROR: request ID is in ReadMap but minionID is not in vector - ignoring the reply");

	return DONT_REPLY_YET; 
}

Master::ReplyStatus Master::ProcessWriteReplyIMP(protocols::ID id_, size_t minionID_) 
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
		return DONT_REPLY_YET;
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

			// if vector is empty - remove ID from map & cancel timer & return SOULD_REPLY_TO_OSP
			if (bl.empty())
			{
				Log("[Master] vector empty - removing ID from map & canceling timer");
				m_writeRequests.erase(id_);
			
				{
					str.str("");
					str << "[Master] cancel timer handle no.  " << request.data.handle;
					Log(str.str());
				}
			
				m_timer.Cancel(request.data.handle);

				Log("[Master] returning SOULD_REPLY_TO_OSP");
				return SOULD_REPLY_TO_OSP;
			}

			break;
		}
	}


	// if didn't find minion in vector - ignore it
	Log("[Master] ERROR: request ID is in WriteMap but minionID is not in vector - ignoring the reply");

	return DONT_REPLY_YET; 
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

	// resend the requests to all minion proxys that haven't replied yet
	SendReadRequestsIMP(id_); // will also reset timer
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
	SendWriteRequestsIMP(id_); //will also reset the timer
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

	// create RequestData to insert in map
	RequestData data = {0, requests}; //TODO: 0 is invalid val

	return data;
}

// TODO: change requests_ to reference& for for SendWriteRequestsIMP & SendReadRequestsIMP
//SendWriteRequestsIMP is passed as a callback to Encrypt
void Master::SendWriteRequestsIMP(protocols::ID id_)
{
	assert(m_writeRequests.end() != m_writeRequests.find(id_));
	
	WriteIterator found(m_writeRequests.find(id_));

	BlockLocations requests = (*found).second.data.blockLocations;

	// Set timer & update in map
	(*found).second.data.handle = SetTimerIMP(id_);

	// send the requests to minion proxys
	for (size_t i = 0; i < requests.size(); ++i)
	{
		BlockTable::BlockLocation curr = requests[i];
		minion::WriteRequest minionRequest((*found).second.ospRequest, curr.blockOffset);

		// write to log
		stringstream str;
		str << "[Master] calling MinionProxy::WriteReq | minionID = " << \
		curr.minionID << " block = " << curr.blockOffset;
		Log(str.str()); 

		m_minionProxies[curr.minionID]->WriteReq(minionRequest);
	}
}

void Master::SendReadRequestsIMP(protocols::ID id_)
{
	assert(m_readRequests.end() != m_readRequests.find(id_));
	
	ReadIterator found(m_readRequests.find(id_));

	BlockLocations requests = (*found).second.data.blockLocations;

	// Set timer & update in map
	(*found).second.data.handle = SetTimerIMP(id_);

	// send the requests to minion proxys
	for (size_t i = 0; i < requests.size(); ++i)
	{
		BlockTable::BlockLocation curr = requests[i];
		minion::ReadRequest minionRequest((*found).second.ospRequest, curr.blockOffset);

		// write to log
		stringstream str;
		str << "[Master] calling MinionProxy::ReadReq | minionID = " << \
		curr.minionID << " block = " << curr.blockOffset;
		Log(str.str()); 
		
		m_minionProxies[curr.minionID]->ReadReq(minionRequest);
	}
}

} // namespace ilrd
