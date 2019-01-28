#include <cassert>
#include <iostream>

#include "logger.hpp"
#include "master.hpp"
#include "protocols.hpp"
#include "routines.hpp"

namespace ilrd
{

Master::Master() : m_os(), m_disk(static_cast<char*>(operator new(10000000000)))
{
    Log("Constructing Master");
}

void Master::SetOsProxy(OsProxy* os_)
{
    Log("Setting Master's OsProxy");
    m_os = os_;
}

void Master::Read(protocols::os::ReadRequest req_)
{
    Log("In Master::Read()");

    boost::shared_ptr<std::vector<char> > data(
        new std::vector<char>(req_.GetLen()));

    DEBUG(std::cerr << "***************Read request of " << req_.GetLen()
                    << " bytes from offset " << req_.GetOffset() << std::endl
                    << "Reading from address " << data->data() << std::endl;)

    std::memcpy(&((*data)[0]), m_disk + req_.GetOffset(), req_.GetLen());

    Log("Replying protocols::os::ReadReply back to nbd");
    ReplyRead(protocols::os::ReadReply(req_.GetID(), 0, data));
}

void Master::Write(protocols::os::WriteRequest req_)
{
    Log("In Master::Write()");

    DEBUG(std::cerr << "***************Write request of " << req_.GetLength()
                    << " bytes from offset " << req_.GetOffset() << std::endl;)

    std::memcpy(m_disk + req_.GetOffset(), req_.GetData(), req_.GetLength());

    ReplyWrite(protocols::os::WriteReply(req_.GetID(), 0));
}

void Master::ReplyRead(protocols::os::ReadReply reply_) { m_os->ReplyRead(reply_); }

void Master::ReplyWrite(protocols::os::WriteReply reply_) { m_os->ReplyWrite(reply_); }

} // namespace ilrd
