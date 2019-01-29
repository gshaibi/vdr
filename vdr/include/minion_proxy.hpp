#ifndef MINION_PROXY_HPP
#define MINION_PROXY_HPP

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

	MinionProxy(int minionID_, Master& master_, Reactor& reac_);
	~MinionProxy(); //close udp socket
	
	void ReadRequest(protocols::minion::ReadRequest req_);
	void WriteRequest(protocols::minion::WriteRequest req_);

private:
	typedef protocols::minionUDP::request UDPRequest;
	typedef protocols::minionUDP::reply UDPReply;
	typedef protocols::minion::ReadRequest ReadRequest; 
	typedef protocols::minion::WriteRequest WriteRequest;
	typedef protocols::minionUDP::RequestType RequestType;
	typedef protocols::ID ID;

	void RegisterToReactorIMP();
	int CreateUDPSocketIMP();
	UDPRequest CreateUdpRequestIMP(RequestType type_, const ID& id_, size_t blockNum_) const;

	void RecieveFromMinionCB(int socket_); //reactor callback

	static const int UDP_PORT =  3000;
	const int m_minionID;
	Master& m_master;
	Reactor& m_reactor;
	int m_minionSocket;
};

} //namspace ilrd

#endif //MINION_PROXY_HPP