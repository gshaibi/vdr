TARGETS=minion minion_app master_proxy
OBJS=$(patsubst %, %.o, $(TARGETS))
CC=/usr/bin/g++
CXXFLAGS=-ansi -std=c++98 -fPIC -pedantic-errors $(INCLUDES) -Wall -Wextra -O0 -g3 
INCS=../../framework/communication/include/ ../../include/ ../master_proxy/ ../minion/ ../protocols/ ../../vdr/include/ ../include
INCLUDES=$(patsubst %, -I%, $(INCS))
VPATH+=../../framework/communication/ ../master_proxy/ ../minion/ ../../include/

libminion.so: $(OBJS)
	$(CC) -shared $^ -o $@


.PHONY: clean remove
clean:
	$(RM) *.a *.o *.out core *.hpp.gch *.so


# when cross-compiling, use something like this 

#/rpi-tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++ 
#-ansi -Wall -I ~/Downloads/boost_1_62_0 -std=c++11 reactor_test.cpp reactor.cpp 
#-L pwd -lboost_system -lboost_thread -lboost_filesystem