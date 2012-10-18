# Copyright (c) 2010, Michigan State University.  
# All rights reserved.
#
# Originally released under BSD License
# Downloaded from https://github.com/ged-lab/khmer
#
# 
# Copyright (c) 2012-2012, Gianluca Della Vedova (http://gianluca.dellavedova.org)
NO_UNIQUE_RC=0
CXX ?= g++ 	# Do not ovveride previous settings
ifeq (XXg++, $(findstring XXg++,XX$(CXX)))

override IS_G++:=yes
$(info The compiler seems to belong to the gcc series...)

ifeq (1, $(shell expr `$(CXX) -dumpversion | cut -d "." -f 2` \>= 7))
$(info ...and its version is greater or equal to 7)
override IS_C11:=yes
else
$(info ...but does not support new c++11 features)
override IS_C11:=no
endif


else

override IS_G++:=no
$(info The compiler does not seem to belong to the gcc series... Using reasonable default values.)

endif


CXXFLAGS=-g -fPIC -Wall -Wextra -pedantic -Weffc++

ifeq ($(IS_G++)$(IS_C11), yesno)
# g++ <= 4.6
CXXFLAGS+=-std=c++0x
endif

ifeq ($(IS_G++)$(IS_C11), yesyes)
# g++ >= 4.7
CXXFLAGS+=-std=c++11
endif

ifeq ($(IS_G++), no)
# others
CXXFLAGS+=-std=c++0x
endif

CXXFLAGS+=-O0
## Uncomment the following line to enable compiler optimizations
CXXFLAGS+=-O2 -march=native

## Uncomment the following line to disable assertions
#CXXFLAGS+=-DNDEBUG


.PHONY: all clean run_test

all: tightString.o run_test test

clean:
	rm -f *.o test

test: test.cpp tightString.o
	$(CXX) $(CXXFLAGS) -o test test.cpp tightString.o -I /usr/include/

tightString.o: tightString.cpp tightString.hpp globals.hpp

run_test: test
	time ./test
