#include <exception>        // manage exceptions if possible
#include <sys/socket.h>     // sockaddr
#include <netinet/in.h>     // sockaddr
#include <netinet/ip.h>     // sockaddr
#include <arpa/inet.h>      // inet_aton

#include "reactor.hpp"      // receive requests from minion_proxy
#include "routines.hpp"     // DEBUG flag 
#include "logger.hpp"       // keep log
#include "minion_app.hpp"

using namespace ilrd;

namespace minion
{

MinionApp::MinionApp(const sockaddr_in& masterAddr, size_t numBlocks_)
{
	Log("constructing Reactor for master_proxy");
	ilrd::Reactor reactor;

	Log("constructing Minion");
	Minion min(NUM_BLOCKS); // dynamic allocation or file handling. should try-catch?

	Log("constructing MasterProxy");
	MasterProxy mp(reactor, min, masterAddr);

	min.SetMasterProxy(&mp);
	try
	{
			reactor.Start();
	}
	catch (const std::exception& e)
	{
			Log(e.what());
	}
	catch (...)
	{
			Log("Reactor threw");
	}
}

} // minion