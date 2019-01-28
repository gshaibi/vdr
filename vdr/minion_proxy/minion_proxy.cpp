#include <cassert> //using assert
#include <arpa/inet.h> //using htonl
#include <sys/socket.h> //using socket

#include "minion_proxy.hpp" //minion proxy declarations

#include "reactor.hpp" //using reactor
#include "master.hpp" //using master

namespace ilrd
{

MinionProxy::MinionProxy(int minionID_, Master& master_, Reactor& reactor_):
														m_minionID(minionID_),
														m_master(master_),
														m_reactor(reactor_),
														m_minionSocket(CreateUDPSocketIMP())
{
	//CreateUdpSocketIMP()
	//Register to the reactor
}



void MinionProxy::RegisterToReactorIMP()
{
	m_reactor.AddFD(m_minionSocket, Reactor::READ, RecieveFromMinionCB);
}

int MinionProxy::CreateUDPSocketIMP()
{
	int udpSock = socket(PF_INET, SOCK_DGRAM, 0);
	
	sockaddr_in udpIncome;
  	udpIncome.sin_family = AF_INET;
  	udpIncome.sin_addr.s_addr = htonl(INADDR_ANY);
  	udpIncome.sin_port = htons(UDP_PORT);

	bind(udpSock, (sockaddr *)&udpIncome, sizeof(udpIncome));

	return udpSock;
}

void MinionProxy::RecieveFromMinionCB(int socket_)
{
	//read the udp massage from socket to buffer
	int n_bytes = 0;
	char *buffer(new char[sizeof(protocols::minionUDP::reply)]);

	n_bytes = recvfrom(socket_, &buffer, sizeof(protocols::minionUDP::reply), MSG_DONTWAIT, 
																	NULL, NULL);
	if(-1 == n_bytes)
	{
		perror("recvfrom");
	}

	

	
}

void MinionProxy::SendToMinionIMP()
{
	
}

}//namespace ilrd