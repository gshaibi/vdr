CXX=g++
CXXFLAGS=$(INCLUDES) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC 
space :=
space +=
INCS=include/ vdr/app/ vdr/include/ framework/communication/include/  minion/minion_app minion/include 
INCLUDES=$(patsubst %, -I%, $(INCS))
VDR_TARGETS=vdr_main
MINION_TARGETS=minion_main
VDR_OBJS=$(patsubst %, %.o, $(VDR_TARGETS))
MINION_OBJS=$(patsubst %, %.o, $(MINION_TARGETS))
SHLIBS_DIRS=vdr/app minion/minion_app minion/master_proxy minion/minion vdr/master framework/communication vdr/os_proxy vdr/protocols vdr/minion_proxy vdr/block_table 
SHLIBS_PATHS_COLLON_SEP=$(subst $(space),:,$(SHLIBS_DIRS))
SHLIBS_PATH=$(patsubst %, -L%, $(SHLIBS_DIRS))
SHLIBS= app minion_app master_proxy minion master communication os_proxy protocols boost_thread boost_system boost_chrono minion_proxy block_table 
SHLIBS_LINK=$(patsubst %, -l%, $(SHLIBS))


minion: $(SHLIBS_DIRS) minion.out

minion.out: $(MINION_OBJS)
	$(CXX) $(CXXFLAGS) $^ $(SHLIBS_PATH) -Wl,-rpath=$(SHLIBS_PATHS_COLLON_SEP)  -o $@ $(SHLIBS_LINK)

vdr: $(SHLIBS_DIRS) vdr.out

vdr.out: $(VDR_OBJS) 
	$(CXX) $(CXXFLAGS) $^ $(SHLIBS_PATH) -Wl,-rpath=$(SHLIBS_PATHS_COLLON_SEP)  -o $@ $(SHLIBS_LINK)

$(SHLIBS_DIRS):
	$(MAKE) --directory=$@

clean:
	-rm *.o *.out
	-find -name '*.so' -delete 
	-find -name '*.gch' -delete
	-find -name '*.o' -delete
	-find -name '*.out' -delete

.PHONY: all clean $(SHLIBS_DIRS) 

