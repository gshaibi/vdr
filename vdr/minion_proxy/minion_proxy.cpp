#include <cassert>		//using assert
#include <arpa/inet.h>  //using htonl
#include <sys/socket.h> //using socket
#include <cstring>		//using memcpy
#include <cstdio>		//using perror

#include <boost/bind.hpp> //using boost bind

#include "minion_proxy.hpp" //minion proxy declarations

#include "reactor.hpp" //using reactor
#include "master.hpp"  //using master

namespace ilrd
{

MinionProxy::MinionProxy(int minionID_, Master &master_, Reactor &reactor_) : m_minionID(minionID_),
																			  m_master(master_),
																			  m_reactor(reactor_),
																			  m_minionSocket(CreateUDPSocketIMP())
{
	//register a udp recieve callback in the reactor
	RegisterToReactorIMP();
}

MinionProxy::~MinionProxy()
{
	close(m_minionSocket);
}

void MinionProxy::ReadReq(ReadRequest req_)
{
	//fill the request protocol struct
	UDPRequest request = CreateUdpRequestIMP(protocols::minionUDP::READ, req_.GetID(),
											 req_.GetBlock());
	//send request to minion
	SendRequestIMP(request);
}

void MinionProxy::WriteReq(WriteRequest req_)
{
	//fill the request protocol struct
	UDPRequest request = CreateUdpRequestIMP(protocols::minionUDP::WRITE, req_.GetID(),
											 req_.GetBlock());
	//copy the data to the udp request member
	memcpy(request.data, req_.GetData(), protocols::minionUDP::BLK_SIZE);

	//send request to minion
	SendRequestIMP(request);
}

MinionProxy::UDPRequest MinionProxy::CreateUdpRequestIMP(protocols::minionUDP::RequestType type_,
														 const ID &id_,
														 size_t blockNum_) const
{
	UDPRequest request;

	request.type = htonl(type_);
	memcpy(&request.ID, id_.GetID(), protocols::minionUDP::ID_SIZE);
	request.blockNum = htobe64(blockNum_);

	return request;
}

void MinionProxy::SendRequestIMP(UDPRequest req_)
{
	//send request to minion
	if (sizeof(req_) != sendto(m_minionSocket, &req_, sizeof(req_), MSG_DONTWAIT,
								  NULL, 0))
	{
		throw std::runtime_error("[MinionProxy] sendto failed");
	}
}


void MinionProxy::RegisterToReactorIMP()
{
	m_reactor.AddFD(m_minionSocket, Reactor::READ, boost::bind(&MinionProxy::RecieveFromMinionCB, this, _1));
}

int MinionProxy::CreateUDPSocketIMP()
{
	int udpSock = socket(PF_INET, SOCK_DGRAM, 0);
	if (-1 == udpSock)
	{
		throw std::runtime_error("[MinionProxy] socket creation failed");
	}

	sockaddr_in udpIncome;
	udpIncome.sin_family = AF_INET;
	//TODO: currently address set to any address
	udpIncome.sin_addr.s_addr = htonl(INADDR_ANY);
	udpIncome.sin_port = htons(UDP_PORT);

	if (-1 == bind(udpSock, (sockaddr *)&udpIncome, sizeof(udpIncome)))
	{
		throw std::runtime_error("[MinionProxy] bind socket failed");
	}

	return udpSock;
}

void MinionProxy::RecieveFromMinionCB(int socket_)
{
	int n_bytes = 0;
	UDPReply minionRep;

	//read the udp massage from socket to minionUDP reply struct
	n_bytes = recvfrom(socket_, &minionRep, sizeof(minionRep), MSG_DONTWAIT,
					   NULL, NULL);
	if (-1 == n_bytes)
	{
		throw std::runtime_error("[MinionProxy] recvfrom failed");
	}

	//TODO: handle incomplete packet
	assert(n_bytes == sizeof(minionRep));

	// case swith on reply type
	switch (ntohl(minionRep.type))
	{
	case protocols::minionUDP::READ:
	{
		//create buffer
		protocols::minion::ReadReply::SharedBuffer buffer(new std::vector<char>(protocols::minionUDP::BLK_SIZE));

		//copy data to buffer
		//had to remove constness
		memcpy(const_cast<char *>(buffer->data()), minionRep.data, protocols::minionUDP::BLK_SIZE);

		//send master the reply
		m_master.ReplyReadIMP(protocols::minion::ReadReply(protocols::ID(minionRep.ID), 0, ntohl(minionRep.status), buffer));

		break;
	}

	case protocols::minionUDP::WRITE:
	{
		//reply to master that write
		m_master.ReplyWriteIMP(protocols::minion::WriteReply(protocols::ID(minionRep.ID), 0, ntohl(minionRep.status)));
		break;
	}

	default:
	{
		throw std::runtime_error("[MinionProxy] recieved unknown udp packet");
	}
	}
}

} //namespace ilrd