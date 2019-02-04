#include "unit_test.hpp"
#include "encryptor.hpp"

#include <iostream>
#include <cstring> //using strcpy

using namespace ilrd;

TestResult BasicEncryption();

/*********************** CallBacks Functor ************************/
class OnFinish
{
public:
	OnFinish(boost::shared_ptr<std::vector<char> > buff_): m_buff(buff_)
	{
	}
	void operator()()
	{
		std::cout<<"On Finish buff = "<< m_buff->data()<<std::endl;
		
	}
private:
	boost::shared_ptr<std::vector<char> > m_buff;

};
/*********************************************************************/
void ReacRun(Reactor *reac_)
{
	reac_->Start();
}
/***********************************************************************/
int main()
{
	RUNTEST(BasicEncryption);

	return SUCCESS;
}

TestResult BasicEncryption()
{
	Reactor reactor;
	Eventer eventer(reactor);
	ThreadPool tpool(1);
	Encryptor encryptor(eventer, tpool);

	tpool.Start();

	boost::shared_ptr<std::vector<char> > buff_(new std::vector<char>(7));

 	strcpy(&buff_->at(0), "Chalil");

	boost::shared_ptr<std::vector<char> > buff2_(new std::vector<char>(6));

 	strcpy(&buff2_->at(0), "Maxim");
	
	encryptor.Encrypt(OnFinish(buff_),buff_);
	encryptor.Encrypt(OnFinish(buff2_),buff2_);
	
	boost::thread thread(&ReacRun, &reactor);

	sleep(1);
	
	REQUIRE(strcmp(&buff_->at(0), "Chalil") != 0);
	REQUIRE(strcmp(&buff2_->at(0), "Maxim") != 0);

	encryptor.Decrypt(OnFinish(buff_),buff_);
	encryptor.Decrypt(OnFinish(buff2_),buff2_);

	sleep(1);

	REQUIRE(strcmp(&buff_->at(0), "Chalil") == 0);
	REQUIRE(strcmp(&buff2_->at(0), "Maxim") == 0);

	reactor.Stop();

	thread.detach();

	return SUCCESS;
}
