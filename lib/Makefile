# Copyright (c) 2010, Michigan State University.  
# All rights reserved.
#
# Originally released under BSD License
# Downloaded from https://github.com/ged-lab/khmer
#
# 
# Copyright (c) 2012-2012, Gianluca Della Vedova (http://gianluca.dellavedova.org)
NO_UNIQUE_RC=0
CXXFLAGS=-g -fPIC -Wall -Wextra -pedantic -Weffc++ -O0 -std=c++11
# Clang
#CXX=clang++
# G++
CXX=g++-4.7

## Uncomment the following line to enable compiler optimizations
#CXXFLAGS+=-O2 -march=native

## Uncomment the following line to disable assertions
#CXXFLAGS+=-DNDEBUG


.PHONY: all clean run_test

all: tightString.o run_test test

clean:
	rm -f *.o test

test: test.cpp tightString.o
	$(CXX) $(CXXFLAGS) -o test test.cpp tightString.cpp -I /usr/include/

tightString.o: tightString.cpp tightString.hpp globals.hpp

run_test: test
	./test
