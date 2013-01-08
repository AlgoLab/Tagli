# Copyright (c) 2010, Michigan State University.
# All rights reserved.
#
# Originally released under BSD License
# Downloaded from https://github.com/ged-lab/khmer
#
# This file is a part of Tagli
#
# Copyright (c) 2012-2012, Gianluca Della Vedova (http://gianluca.dellavedova.org)
# Copyright (c) 2012-2012, Yuri Pirola (http://www.algolab.eu/pirola)
#
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

SHELL=/bin/bash

.PHONY: all printvars clean build-thirdparty #reall main remain build test-data-create retest-data-create dist prepare-dist install

DEFAULT_STATUS=production
DEFAULT_STATUS=debug
DEFAULT_PROF=no
DEFAULT_CHECKS_debug=yes
DEFAULT_CHECKS_production=no
DEFAULT_USE_COLOR=yes
DEFAULT_USE_PAR=yes

PROG_DIR=/usr/local/bin

SRC_DIR= src
THIRDPARTY_DIR := thirdparty
THIRDPARTY_LIBS := $(wildcard ${THIRDPARTY_DIR}/*)
THIRDPARTY_DEPS := ${THIRDPARTY_DIR}/local-deps
THIRDPARTY_DEPS_FULL := $(shell pwd)/${THIRDPARTY_DIR}/local-deps

LIB_DIR= lib
TEST_DIR= t


DIST_DOC_DIR= dist-docs
SCRIPTS_DIR= scripts
SCRIPTS:= $(wildcard $SCRIPTS_DIR)/*)
DIST_DIR= dist

ifneq ($(STATUS), debug)
ifneq ($(STATUS), production)
override STATUS := ${DEFAULT_STATUS}
$(warning Status not specified or invalid! Using default status "${STATUS}")
endif
endif

ifneq ($(PROF), yes)
ifneq ($(PROF), no)
override PROF := ${DEFAULT_PROF}
$(warning Profiling status not specified or invalid! Using default profiling status "${PROF}")
endif
endif

ifneq ($(CHECKS), yes)
ifneq ($(CHECKS), no)
override CHECKS := $(DEFAULT_CHECKS_$(STATUS))
$(warning Checks status not specified or invalid! Using default checks status "${CHECKS}")
endif
endif


use_gcc=no
ifeq ($(CC), gcc)
use_gcc=yes
endif
ifeq ($(CC), cc)
use_gcc=yes
endif

# Customize compiler options for production code
ifeq ($(STATUS), production)
OPTP+=-march=native
endif
#####################



#####################
# Compiler options for production code
#
ifeq ($(STATUS), production)
ifeq ($(CC), icc)
OPTP=-O2 -xHost -ip -ipo -vec-report1 -fomit-frame-pointer
else
OPTP=-O2 -fomit-frame-pointer
endif
LOG_LEVEL=INFO
else
OPTP=
endif
#####################

#####################
# Compiler options for debugging code
#
ifeq ($(STATUS), debug)
ifeq ($(CC), icc)
OPTD=-O0
else
OPTD=-O0
endif
LOG_LEVEL=DEBUG
else
OPTD=
endif
#####################

#####################
# Compiler options for profiling code
#
#
ifeq ($(PROF), yes)
ifeq ($(CC), icc)
OPTPROF=-pg
else
OPTPROF=-pg -fprofile-arcs
endif
else
OPTPROF=
endif
#####################

#####################
# Compiler options for checking code
#
#
ifeq ($(CHECKS), yes)
DCHECK=-UNDEBUG
else
DCHECK=-DNDEBUG
endif
#####################


COMPFLAGS=$(ARCH_DEP) $(WLBIT) $(OPTB) $(OPTP) $(OPTD) $(OPTPROF)

INCLUDE=-I. -I$(LIB_DIR)/ -I$(THIRDPARTY_DEPS)/include

LIBS=-lz -lboost_filesystem -lboost_system -llog4cxx

override ADD_DFLAGS:=$(patsubst %,-%,$(subst :, ,$(PERS_DEFINE)))

ifeq (XX$(PERS_DEFINE)XX, XXXX)
PERS_DEFINE=NONE
endif

#####################
# Simboli del preprocessore
#
# NDEBUG Disabilita vari controlli
# LOG_MSG Abilita il log dei messaggi
# LOG_THRESHOLD Livello di visualizzazione dei messaggi di log.
# Puo' assumere il valore LOG_LEVEL_XXX con XXX uguale a
# FATAL, ERROR, WARN, INFO, DEBUG, TRACE
# Assume il valore di LOG_LEVEL_INFO se non definito.
#
DFLAGS=$(DCHECK) -D_GNU_SOURCE -DLOG_MSG -DLOG_THRESHOLD=LOG_LEVEL_$(LOG_LEVEL) $(ADD_DFLAGS)
#####################

ADD_CFLAGS= $(COMPFLAGS) $(DFLAGS) $(INCLUDE) $(CFLAGS)

COMPILING_DESC=$(CXX)-$(current_sys)-$(compbit)bit-status_$(STATUS)-prof_$(PROF)-log_$(LOG_LEVEL)-checks_$(CHECKS)-persdefine_$(subst :,_next_,$(PERS_DEFINE))

BIN_DIR= bin

ALL_DIR= $(BIN_DIR) $(SRC_DIR)



base_SOURCE= \
	$(SRC_DIR)/multiple_passes.cpp \


all_header_files:=$(wildcard ${INCLUDE_DIR}/*.hpp $(THIRDPARTY_DIR)/*/*.hpp $(THIRDPARTY_DIR)/*/*.hpp)

#####################
# System information
__DATETIME:=$(shell LANG=C date)
__HOST:=$(shell hostname)


# Sanitize
#__C_SRC_DESC:=$(subst ",\\\",$(___SRC_DESC))
__COMPILER_VER:=$(shell $(CXX) --version | head -n 1)
DSYSINFO=-D__BUILD_DATETIME="\"$(__DATETIME)\"" -D__BUILD_HOST="\"$(__HOST)\""
DSYSINFO+=-D__SRC_DESC="\"$(__C_SRC_DESC)\""
DSYSINFO+=-D__BUILD_DESC="\"compiler=$(CXX) status=$(STATUS) cflags='$(ADD_CFLAGS)'\""
DSYSINFO+=-D__COMPILER_VER="\"$(__COMPILER_VER)\""
#####################

INNER_DIST_DIR= tagli-$(___SRC_DESC)-$(full_current_sys)
FULL_DIST_DIR= $(DIST_DIR)/$(INNER_DIST_DIR)
FULL_SRC_DIST_DIR= $(DIST_DIR)/tagli-$(___SRC_DESC)-src

all: .make thirdparty $(BIN_DIR)/tagli1 run_test

reall	: clean all
	@echo "Cleaned and rebuilt all!"

clean   :
	@echo "Cleaning objects and programs" ; \
	find . -name '*.[ao]' -delete ; find . -name '*.la' -delete ; \
	rm -rf $(BIN_DIR)/* \
	$(SRC_DIR)/options.c $(INCLUDE_DIR)/options.h \
	$(DIST_DIR)/*


.make   :  Makefile
	@echo "Makefile modified! Cleaning enforced."; \
	make clean
	touch .make

test-data-create	: $(test_data_create_PROG)
	@ln -f $(test_data_create_PROG) $(BIN_DIR)

retest-data-create	: clean test-data-create
	@echo 'Cleaned and rebuilt!'

$(test_data_create_OBJ)	: $(stree_OBJ) $(base_OBJ) $(test_data_create_SOURCE)

$(test_data_create_PROG)	: $(stree_OBJ) $(base_OBJ) $(test_data_create_OBJ)
	@echo ' * Linking $(notdir $@)'; \
	mkdir -pv $(BIN_DIR) ; \
	$(CC) -o $(test_data_create_PROG) $(ADD_CFLAGS) $(LDFLAGS_ARCH) $^ $(LIBS) ; \
	echo '   ...done.'; \




$(min_factorization_OBJ)	: $(base_OBJ) $(min_factorization_SOURCE)

$(min_factorization_PROG)	: $(base_OBJ) $(min_factorization_OBJ)
	@echo ' * Linking $(notdir $@)'; \
	mkdir -pv $(BIN_DIR) ; \
	$(CC) -o $(min_factorization_PROG) $(ADD_CFLAGS) $(LDFLAGS_ARCH) $^ $(LIBS) ; \
	echo '   ...done.'; \



raw_script_copy = cp $(script) $(FULL_DIST_DIR)/bin/$(notdir $(basename $(script))) && sed -i -e "s/_____%TAGLI_VERSION%_____/ ${___SRC_DESC}/" $(FULL_DIST_DIR)/bin/$(notdir $(basename $(script))) &&


script_copy = $(if $(findstring .pl,$(suffix $(script))),$(perl_script_copy),$(raw_script_copy))

prepare-dist	: all
	@echo ' * Preparing distribution...'; \
	rm -rf $(DIST_DIR)/tagli-*-$(full_current_sys)/ && \
	mkdir -p $(FULL_DIST_DIR) && \
	mkdir -p $(FULL_DIST_DIR)/bin && \
	mkdir -p $(FULL_DIST_DIR)/doc && \
	cp $(est_fact_PROG) $(FULL_DIST_DIR)/bin && \
	cp $(min_factorization_PROG) $(FULL_DIST_DIR)/bin && \
	cp $(intron_agreement_PROG) $(FULL_DIST_DIR)/bin && \
	cp $(max_transcr_PROG) $(FULL_DIST_DIR)/bin && \
	cp $(cds_annotation_PROG) $(FULL_DIST_DIR)/bin && \
	$(foreach script,$(SCRIPTS),$(script_copy)) \
	cp -r $(DIST_DOC_DIR)/* $(FULL_DIST_DIR)/doc && \
	( cd $(DIST_DIR) && rm -f tagli-latest && ln -s $(INNER_DIST_DIR) tagli-latest; ) && \
	echo '   ...preparation done.' && \
	echo '** The software package has been prepared for $(full_current_sys) in directory $(FULL_DIST_DIR)';

dist	: prepare-dist
	@echo ' * Compressing...'; \
	rm -rf $(DIST_DIR)/tagli-*-$(full_current_sys).tar.gz && \
	tar czf $(FULL_DIST_DIR).tar.gz -C $(DIST_DIR) `basename $(FULL_DIST_DIR)` && \
	echo '   ...done.'; \
	echo '** The software package has been prepared for $(full_current_sys) in file $(FULL_DIST_DIR).tar.gz';

install : prepare-dist


doc_symb_link = ln -s $(doc) $(FULL_SRC_DIST_DIR)/ &&

.PHONY: base-prepare-src-dist
base-prepare-src-dist	: clean
	@echo ' * Preparing source distribution...'; \
	rm -rf $(DIST_DIR)/tagli-*-src/ && \
	mkdir -p $(FULL_SRC_DIST_DIR) && \
	cp Makefile $(FULL_SRC_DIST_DIR) && \
	cp -r $(SRC_DIR) $(FULL_SRC_DIST_DIR) && \
	cp -r $(INCLUDE_DIR) $(FULL_SRC_DIST_DIR) && \
	cp -r $(SCRIPTS_DIR) $(FULL_SRC_DIST_DIR) && \
	cp -r $(DIST_DOC_DIR) $(FULL_SRC_DIST_DIR) && \
	$(foreach doc,$(wildcard $(DIST_DOC_DIR)/*),$(doc_symb_link)) \
	echo '   ...preparation done.' && \
	echo '** The source of the software package has been prepared in directory $(FULL_SRC_DIST_DIR)';

.PHONY: prepare-src-dist
prepare-src-dist	: base-prepare-src-dist
	@echo ' * Determining source version...'; \
	echo '$(___SRC_DESC)' > $(FULL_SRC_DIST_DIR)/VERSION ; \
	echo '   ...done.'; \

.PHONY: src-dist
src-dist	: prepare-src-dist
	@echo ' * Compressing...'; \
	rm -rf $(FULL_SRC_DIST_DIR).tar.gz && \
	tar czf $(FULL_SRC_DIST_DIR).tar.gz -C $(DIST_DIR) `basename $(FULL_SRC_DIST_DIR)` && \
	echo '   ...done.'; \
	echo '** The source of software package has been prepared in file $(FULL_SRC_DIST_DIR).tar.gz';

check-syntax:
	gcc -o /dev/null -D$(DEFINE) $(CFLAGS) -S ${CHK_SOURCES}



NO_UNIQUE_RC=0
CXX ?= g++      # Do not ovveride previous settings
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


CXXFLAGS=-g -fPIC -O0


ifeq ($(IS_G++)$(IS_C11), yesyes)
# g++ >= 4.7
CXXFLAGS+=-std=c++11
else
CXXFLAGS+=-std=c++0x
endif

## Uncomment the following line to enable compiler optimizations
CXXFLAGS+=-O2 -march=native
CXXFLAGS_EXTRA= -Wall -Wextra -pedantic -Weffc++

## Uncomment the following line to disable assertions
#CXXFLAGS+=-DNDEBUG


.PHONY: all clean run_test thirdparty cmph kseq

thirdparty: cmph kseq cxx-prettyprint gtest

cmph: $(THIRDPARTY_DEPS)/lib/libcxxmph.a

$(THIRDPARTY_DEPS)/lib/libcxxmph.a:
	@echo "Building and installing CMPH..." ; \
	mkdir -pv ${THIRDPARTY_DEPS}/ && \
	pushd ${THIRDPARTY_DIR}/cmph && \
	autoreconf --force && \
	./configure --enable-cxxmph --disable-shared --prefix=${THIRDPARTY_DEPS_FULL} && \
	make clean && \
	pushd cxxmph && \
	make && \
	make install && \
	popd && \
	popd && \
	touch $@

kseq: $(THIRDPARTY_DEPS)/include/kseq.h

$(THIRDPARTY_DEPS)/include/kseq.h: ${THIRDPARTY_DIR}/kseq/kseq.h
	mkdir -pv ${THIRDPARTY_DEPS}/include ; \
	cp $< $@

cxx-prettyprint: $(THIRDPARTY_DEPS)/include/prettyprint.hpp

$(THIRDPARTY_DEPS)/include/prettyprint.hpp: ${THIRDPARTY_DIR}/cxx-prettyprint/prettyprint.hpp
	mkdir -pv ${THIRDPARTY_DEPS}/include ; \
	cp $< $@

$(BIN_DIR)/test: $(TEST_DIR)/test.cpp $(LIB_DIR)/tightString.o
	$(CXX) $(CXXFLAGS) -o $@  $^ $(INCLUDE)


$(BIN_DIR)/tagli1: $(SRC_DIR)/multiple_passes.cpp $(LIB_DIR)/log.cpp $(LIB_DIR)/junctions.o $(LIB_DIR)/tightString.o $(THIRDPARTY_DEPS)/lib/libcxxmph.a
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_EXTRA) -o $@  $^ $(INCLUDE) $(LIBS)


$(LIB_DIR)/tightString.o: $(LIB_DIR)/tightString.cpp $(LIB_DIR)/tightString.hpp $(LIB_DIR)/globals.hpp
	$(CXX) -c $(CXXFLAGS) $(CXXFLAGS_EXTRA) -o $@  $< $(INCLUDE)

$(LIB_DIR)/junctions.o: $(LIB_DIR)/junctions.cpp $(LIB_DIR)/junctions.hpp $(LIB_DIR)/globals.hpp $(LIB_DIR)/tightString.o
	$(CXX) -c $(CXXFLAGS) $(CXXFLAGS_EXTRA) -o $@  $< $(INCLUDE)

run_test: $(BIN_DIR)/test build gtests
	time $(BIN_DIR)/tightString-test


build: $(BIN_DIR)/tagli1

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = thirdparty/gtest/
GTEST_HEADERS = thirdparty/gtest/include/gtest/*.h \
                thirdparty/gtest/include/gtest/internal/*.h

# Flags passed to the preprocessor.
# Do not use pthreads
GTEST_CXXFLAGS = $(CXXFLAGS) -I$(GTEST_DIR)/include -I$(INCLUDE)  -DGTEST_HAS_PTHREAD=0

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS = $(BIN_DIR)/tightString-test

# House-keeping build targets.

gtests : gtest $(TESTS)

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)
gtest : $(LIB_DIR)/gtest.a $(LIB_DIR)/gtest_main.a
# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
$(LIB_DIR)/gtest-all.o : $(GTEST_SRCS_)
	$(CXX) -I$(GTEST_DIR) $(GTEST_CXXFLAGS)  -c $(GTEST_DIR)/src/gtest-all.cc  -o $@

$(LIB_DIR)/gtest_main.o : $(GTEST_SRCS_)
	$(CXX) -I$(GTEST_DIR) $(GTEST_CXXFLAGS) -c $(GTEST_DIR)/src/gtest_main.cc -o $@

$(LIB_DIR)/gtest.a : $(LIB_DIR)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^
$(LIB_DIR)/gtest_main.a : $(LIB_DIR)/gtest-all.o $(LIB_DIR)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# Builds a sample test.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.

$(LIB_DIR)/tightString-test.o : $(TEST_DIR)/tightString-test.cpp $(LIB_DIR)/tightString.o $(GTEST_HEADERS)
	$(CXX) $(GTEST_CXXFLAGS) -c $(TEST_DIR)/tightString-test.cpp -o $@

$(BIN_DIR)/tightString-test : $(LIB_DIR)/tightString.o $(LIB_DIR)/tightString-test.o $(LIB_DIR)/gtest_main.a
	$(CXX) $(GTEST_CXXFLAGS) $^ -o $@
