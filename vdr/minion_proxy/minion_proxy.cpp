#include <cassert>		//using assert
#include <arpa/inet.h>  //using htonl
#include <sys/socket.h> //using socket
#include <cstring>		//using memcpy
#include <cstdio>		//using perror
#include <cerrno>		//errno

#include <boost/bind.hpp> //using boost bind

#include "minion_proxy.hpp" //minion proxy declarations

#include "reactor.hpp" //using reactor
#include "master.hpp"  //using master
#include "logger.hpp" //using logger

namespace ilrd
{

MinionProxy::MinionProxy(int minionID_, Master &master_, 
								Reactor &reactor_, const sockaddr_in& minionAddr_) : 
																m_minionID(minionID_),
																m_master(master_),
																m_reactor(reactor_),
																m_minionSocket(CreateUDPSocketIMP()),
																m_minionAddr(minionAddr_)
{
	//register a udp recieve callback in the reactor
	RegisterToReactorIMP();
}

MinionProxy::~MinionProxy()
{
	m_reactor.RemFD(m_minionSocket, ilrd::Reactor::READ);

	close(m_minionSocket);
}

void MinionProxy::ReadReq(ReadRequest req_)
{
	ilrd::Log("[MinionProxy] read request");

	//fill the request protocol struct
	UDPRequest request = CreateUdpRequestIMP(protocols::minionUDP::READ, req_.GetID(),
											 req_.GetBlock());

	{
		std::stringstream msg;
		msg << "[MinionProxy] read request to block number - " << req_.GetBlock() <<std::endl;
		ilrd::Log(msg.str());
	}
	//send request to minion
	SendRequestIMP(request);
}

void MinionProxy::WriteReq(WriteRequest req_)
{
	ilrd::Log("[MinionProxy] write request");
	//fill the request protocol struct
	UDPRequest request = CreateUdpRequestIMP(protocols::minionUDP::WRITE, req_.GetID(),
											 req_.GetBlock());
	
	{
		std::stringstream msg;
		msg << "[MinionProxy] write request to block number - " << req_.GetBlock() <<std::endl;
		ilrd::Log(msg.str());
	}

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
								 (sockaddr *)&m_minionAddr, sizeof(m_minionAddr)))
	{
		ilrd::Log("[MinionProxy] sendto failed");
		throw std::runtime_error(strerror(errno));
	}
}


void MinionProxy::RegisterToReactorIMP()
{
	ilrd::Log("[MinionProxy] add socket to reactor");
	m_reactor.AddFD(m_minionSocket, Reactor::READ, boost::bind(&MinionProxy::RecieveFromMinionCB, this, _1));
}

int MinionProxy::CreateUDPSocketIMP()
{
	ilrd::Log("[MinionProxy] create udp socket");

	int udpSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == udpSock)
	{
		ilrd::Log("[MinionProxy] socket creation failed");
		throw std::runtime_error(strerror(errno));
	}

	sockaddr_in udpIncome;
	udpIncome.sin_family = AF_INET;
	//TODO: currently address set to any address
	udpIncome.sin_addr.s_addr = htonl(INADDR_ANY);
	udpIncome.sin_port = htons(UDP_PORT);

	ilrd::Log("[MinionProxy] Binding udp socket to minion address");
	if (-1 == bind(udpSock, (sockaddr *)&udpIncome, sizeof(udpIncome)))
	{
		ilrd::Log("[MinionProxy] bind socket failed");
		throw std::runtime_error(strerror(errno));
	}

	return udpSock;
}

void MinionProxy::RecieveFromMinionCB(int socket_)
{
	int n_bytes = 0;
	UDPReply minionRep;

	ilrd::Log("[MinionProxy] recieving respond from minion");

	//read the udp massage from socket to minionUDP reply struct
	n_bytes = recvfrom(socket_, &minionRep, sizeof(minionRep), MSG_DONTWAIT,
					   NULL, NULL);
	if (-1 == n_bytes)
	{
		ilrd::Log("[MinionProxy] recvfrom failed");
		throw std::runtime_error(strerror(errno));
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

		{
			std::stringstream msg;
			msg << "[MinionProxy] Packet details: type - READ" << " packet size " << sizeof(minionRep)<<std::endl;
			ilrd::Log(msg.str());
		}

		//send master the reply
		ilrd::Log("[MinionProxy] sending packet to master");
		m_master.ReplyReadIMP(protocols::minion::ReadReply(protocols::ID(minionRep.ID), 0, ntohl(minionRep.status), buffer));

		break;
	}

	case protocols::minionUDP::WRITE:
	{
		{
			std::stringstream msg;
			msg << "[MinionProxy] Packet details: type - WRITE" << " packet size " << sizeof(minionRep)<<std::endl;
			ilrd::Log(msg.str());
		}

		//send master the reply
		ilrd::Log("[MinionProxy] sending packet to master");
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