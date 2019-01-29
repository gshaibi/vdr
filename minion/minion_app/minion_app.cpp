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

MinionApp::MinionApp(char* ip_, char* port_)
{
	Log("constructing Reactor for master_proxy");
	ilrd::Reactor reactor;

	Log("constructing Minion");
	Minion min(NUM_BLOCKS); // dynamic allocation or file handling. should try-catch?

	struct sockaddr_in master_addr;
	memset(&master_addr, 0, sizeof(master_addr));
	master_addr.sin_family = AF_INET;
	master_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	// if (1 != inet_pton(AF_INET, ip_, &master_addr.sin_addr))
	// {
	// 		Log("bad ip address");
	// 		return;
	// }
	// master_addr.sin_addr.s_addr = inet_aton(ip_, &master_addr.sin_addr); // deprecated
	master_addr.sin_port = htons(7000);

	Log("constructing MasterProxy");
	MasterProxy mp(reactor, min, master_addr);

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