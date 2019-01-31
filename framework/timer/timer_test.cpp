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
    boost::chrono::steady_clock::duration(1<<29);

  boost::chrono::steady_clock::duration time2 = 
    boost::chrono::steady_clock::duration(999999999);// more than 10 dig num = failure

  boost::chrono::steady_clock::duration time3 = 
    boost::chrono::steady_clock::duration(999999);

  int h1 = t.Set(time1, PrintA);
  int h2 = t.Set(time2, PrintB);
  int h3 = t.Set(time2, PrintC);
  int h4 = t.Set(time3, PrintB);


  t.Cancel(h2);
  // t.Cancel(h3);
  // t.Cancel(h4);

  r.Start();

  return SUCCESS;
}

int main()
{
  RUNTEST(Overall);
  return 0;
}
