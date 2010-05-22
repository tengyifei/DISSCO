# note: dependancies are not correct yet

# Just a test if CVS works - Nasko

# --------
# Variables

SRCS = $(wildcard src/*.cpp)
OBJS = $(subst src,obj,$(subst .cpp,.o,$(SRCS)))

CC = c++
CFLAGS = -std=c++0x -g -O2 -ftemplate-depth-25	# Normal
#CFLAGS = -O2 -ftemplate-depth-25 -pg	# Profiling

# --------
# Main Commands

all: lib

clean:
	-rm -f obj/*.o
	-rm -f lib/lass.a
	cd samples; make clean

re:	clean all

lib: $(OBJS)
	# Static library:
	ar -r lib/lass.a obj/*.o	
	ranlib lib/lass.a

depend:
	makedepend -Y -f- -- $(CFLAGS) -- $(SRCS) > Makefile.depends 2> /dev/null
	sed -e "s/^src/obj/" Makefile.depends > Makefile.deps
	-rm Makefile.depends Makefile.bak


# --------
# Individual Objects
include Makefile.deps

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
