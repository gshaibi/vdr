CC=g++
CPPFLAGS=$(INCLUDES) $(SHLIBS_PATH) -ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 -fPIC -shared
ELFFLAGS=-ansi -std=c++98 -pedantic-errors -Wall -Wextra -O0 -g3 $(INCLUDES) $(SHLIBS_PATH)
space :=
space +=
INCS= ../thread_pool/ ../include/ ../communication/include/ ../../include/ 
INCLUDES=$(patsubst %, -I%, $(INCS))
TARGETS=encryptor 
OBJS=$(patsubst %, %.o, $(TARGETS))
SHLIBS_DIRS=/home/beyond/vdr/framework/thread_pool/ /home/beyond/vdr/framework/eventer/ /home/beyond/vdr/framework/communication/ .
SHLIBS_PATH=$(patsubst %, -L%, $(SHLIBS_DIRS))
SHLIBS_PATHS_COLLON_SEP=$(subst $(space),:,$(SHLIBS_DIRS))
SHLIBS=pthread boost_thread boost_system communication eventer
SHLIBS_LINK=$(patsubst %, -l%, $(SHLIBS))

#TODO: thread_pool shared lib name is not conventional
libencryptor.so: $(OBJS) 
	$(CC) $(CPPFLAGS) $^ -o $@ 

test: libs libencryptor.so test.out | libs 

test.out: encryptor_test.cpp 
	$(CC) $(ELFFLAGS) $< -o $@  -Wl,-rpath=$(SHLIBS_PATHS_COLLON_SEP) $(SHLIBS_LINK) libencryptor.so ../thread_pool/thread_pool.so

libs: 
	cd /home/beyond/vdr/framework/thread_pool/ && $(MAKE)
	cd /home/beyond/vdr/framework/eventer/ && $(MAKE)
	cd /home/beyond/vdr/framework/communication/ && $(MAKE)


.PHONY: clean 
clean:
	$(RM) *.o *.out *.so

