#include <boost/bind.hpp> // using bind

#include "encryptor.hpp" //header file

namespace ilrd
{

//ctor
Encryptor::Encryptor(Eventer& eventer_, ThreadPool& threadPool_):
											m_eventer(eventer_),
											m_threadPool(threadPool_)
{
	//start thread pool in case not running?
	//TODO: throw in case its not running
}

//methods
void Encryptor::Encrypt(CallBack cb_, Buffer buff_)
{
	SendTaskToThreadPoolIMP(cb_, buff_);	
}

void Encryptor::Decrypt(CallBack cb_, Buffer buff_)
{
	SendTaskToThreadPoolIMP(cb_, buff_);
}

//methods
void Encryptor::SendTaskToThreadPoolIMP(CallBack cb_, Buffer buff_)
{
	//set event
	Eventer::Handle handle = m_eventer.SetEvent(cb_); //thread safe function
	
	//add task to thread pool
	m_threadPool.Add(boost::bind(&Encryptor::EncryptDecryptIMP, this, handle, buff_), 
														ThreadPool::Priority::LOW); //thread safe function
}

void Encryptor::EncryptDecryptIMP(Eventer::Handle handle_, Buffer buff_)
{
	char key = 'K'; //Any char will work
    
	size_t size = buff_->size();

    for (size_t i = 0; i < size; i++)
	{
        buff_->at(i) ^= key;
	}

	//signal event completed
	m_eventer.SignalEvent(handle_);
}

} //namespace ilrd