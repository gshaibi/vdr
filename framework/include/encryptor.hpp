#ifndef ENCRYPTOR_HPP
#define ENCRYPTOR_HPP

#include <vector> //using vector

#include <boost/noncopyable.hpp> //using noncopyable
#include <boost/function.hpp> //using function
#include <boost/shared_ptr.hpp> // using shared_ptr

#include "thread_pool.hpp" //using thread pool
#include "eventer.hpp" //using eventer

namespace ilrd
{

class Encryptor: private boost::noncopyable
{
public:
	//ctor
	explicit Encryptor(Eventer& eventer_, ThreadPool& threadPool_);
	//cctor and op= are disabled
	//using generated dtor

	//methods
	void Encrypt(boost::function<void(void)> cb_, 
									boost::shared_ptr<std::vector<char> > buff_);
	
	void Decrypt(boost::function<void(void)> cb_, 
									boost::shared_ptr<std::vector<char> > buff_);

private:
	typedef boost::shared_ptr<std::vector<char> > Buffer;
	typedef boost::function<void(void)> CallBack;

	//members
	Eventer& m_eventer;
	ThreadPool& m_threadPool;
	
	//private methods
	void EncryptDecryptIMP(Eventer::Handle, Buffer buff_);
	void SendTaskToThreadPoolIMP(CallBack cb_, Buffer buff_); 
};

} //namespace ilrd

#endif //ENCRYPTOR_HPP