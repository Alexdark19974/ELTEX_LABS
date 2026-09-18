# $@ - name of the target
# $^ - all prerequisites
# $< - the first prerequisite required to create the output file
# $* - everything that replaces % in targets
# do not mix bash syntax and makefile syntax
# $? - holds a list of all the prerequisites (dependencies) that are newer than the target.
# $? (bash) - holds the exit status code of the last executed command ($$? for make recipes)
OPEN_SOURCE_PROJECTS := Open_Source_Projects
SHELL := /bin/sh
CC := /bin/cc
VLGRND := /bin/valgrind
GDB	:= /bin/gdb
CPPCHECK :=	/bin/cppcheck
MOVE := /bin/mv
MKDIR := /bin/mkdir
COPY := /bin/cp
REMOVE := rm
AR := /bin/ar
LIB_PREFIX := lib
AR_EXT := a
SO_EXT := so
Q := @
DEBUG ?= n
LAB ?= all
USE_VALGRIND ?= n
USE_GDB ?= n
USE_SANITIZER ?= n
DEBUG_SUFFIX := debug
GDB_SUFFIX := gdb
PROG_ARGS ?=
VARS_MAKE := vars.make
