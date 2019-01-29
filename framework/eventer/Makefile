CXX=g++
CXXFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared 
INCS=include ../include ../../include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=thread_pool 
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=-lboost_system -lboost_thread -lboost_chrono -lpthread

#preprocessor flags used in CXXFLAGS
IPATH=$(patsubst %, -I ../%, $(TARGETS)) -I../include

#using default CXX (g++)
CXXFLAGS+=-std=c++98 -ansi -pedantic-errors $(IPATH) -Wall -Wextra -g3 -O0
FILENAME=$(shell basename $(CURDIR))

a.out: $(FILENAME).o $(FILENAME)_test.o $(OBJS) $(FILENAME).hpp
	$(CXX) $(CXXFLAGS) $^
	rm *.o *.gch

.PHONY: clean remove
clean:
	$(RM) *.a *.o *.out core *.hpp.gch

remove:
	$(RM) oferdinho_*.cpp guy_*.cpp yonatan_*.cpp chen_*.cpp chalil_*.cpp \
				maxim_*.cpp matan_*.cpp hila_*.cpp malachi_*.cpp
