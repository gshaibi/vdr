#include <arpa/inet.h> // inet_pton
#include <cstdlib>
#include <iostream>
#include <libconfig.h++> // Config

#include "singleton.hpp"
#include "minion_app.hpp"

using namespace minion;
using namespace ilrd;
using namespace libconfig;

int main(int argc, char* argv[])
{
    Singleton<Config> cfg;
    try
    {
        cfg.GetInstance().readFile("conf/minion.conf");
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
        int numBlocks = cfg.GetInstance().lookup("numBlocks");

		const Setting& master = cfg.GetInstance().lookup("master");

		sockaddr_in masterAddr;
		masterAddr.sin_family = AF_INET;
		masterAddr.sin_port = htons(int(master["port"]));

		inet_pton(AF_INET, (master["ip"]).c_str(), &(masterAddr.sin_addr));

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