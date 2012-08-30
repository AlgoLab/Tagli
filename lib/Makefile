# Copyright (c) 2010, Michigan State University.  
# All rights reserved.
#
# Originally released under BSD License
# Downloaded from https://github.com/ged-lab/khmer
#
# 
# Copyright (c) 2012-2012, Gianluca Della Vedova (http://gianluca.dellavedova.org)
NO_UNIQUE_RC=0
CXXFLAGS=-g -fPIC -Wall -O2 -std=c++0x


all: tightString.o test

clean:
	rm -f *.o

test: test.cpp tightString.o
	$(CXX) $(CXXFLAGS) -o test test.cpp tightString.cpp -I /usr/include/

tightString.o: tightString.cpp tightString.hpp globals.hpp

