#include <iostream>     // std::cout

#include "minion_app.hpp"

using namespace minion;

int main(int argc, char* argv[])
{
    if (3 != argc)
    {
        std::cout << "minion_main() wrong input, required: <ip> <port>\n";
        return 1;
    }
    
    MinionApp app(argv[1], argv[2]);

    return 0;
}