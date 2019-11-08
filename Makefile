CC=g++

INSTALL_DIR = bin
INCLUDE_DIR = 


EXE      = rb
#SOURCE   = cqueue.cpp rb_generic.cpp main.cpp
SOURCE   = cqueue.cpp main.cpp
LINKLIBS = -lpthread 

OBJS     = $(SOURCE:.cpp=.o) 
DEPS     = $(SOURCE:.cpp=.d) 

-include $(DEPS)

CFLAGSALL     = -std=c++11 -Wall -Wextra -Wshadow -march=native 
CFLAGSRELEASE = -O2 -DNDEBUG $(CFLAGSALL)
CFLAGSDEBUG   = -ggdb3 -DDEBUG $(CFLAGSALL)

.PHONY: release
release: CFLAGS = $(CFLAGSRELEASE)
release: all

.PHONY: debug
debug: CFLAGS = $(CFLAGSDEBUG)
debug: all


# compile and link

all : $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(LINKLIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE_DIR) -MMD -MP -c $< -o $@

install : all
	install -d $(INSTALL_DIR)
	install -m 750 $(EXE) $(INSTALL_DIR)

uninstall :
	/bin/rm -rf $(INSTALL_DIR)

clean :
	rm -f *.o $(EXE) *.d
