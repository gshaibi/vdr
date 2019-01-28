#ifndef MASTER_PROXY_HPP
#define MASTER_PROXY_HPP

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "protocols.hpp"
#include "reactor.hpp"

namespace minion
{

class Minion;

class MasterProxy : boost::noncopyable
{
public:
	explicit MasterProxy(ilrd::Reactor& r_, Minion& m_);
	// generated dtor

	void ReplyRead(const ilrd::protocols::ID& id_, int status_, boost::shared_ptr< std::vector<char> > data_);
	void ReplyWrite(const ilrd::protocols::ID& id_, int status_);

private:
	void OnPacketCB(int fd_);

	static const int SERVER_UDP_PORT = 3000;
	
	Minion& m_minion;
};

} // namespace minion


#endif // MASTER_PROXY_HPP
