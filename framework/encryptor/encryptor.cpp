#include <boost/bind.hpp> // using bind

#include "encryptor.hpp" //header file
#include "logger.hpp"

namespace ilrd
{

//ctor
Encryptor::Encryptor(Eventer& eventer_, ThreadPool& threadPool_):
											m_eventer(eventer_),
											m_threadPool(threadPool_)
{
}

//methods
void Encryptor::Encrypt(CallBack cb_, Buffer buff_)
{
	ilrd::Log("[Encryptor] got Encryption request");
	SendTaskToThreadPoolIMP(cb_, buff_);	
}

void Encryptor::Decrypt(CallBack cb_, Buffer buff_)
{
	ilrd::Log("[Encryptor] got Decryption request");
	SendTaskToThreadPoolIMP(cb_, buff_);
}

//methods
void Encryptor::SendTaskToThreadPoolIMP(CallBack cb_, Buffer buff_)
{
	ilrd::Log("[Encryptor] setting new event");
	//set event
	Eventer::Handle handle = m_eventer.SetEvent(cb_); //thread safe function
	
	ilrd::Log("[Encryptor] add encryption task to thread pool");
	//add task to thread pool (using default priority : MEDIUM)
	m_threadPool.Add(boost::bind(&Encryptor::EncryptDecryptIMP, this, handle, buff_)); //thread safe function
}

void Encryptor::EncryptDecryptIMP(Eventer::Handle handle_, Buffer buff_)
{
	ilrd::Log("[Encryptor] stating encryption/decryption task");
	char key = 'K'; //Any char will work
    
	size_t size = buff_->size();

    for (size_t i = 0; i < size; i++)
	{
        buff_->at(i) ^= key;
	}

	ilrd::Log("[Encryptor] encryption/decryption task finished");
	ilrd::Log("[Encryptor] signaling event");
	//signal event completed
	m_eventer.SignalEvent(handle_);
}

} //namespace ilrd