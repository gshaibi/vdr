#include <iostream>

#include "logger.hpp"

using namespace ilrd;

bool Simple();
bool Longer();
bool LoopOfPrints();

int main()
{
    if (1 == Simple())
    {
        std::cout << "Simple [OK]" << std::endl;
    }

    if (1 == Longer())
    {
        std::cout << "Longer [OK]" << std::endl;
    }

    if (1 == LoopOfPrints())
    {
        std::cout << "LoopOfPrints [OK]" << std::endl;
    }

    std::cout << "GREAT SUCCESS" << std::endl;

    return 0;
}

bool Simple() // one print
{
    Log("MOFOZ DIE!", 1);

    sleep(3);

    return 1;
}

bool Longer() // several prints - different msg types
{
    Log("MOFOZ DIE!", 1);
    sleep(1);

    Log("YES! BUT HOW?", 2);
    sleep(1);
    
    Log("SLOW", 3);
    sleep(1);

    return 1;
}

bool LoopOfPrints() // just loop
{
    for (int i = 0; i < 10; ++i)
    {
        Log("MOFOZ DIE!", 1);
        sleep(1);
    }

    return 1;
}



