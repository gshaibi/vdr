CC=g++
CPPFLAGS=$(INCLUDES) $(SHLIBS_PATH) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared
ELFFLAGS=-ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 $(INCLUDES) $(SHLIBS_PATH)
space :=
space +=
INCS= ../thread_pool/ ../include/ ../communication/include/ ../../include/ 
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=encryptor 
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS_DIRS=../../vdr/protocols ../../framework/communication ../../vdr/master ../../vdr/os_proxy ../thread_pool/ ../eventer .
SHLIBS_PATH=$(patsubst %, -L%, $(SHLIBS_DIRS))
SHLIBS_PATHS_COLLON_SEP=$(subst $(space),:,$(SHLIBS_DIRS))
SHLIBS=master os_proxy pthread boost_thread boost_system protocols communication eventer
SHLIBS_LINK=$(patsubst %, -l%, $(SHLIBS))

#TODO: thread_pool shared lib name is not conventional
libencryptor.so: $(OBJS) 
	$(CC) $(CPPFLAGS) $^ -o $@ ../thread_pool/thread_pool.so $(SHLIBS_LINK)

test: libencryptor.so test.out 

test.out: encryptor_test.cpp 
	$(CC) $(ELFFLAGS) $< -o $@  -Wl,-rpath=$(SHLIBS_PATHS_COLLON_SEP) $(SHLIBS_LINK) libencryptor.so ../thread_pool/thread_pool.so

$(SHLIBS_DIRS):
	$(MAKE) --directory=$@


.PHONY: clean 
clean:
	$(RM) *.o *.out *.so

