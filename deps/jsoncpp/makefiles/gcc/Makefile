#
# Makefile for json-cpp
#

# set compiler and linker
CC = g++
LD = g++

# binaries and folders
LIBDIR = /usr/local/lib
LIBNAME = libjson-cpp.so
LIB = $(LIBDIR)/$(LIBNAME)
STATIC = $(LIBDIR)/libjson-cpp.a

# set library and include paths
INCLUDE =  -Iinclude -I/usr/local/include

# set compiler and linker flags
CCFLAGS = -fPIC -O3 -W -Wall
LDFLAGS = -W -Wall -L/usr/local/lib

# source files
SRCS =  $(wildcard src/lib_json/*.cpp)

# dependencies
# object files
OBJS = $(SRCS:.cpp=.o)

# dynamic lib rule
$(LIB): $(OBJS) $(LIBDIR)
	$(LD) $(LDFLAGS) -shared -Wl,-soname,$(LIBNAME) -o $(LIB) $(OBJS)

# static lib rule
$(STATIC): $(OBJS) $(LIBDIR)
	ar rcs $(STATIC) $(OBJS)

# compile rule
.cpp.o:
	$(CC) $(CCFLAGS) $(INCLUDE) -c $<  -o $@

$(LIBDIR):
	@mkdir -p $(LIBDIR)

# tasks
.PHONY:  clean

clean:
	@rm -rf src/lib_json/*.o
	@rm -rf lib

dynamiclibrary: $(LIB)

staticlibrary: $(STATIC)
