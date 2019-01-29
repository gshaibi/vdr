#include <cassert>
#include <cstring> // std::memcpy
#include <exception>
#include <iostream>
#include <boost/bind.hpp>
#include <sstream> 
#include <cerrno> // errno

#include <netinet/ip.h> //sockaddr_in
#include <sys/socket.h>
#include <sys/types.h> // ssize_t
#include <endian.h> // be64toh

#include "logger.hpp"
#include "minion.hpp"
#include "protocols.hpp"
#include "routines.hpp"
#include "master_proxy.hpp"

namespace minion
{

MasterProxy::MasterProxy(ilrd::Reactor& r_, Minion& m_, const sockaddr_in& vdrAddr_) : 
	m_reactor(r_), m_minion(m_), m_udpSock(), m_vdrAddr(vdrAddr_)
{
    ilrd::Log("Opening udp socket");
    m_udpSock = socket(PF_INET, SOCK_DGRAM, 0);

    if (-1 == m_udpSock) // TODO: Maybe create specific exceptions?
    {
		ilrd::Log("Failed opening socket.");
        throw std::runtime_error(strerror(errno));
    }

    // Construct minionAddr
    sockaddr_in minionAddr;
    minionAddr.sin_family = AF_INET;
    minionAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    minionAddr.sin_port = htons(INCOME_UDP_PORT);

	int enable = 1;
	if (setsockopt(m_udpSock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
		ilrd::Log("setsockopt(SO_REUSEADDR) failed");
	}

    ilrd::Log("Binding udp socket to minion address");
    if (0 != bind(m_udpSock, (sockaddr*)(&minionAddr), sizeof(minionAddr)))
    {
		ilrd::Log("Failed binding address to socket.");
		close(m_udpSock);
        throw std::runtime_error(strerror(errno));
	}

    r_.AddFD(m_udpSock, ilrd::Reactor::READ, boost::bind(&MasterProxy::OnPacketCB, this, _1));
}

MasterProxy::~MasterProxy() noexcept
{
	close(m_udpSock);
	m_reactor.RemFD(m_udpSock, ilrd::Reactor::READ);
}

void MasterProxy::ReplyRead(const ilrd::protocols::ID& id_, int status_, boost::shared_ptr< std::vector<char> > dataPtr_)
{
	ilrd::protocols::minionUDP::reply rep; 
	ConstructReplyImp(&rep, id_, ilrd::protocols::minionUDP::READ, status_);
	std::memcpy(rep.data, dataPtr_->data(), sizeof(rep.data));

	SendReplyImp(rep);
}

void MasterProxy::ReplyWrite(const ilrd::protocols::ID& id_, int status_)
{
	ilrd::protocols::minionUDP::reply rep;
	ConstructReplyImp(&rep, id_, ilrd::protocols::minionUDP::WRITE, status_);
	SendReplyImp(rep);
}

void MasterProxy::ConstructReplyImp(ilrd::protocols::minionUDP::reply* rep_, 
				const ilrd::protocols::ID& id_, ilrd::protocols::minionUDP::RequestType type_, int status_)
{
	std::memcpy(rep_->ID, id_.GetID(), sizeof(rep_->ID));
	rep_->type = htonl(type_);
	rep_->status = htonl(status_);
}

void MasterProxy::SendReplyImp(const ilrd::protocols::minionUDP::reply& rep_)
{
	sendto(m_udpSock, &rep_, sizeof(rep_), MSG_DONTWAIT, (sockaddr *)&m_vdrAddr, sizeof(m_vdrAddr));
}

void MasterProxy::OnPacketCB(int fd_)
{
	ilrd::Log("MasterProxy got new package!");
    ilrd::protocols::minionUDP::request req;

    ssize_t bytesRead(
					recvfrom(fd_, &req, sizeof(req), MSG_DONTWAIT, NULL,NULL)); // TODO: Should I check who sent?

	assert(bytesRead == sizeof(req));

    if (-1 == bytesRead)
    {
        throw std::runtime_error(
            "Error receiving from ready socket. Check errno.");
    }

    ilrd::protocols::ID reqID(req.ID);

	{
		std::stringstream msg;
		msg << "Package details: ID - " << req.ID << ", type - "
				 << ntohl(req.type) << ", block - " 
				 << be64toh(req.blockNum) << std::endl;
		ilrd::Log(msg.str());
	}
        

    switch (ntohl(req.type)) // TODO: Replace with factory
    {
    case ilrd::protocols::minionUDP::READ:
	{
        m_minion.Read(ilrd::protocols::ID(req.ID), be64toh(req.blockNum));
        break;
	}
    case ilrd::protocols::minionUDP::WRITE:
	{
		boost::shared_ptr<std::vector<char> > dataPtr(
													new std::vector<char>(sizeof(req.data)));
        std::memcpy(dataPtr->data(), req.data, sizeof(req.data));
        m_minion.Write(
					ilrd::protocols::ID(req.ID), be64toh(req.blockNum),dataPtr);
	}
        break;
	default:
		UnknownRequestImp();
    }
}

void MasterProxy::UnknownRequestImp()
{
	throw std::runtime_error("Recieved Unknown request");
}

} // namespace minion
