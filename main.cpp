#include "app.hpp"
#include <cstdlib>
#include <iostream>

using namespace ilrd;

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cout << "USAGE: ./executable PathToNbd NumBlocks(4kb each)" << std::endl;
		return 1;
	}
	App a(argv[1], std::atoi(argv[2]));
	return 0;
}