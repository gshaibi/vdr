#include <iostream>
#include <cassert>

#include "master_proxy.hpp"
#include "routines.hpp"
#include "reactor.hpp"
#include "os_proxy.hpp"
#include "app.hpp"
#include "logger.hpp"
#include <netinet/in.h> // sockaddr_in

namespace ilrd
{

App::App(const std::string& devicePath_, size_t numBlocks_)
{
	Log("Constructing Reactor");
	Reactor r;

	sockaddr_in minionAddr;
	minionAddr.sin_family = AF_INET;
	minionAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	minionAddr.sin_port = htons(3000);

	Log("Constructing Master");
	Master m(1, r, minionAddr);

	Log("Constructing OsProxy");
	OsProxy os(r, devicePath_, numBlocks_, m);

	m.SetOsProxy(&os);
	r.Start();
}

} // ilrd
