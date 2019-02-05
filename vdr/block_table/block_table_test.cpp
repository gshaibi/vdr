#include <algorithm>
#include <iostream>
#include <cstring>


#include "block_table.hpp"
#include "unit_test.hpp"

const int NUM_MINIONS = 3;
const int NUM_BLOCKS = 19;
const int BLOCK_SIZE = 1;

void FillInMap(ilrd::BlockTable::BlockLocation blkLocation_)
{
	static bool arr[NUM_MINIONS][NUM_BLOCKS / NUM_MINIONS * 2];

	arr[blkLocation_.minionID][blkLocation_.blkOffset] = true;

	for(size_t minion = 0; minion < sizeof(arr) / sizeof(arr[0]); minion++)
	{
		for(size_t offset = 0; offset < sizeof(arr[0]) / sizeof(arr[0][0]); offset++)
		{
			std::cout << "| " << arr[minion][offset];
		}
		std::cout  << std::endl;
	}
	std::cout << "NEXT" << std::endl;
}

TestResult Overall()
{
    ilrd::BlockTable bt(BLOCK_SIZE, NUM_BLOCKS, NUM_MINIONS);

	//TODO: Check 2
	// auto arr = bt.Translate(3);
	
	// REQUIRE(arr.size() == 2);

	// std::cout << "Minion num " << arr[0].minionID << std::endl;
	// std::cout << "Offset  " << arr[0].blkOffset << std::endl << std::endl;
	
	// std::cout << "Minion num " << arr[1].minionID << std::endl;
	// std::cout << "Offset  " << arr[1].blkOffset << std::endl;
	

	for(size_t i = 0; i < NUM_BLOCKS * BLOCK_SIZE; i++)
	{
		auto res = bt.Translate(i);
		std::for_each(res.begin(), res.end(), FillInMap);
	}
	

    return SUCCESS;
}

int main()
{
    RUNTEST(Overall);
    return 0;
}
