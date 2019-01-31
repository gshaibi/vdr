#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <libconfig.h++>

using namespace libconfig;

int main()
{
    Config cfg;
    try
    {
        cfg.readFile("master.conf");
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
		std::cout << numBlocks << std::endl;

		std::string devicePath = cfg.lookup("devicePath");
		std::cout << devicePath << std::endl;

		const Setting &minions = cfg.lookup("minions");

		std::cout <<minions.getLength()  << std::endl;

		int port = minions[0]["port"];
		std::cout <<  port << std::endl;
		

		std::string firstMinionIp = cfg.lookup("minions.[0].ip");
		std::cout << firstMinionIp << std::endl;

		int firstMinionPort = cfg.lookup("minions.[0].port");
		std::cout << firstMinionPort << std::endl;
    }
    catch (const SettingNotFoundException& nfex)
    {
        std::cerr << "Setting '" << nfex.getPath() << "' is missing from conf file." << std::endl;
    }
	catch (const SettingTypeException& tex)
	{
		std::cout << "Setting '" <<  tex.getPath() << "' doesnt match it's type." << std::endl;
	}


    // const Setting& root = cfg.getRoot();

    // const Setting &minions = root["minions"];
    // int count = minions.getLength();
}