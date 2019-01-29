#include <cassert>
#include <cstring> // std::memcpy
#include <exception>
#include <iostream>
#include <boost/bind.hpp>
#include <sstream> 

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

MasterProxy::MasterProxy(ilrd::Reactor& r_, Minion& m_) : m_minion(m_)
{
    ilrd::Log("Opening udp socket");
    int vdrSock(socket(PF_INET, SOCK_DGRAM, 0));

    if (-1 == vdrSock) // TODO: Maybe create specific exceptions?
    {
        throw std::runtime_error("Error opening socket. Check errno.");
    }

    // Construct minionAddr
    sockaddr_in minionAddr;
    minionAddr.sin_family = AF_INET;
    minionAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    minionAddr.sin_port = htons(SERVER_UDP_PORT);

    ilrd::Log("Binding udp socket to minion address");
    if (!bind(vdrSock, (sockaddr*)(&minionAddr), sizeof(minionAddr)))
    {
        throw std::runtime_error(
            "Error binding minion address to socket. Check errno.");
    }

    r_.AddFD(vdrSock, ilrd::Reactor::READ, boost::bind(&MasterProxy::OnPacketCB, this, _1));
}

void MasterProxy::OnPacketCB(int fd_)
{
	ilrd::Log("MasterProxy got new package!");
    ilrd::protocols::minionUDP::request req;

    ssize_t bytesRead(
					recvfrom(fd_, &req, sizeof(req), 0, NULL,NULL)); // TODO: Should I check who sent?

    if (-1 == bytesRead)
    {
        throw std::runtime_error(
            "Error receiving from ready socket. Check errno.");
    }

    ilrd::protocols::ID reqID(req.ID);

	{
		std::stringstream msg;
		msg << "Package details: ID - " << req.ID << ", type - " << ntohl(req.type) << ", block - " << be64toh(req.blockNum) << std::endl;
		ilrd::Log(msg.str());
	}

    switch (ntohl(req.type)) // TODO: Replace with factory
    {
    case ilrd::protocols::minionUDP::READ:
        m_minion.Read(ilrd::protocols::ID(req.ID), be64toh(req.blockNum));
        break;
    case ilrd::protocols::minionUDP::WRITE:
        boost::shared_ptr<std::vector<char> > dataPtr(
        									    new std::vector<char>(sizeof(req.data)));
        std::memcpy(dataPtr->data(), req.data, sizeof(req.data));
        m_minion.Write(
					ilrd::protocols::ID(req.ID), be64toh(req.blockNum),dataPtr);
        break;
    }
}

} // namespace minion
