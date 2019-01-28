CC=g++
CXXFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC 
space :=
space +=
INCS=include/ vdr/app/ vdr/include/ framework/communication/include/  
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=main
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS_DIRS=vdr/app vdr/master framework/communication vdr/os_proxy vdr/protocols
SHLIBS_PATHS_COLLON_SEP=$(subst $(space),:,$(SHLIBS_DIRS))
SHLIBS_PATH=$(patsubst %, -L%, $(SHLIBS_DIRS))
SHLIBS= app master communication os_proxy protocols boost_thread boost_system boost_chrono
SHLIBS_LINK=$(patsubst %, -l%, $(SHLIBS))

minion: 

vdr: $(SHLIBS_DIRS) vdr.out

vdr.out: $(OBJS) 
	$(CC) $(CXXFLAGS) $^ $(SHLIBS_PATH) -Wl,-rpath=$(SHLIBS_PATHS_COLLON_SEP)  -o $@ $(SHLIBS_LINK)

$(SHLIBS_DIRS):
	$(MAKE) --directory=$@

clean:
	-rm *.o *.out
	-find -name '*.so' -delete 
	-find -name '*.gch' -delete
	-find -name '*.o' -delete
	-find -name '*.out' -delete

.PHONY: all clean $(SHLIBS_DIRS) 

