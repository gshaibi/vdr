CXX=g++
cross:CXX=/rpi-tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++
CXXFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared
INCS=include ../include ../../include /home/beyond/Downloads/boost_1_62_0/ #Add for raspberry pi
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=reactor sockpair tcp_reader tcp_writer
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=

libcommunication.so: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(SHLIBS) -o $@

cross: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(SHLIBS) -o libcommunication.so

reactor_test.out: libcommunication.so unit_tests/reactor_test.o
	$(CXX) $^ $(SHLIBS) -o $@ -L. -lcommunication 

clean:
	-rm *.o *.out *.so

.PHONY: clean test

