CC=g++
CPPFLAGS=-I$(INCS) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3
INCS=../include/
TARGETS=block_table block_table_test
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=-lpthread

a.out: $(OBJS)
	$(CC) $(CPPFLAGS) $^ $(SHLIBS)
	-rm *.o *.gch

clean:
	-rm *.o *.out *.so

.PHONY: clean 

