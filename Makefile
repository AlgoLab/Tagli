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


.PHONY: all clean build-thirdparty #reall main remain build test-data-create retest-data-create dist prepare-dist install 

DEFAULT_STATUS=production
DEFAULT_PROF=no
DEFAULT_CHECKS_debug=yes
DEFAULT_CHECKS_production=no
DEFAULT_LOG_GRAPHS=no
DEFAULT_FORCE_32BIT=no
DEFAULT_USE_COLOR=no
DEFAULT_USE_PAR=yes

PROG_DIR=/usr/local/bin

SRC_DIR= src
THIRDPARTY_DIR = thirdparty
STREE_DIR= stree_src
INCLUDE_DIR= include

DIST_DOC_DIR= dist-docs
DIST_SCRIPTS_DIR= dist-scripts
DIST_SCRIPTS:= $(wildcard $(DIST_SCRIPTS_DIR)/*)
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

ifneq ($(LOG_GRAPHS), yes)
ifneq ($(LOG_GRAPHS), no)
override LOG_GRAPHS := $(DEFAULT_LOG_GRAPHS)
$(warning Logging graph status not specified or invalid! Using default logging graph status "${LOG_GRAPHS}")
endif
endif

ifneq ($(USE_COLOR), yes)
ifneq ($(USE_COLOR), no)
override USE_COLOR:= $(DEFAULT_USE_COLOR)
endif
endif

#####################
## 64 BIT CHECK CODE
tmp_is_64_bit:=/$(shell uname -m)/
ifeq ($(tmp_is_64_bit), /x86_64/)
is_64_bit:=yes
else
ifeq ($(tmp_is_64_bit), /ia64/)
is_64_bit:=yes
else
is_64_bit:=no
endif
endif

ifeq ($(is_64_bit), yes)
ifneq ($(FORCE_32BIT), yes)
ifneq ($(FORCE_32BIT), no)
override FORCE_32BIT:=$(DEFAULT_FORCE_32BIT)
$(warning The machine is running 64 bit kernel but the pointer size has not been specified! Using 32bit? $(FORCE_32BIT))
endif
endif
ifeq ($(FORCE_32BIT), yes)
override WLBIT:=-m32
override compbit:=32
else
override WLBIT:=-m64
override compbit:=64
endif
else
override WLBIT:=
override compbit:=32
endif

## END 64 BIT CHECK CODE
#####################

ifeq ($(USE_COLOR), yes)
PHF:=\033[01;32m
PF:=\033[32m
SF:=\033[00m
else
PHF:=
PF:=
SF:=
endif

use_gcc=no
ifeq ($(CC), gcc)
use_gcc=yes
endif
ifeq ($(CC), cc)
use_gcc=yes
endif

#####################
# Architecture-dependent flags
#
ifeq ($(current_sys), Linux)
CFLAGS_ARCH:=
LDFLAGS_ARCH:=
# Customize compiler options for production code
ifeq ($(STATUS), production)
OPTP+=-march=native
endif
else
CFLAGS_ARCH:=
LDFLAGS_ARCH:=
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

#####################
# Compiler options for logging graphs
#
#
ifeq ($(LOG_GRAPHS), yes)
DLOGGRAPHS=-DLOG_GRAPHS
else
DLOGGRAPHS=-ULOG_GRAPHS
endif
#####################


COMPFLAGS=$(ARCH_DEP) $(WLBIT) $(OPTB) $(OPTP) $(OPTD) $(OPTPROF)

INCLUDE=-I. -I$(INCLUDE_DIR)/ -I$(STREE_DIR)/

LIBS=-lm #-lgsl -lgslcblas #-lefence

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
DFLAGS=$(DCHECK) $(DLOGGRAPHS) -D_GNU_SOURCE -DLOG_MSG -DLOG_THRESHOLD=LOG_LEVEL_$(LOG_LEVEL) $(ADD_DFLAGS)
#####################

ADD_CFLAGS= $(COMPFLAGS) $(DFLAGS) $(INCLUDE) $(CFLAGS)

COMPILING_DESC=$(CC)-$(current_sys)-$(compbit)bit-status_$(STATUS)-prof_$(PROF)-log_$(LOG_LEVEL)-loggraphs_$(LOG_GRAPHS)-checks_$(CHECKS)-persdefine_$(subst :,_next_,$(PERS_DEFINE))

BASE_BIN_DIR= bin
BIN_DIR= $(BASE_BIN_DIR)/_tmp/$(COMPILING_DESC)
BASE_OBJ_DIR= obj
OBJ_DIR= $(BASE_OBJ_DIR)/$(COMPILING_DESC)

ALL_DIR= $(BIN_DIR) $(SRC_DIR) $(OBJ_DIR)



base_SOURCE= \
	$(SRC_DIR)/multiple_passes.cpp \


base_OBJ= \
	$(OBJ_DIR)/multiple_passes.o \


all_header_files:=$(wildcard ${INCLUDE_DIR}/*.h)

stree_header_files:=$(wildcard ${STREE_DIR}/*.h)

#####################
# System information
__DATETIME:=$(shell LANG=C date)
__HOST:=$(shell hostname)
have_git:=/$(shell which git)/
is_git_repository:=/$(wildcard .git)/
have_VERSION:=/$(wildcard VERSION)/
ifeq ($(is_git_repository), //)      # We are NOT in the original git repository

ifeq (/$(wildcard VERSION)/,//)          # We do NOT have the VERSION file
___SRC_DESC:=version-unknown
$(warning Impossible to get the source-code version.)
else                                     # We HAVE the VERSION file
___SRC_DESC:=$(shell cat VERSION)
endif # VERSION

else                                 # We ARE in the original git repository

ifeq ($(have_git), //)                   # We do NOT have git
___GIT_REF:=.git/$(shell cat .git/HEAD | cut -d ' ' -f 2)
ifeq ($(___GIT_REF),.git/)
___GIT_REF:= .git/refs/heads/master
endif
ifeq (/$(wildcard $(___GIT_REF))/,//)        # We CANNOT find the current head
ifeq (/$(wildcard VERSION)/,//)                  # We do NOT have the VERSION file
___SRC_DESC:=version-unknown
$(warning Impossible to get the source-code version.)
else                                             # We HAVE the VERSION file
___SRC_DESC:=$(shell cat VERSION)
endif # VERSION
else                                         # We FOUND the current head
___SRC_DESC:=commit-$(shell tail -n1 $(___GIT_REF))
endif # ___GIT_REF
else                                     # We HAVE git
___SRC_DESC:=$(shell git describe)
endif # have_git

endif # is_git_repository

# Sanitize
#__C_SRC_DESC:=$(subst ",\\\",$(___SRC_DESC))
__COMPILER_VER:=$(shell $(CC) --version | head -n 1)
DSYSINFO=-D__BUILD_DATETIME="\"$(__DATETIME)\"" -D__BUILD_HOST="\"$(__HOST)\""
DSYSINFO+=-D__SRC_DESC="\"$(__C_SRC_DESC)\""
DSYSINFO+=-D__BUILD_DESC="\"compiler=$(CC) status=$(STATUS) cflags='$(ADD_CFLAGS)'\""
DSYSINFO+=-D__COMPILER_VER="\"$(__COMPILER_VER)\""
#####################

INNER_DIST_DIR= pintron-$(___SRC_DESC)-$(full_current_sys)
FULL_DIST_DIR= $(DIST_DIR)/$(INNER_DIST_DIR)
FULL_SRC_DIST_DIR= $(DIST_DIR)/pintron-$(___SRC_DESC)-src

all	: build

script_copy_to_bin = cp $(script) $(BASE_BIN_DIR)/$(notdir $(basename $(script))) &&

all	:
	@echo "${PHF}All compiled!${SF}"

build-thirdparty : .make
	cd $(THIRDPARTY_DIR)
	for d in *
	do		
		test -d $d && test -f $d/Makefile && (cd $d ; make)
	done

reall	: clean all
	@echo "${PHF}Cleaned and rebuilt all!${SF}"

clean   :
	@echo "${PHF}Cleaning objects and programs${SF}" ; \
	rm -rf $(BASE_OBJ_DIR)/* $(BASE_BIN_DIR)/* \
		$(SRC_DIR)/options.c $(INCLUDE_DIR)/options.h \
		$(DIST_DIR)/* t/test

build	: .make est-fact min-factorization intron-agreement max-transcr cds-annotation # build-transcr
	@$(foreach script,$(DIST_SCRIPTS),$(script_copy_to_bin)) \
	echo "Configuration: ${COMPFLAGS}"; \
	echo "Compiler:      ${CC}"; \
	echo "Symbols:       ${DFLAGS}"; \
	echo '${PHF}All built!${SF}'


$(SRC_DIR)/options.c: $(SRC_DIR)/options.ggo
	@echo "${PHF} * Generating the configuration parser...${SF}"; \
	gengetopt --output-dir=`dirname $@` \
		--file-name=`basename $@ .c` < $<; \
	mv $(SRC_DIR)/`basename $@ .c`.h $(INCLUDE_DIR)/

.make   :  Makefile
	@echo "${PHF}Makefile modified! Cleaning enforced.${SF}"; \
	rm -rf $(BASE_OBJ_DIR)/* $(BASE_BIN_DIR)/* \
           $(SRC_DIR)/options.c $(INCLUDE_DIR)/options.h; \
        touch .make

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(all_header_files) $(stree_header_files)
	@echo '${PF} * Compiling ${SF}$<'; \
	mkdir -pv $(dir $@) ; \
	$(CC) $(ADD_CFLAGS)  $(DSYSINFO) $(CFLAGS_ARCH) -o $@ -c $<

#################################################################
$(OBJ_DIR)/%.o: $(STREE_DIR)/%.c $(stree_header_files)
	@echo '${PF} * Compiling ${SF}$<'; \
	mkdir -pv $(dir $@) ; \
	$(CC) $(ADD_CFLAGS) $(CFLAGS_ARCH) -o $@ -c $<
#################################################################



est-fact	: $(est_fact_PROG)
	@ln -f $(est_fact_PROG) $(BASE_BIN_DIR) ; \

reest-fact	: clean est-fact
	@echo '${PHF}Cleaned and rebuilt!${SF}'

$(est_fact_OBJ)	: $(stree_OBJ) $(base_OBJ) $(est_fact_SOURCE)

$(est_fact_PROG)	: $(stree_OBJ) $(base_OBJ) $(est_fact_OBJ)
	@echo '${PHF} * Linking${SF} $(notdir $@)'; \
	mkdir -pv $(BIN_DIR) ; \
	$(CC) -o $(est_fact_PROG) $(ADD_CFLAGS) $(LDFLAGS_ARCH) $(DSYSINFO) $^ $(LIBS); \
	echo '   ${PHF}...done.${SF}'; \



test-data-create	: $(test_data_create_PROG)
	@ln -f $(test_data_create_PROG) $(BASE_BIN_DIR)

retest-data-create	: clean test-data-create
	@echo '${PHF}Cleaned and rebuilt!${SF}'

$(test_data_create_OBJ)	: $(stree_OBJ) $(base_OBJ) $(test_data_create_SOURCE)

$(test_data_create_PROG)	: $(stree_OBJ) $(base_OBJ) $(test_data_create_OBJ)
	@echo '${PHF} * Linking${SF} $(notdir $@)'; \
	mkdir -pv $(BIN_DIR) ; \
	$(CC) -o $(test_data_create_PROG) $(ADD_CFLAGS) $(LDFLAGS_ARCH) $^ $(LIBS) ; \
	echo '   ${PHF}...done.${SF}'; \



min-factorization	: $(min_factorization_PROG)
	@ln -f $(min_factorization_PROG) $(BASE_BIN_DIR)

remin-factorization : clean min-factorization
	@echo '${PHF}Cleaned and rebuilt!${SF}'

$(min_factorization_OBJ)	: $(base_OBJ) $(min_factorization_SOURCE)

$(min_factorization_PROG)	: $(base_OBJ) $(min_factorization_OBJ)
	@echo '${PHF} * Linking${SF} $(notdir $@)'; \
	mkdir -pv $(BIN_DIR) ; \
	$(CC) -o $(min_factorization_PROG) $(ADD_CFLAGS) $(LDFLAGS_ARCH) $^ $(LIBS) ; \
	echo '   ${PHF}...done.${SF}'; \



intron-agreement	: $(intron_agreement_PROG)
	@ln -f $(intron_agreement_PROG) $(BASE_BIN_DIR)

reintron-agreement : clean intron-agreement
	@echo '${PHF}Cleaned and rebuilt!${SF}'

$(intron_agreement_OBJ)	: $(base_OBJ) $(intron_agreement_SOURCE)

$(intron_agreement_PROG)	: $(base_OBJ) $(intron_agreement_OBJ)
	@echo '${PHF} * Linking${SF} $(notdir $@)'; \
	mkdir -pv $(BIN_DIR) ; \
	$(CC) -o $(intron_agreement_PROG) $(ADD_CFLAGS) $(LDFLAGS_ARCH) $^ $(LIBS) ; \
	echo '   ${PHF}...done.${SF}'; \


max-transcr	: $(max_transcr_PROG)
	@ln -f $(max_transcr_PROG) $(BASE_BIN_DIR)

remax-transcr : clean max-transcr
	@echo '${PHF}Cleaned and rebuilt!${SF}'

$(max_transcr_OBJ)	: $(max_transcr_SOURCE)

$(max_transcr_PROG)	: $(max_transcr_OBJ)
	@echo '${PHF} * Linking${SF} $(notdir $@)'; \
	mkdir -pv $(BIN_DIR) ; \
	$(CC) -o $(max_transcr_PROG) $(ADD_CFLAGS) $(LDFLAGS_ARCH) $^ $(LIBS) ; \
	echo '   ${PHF}...done.${SF}'; \



$(cds_annotation_OBJ)	: $(cds_annotation_SOURCE)

$(cds_annotation_PROG)	: $(cds_annotation_OBJ)
	@echo '${PHF} * Linking${SF} $(notdir $@)'; \
	mkdir -pv $(BIN_DIR) ; \
	$(CC) -o $(cds_annotation_PROG) $(ADD_CFLAGS) $(LDFLAGS_ARCH) $^ $(LIBS) ; \
	echo '   ${PHF}...done.${SF}'; \


raw_script_copy = cp $(script) $(FULL_DIST_DIR)/bin/$(notdir $(basename $(script))) && sed -i -e "s/_____%PINTRON_VERSION%_____/ ${___SRC_DESC}/" $(FULL_DIST_DIR)/bin/$(notdir $(basename $(script))) &&


script_copy = $(if $(findstring .pl,$(suffix $(script))),$(perl_script_copy),$(raw_script_copy))

prepare-dist	: all
	@echo '${PHF} * Preparing distribution...${SF}'; \
	rm -rf $(DIST_DIR)/pintron-*-$(full_current_sys)/ && \
	mkdir -p $(FULL_DIST_DIR) && \
	mkdir -p $(FULL_DIST_DIR)/bin && \
	mkdir -p $(FULL_DIST_DIR)/doc && \
	cp $(est_fact_PROG) $(FULL_DIST_DIR)/bin && \
	cp $(min_factorization_PROG) $(FULL_DIST_DIR)/bin && \
	cp $(intron_agreement_PROG) $(FULL_DIST_DIR)/bin && \
	cp $(max_transcr_PROG) $(FULL_DIST_DIR)/bin && \
	cp $(cds_annotation_PROG) $(FULL_DIST_DIR)/bin && \
	$(foreach script,$(DIST_SCRIPTS),$(script_copy)) \
	cp -r $(DIST_DOC_DIR)/* $(FULL_DIST_DIR)/doc && \
	( cd $(DIST_DIR) && rm -f pintron-latest && ln -s $(INNER_DIST_DIR) pintron-latest; ) && \
	echo '${PHF}   ...preparation done.${SF}' && \
	echo '${PHF}** The software package has been prepared for $(full_current_sys) in directory $(FULL_DIST_DIR)${SF}';

dist	: prepare-dist
	@echo '${PHF} * Compressing...${SF}'; \
	rm -rf $(DIST_DIR)/pintron-*-$(full_current_sys).tar.gz && \
	tar czf $(FULL_DIST_DIR).tar.gz -C $(DIST_DIR) `basename $(FULL_DIST_DIR)` && \
	echo '${PHF}   ...done.${SF}'; \
	echo '${PHF}** The software package has been prepared for $(full_current_sys) in file $(FULL_DIST_DIR).tar.gz${SF}';

install : prepare-dist


doc_symb_link = ln -s $(doc) $(FULL_SRC_DIST_DIR)/ &&

.PHONY: base-prepare-src-dist
base-prepare-src-dist	: clean
	@echo '${PHF} * Preparing source distribution...${SF}'; \
	rm -rf $(DIST_DIR)/pintron-*-src/ && \
	mkdir -p $(FULL_SRC_DIST_DIR) && \
	cp Makefile $(FULL_SRC_DIST_DIR) && \
	cp -r $(SRC_DIR) $(FULL_SRC_DIST_DIR) && \
	cp -r $(STREE_DIR) $(FULL_SRC_DIST_DIR) && \
	cp -r $(INCLUDE_DIR) $(FULL_SRC_DIST_DIR) && \
	cp -r $(DIST_SCRIPTS_DIR) $(FULL_SRC_DIST_DIR) && \
	cp -r $(DIST_DOC_DIR) $(FULL_SRC_DIST_DIR) && \
	$(foreach doc,$(wildcard $(DIST_DOC_DIR)/*),$(doc_symb_link)) \
	echo '${PHF}   ...preparation done.${SF}' && \
	echo '${PHF}** The source of the software package has been prepared in directory $(FULL_SRC_DIST_DIR)${SF}';

.PHONY: prepare-src-dist
prepare-src-dist	: base-prepare-src-dist
	@echo '${PHF} * Determining source version...${SF}'; \
	echo '$(___SRC_DESC)' > $(FULL_SRC_DIST_DIR)/VERSION ; \
	echo '${PHF}   ...done.${SF}'; \

.PHONY: src-dist
src-dist	: prepare-src-dist
	@echo '${PHF} * Compressing...${SF}'; \
	rm -rf $(FULL_SRC_DIST_DIR).tar.gz && \
	tar czf $(FULL_SRC_DIST_DIR).tar.gz -C $(DIST_DIR) `basename $(FULL_SRC_DIST_DIR)` && \
	echo '${PHF}   ...done.${SF}'; \
	echo '${PHF}** The source of software package has been prepared in file $(FULL_SRC_DIST_DIR).tar.gz${SF}';

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


test: test.cpp tightString.o
	$(CXX) $(CXXFLAGS) -o test test.cpp tightString.o -I /usr/include/

tightString.o: tightString.cpp tightString.hpp globals.hpp

run_test: test
	time ./test
