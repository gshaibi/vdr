CC=g++
CXXFLAGS=-I$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared
INCS=include ../include ../../include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=protocols
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=

libprotocols.so: $(OBJS)
	$(CC) $(CXXFLAGS) $^ $(SHLIBS) -o $@

clean:
	-rm *.o *.out *.so

.PHONY: clean 

