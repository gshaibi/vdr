#ifndef MASTER_HPP
#define MASTER_HPP

#include "os_proxy.hpp"
// #include "block_table.hpp"

namespace ilrd
{

class Master : boost::noncopyable
{
public:
    Master(/* size_t numBlocks_ ,size_t numMinions_ */); // Object is incomplete before calling SetOsProxy.

    void SetOsProxy(OsProxy* os_);

    void Read(protocols::os::ReadRequest req_);
    void Write(protocols::os::WriteRequest req_);

    void ReplyRead(protocols::os::ReadReply reply_);
    void ReplyWrite(protocols::os::WriteReply reply_);

private:
	// friend class BlockTable;
    OsProxy* m_os;
    char* m_disk;
};

} // namespace ilrd

#endif // master_hpp
