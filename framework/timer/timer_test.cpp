#include <iostream>
#include <boost/chrono.hpp>

#include "reactor.hpp"
#include "timer.hpp"
#include "unit_test.hpp"

void PrintA()
{
  std::cout << "A\n";
}

void PrintB()
{
  std::cout << "B\n";
}

void PrintC()
{
  std::cout << "C\n";
}

using namespace ilrd;

TestResult Overall()
{
  Reactor r;
  Timer t(r);

  boost::chrono::steady_clock::duration time1 = 
    boost::chrono::steady_clock::duration(500000000); // 0.5 sec

  boost::chrono::steady_clock::duration time2 = 
    boost::chrono::steady_clock::duration(2000000000); // 2 sec

  boost::chrono::steady_clock::duration time3 = 
    boost::chrono::steady_clock::duration(999999); // very short

  int h1 = t.Set(time1, PrintA);
  int h2 = t.Set(time2, PrintB);
  int h3 = t.Set(time3, PrintC);

  int h4 = t.Set(time1, PrintA);
  int h5 = t.Set(time2, PrintB);
  int h6 = t.Set(time3, PrintC);

  // t.Cancel(h1);
  // t.Cancel(h2);
  // t.Cancel(h3);
  // t.Cancel(h4);
  t.Cancel(h5);
  // t.Cancel(h6);

  r.Start();

  return SUCCESS;
}

int main()
{
  RUNTEST(Overall);
  return 0;
}
