#ifndef MASTER_PROXY_HPP
#define MASTER_PROXY_HPP

#include <boost/noncopyable.hpp>
#include <vector>

#include "protocols.hpp"
#include "reactor.hpp"

namespace minion
{

class MasterProxy : boost::noncopyable
{
public:
	explicit MasterProxy(Reactor& r_);
	// generated dtor

	void ReplyRead(const protocols::ID& id_, int status_, std::vector<char> data_);
	void ReplyWrite(const protocols::ID& id_, int status_);

private:
	static const int SERVER_UDP_PORT = 3000;

	void OnPacketCB();
};

} // namespace minion


#endif // MASTER_PROXY_HPP
