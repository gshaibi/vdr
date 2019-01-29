CC=g++
CPPFLAGS=$(INCLUDES) $(SHLIBS_PATH) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3
INCS=../include/ ../../framework/communication/include ../../include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=minion_proxy minion_proxy_test
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS_DIRS=../protocols ../../framework/communication ../master ../os_proxy
SHLIBS_PATH=$(patsubst %, -L%, $(SHLIBS_DIRS))
SHLIBS=-lmaster -los_proxy -lpthread -lboost_thread -lboost_system -lprotocols -lcommunication  

a.out: $(OBJS)
	$(CC) $(CPPFLAGS) $^ $(SHLIBS)
	-rm *.o *.gch

clean:
	-rm *.o *.out *.so

.PHONY: clean 
