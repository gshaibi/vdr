#include <cstdlib>
#include <iostream>
#include <libconfig.h++> // Config
#include <arpa/inet.h> // inet_pton

#include "singleton.hpp"
#include "app.hpp"

using namespace ilrd;
using namespace libconfig;

int main()
{
	Singleton<Config> cfg;
    try
    {
        cfg.GetInstance().readFile("conf/master.conf");
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
		std::string devicePath = cfg.GetInstance().lookup("devicePath");

		std::vector<sockaddr_in> minionAddrs;
		const Setting &minions = cfg.GetInstance().lookup("minions");
		for (int i = 0; i < minions.getLength(); ++i)
		{
			sockaddr_in minionAddr;
			minionAddr.sin_family = AF_INET;
			minionAddr.sin_port = htons(int(minions[i]["port"]));

			inet_pton(AF_INET, (minions[i]["ip"]).c_str(), &(minionAddr.sin_addr));

			minionAddrs.push_back(minionAddr);
		}
		
		App a(devicePath, numBlocks, minionAddrs);
		return 0;
    }
    catch (const SettingNotFoundException& nfex)
    {
        std::cerr << "Setting '" << nfex.getPath() << "' is missing from conf file." << std::endl;
    }
	catch (const SettingTypeException& tex)
	{
		std::cout << "Setting '" <<  tex.getPath() << "' doesnt match it's type." << std::endl;
	}

	return EXIT_FAILURE;
}