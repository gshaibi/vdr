#ifndef tcp_readwrite_hpp
#define tcp_readwrite_hpp

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "routines.hpp"
#include "reactor.hpp"

namespace ilrd
{

class TcpReader : boost::noncopyable
{
  public:
	typedef boost::shared_ptr<std::vector<char> > SharedBuffer;

	explicit TcpReader(int fd_, size_t headerSize_, Reactor &r_,
								  boost::function<size_t(SharedBuffer)> onHeader_,
								  boost::function<void(SharedBuffer)> onPacket_);
	~TcpReader() noexcept;

  private:
	void Read(int fd_);

	boost::function<size_t(SharedBuffer)> m_onHeader;
	boost::function<void(SharedBuffer)> m_onPacket;
	Reactor &m_r;

	SharedBuffer m_packet;

	size_t m_headerSize;
	size_t m_packetSize;
	size_t m_readenCount;
	int m_fd; //for dtor
};

} // namespace ird
#endif // tcp_readwrite_hpp
