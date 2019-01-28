#ifndef TESTHEADER
#define TESTHEADER

#include <cstdio> /* Printf */
#include <cstdlib>  /* __FILE__ , __LINE__ */

typedef enum {SUCCESS, FAIL} TestResult;

#define RUNTEST(test) (!(test()) ? printf(#test" = [OK]\n") : printf(#test" = [Fail]\n"))

#define REQUIRE(a) do { if (!(a)) {\
						printf("Test assertion ("#a") FAILED %s, %d.........[Failed]\n", __FILE__, __LINE__);\
						return FAIL; }\
					} while (0)

#endif
