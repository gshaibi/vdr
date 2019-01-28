#include <iostream>
#include <cassert>

#include "routines.hpp"
#include "reactor.hpp"
#include "os_proxy.hpp"
#include "app.hpp"
#include "logger.hpp"

namespace ilrd
{

App::App(const std::string& devicePath_, size_t numBlocks_)
{
	Log("Constructing Reactor");
	Reactor r;

	Log("Constructing Master");
	Master m;

	Log("Constructing OsProxy");
	OsProxy os(r, devicePath_, numBlocks_, m);

	m.SetOsProxy(&os);
	r.Start();
}

} // ilrd
