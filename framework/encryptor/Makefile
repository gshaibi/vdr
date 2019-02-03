CC=g++
CPPFLAGS=$(INCLUDES) $(SHLIBS_PATH) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared

INCS= ../thread_pool/ ../include/ ../communication/include/ ../../include/ 
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=encryptor 
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS_DIRS=../../vdr/protocols ../../framework/communication ../../vdr/master ../../vdr/os_proxy ../thread_pool/
SHLIBS_PATH=$(patsubst %, -L%, $(SHLIBS_DIRS))
SHLIBS=master os_proxy pthread boost_thread boost_system protocols communication 
SHLIBS_LINK=$(patsubst %, -l%, $(SHLIBS))

#TODO: thread_pool shared lib name is not conventional
libencryptor.so: $(OBJS) 
	$(CC) $(CPPFLAGS) $^ -o $@ ../thread_pool/thread_pool.so $(SHLIBS_LINK)


test: $(SHLIBS_DIRS) libencryptor.so

$(SHLIBS_DIRS):
	$(MAKE) --directory=$@


.PHONY: clean 
clean:
	$(RM) *.o *.out *.so

