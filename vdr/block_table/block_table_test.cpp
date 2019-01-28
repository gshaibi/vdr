#include "block_table.hpp"
#include "unit_test.hpp"

using namespace ilrd;

TestResult Overall()
{
  BlockTable bt(12, 3);
  auto res = bt.Translate(3);

	res.

  return SUCCESS;
}

int main()
{
  RUNTEST(Overall);
  return 0;
}
