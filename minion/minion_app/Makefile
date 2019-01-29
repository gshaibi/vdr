CC=g++
CXXFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared 
INCS=include ../include ../../include ../../vdr/include ../../framework/communication/include ../minion .
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=minion_app 
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=

minion_app.so: $(OBJS)
	$(CC) $(CXXFLAGS) $^ $(SHLIBS) -o $@

test: master_proxy_test.cpp master_proxy.cpp
	g++ -g3 $^  $(INCLUDES) $(SHLIBS) -L../../framework/communication -lcommunication -L../../vdr/protocols -lprotocols

clean:
	-rm *.o *.out *.so

.PHONY: clean test 