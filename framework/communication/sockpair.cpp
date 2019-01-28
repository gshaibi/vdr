#include <cerrno>
#include <stdexcept>
#include <sys/socket.h> //socketpair
#include <sys/types.h>  //socketpair
#include <unistd.h>     //close
#include <iostream> //cerr

#include "logger.hpp"
#include "sockpair.hpp"

namespace ilrd
{

Sockpair::Sockpair(int domain_, int type_, int protocol_) : m_sv()
{
	Log("Constructing Sockpair");
    if (0 != socketpair(domain_, type_, protocol_, m_sv))
    {
        throw std::runtime_error("socketpair creation error. check errno");
    }
}

Sockpair::~Sockpair() noexcept
{
    while (0 != close(m_sv[0]))
    {
        switch (errno)
        {
        case EINTR:
            continue;
        default:
            std::cerr << "closing socket error. check errno" << std::endl;
        }
    }

    while (0 != close(m_sv[1]))
    {
        switch (errno)
        {
        case EINTR:
            continue;
        default:
            std::cerr << "closing socket error. check errno" << std::endl;
        }
    }
}

int Sockpair::GetFirst() const { return m_sv[0]; }

int Sockpair::GetSecond() const { return m_sv[1]; }

} // namespace ilrd
