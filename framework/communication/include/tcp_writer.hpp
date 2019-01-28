#ifndef tcp_writer_hpp
#define tcp_writer_hpp

#include "reactor.hpp"
#include <boost/shared_ptr.hpp>
#include <queue>

namespace ilrd
{

class TcpWriter : boost::noncopyable
{
public:
    typedef boost::shared_ptr<const std::vector<char> > SharedBuffer;

    explicit TcpWriter(int fd_, Reactor& r_);
    // generated dtor

    void Write(SharedBuffer packet_);

private:
    void WriteCB(int fd_);

    Reactor& m_r;
    size_t m_writtenCount;
    int m_fd;

    std::queue<SharedBuffer> m_packets;
};

} // namespace ilrd
#endif // tcp_writer_hpp
