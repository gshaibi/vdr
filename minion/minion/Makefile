CC=g++
CXXFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared 
INCS=include ../include ../../include ../../vdr/include ../../framework/communication/include ../minion . ../../framework/include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=minion 
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=

libminion.so: $(OBJS)
	$(CC) $(CXXFLAGS) $^ $(SHLIBS) -o $@

test: minion_test.cpp minion.cpp
	g++ -g3 $^  $(INCLUDES) $(SHLIBS) -L../../framework/communication -lcommunication -L../../vdr/protocols -lprotocols #-L../minion -lminion

clean:
	-rm *.o *.out *.so

.PHONY: clean test 

