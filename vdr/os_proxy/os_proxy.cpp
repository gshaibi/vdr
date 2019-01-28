#include <boost/bind.hpp>
#include <cassert>
#include <iostream>
#include <cerrno>
#include <cstdio> //perror
#include <boost/thread.hpp>
#include <boost/thread/scoped_thread.hpp>
#include <boost/chrono.hpp>

#include <arpa/inet.h> //ntohl
#include <endian.h> //be64toh
#include <fcntl.h>  //open
#include <linux/nbd.h>
#include <sys/ioctl.h>
#include <sys/socket.h> //recv
#include <sys/stat.h>   //open
#include <sys/types.h>  //recv

#include "os_proxy.hpp"
#include "master.hpp"
#include "routines.hpp"
#include "logger.hpp"

namespace ilrd
{

void *OsProxy::NbdDoItIMP(boost::shared_ptr<int> nbdFd)
{
	Log("Entering NBD_DO_IT");
    DEBUG(sleep(1));
    while (-1 == ioctl(*nbdFd, NBD_DO_IT))
    {
		DEBUG(static int counter = 0;)
        switch (errno)
        {
        case EINTR:
            continue;
        default:
            Log("NbdDoIt error");
            perror("");
			DEBUG(if (30 == ++counter) {return NULL;})
        }
    }
    Log("Exiting NbdDoIt thread");
	return NULL;
}

void OsProxy::CloseNbdIMP(int *nbdFd)
{
    if (-1 == ioctl(*nbdFd, NBD_CLEAR_QUE))
    {
        std::cerr << "error in nbd cleanup" << std::endl;
    }

    if (-1 == ioctl(*nbdFd, NBD_CLEAR_SOCK))
    {
        std::cerr << "error in nbd cleanup" << std::endl;
    }
    
    close(*nbdFd);

    delete nbdFd;
}

void OsProxy::InitNbdIMP(size_t deviceNumBlocks_, const std::string &devicePath_,
                      int socket_)
{
	Log("Opening nbd file");
    boost::shared_ptr<int> nbdFd(new int(open(devicePath_.c_str(), O_RDWR)), CloseNbdIMP);

    if (-1 == *nbdFd)
    {
        throw std::runtime_error("nbd opening error. check errno.");
    }

	Log("Clearing Nbd socket");
    if (-1 == ioctl(*nbdFd, NBD_CLEAR_SOCK))
    {
        throw std::runtime_error("ioctl error. check errno.");
    }

	//FIXME: log
	std::stringstream s;
	s << "Setting nbd socket to " << socket_;
	Log(s.str());
    if (-1 == ioctl(*nbdFd, NBD_SET_SOCK, socket_))
    {
        throw std::runtime_error("ioctl error. check errno.");
    }

	Log("Setting Timeout of nbd to infinity");
	if (-1 == ioctl(*nbdFd, NBD_SET_TIMEOUT, 99999999))
	{
		throw std::runtime_error("ioctl error. check errno.");
	}

	Log("Setting block size");
	if (-1 == ioctl(*nbdFd, NBD_SET_BLKSIZE, 0x1000))
	{
		throw std::runtime_error("ioctl error. check errno.");
	}

	Log("Setting num blocks");
	if (-1 == ioctl(*nbdFd, NBD_SET_SIZE_BLOCKS, deviceNumBlocks_))
	{
		throw std::runtime_error("ioctl error. check errno.");
	}

	Log("Creating nbdRespirator thread");
	boost::scoped_thread<> nbdRespirator(NbdDoItIMP, nbdFd);
    m_nbdRespirator.swap(nbdRespirator);

    if (nbdRespirator.try_join_for(boost::chrono::milliseconds(500)))
    {
        throw std::runtime_error("nbdRespirator thread finished prematurely");
    }
}

OsProxy::OsProxy(Reactor &r_, const std::string &devicePath_,
                 size_t numBlocks_, Master &master_)
    : m_master(master_), m_sockpair(PF_UNIX, SOCK_STREAM, 0),
      m_reader(m_sockpair.GetFirst(), HDR_SIZE, r_,
               boost::bind(&OsProxy::OnHeaderCB, this, _1), 
               boost::bind(&OsProxy::OnPacketCB, this, _1)),
      m_writer(m_sockpair.GetFirst(), r_), 
      m_nbdRespirator()
{
    // Init connection with nbd
    InitNbdIMP(numBlocks_, devicePath_, m_sockpair.GetSecond());
}

size_t OsProxy::OnHeaderCB(TcpReader::SharedBuffer header_)
{
	Log("In OsProxy::OnHeaderCB");

    nbd_request request;
    memcpy(&request, &(*header_)[0], HDR_SIZE);

    DEBUG(std::cerr << "Got request!!!\nRequest type is " << ntohl(request.type) << " magic is " << ntohl(request.magic)  << std::endl;)
    assert(ntohl(request.magic) == NBD_REQUEST_MAGIC);

	if (ntohl(request.type) == NBD_CMD_READ)
	{
		return HDR_SIZE;
	}
    // return the header_ len.
    return HDR_SIZE + ntohl(request.len);
}

void OsProxy::OnPacketCB(TcpReader::SharedBuffer packet_)
{
	Log("In OsProxy::OnPacketCB");

    nbd_request request;

    memcpy(&request, &(*packet_)[0], HDR_SIZE);

    // Make sure the package is legit (magic)
    assert(ntohl(request.magic) == NBD_REQUEST_MAGIC);
	DEBUG(std::cerr << "~~~~~~~~~~~~~~~~REQUEST LEN: " <<  ntohl(request.len) << std::endl;)
	assert(ntohl(request.len) == 0x1000 || ntohl(request.len) == 0); //0 for disconnect request.
    // Determine the packet type.
    switch (ntohl(request.type))
    {
    case NBD_CMD_READ:
        m_master.Read(protocols::os::ReadRequest(protocols::ID(request.handle), ntohl(request.len),
                                  be64toh(request.from)));
        break;
    case NBD_CMD_WRITE:
        m_master.Write(protocols::os::WriteRequest(protocols::ID(request.handle), packet_,
                                    HDR_SIZE,
                                    be64toh(request.from)));
		break;
	case NBD_CMD_DISC:
		Disconnect();
        break;
    default:
        ReplyRest();
    }
}

void OsProxy::Disconnect()
{
	//TODO: Disconnect gracefully.
	m_nbdRespirator.interrupt();
	m_nbdRespirator.join();
	exit(0);
}

void OsProxy::ReplyRest()
{
	Log("In OsProxy::ReplyRest");

    DEBUG(std::cout << "Replying on unknown request" << std::endl;)
}

void OsProxy::SendNbdReply(const protocols::ID& id_, int status_)
{

    nbd_reply nbdReply; //TODO: instead of writing to local, write to the sharedbuffer immidietly.

	nbdReply.magic = htonl(NBD_REPLY_MAGIC);
	nbdReply.error = htonl(status_);
	std::memcpy(&nbdReply.handle, id_.m_id, 8);
    DEBUG(std::cerr << "Sending nbdReply with magic - " << nbdReply.magic << " And error -  " << nbdReply.error << std::endl;)

    // Send the nbd_reply to nbd
	boost::shared_ptr<std::vector <char> > replyBuffer(new std::vector<char>(sizeof(nbdReply)));
	
	Log("Sending NbdReply");
	std::memcpy(&(((*replyBuffer))[0]), &nbdReply, sizeof(nbd_reply));
    m_writer.Write(replyBuffer);
}

void OsProxy::ReplyRead(protocols::os::ReadReply reply_)
{
	Log("Replying read back to nbd. ");
    SendNbdReply(reply_.GetID(), reply_.GetStatus());

    // Send the data to nbd
	m_writer.Write(reply_.GetData());
}

void OsProxy::ReplyWrite(protocols::os::WriteReply reply_)
{
	Log("Replying write back to nbd. ");

    SendNbdReply(reply_.GetID(), reply_.GetStatus());
}

} // namespace ilrd
