#include <iostream>

#include "thread_pool.hpp"
#include "unit_test.hpp"

using namespace ilrd;


void Foo()
{
	int counter = 0;
	while(1);
}

TestResult Overall()
{
	ThreadPool t(1);

	t.Add(Foo, ThreadPool::HIGH);
	// t.Add(Foo, ThreadPool::HIGH);
	// t.Add(Foo, ThreadPool::HIGH);
	// t.Add(Foo, ThreadPool::HIGH);
	// std::cout <<   t.IsRunning() << std::endl;

	t.Start();
	REQUIRE(t.IsRunning());
	// boost::this_thread::yield();
	sleep(0);
	// REQUIRE(false == t.Stop());

	// REQUIRE(t.IsRunning());
	// // t.SetNum(5);
	// t.SetNum(30);
	// REQUIRE(true == t.Stop(boost::chrono::seconds(3)));
	// t.Add(Foo, ThreadPool::MEDIUM);
	// REQUIRE(boost::cv_status::timeout == t.Stop(boost::chrono::milliseconds(1)));
	// REQUIRE(boost::cv_status::timeout != t.Stop(boost::chrono::milliseconds(10000)));
	// t.SetNum(1);
	// sleep(5);
	// t.Stop(boost::chrono::seconds(1));
	// while(1);
	// t.SetNum(1);
	// while(1);
	// sleep(1);
	std::cout << "after setnum" << std::endl;
	return SUCCESS;
}

int main()
{
	RUNTEST(Overall);
	return 0;
}
