CXX=g++
CXXFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared 
INCS=include ../include ../../include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS=

.so: $(OBJS)
$(CXX) $(CXXFLAGS) $^ $(SHLIBS) -o 

test: _test.cpp .cpp
g++ -g3 $^  $(INCLUDES) $(SHLIBS)

clean:
-rm *.o *.out *.so

.PHONY: clean test 
