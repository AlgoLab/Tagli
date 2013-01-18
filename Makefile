# Copyright (c) 2010, Michigan State University.
# All rights reserved.
#
# Originally released under BSD License
# Downloaded from https://github.com/ged-lab/khmer
#
# This file is a part of Tagli
#
# Copyright (c) 2012-2013, Gianluca Della Vedova (http://gianluca.dellavedova.org)
# Copyright (c) 2012-2013, Yuri Pirola (http://www.algolab.eu/pirola)
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

DEFAULT_STATUS=debug
DEFAULT_PROF=no
DEFAULT_CHECKS_debug=yes
DEFAULT_CHECKS_production=no

SRC_DIR= src
LIB_DIR= lib
TEST_DIR= t
BIN_DIR= bin
OBJ_DIR= obj
DEP_DIR= deps

THIRDPARTY_DIR := thirdparty
THIRDPARTY_DIR_FULL := $(PWD)/thirdparty
THIRDPARTY_LOCAL := $(THIRDPARTY_DIR)/local-deps
THIRDPARTY_LOCAL_FULL := $(PWD)/$(THIRDPARTY_LOCAL)

BASE_CXXFLAGS=-g -fPIC -Wall --std=c++11


ifneq ($(STATUS), debug)
ifneq ($(STATUS), production)
override STATUS := $(DEFAULT_STATUS)
$(warning Status not specified or invalid! Using default status "$(STATUS)")
endif
endif

ifneq ($(PROF), yes)
ifneq ($(PROF), no)
override PROF := $(DEFAULT_PROF)
$(warning Profiling status not specified or invalid! Using default profiling status "$(PROF)")
endif
endif

ifneq ($(CHECKS), yes)
ifneq ($(CHECKS), no)
override CHECKS := $(DEFAULT_CHECKS_$(STATUS))
$(warning Checks status not specified or invalid! Using default checks status "$(CHECKS)")
endif
endif




#####################
# Compiler options for production code
#
ifeq ($(STATUS), production)
OPTP=-march=native -O2
ifeq ($(PROF), no)
OPTP+=-fomit-frame-pointer
endif
else
OPTP=
endif
#####################

#####################
# Compiler options for debugging code
#
ifeq ($(STATUS), debug)
OPTD=-O0
else
OPTD=
endif
#####################

#####################
# Compiler options for profiling code
#
#
ifeq ($(PROF), yes)
OPTPROF=-pg -fprofile-arcs
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


COMPFLAGS=$(OPTP) $(OPTD) $(OPTPROF)

INCLUDE=-I. -I$(LIB_DIR)/ -I$(THIRDPARTY_LOCAL_FULL)/include

LIBS=-lz -lboost_filesystem -lboost_system -llog4cxx

override ADD_DFLAGS:=$(patsubst %,-%,$(subst :, ,$(PERS_DEFINE)))

ifeq (XX$(PERS_DEFINE)XX, XXXX)
PERS_DEFINE=NONE
endif

#####################
# Preprocessor's symbols
#
# NDEBUG   Disable runtime checks in stdlib
#
DFLAGS=$(DCHECK) -D_GNU_SOURCE $(ADD_DFLAGS)
#####################

ADD_CXXFLAGS= $(COMPFLAGS) $(DFLAGS) $(INCLUDE)

COMPILING_DESC=$(CXX)-status_$(STATUS)-prof_$(PROF)-checks_$(CHECKS)-persdefine_$(subst :,_next_,$(PERS_DEFINE))

override BIN_DIR_FULL:=$(PWD)/$(BIN_DIR)/$(COMPILING_DESC)
override OBJ_DIR_FULL:=$(PWD)/$(OBJ_DIR)/$(COMPILING_DESC)
override DEP_DIR_FULL:=$(PWD)/$(DEP_DIR)/$(COMPILING_DESC)

base_SOURCE= \
	$(SRC_DIR)/multiple_passes.cpp \

lib_SOURCE= \
	$(LIB_DIR)/junctions.cpp \
	$(LIB_DIR)/log.cpp \
	$(LIB_DIR)/tightString.cpp \


.PHONY:	all
all: .make thirdparty $(BIN_DIR)/tagli1

.PHONY:	clean
clean:
	@echo " **  Cleaning objects and programs..."
	@rm -rf $(OBJ_DIR_FULL) $(BIN_DIR_FULL) $(DEP_DIR_FULL)

.PHONY:	depclean
depclean: clean
	@echo " **  Cleaning thirdparty libraries..."
	@rm -rf $(THIRDPARTY_LOCAL_FULL)

.make: Makefile
	@echo " *** Makefile modified! Cleaning enforced."
	@$(MAKE) clean
	@touch .make


################################
# Third-party (local) dependencies

.PHONY: thirdparty
thirdparty: cmph kseq cxx-prettyprint


.PHONY: cmph
cmph: $(THIRDPARTY_LOCAL_FULL)/lib/libcxxmph.a

$(THIRDPARTY_LOCAL_FULL)/lib/libcxxmph.a:
	@echo " *** Building and installing CMPH..."
	@mkdir -p $(THIRDPARTY_LOCAL_FULL)
	@cd $(THIRDPARTY_DIR_FULL)/cmph && \
	autoreconf --force -i && \
	./configure --enable-cxxmph --disable-shared --prefix=$(THIRDPARTY_LOCAL_FULL) && \
	make clean && \
	cd cxxmph && \
	make && \
	make install
	@touch $@


.PHONY: kseq
kseq: $(THIRDPARTY_LOCAL_FULL)/include/kseq.h

