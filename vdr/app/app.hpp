#ifndef app_hpp
#define app_hpp

#include <vector>
#include <netinet/in.h> //sockaddr_in

#include "master.hpp"
#include "os_proxy.hpp"

namespace ilrd
{

class App
{
public:
	App(const std::string& devicePath_, size_t numBlocks_, const std::vector<sockaddr_in> minionAddrs);


private:
};

} // ilrd


#endif // app_hpp
