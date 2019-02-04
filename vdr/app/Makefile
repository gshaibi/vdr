CC=g++
CXXFLAGS=-I$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared
INCS=include ../include ../../include ../../framework/communication/include/ 
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=app
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=

libapp.so: $(OBJS)
	$(CC) $(CXXFLAGS) $^ $(SHLIBS) -o $@

clean:
	$(RM) *.o *.out *.so

.PHONY: clean 

