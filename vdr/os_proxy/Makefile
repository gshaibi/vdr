CC=g++
CXXFLAGS=-I$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared
INCS=include ../include ../../include ../../framework/communication/include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=os_proxy
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=boost_chrono
SHLIBS_L=$(patsubst %, -l%, $(SHLIBS))

libos_proxy.so: $(OBJS)
	$(CC) $(CXXFLAGS) $^ $(SHLIBS_L) -o $@

clean:
	-rm *.o *.out *.so

.PHONY: clean 

