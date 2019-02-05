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
	OnFinish(char* buff_): m_buff(buff_)
	{
	}
	void operator()()
	{
		std::cout<<"On Finish buff = "<< m_buff <<std::endl;
	}
private:
	char *m_buff;

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

	char *buff_(new char[7]);

 	strcpy(buff_, "Chalil");

	char *buff2_(new char[6]);

 	strcpy(buff2_, "Maxim");
	
	encryptor.Encrypt(OnFinish(buff_),buff_, 7);
	encryptor.Encrypt(OnFinish(buff2_),buff2_, 6);
	
	boost::thread thread(&ReacRun, &reactor);

	sleep(1);
	
	REQUIRE(strcmp(buff_, "Chalil") != 0);
	REQUIRE(strcmp(buff2_, "Maxim") != 0);

	encryptor.Decrypt(OnFinish(buff_),buff_, 7);
	encryptor.Decrypt(OnFinish(buff2_),buff2_, 6);

	sleep(1);

	REQUIRE(strcmp(buff_, "Chalil") == 0);
	REQUIRE(strcmp(buff2_, "Maxim") == 0);

	reactor.Stop();

	thread.detach();

	return SUCCESS;
}
