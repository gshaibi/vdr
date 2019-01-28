#include <iostream>
#include <cassert>
#include <exception>

#include <sys/types.h> // ssize_t
#include <sys/socket.h>
#include <netinet/ip.h> //sockaddr_in

#include "routines.hpp"
#include "master_proxy.hpp"
#include "logger.hpp"
#include "protocols.hpp"

namespace minion
{

MasterProxy::MasterProxy(ilrd::Reactor& r_, Minion& m_) : m_minion(m_) 
{
	ilrd::Log("Opening udp socket");
	int vdrSock(socket(PF_INET, SOCK_DGRAM, NULL));
	
	if (-1 == vdrSock) //TODO: Maybe create specific exceptions?
	{
		throw std::runtime_error("Error opening socket. Check errno.");
	}

	// Construct minionAddr 
	sockaddr_in minionAddr;
	minionAddr.sin_family = AF_INET;
  	minionAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  	minionAddr.sin_port = htons(SERVER_UDP_PORT);
	
	ilrd::Log("Binding udp socket to minion address");
	if (!bind(vdrSock, (sockaddr *)(&minionAddr), sizeof(minionAddr)))
	{
		throw std::runtime_error("Error binding minion address to socket. Check errno.");
	}

	r_.AddFD(vdrSock, ilrd::Reactor::READ, OnPacketCB);
}

void MasterProxy::OnPacketCB(int fd_)
{
	ilrd::protocols::minionUDP::request req;

	ssize_t bytesRead(recvfrom(fd_, &req, sizeof(req), NULL, NULL, NULL)); //TODO: Should I check who sent?

	if (-1 == bytesRead)
	{
		throw std::runtime_error("Error receiving from ready socket. Check errno.");
	}

	switch(ntohl(req.reqType)) //TODO: Replace with factory
	{
		case ilrd::protocols::minionUDP::REQ_READ:
			//FIXME: Fill when Minion header ready.
			// m_minion.Read() 
			break;
		case ilrd::protocols::minionUDP::REQ__WRITE:
			//	m_minion.Write() 
			break;
	}
}


} // ilrd
