
CC=g++
CXXFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared 
INCS=../include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=logger
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=

liblog.so: $(OBJS)
	$(CC) $(CXXFLAGS) $^ $(SHLIBS) -o $@

a.out: logger.cpp logger_test.cpp
	$(CC) $(CPPFLAGS) $^ -o a.out -lconfig++ -lboost_thread -lboost_system -lboost_chrono

clean:
	-rm *.o *.out *.so kill.txt mofo.txt
	-clear

.PHONY: clean 

