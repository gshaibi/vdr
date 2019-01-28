CC=g++
CXXFLAGS= -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared 
INCS=include ../include ../../include
INCLUDES=
TARGETS=minion 
OBJS=
SHLIBS=

minion.so: 
	  $^  -o minion

test: minion_test.cpp minion.cpp
	g++ -g3 $^   

clean:
	-rm *.o *.out *.so

.PHONY: clean test 


