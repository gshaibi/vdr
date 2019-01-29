#ifndef MASTER_PROXY_HPP
#define MASTER_PROXY_HPP

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "protocols.hpp"
#include "reactor.hpp"

namespace minion
{

class Minion;

class MasterProxy : boost::noncopyable
{
public:
	explicit MasterProxy(ilrd::Reactor& r_, Minion& m_, const sockaddr_in& vdrAddr_);
	~MasterProxy() noexcept;

	void ReplyRead(const ilrd::protocols::ID& id_, int status_, boost::shared_ptr< std::vector<char> > dataPtr_);
	void ReplyWrite(const ilrd::protocols::ID& id_, int status_);

private:
	void OnPacketCB(int fd_);
	void UnknownRequestImp();
	void SendReplyImp(const ilrd::protocols::minionUDP::reply& rep_);
	void ConstructReplyImp(ilrd::protocols::minionUDP::reply* rep_, const ilrd::protocols::ID& id_, 
														ilrd::protocols::minionUDP::RequestType type_, int status_);

	static const int INCOME_UDP_PORT = 3000;
	
	ilrd::Reactor& m_reactor;
	Minion& m_minion;
	int m_udpSock;
	sockaddr_in m_vdrAddr;
};

} // namespace minion


#endif // MASTER_PROXY_HPP
