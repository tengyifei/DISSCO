#!/bin/sh
flex parser.l
bison --verbose --debug -d -o parser.cpp parser.y
mv parser.hpp parser.h
gcc -g -c lex.yy.c -o lex.yy.o
g++ -g lex.yy.o parser.cpp -o parser
