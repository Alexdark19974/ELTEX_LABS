FLAG_1 = -o
FLAG_2 = -fPIC
FLAG_3 = -c
FLAG_4 = -g
FLAG_5 = -Wall
LIB_FLAG = -shared
L_FLAG_7 = -ldl
DIR = ./plugin
SUBDIR = /calc_lib
SUBDIR_2 = /libs
SUBDIR_3 = /src_files
SUBDIR_4 = /obj_files
MOVE = mv
CC = gcc
LIB_NAME = libmy_calc
LIB_NAME_2 = libmy_add
LIB_NAME_3 = libmy_sub
LIB_NAME_4 = libmy_mul
LIB_NAME_5 = libmy_div
LIB_NAME_6 = libmy_stack
LIB_NAME_7 = libmy_get_line
HEADER = calc_hub.h
FUNC_0 = main
FUNC_1 = calc
FUNC_2 = add
FUNC_3 = sub
FUNC_4 = mul
FUNC_5 = div
FUNC_6 = push_into_stack
FUNC_7 = stack_pop
FUNC_8 = my_get_line
CHAIN_1 = .$(SUBDIR_3)/$(FUNC_1).c .$(SUBDIR_3)/$(FUNC_2).c .$(SUBDIR_3)/$(FUNC_3).c .$(SUBDIR_3)/$(FUNC_4).c
CHAIN_2 = .$(SUBDIR_3)/$(FUNC_5).c .$(SUBDIR_3)/$(FUNC_6).c .$(SUBDIR_3)/$(FUNC_7).c .$(SUBDIR_3)/$(FUNC_8).c
CHAIN_3 = $(FUNC_1).o $(FUNC_2).o $(FUNC_3).o $(FUNC_4).o
CHAIN_4 = $(FUNC_5).o $(FUNC_6).o $(FUNC_7).o $(FUNC_8).o
EXEC_FILE_1 = dirplugin

.PHONY: pic lib_all all test_dirplugin

pic: $(CHAIN_1) $(CHAIN_2)

		$(CC) $(FLAG_3) $(CHAIN_1) $(CHAIN_2) $(FLAG_2) $(FLAG_4)
		$(MOVE) $(CHAIN_3) $(CHAIN_4) .$(SUBDIR_4)

lib_all:

		$(CC) $(LIB_FLAG) .$(SUBDIR_4)/$(FUNC_1).o $(FLAG_1) $(LIB_NAME).so $(FLAG_4)

		$(MOVE) $(LIB_NAME).so .$(SUBDIR)

		$(CC) $(LIB_FLAG) .$(SUBDIR_4)/$(FUNC_2).o $(FLAG_1) $(LIB_NAME_2).so $(FLAG_4)

		$(CC) $(LIB_FLAG) .$(SUBDIR_4)/$(FUNC_3).o $(FLAG_1) $(LIB_NAME_3).so $(FLAG_4)

		$(CC) $(LIB_FLAG) .$(SUBDIR_4)/$(FUNC_4).o $(FLAG_1) $(LIB_NAME_4).so $(FLAG_4)

		$(CC) $(LIB_FLAG) .$(SUBDIR_4)/$(FUNC_5).o $(FLAG_1) $(LIB_NAME_5).so $(FLAG_4)

		$(CC) $(LIB_FLAG) .$(SUBDIR_4)/$(FUNC_6).o .$(SUBDIR_4)/$(FUNC_7).o $(FLAG_1) $(LIB_NAME_6).so $(FLAG_4)

		$(CC) $(LIB_FLAG) .$(SUBDIR_4)/$(FUNC_8).o $(FLAG_1) $(LIB_NAME_7).so $(FLAG_4)

		$(MOVE) $(LIB_NAME_2).so $(LIB_NAME_3).so $(LIB_NAME_4).so $(LIB_NAME_5).so $(LIB_NAME_6).so $(LIB_NAME_7).so .$(SUBDIR_2)

all: .$(SUBDIR_3)/$(FUNC_0).c

		$(CC) $(FLAG_3) .$(SUBDIR_3)/$(FUNC_0).c $(FLAG_4)

		$(MOVE) $(FUNC_0).o .$(SUBDIR_4)

		$(CC) .$(SUBDIR_4)/$(FUNC_0).o $(FLAG_1) $(EXEC_FILE_1) $(L_FLAG_7) $(FLAG_4)

test_dirplugin:

		./$(EXEC_FILE_1)


clean:

		rm ./$(EXEC_FILE_1)
		rm .$(SUBDIR)/$(LIB_NAME).so
		rm .$(SUBDIR_2)/$(LIB_NAME_2).so
		rm .$(SUBDIR_2)/$(LIB_NAME_3).so
		rm .$(SUBDIR_2)/$(LIB_NAME_4).so
		rm .$(SUBDIR_2)/$(LIB_NAME_5).so
		rm .$(SUBDIR_2)/$(LIB_NAME_6).so
		rm .$(SUBDIR_2)/$(LIB_NAME_7).so
		rm .$(SUBDIR_4)/$(FUNC_1).o .$(SUBDIR_4)/$(FUNC_2).o .$(SUBDIR_4)/$(FUNC_3).o .$(SUBDIR_4)/$(FUNC_4).o
		rm .$(SUBDIR_4)/$(FUNC_5).o .$(SUBDIR_4)/$(FUNC_6).o .$(SUBDIR_4)/$(FUNC_7).o .$(SUBDIR_4)/$(FUNC_8).o
		rm .$(SUBDIR_4)/$(FUNC_0).o
