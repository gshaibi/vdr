CC=g++
CPPFLAGS=$(INCLUDES) $(SHLIBS_PATH) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared

INCS=../include/ ../../framework/communication/include ../../include ../../framework/include
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=minion_proxy 
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS_DIRS=../protocols ../../framework/communication ../master ../os_proxy 
SHLIBS_PATH=$(patsubst %, -L%, $(SHLIBS_DIRS))
SHLIBS=master os_proxy pthread boost_thread boost_system protocols communication  
SHLIBS_LINK=$(patsubst %, -l%, $(SHLIBS))

libminion_proxy.so: $(OBJS) 
	$(CC) $(CPPFLAGS) $^ -o $@ $(SHLIBS_LINK)


test: $(SHLIBS_DIRS) libminion_proxy.so

$(SHLIBS_DIRS):
	$(MAKE) --directory=$@


.PHONY: clean 
clean:
	$(RM) *.o *.out *.so

