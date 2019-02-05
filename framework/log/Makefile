
CC=g++
CPPFLAGS= -ansi -std=c++98 $(INCLUDES) -pedantic-errors -Wall -Wextra -O0 -g3
INCS=../include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=log
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=

a.out: logger.cpp logger_test.cpp
	$(CC) $(CPPFLAGS) $^ -o a.out
#liblog.so: $(OBJS)
#	$(CC) $(CPPFLAGS) $^ $(SHLIBS) -o $<.so

clean:
	-rm *.o *.out *.so
	-clear

.PHONY: clean 

