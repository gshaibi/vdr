#ifndef MINION_PROXY_HPP
#define MINION_PROXY_HPP

#include <netinet/ip.h> /* struct sockaddr_in */

#include <vector> //using std::vector

#include <boost/noncopyable.hpp> //using boost::noncopyable
#include <boost/shared_ptr.hpp> //using boost::shared_ptr

#include "protocols.hpp" //using minionMsgProtocol

namespace ilrd
{

class Reactor;
class Master;

class MinionProxy :private boost::noncopyable
{
public:
// Minion Proxy Write/Read Rquest methods might throw runtime_error in case of 
// a network operation faliuere (e.g - send)

	MinionProxy(int minionID_, Master& master_, Reactor& reac_, const sockaddr_in& minionAddr_);
	~MinionProxy(); //close udp socket
	
	void ReadReq(protocols::minion::ReadRequest req_);
	void WriteReq(protocols::minion::WriteRequest req_);

private:
	typedef protocols::minionUDP::request UDPRequest;
	typedef protocols::minionUDP::reply UDPReply;
	typedef protocols::minion::ReadRequest ReadRequest; 
	typedef protocols::minion::WriteRequest WriteRequest;
	typedef protocols::ID ID;

	void RegisterToReactorIMP();
	int CreateUDPSocketIMP();
	UDPRequest CreateUdpRequestIMP(protocols::minionUDP::RequestType type_, const ID& id_, size_t blockNum_) const;
	void SendRequestIMP(UDPRequest req_);

	void RecieveFromMinionCB(int socket_); //reactor callback

	static const int UDP_PORT = 5000;
	const int m_minionID;
	Master& m_master;
	Reactor& m_reactor;
	int m_minionSocket;
	sockaddr_in m_minionAddr;
};

} //namspace ilrd

#endif //MINION_PROXY_HPP