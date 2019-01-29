CC=g++
CXXFLAGS=-I$(INCLUDES) $(SHLIBS_PATH) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared
INCS=include ../include ../../include ../../framework/communication/include/
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=master
OBJS=$(patsubst %, %.o, $(TARGETS))

libmaster.so: $(OBJS)
	$(CC) $(CXXFLAGS) $^ -o $@

clean:
	-rm *.o *.out *.so

.PHONY: clean 

