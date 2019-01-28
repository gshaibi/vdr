#ifndef OS_PROXY_HPP
#define OS_PROXY_HPP

#include <string>
#include <boost/thread/scoped_thread.hpp>
#include <linux/nbd.h>

#include "protocols.hpp"
#include "reactor.hpp"
#include "sockpair.hpp"
#include "tcp_reader.hpp"
#include "tcp_writer.hpp"

namespace ilrd
{

class Master;

class OsProxy : boost::noncopyable
{
public:
    explicit OsProxy(Reactor& r_, const std::string& devicePath_,
                     size_t numBlocks_, Master& master_);
    // generated dtor

    // reply back to NBD
    void ReplyRead(protocols::os::ReadReply reply_);
    void ReplyWrite(protocols::os::WriteReply reply_);

private:
    // header_ vector isn't fit to header size.
    size_t OnHeaderCB(TcpReader::SharedBuffer header_);
    // packet_ vector is fit to the packet size.
    void OnPacketCB(TcpReader::SharedBuffer packet_);

    void InitNbdIMP(size_t numBlocks_, const std::string& devicePath_,
                           int socket_);
    static void* NbdDoItIMP(boost::shared_ptr<int> nbdFd);
    static void CloseNbdIMP(int *nbdFd);
    void SendNbdReply(const protocols::ID& id_, int status_);
    void ReplyRest();
	void Disconnect();

	const static size_t HDR_SIZE = sizeof(nbd_request);
	const static size_t BLK_SIZE = 0x1000;

    Master& m_master;
    Sockpair m_sockpair;
    TcpReader m_reader;
    TcpWriter m_writer;
    boost::scoped_thread<> m_nbdRespirator;
};

} // namespace ilrd

#endif