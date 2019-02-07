CXX=g++
CXXFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared 
INCS=include ../include ../../include ../communication/include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=timer
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=-lboost_system -lboost_chrono

libtimer.so: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(SHLIBS) -o $@ 

test: timer_test.cpp timer.o 
	g++ -g3 $^  $(INCLUDES) $(SHLIBS) $(BOOSTFLAGS) ../communication/libcommunication.so

clean:
	-rm *.o *.out *.so

.PHONY: clean test 
