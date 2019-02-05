CXX=/rpi-tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++
CXXFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared 
INCS=include ../include ../../include ../../vdr/include ../../framework/communication/include ../minion  ../minion /home/beyond/Downloads/boost_1_62_0
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=minion_app 
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=

libminion_app.so: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(SHLIBS) -o $@

test: master_proxy_test.cpp master_proxy.cpp
	g++ -g3 $^  $(INCLUDES) $(SHLIBS) -L../../framework/communication -lcommunication -L../../vdr/protocols -lprotocols

clean:
	-rm *.o *.out *.so

.PHONY: clean test 
