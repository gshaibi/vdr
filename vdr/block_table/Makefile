CC=g++
CPPFLAGS=$(INCLUDES) -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC
SHARED=-shared
INCS=../include/ ../../framework/communication/include ../../include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=block_table block_table_test
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=-lpthread -lboost_system

libblock_table.so: block_table.o
	$(CC) $(CPPFLAGS) $(SHARED) $^ -o $@ 

a.out: $(OBJS)
	$(CC) $(CPPFLAGS) $^ $(SHLIBS)

clean:
	$(RM) *.o *.out *.so

.PHONY: clean 

