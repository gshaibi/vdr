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
	MinionProxy(int minionID_, Master& master_, Reactor& reac_);
	~MinionProxy(); //delete memory
	
	void ReadRequest(protocols::minion::ReadRequest req_);
	void WriteRequest(protocols::minion::WriteRequest req_);

private:
	void RegisterToReactorIMP();
	int CreateUDPSocketIMP();

	void RecieveFromMinionCB(int socket_);
	void SendToMinionIMP(int socket_);

	static const int UDP_PORT =  3000;
	const int m_minionID;
	Master& m_master;
	Reactor& m_reactor;
	int m_minionSocket;
};

} //namspace ilrd

#endif //MINION_PROXY_HPP