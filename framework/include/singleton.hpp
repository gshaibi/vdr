#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <boost/atomic.hpp>
#include <boost/noncopyable.hpp>
#include <cstdlib>
#include <boost/thread.hpp> //yield
 
namespace ilrd
{

template <typename T>
class Singleton : private boost::noncopyable
{
public:
	static T& GetInstance();

private:
	static void Deleter();
	static T * s_instance; //TODO: used also as shouldInit... maybe abusish
	static boost::atomic<bool> s_shouldInit;
	static boost::atomic<bool> s_isReady;
};

template <typename T>
T * Singleton<T>::s_instance(NULL);

template <typename T>
boost::atomic<bool> Singleton<T>::s_shouldInit(true);

template <typename T>
boost::atomic<bool> Singleton<T>::s_isReady(false);

template <typename T>
void Singleton<T>::Deleter()
{
	delete s_instance;
	s_instance = NULL;
}

template <typename T>
T& Singleton<T>::GetInstance()
{
	bool tmpTrue = true;
	if (s_shouldInit.compare_exchange_strong(tmpTrue, false, boost::memory_order_acquire))//TODO: Continue. Learn about memory ordering.
	{
		if (!s_isReady.load())
		{
			s_instance = new T();
			std::atexit(Deleter);
			s_isReady.store(true, boost::memory_order_seq_cst);
		}
		else
		{
			while (!s_isReady.load(boost::memory_order_seq_cst))
			{
				boost::this_thread::yield();
			}
		}
	}
	return *s_instance;
}

}//namesp ilrd
#endif // singleton_hpp
