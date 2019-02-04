CC=g++
CPPFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC
SHARED=-shared
INCS=../include/ ../../framework/communication/include ../../include ../../framework/include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=block_table block_table_test
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=-lpthread

libblock_table.so: block_table.o
	$(CC) $(CPPFLAGS) $(SHARED) $^ -o $@ 


a.out: $(OBJS)
	$(CC) $(CPPFLAGS) $^ $(SHLIBS)
	$(RM) *.o *.gch

clean:
	$(RM) *.o *.out *.so

.PHONY: clean 

