#ifndef MINION_PROXY_HPP
#define MINION_PROXY_HPP

#include <vector> //using std::vector

#include <boost/noncopyable.hpp> //using boost::noncopyable
#include <boost/shared_ptr.hpp> //using boost::shared_ptr

#include "msg_protocol.hpp" //using minionMsgProtocol

namespace ilrd
{

class MinionProxy :private boost::noncopyable
{
public:
	MinionProxy(int minionID_, size_t numBlocks_, size_t blockSize_);
	~MinionProxy(); //delete memory
	
	void ReadRequest(minionMsgProtocol::ReadRequest req_);
	void WriteRequest(minionMsgProtocol::WriteRequest req_);

private:
	int m_minionID;
	char *m_memory;
	size_t m_blockSize;
};

} //namspace ilrd


#endif //MINION_PROXY_HPP