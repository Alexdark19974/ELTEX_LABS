# $@ - name of the target
# $^ - all prerequisites
# $< - the first prerequisite required to create the output file
# $* - everything that replaces % in targets
# do not mix bash syntax and makefile syntax
# $? - holds a list of all the prerequisites (dependencies) that are newer than the target.
# $? (bash) - holds the exit status code of the last executed command ($$? for make recipes)
OPEN_SOURCE_PROJECTS := Open_Source_Projects
SHELL := /bin/sh
# Single-shell recipes; -e aborts on the first failing command (needed with .ONESHELL).
.SHELLFLAGS := -ec
.ONESHELL:
.DELETE_ON_ERROR:
CC := cc
VLGRND := valgrind
GDB := gdb
CPPCHECK := cppcheck
MOVE := mv
MKDIR := mkdir
COPY := cp
REMOVE := rm
AR := ar
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
