#include <boost/bind.hpp>
#include <cassert>
#include <cerrno>
#include <exception>
#include <iostream>
#include <sys/socket.h> //recv
#include <sys/types.h>  //recv

#include "logger.hpp"
#include "routines.hpp"
#include "tcp_reader.hpp"

namespace ilrd
{

TcpReader::TcpReader(
    int fd_, size_t headerSize_, Reactor &r_,
    boost::function<size_t(SharedBuffer)> onHeader_,
    boost::function<void(SharedBuffer)> onPacket_)
    : m_onHeader(onHeader_), m_onPacket(onPacket_), m_r(r_),
      m_packet(new std::vector<char>(headerSize_)), m_headerSize(headerSize_),
      m_readenCount(0), m_fd(fd_)
{
	Log("Constructing TcpReader");
    m_r.AddFD(m_fd, Reactor::READ,
              boost::bind<void>(&TcpReader::Read, this, _1));
}

TcpReader::~TcpReader() noexcept 
{ 
	Log("Destructing TcpReader");
	m_r.RemFD(m_fd, Reactor::READ); 
}

void TcpReader::Read(int fd_)
{
	Log("In TcpReader::Read");
    if (m_readenCount < m_headerSize) // haven't read the whole header yet.
        {
            DEBUG(std::cout << "Getting header" << std::endl;)

            // non blocking. fd_ must have something to recv
            ssize_t received = recv(fd_, &(*m_packet)[m_readenCount],
                                    					m_headerSize - m_readenCount, 0);

            if (-1 == received)
                {
                    switch (errno)
                        {
                        case EINTR:
                            return;
                        case EBADF:
                        case ENOTSOCK:
                            throw(std::runtime_error(
                                "Bad file descriptor in TcpReader::Read"));
                        default:
                            throw(std::runtime_error(
                                "Unexpected error. Check errno"));
                        }
                }

            m_readenCount += received;

            DEBUG(std::cout << "Got " << m_readenCount << " bytes."
                            << std::endl;)

            if (m_readenCount < m_headerSize)
                {
					Log("Got part of header.");

                    return;
                }
			
			Log("Got full header");
			size_t packetSize = m_onHeader(m_packet);
			DEBUG(std::cerr << "Packet size is " << packetSize << std::endl;)
            m_packet->resize(packetSize);
        }

	DEBUG(std::cout << "Trying to receive " << m_packet->size() - m_readenCount << " bytes of data." << std::endl;)

	if (m_packet->size() != m_headerSize)
	{
		int received = recv(fd_, &(*m_packet)[m_readenCount],
												m_packet->size() - m_readenCount, 
												MSG_DONTWAIT);

		DEBUG(std::cout << "Got " << received << " bytes OF DATA" << std::endl);

		//TODO: Fix condition
		if (-1 == received)
			{
				switch (errno)
					{
					case EINTR:
						Log("Got interrupted by signal");
						return;
					case EAGAIN:
						Log("Blocking recv preempted");
						return;
					case EBADF:
					case ENOTSOCK:
						throw(std::runtime_error(
							"Bad file descriptor in TcpReader::Read"));
					default:
						throw(std::runtime_error("Unexpected error. Check errno"));
					}
			}

    m_readenCount += received;
    DEBUG(std::cout << "Got " << m_readenCount << " bytes OF DATA in TOTAL" << std::endl;)
	}

    if (m_readenCount == m_packet->size())
        {
            DEBUG(std::cout << "Got full packet" << std::endl;)
            m_onPacket(m_packet);
            m_readenCount = 0;
        }
    DEBUG(std::cout << "Continuing..." << std::endl;)
}

} // namespace ilrd
