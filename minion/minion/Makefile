CXX=/rpi-tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++
CXXFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared 
INCS=include ../include ../../include ../../vdr/include ../../framework/communication/include ../minion  ../minion /home/beyond/Downloads/boost_1_62_0
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=minion 
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=

libminion.so: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(SHLIBS) -o $@

test: minion_test.cpp minion.cpp
	g++ -g3 $^  $(INCLUDES) $(SHLIBS) -L../../framework/communication -lcommunication -L../../vdr/protocols -lprotocols #-L../minion -lminion

clean:
	-rm *.o *.out *.so

.PHONY: clean test 

