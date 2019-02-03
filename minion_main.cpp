#include <arpa/inet.h> // inet_pton
#include <cstdlib>
#include <iostream>
#include <libconfig.h++> // Config

#include "minion_app.hpp"

using namespace minion;
using namespace ilrd;
using namespace libconfig;

int main(int argc, char* argv[])
{
    Config cfg;
    try
    {
        cfg.readFile("conf/minion.conf");
    }
    catch (const FileIOException& fioex)
    {
        std::cerr << "I/O error while reading file." << std::endl;
        return (EXIT_FAILURE);
    }
    catch (const ParseException& pex)
    {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        return (EXIT_FAILURE);
    }

    try
    {
        int numBlocks = cfg.lookup("numBlocks");

		const Setting& master = cfg.lookup("master");

		sockaddr_in masterAddr;
		masterAddr.sin_family = AF_INET;
		masterAddr.sin_port = htons(int(master["port"]));

		std::string masterIp = master["ip"];

		inet_pton(AF_INET, (masterIp).c_str(), &(masterAddr.sin_addr));

    	MinionApp app(masterAddr, numBlocks);

        return 0;
    }
    catch (const SettingNotFoundException& nfex)
    {
        std::cerr << "Setting '" << nfex.getPath()
                  << "' is missing from conf file." << std::endl;
    }
    catch (const SettingTypeException& tex)
    {
        std::cout << "Setting '" << tex.getPath() << "' doesnt match it's type."
                  << std::endl;
    }
    return 0;
}