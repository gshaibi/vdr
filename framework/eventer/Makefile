TARGETS=
OBJS=$(patsubst %, %.o, $(TARGETS))
VPATH=$(patsubst %, ../../ds/%, $(TARGETS))

#preprocessor flags used in CXXFLAGS
IPATH=$(patsubst %, -I ../%, $(TARGETS)) -I../include

#using default CXX (g++)
CXXFLAGS+=-std=c++98 -ansi -pedantic-errors $(IPATH) -Wall -Wextra -g3 -O0
FILENAME=$(shell basename $(CURDIR))

a.out: $(FILENAME).o $(FILENAME)_test.o $(OBJS) $(FILENAME).hpp
	$(CXX) $(CXXFLAGS) $^
	rm *.o *.gch

.PHONY: clean remove
clean:
	$(RM) *.a *.o *.out core *.hpp.gch

remove:
	$(RM) oferdinho_*.cpp guy_*.cpp yonatan_*.cpp chen_*.cpp chalil_*.cpp \
				maxim_*.cpp matan_*.cpp hila_*.cpp malachi_*.cpp
