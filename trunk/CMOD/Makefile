# Makefile for the DISSCO CMOD files
# ----------------------
# Compile flags
CXX = g++
C = gcc
LEX = flex
YACC = bison
#CFLAGS =

# Normal
CFLAGS = -std=c++0x -g -ftemplate-depth-25 -I../LASS/src/ 
YACCFLAGS = --verbose --debug -d
# Profiling
#CFLAGS = -O2 -ftemplate-depth-25 -I  -pg	

# Link flags
LFLAGS=-L../LASS/lib/ -lpthread	# Normal
#LFLAGS = -pg					# Profiling

# Library file
LIB= ../LASS/lib/lass.a
#LIB=lass.a
LIB_NAME=lass

# ----------------------
# The object files needed for CMOD
COMP_OBJS = obj/eventfactory.o obj/random.o obj/sieve.o obj/note.o obj/patter.o obj/event.o \
obj/matrix.o obj/bottom.o obj/modifier.o obj/filevalue.o obj/cmodpiece.o obj/main.o
PARSE_OBJS = obj/parser/lex.yy.o obj/parser/parser.o obj/parser/eventparser.o
cmod: $(COMP_OBJS) $(PARSE_OBJS)
	$(CXX) $(CFLAGS) $(LFLAGS) -o cmod $(COMP_OBJS) $(LIB) $(PARSE_OBJS)

# Make destinations
all: 	cmod

clean:
	rm -f obj/*.o obj/parser/*.o *.out *.au cmod

re:    	clean all

obj/%.o: src/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

obj/parser/parser.o: src/lexyacc/parser.y\
						src/eventfactory.h src/filevalue.h
	$(YACC) $(YACCFLAGS) -o src/lexyacc/parser.cpp src/lexyacc/parser.y
	mv src/lexyacc/parser.hpp src/lexyacc/parser.h
	$(CXX) $(CFLAGS) -c src/lexyacc/parser.cpp -o obj/parser/parser.o

obj/parser/lex.yy.o: src/lexyacc/parser.l obj/parser/parser.o
	$(LEX) -osrc/lexyacc/lex.yy.c src/lexyacc/parser.l 
	$(C) $(CFLAGS) -c src/lexyacc/lex.yy.c -o obj/parser/lex.yy.o

obj/parser/eventparser.o: obj/parser/parser.o src/lexyacc/eventparser.h\
						src/lexyacc/eventparser.cpp
	$(CXX) $(CFLAGS) -c src/lexyacc/eventparser.cpp \
					 -o obj/parser/eventparser.o
src/lexyacc/parser.l:
	#empty rule because make complains
src/lexyacc/parser.y:
	#ditto
