#include "reactor.hpp"

#include <iostream> //cerr
#include <cassert>
#include <boost/bind.hpp>
#include <cerrno>

#include "routines.hpp"
#include "logger.hpp"

namespace ilrd
{

void Reactor::ZeroFdsets()
{
	FD_ZERO(&m_fdsets[READ]);
	FD_ZERO(&m_fdsets[WRITE]);
	FD_ZERO(&m_fdsets[EXPT]);
}

Reactor::Reactor() : m_fdsets(), m_shouldStop(true), m_fdFuncs()
{
	ZeroFdsets();
}

void Reactor::AddFD(int fd_, Usage use_, func_type func_)
{
	assert(fd_ >= 0);
	assert(fd_ < FD_SETSIZE);
	assert(m_fdFuncs[use_].find(fd_) != m_fdFuncs[use_].end() || m_fdFuncs[use_][fd_].empty());

	m_fdFuncs[use_][fd_] = func_;
}

int Reactor::GetMaxFdsIMP() const
{
	int maxRFds = m_fdFuncs[READ].empty() ? 0 : m_fdFuncs[READ].rbegin()->first;
	int maxWFds = m_fdFuncs[WRITE].empty() ? 0 : m_fdFuncs[WRITE].rbegin()->first;
	int maxEFds = m_fdFuncs[EXPT].empty() ? 0 : m_fdFuncs[EXPT].rbegin()->first;

	return std::max(std::max(maxRFds, maxWFds), maxEFds);
}

void Reactor::RemFD(int fd_, Usage use_)
{
	assert(fd_ >= 0);
	assert(fd_ < FD_SETSIZE);
	assert(m_fdFuncs[use_].find(fd_) != m_fdFuncs[use_].end());

	DEBUG(std::cerr << "Removing FD " << fd_ << " from reactor." << std::endl;)
	m_fdFuncs[use_][fd_].clear();
}

void Reactor::FdSetIMP(const fd_func_pair_type &fdFunc_, Usage use_)
{ 
	FD_SET(fdFunc_.first, &m_fdsets[use_]);
}

void Reactor::FillFdsetsIMP()
{
	ZeroFdsets();
	for_each(m_fdFuncs[READ].begin(), m_fdFuncs[READ].end(), boost::bind(&Reactor::FdSetIMP, this, _1, READ));
	for_each(m_fdFuncs[WRITE].begin(), m_fdFuncs[WRITE].end(), boost::bind(&Reactor::FdSetIMP, this, _1, WRITE));
	for_each(m_fdFuncs[EXPT].begin(), m_fdFuncs[EXPT].end(), boost::bind(&Reactor::FdSetIMP, this, _1, EXPT));
}

void Reactor::ActivateFuncIfFdRdyIMP(const fd_func_pair_type &fdFunc_, Usage use_)
{
	DEBUG(std::cerr << "Checking if fd " << fdFunc_.first << " is ready." << std::endl;)
	if (FD_ISSET(fdFunc_.first, &m_fdsets[use_]))
	{
		DEBUG(std::cout << "Usage: " << use_ << "fd: " << fdFunc_.first << std::endl;)

		if (!fdFunc_.second.empty())
		{
			fdFunc_.second(fdFunc_.first);
		}
	}
}

void Reactor::CleanEmptyFunctions()
{
	for (int use = READ; use <= EXPT; ++use)
	{
		for (std::map<int, func_type>::iterator i = m_fdFuncs[use].begin(); i != m_fdFuncs[use].end();)
		{
			if (i->second.empty())
			{
				m_fdFuncs[use].erase(i++);
			}
			else
			{
				++i;
			}
		}
	}
}

int Reactor::Start()
{
	m_shouldStop = false;
	FillFdsetsIMP();

	while (!m_shouldStop)
	{
		int selected = select(GetMaxFdsIMP() + 1, &m_fdsets[READ], &m_fdsets[WRITE], &m_fdsets[EXPT], NULL);
		if (-1 ==  selected)
		{
			switch (errno)
			{
			case EINTR:
				continue;
			case EBADF:
				std::cerr << "Bad file descriptor" << std::endl;
				return 1;
			default:
				if (m_fdFuncs[READ].empty() && m_fdFuncs[WRITE].empty() && m_fdFuncs[EXPT].empty())
				{
					std::cerr << "Empty reactor" << std::endl;
					return 2;
				}

				return -1; //unknown error. user should check errno.
			}
		}

		DEBUG(std::cout << selected << " things are ready! Read size - " << m_fdFuncs[READ].size() <<" and write size - " << m_fdFuncs[WRITE].size() << std::endl; sleep(0);)
		DEBUG(for(int i = 0; i < 1000; ++i){ if (FD_ISSET(i, &m_fdsets[READ])) {std::cerr << "FD " << i << "IN READ IS READY!!!-------------------------------------------------------------------------------------------" << std::endl;}})
		DEBUG(for(int i = 0; i < 1000; ++i){ if (FD_ISSET(i, &m_fdsets[WRITE])) {std::cerr << "FD " << i << "IN WRITE IS READY!!!-------------------------------------------------------------------------------------------" << std::endl;}})
		DEBUG(for(int i = 0; i < 1000; ++i){ if (FD_ISSET(i, &m_fdsets[EXPT])) {std::cerr << "FD " << i << "IN EXPT IS READY!!!-------------------------------------------------------------------------------------------" << std::endl;}})
		// for each fd_set, check who's ready from the m_fds list.
		for_each((m_fdFuncs[EXPT].begin()), (m_fdFuncs[EXPT].end()),
				 boost::bind(&Reactor::ActivateFuncIfFdRdyIMP, this, _1, EXPT));
		for_each((m_fdFuncs[READ].begin()), (m_fdFuncs[READ].end()),
				 boost::bind(&Reactor::ActivateFuncIfFdRdyIMP, this, _1, READ));
		for_each((m_fdFuncs[WRITE].begin()), (m_fdFuncs[WRITE].end()),
				 boost::bind(&Reactor::ActivateFuncIfFdRdyIMP, this, _1, WRITE));

		//Clean m_fdFuncs from empty functions.
		CleanEmptyFunctions();
		FillFdsetsIMP();
	}

	return 0;
}

void Reactor::Stop()
{
	m_shouldStop = true;
}

} //namespace ilrd