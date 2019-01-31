#ifndef MINION_APP
#define MINION_APP

#include <cstddef>      // size_t
#include <netinet/in.h> // sockaddr_in

#include "master_proxy.hpp"
#include "minion.hpp"

namespace minion
{

class MinionApp
{
public:
    static const size_t BLOCK_SIZE = 4096;
    static const size_t NUM_BLOCKS = (128 * 1024 * 1024) / BLOCK_SIZE;

    MinionApp(const sockaddr_in& masterAddr, size_t numBlocks_);

private:

}; // MinionApp

} // minion

#endif