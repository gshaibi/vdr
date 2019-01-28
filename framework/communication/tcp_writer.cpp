#include <iostream>
#include <cassert>
#include <boost/bind.hpp>
#include <sys/socket.h>
#include <cerrno>
#include <cstdio>
#include <exception>


#include "logger.hpp"
#include "routines.hpp"
#include "tcp_writer.hpp"

namespace ilrd
{

TcpWriter::TcpWriter(int fd_, Reactor &r_) : m_r(r_), m_writtenCount(0), m_fd(fd_), m_packets()
{
	Log("Constructing TcpWriter");
}

void TcpWriter::Write(SharedBuffer packet_)
{
	if (m_packets.empty())
	{
		Log("Adding WriteCB to reactor");
		m_r.AddFD(m_fd, Reactor::WRITE, boost::bind(&TcpWriter::WriteCB, this, _1));
	}
	Log("Pushing packet writing task to write queue");
	m_packets.push(packet_);
}

void TcpWriter::WriteCB(int fd_)
{
	Log("Write fd is ready - In WriteCB");
	assert(m_fd == fd_);
	// send to the fd the next packet from m_writtenCount idx.
	// update m_writtenCount
	DEBUG(std::cerr << "Trying to send" <<  m_packets.front()->size() - m_writtenCount << " bytes" << std::endl;)

	DEBUG(std::cerr << "m_writtenCount is " << m_writtenCount << std::endl;)

	ssize_t sentCount = send(m_fd, &(*(m_packets.front()))[m_writtenCount], 
														m_packets.front()->size() - m_writtenCount, MSG_DONTWAIT | MSG_NOSIGNAL);
	Log("Sent bytes.");
	DEBUG(std::cerr << "Sent " << sentCount << " bytes." << std::endl;)

	if (-1 == sentCount)
	{
		switch(errno) 
		{
			case EAGAIN:
			case EINTR:
				return;
			case EBADF:
				throw std::runtime_error("Bad file descriptor in TcpWriter::WriteCB");
			default:
				perror("");
				throw std::runtime_error("Unexpected error. check errno");
		}
	}

	m_writtenCount += sentCount;

	DEBUG(std::cout << "Sent " << m_writtenCount << "bytes in TOTAL" << std::endl;)

	if (m_writtenCount == m_packets.front()->size())
	{
		DEBUG(std::cout << "Sent full packet" << std::endl;)
		m_writtenCount = 0;
		Log("Popping write task");
		m_packets.pop();

		if (m_packets.empty())
		{
			Log("Write queue is empty. Removing from reactor");
			m_r.RemFD(m_fd, Reactor::WRITE);
		}
	}
}

} // ilrd
