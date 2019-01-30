CXX=g++
CXXFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared 
INCS=include ../include ../../include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=eventer 
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=#-lboost_system -lboost_thread -lboost_chrono -lpthread

FILENAME=$(shell basename $(CURDIR))

libeventer.so: $(OBJS)
	$(CC) $(CXXFLAGS) $^ $(SHLIBS) -o $@
	$(RM) *.o *.gch

test: eventer.cpp eventer_test.cpp
	$(CXX) -g3 $^  $(INCLUDES) $(SHLIBS)
	$(RM) *.o *.gch

.PHONY: clean remove
clean:
	$(RM) *.a *.o *.out core *.hpp.gch

remove:
	$(RM) oferdinho_*.cpp guy_*.cpp yonatan_*.cpp chen_*.cpp chalil_*.cpp \
				maxim_*.cpp matan_*.cpp hila_*.cpp malachi_*.cpp
