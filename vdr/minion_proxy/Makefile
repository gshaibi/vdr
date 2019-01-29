CC=g++
CPPFLAGS=$(INCLUDES) $(SHLIBS_PATH) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared

INCS=../include/ ../../framework/communication/include ../../include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=minion_proxy minion_proxy_test
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS_DIRS=../protocols ../../framework/communication ../master ../os_proxy 
SHLIBS_PATH=$(patsubst %, -L%, $(SHLIBS_DIRS))
SHLIBS=master os_proxy pthread boost_thread boost_system protocols communication  
SHLIBS_L=$(patsubst %, -l%, $(SHLIBS))
SHLIBS_DEPENDENCIES=$(patsubst %, -l%, $(SHLIBS))


libminion_proxy.so: $(OBJS)
	$(CC) $(CPPFLAGS) $^ $(SHLIBS) -o $@
	$(RM) *.o *.gch

.PHONY: clean 
clean:
	$(RM) *.o *.out *.so

