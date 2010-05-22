# Makefile for the lass sample files
# samplex.cpp (1 <= x <= 5) are generic files used to
#   both test the program and as an example of how to use lass
# sample*.cpp (e.g., sampledvs2.cpp) are specific testing
#   files for various program modules

# Compile flags
CFLAGS = -O2 -ftemplate-depth-25 -I ../src/	# Normal
#CFLAGS = -O2 -ftemplate-depth-25 -I ../src/ -pg# Profiling

# extern EnvelopeLibrary envlib;
# Link flags
LFLAGS = -lpthread # Normal 
#LFLAGS = -pg # Profiling

# Library file
#LIB = -lxerces-c ../lib/lass.a
LIB = ../lib/lass.a
LIBX = -c ../lib/lass.a

LIBX = -c ../lib/lass.a

# Treat *.cpp (in the current directory) as a source file
SRC = $(addsuffix .out,$(basename $(wildcard *.cpp)))

# Make destinations
all: $(SRC)

clean:
	rm -f *.o *.out *.au *.xml

re: clean all

$(LIB):
	cd .. ; make

$(LIBX):
	cd .. ; make	

%.o: %.cpp
	c++ $(CFLAGS) -c $< -o $@

%.out: %.o $(LIBX)
	c++ $(LFLAGS) $< -o $@ $(LIB)
