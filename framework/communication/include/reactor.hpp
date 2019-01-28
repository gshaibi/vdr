#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <sys/select.h>
#include <set> //m_maxFdss
#include <map> //m_fdFuncs

namespace ilrd
{

class Reactor : boost::noncopyable
{
public:
	typedef boost::function<void(int)> func_type;

	explicit Reactor();
	//generated dtor.

	enum Usage
	{
		READ = 0,
		WRITE = 1,
		EXPT = 2
	};
	void AddFD(int fd_, Usage use_, func_type f_); //any exception that will occur in f_ will be thrown from Start
	void RemFD(int fd_, Usage use_);

	int Start(); //returns int for status. 0 - success, 1 - Bad FD, 2 - Empty reactor, Other error - (-1) - check errno
	void Stop();

private:
	typedef std::pair<int, func_type> fd_func_pair_type;
	void FillFdsetsIMP();
	int GetMaxFdsIMP() const;
	void ActivateFuncIfFdRdyIMP(const fd_func_pair_type& fdFunc_, Usage use_);
	void FdSetIMP(const fd_func_pair_type& fdFunc_, Usage use_);
	void CleanEmptyFunctions();
	void ZeroFdsets();

	fd_set m_fdsets[3]; //0 - read, 1- write, 2 - except

	bool m_shouldStop;
	std::map<int, func_type> m_fdFuncs[3];
};

} //namespace ilrd

#endif // REACTOR_HPP
