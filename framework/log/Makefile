CC=g++
CPPFLAGS=-I$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared
INCS=include ../include ../../include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=log
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=

liblog.so: $(OBJS)
	$(CC) $(CPPFLAGS) $^ $(SHLIBS) -o $<.so

clean:
	-rm *.o *.out *.so

.PHONY: clean 