$(THIRDPARTY_LOCAL_FULL)/include/kseq.h: $(THIRDPARTY_DIR_FULL)/kseq/kseq.h
	mkdir -p $(THIRDPARTY_LOCAL_FULL)/include
	cp $< $@


.PHONY: cxx-prettyprint
cxx-prettyprint: $(THIRDPARTY_LOCAL_FULL)/include/prettyprint.hpp

$(THIRDPARTY_LOCAL_FULL)/include/prettyprint.hpp: $(THIRDPARTY_DIR_FULL)/cxx-prettyprint/prettyprint.hpp
	mkdir -p $(THIRDPARTY_LOCAL_FULL)/include
	cp $< $@

# Google Testing Framework. Actual test specifications are in the bottom
.PHONY: gtest
gtest: $(THIRDPARTY_LOCAL_FULL)/lib/gtest.a $(THIRDPARTY_LOCAL_FULL)/lib/gtest_main.a

GTEST_DIR = $(THIRDPARTY_DIR)/gtest
# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# Flags passed to the preprocessor and to the C++ compiler
GTEST_CPPFLAGS := $(CPPFLAGS) -I$(GTEST_DIR)/include
# Do not use pthreads
GTEST_CXXFLAGS = $(CXXFLAGS) -g -Wall -Wextra -DGTEST_HAS_PTHREAD=0

# Internal variables
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

$(GTEST_DIR)/gtest-all.o : $(GTEST_SRCS_)
	$(CXX) -o $@ $(GTEST_CPPFLAGS) -I$(GTEST_DIR) $(GTEST_CXXFLAGS) -c \
		$(GTEST_DIR)/src/gtest-all.cc

$(GTEST_DIR)/gtest_main.o : $(GTEST_SRCS_)
	$(CXX) -o $@ $(GTEST_CPPFLAGS) -I$(GTEST_DIR) $(GTEST_CXXFLAGS) -c \
		$(GTEST_DIR)/src/gtest_main.cc

$(THIRDPARTY_LOCAL_FULL)/lib/gtest.a : $(GTEST_DIR)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

$(THIRDPARTY_LOCAL_FULL)/lib/gtest_main.a : $(GTEST_DIR)/gtest-all.o $(GTEST_DIR)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^


CXXFLAGS += $(BASE_CXXFLAGS) $(ADD_CXXFLAGS)


.PHONY: tagli1

tagli1: .make $(BIN_DIR)/tagli1

$(BIN_DIR_FULL)_tagli1_OBJS := \
	$(addprefix $(OBJ_DIR_FULL)/,  multiple_passes.o log.o junctions.o tightString.o ) \
	$(addprefix $(THIRDPARTY_LOCAL_FULL)/, lib/libcxxmph.a )

$(BIN_DIR_FULL)/tagli1: .make cmph kseq cxx-prettyprint $($(BIN_DIR_FULL)_tagli1_OBJS)
	@echo ' *   Linking "$(notdir $@)"'
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -o $@ $($(BIN_DIR_FULL)_tagli1_OBJS) $(LIBS)

VPATH = $(LIB_DIR):$(SRC_DIR) #:$(DEP_DIR)

$(BIN_DIR)/%: $(BIN_DIR_FULL)/%
	@cp $^ $@

$(OBJ_DIR_FULL)/%.o: %.cpp .make
	@echo ' *   Compiling $<'
	@mkdir -p $(dir $@)
	@mkdir -p $(DEP_DIR_FULL)
	@$(CXX) -MMD $(CXXFLAGS) -o $@ -c $<
	@mv $(basename $@).d $(DEP_DIR_FULL)
	@sed -i -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' -e '/^$$/ d' -e 's/$$/ :/' $(DEP_DIR_FULL)/$(notdir $(basename $@)).d

$(OBJ_DIR_FULL)/%.o: $(TEST_DIR)/%.cpp .make
	@echo ' *   Compiling $<'
	@mkdir -p $(dir $@)
	@$(CXX) -MMD $(GTEST_CPPFLAGS) $(GTEST_CXXFLAGS) -o $@ -c $<
	@mv $(basename $@).d $(DEP_DIR_FULL)
	@sed -i -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' -e '/^$$/ d' -e 's/$$/ :/' $(DEP_DIR_FULL)/$(notdir $(basename $@)).d

-include $(DEP_DIR)/*.d


################################
# Tests
# All the test executables
TESTS = \
	$(BIN_DIR)/tightString-test \
#	$(BIN_DIR)/test \

test: gtest $(TESTS)
	@for bintest in $(TESTS); do \
		time $$bintest ; \
	done

#$(BIN_DIR_FULL)/test: $(OBJ_DIR_FULL)/test.o $(OBJ_DIR_FULL)/tightString.o
#	@echo ' *   Linking "$(notdir $@)"'
#	@mkdir -p $(dir $@)
#	@$(CXX) $(CXXFLAGS) -o $@  $^ $(LIBS)

$(BIN_DIR_FULL)_tightString_test_OBJS := \
	$(addprefix $(OBJ_DIR_FULL)/,  tightString.o tightString-test.o ) \
	$(addprefix $(THIRDPARTY_LOCAL_FULL)/, lib/gtest_main.a )

$(BIN_DIR_FULL)/tightString-test : gtest $($(BIN_DIR_FULL)_tightString_test_OBJS)
	@echo ' *   Linking "$(notdir $@)"'
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -o $@ $($(BIN_DIR_FULL)_tightString_test_OBJS) $(LIBS)
