CC=g++
CXXFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared 
INCS=include ../include ../../include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=thread_pool 
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=-lboost_system -lboost_thread -lboost_chrono -lpthread

thread_pool.so: $(OBJS)
	$(CC) $(CXXFLAGS) $^ $(SHLIBS) -o $@

test: thread_pool_test.cpp thread_pool.cpp
	g++ -g3 $^  $(INCLUDES) $(SHLIBS)

clean:
	-rm *.o *.out *.so

.PHONY: clean test 

