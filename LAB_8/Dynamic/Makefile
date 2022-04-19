FLAG_1 = -o
FLAG_2 = -fPIC
FLAG_3 = -c
ENV_VAR = LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
LIB_FLAG = -shared
L_FLAG_1 = -L.
L_FLAG_2 = -lmy_dynamic_calc
L_FLAG_3 = -Wl,
L_FLAG_4 = -rpath,.
L_FLAG_5 = -soname,libmy_dynamic_calc.so.1
L_FLAG_6 = ln -s
CC = gcc
LIB_NAME = libmy_dynamic_calc
VERS = .1.2.0
VERS_1 = .1
HEADER = calc_hub.h
FUNC_0 = main
FUNC_1 = calc
FUNC_2 = add
FUNC_3 = sub
FUNC_4 = mul
FUNC_5 = div
FUNC_6 = push_stack
FUNC_7 = stack_pop
FUNC_8 = my_get_line
CHAIN_1 = $(FUNC_1).c $(FUNC_2).c $(FUNC_3).c $(FUNC_4).c
CHAIN_2 = $(FUNC_5).c $(FUNC_6).c $(FUNC_7).c $(FUNC_8).c
CHAIN_3 = $(FUNC_1).o $(FUNC_2).o $(FUNC_3).o $(FUNC_4).o
CHAIN_4 = $(FUNC_5).o $(FUNC_6).o $(FUNC_7).o $(FUNC_8).o
EXEC_FILE_1 = calc

pic: $(CHAIN_1) $(CHAIN_2)

		$(CC) $(FLAG_3) $(CHAIN_1) $(CHAIN_2) $(FLAG_2)

all: $(CHAIN_1) $(CHAIN_2) $(FUNC_0) $(HEADER)

		$(CC) $(FUNC_1).c $(FUNC_2).c $(FUNC_3).c $(FUNC_4).c $(FUNC_5).c $(FUNC_6).c $(FUNC_7).c $(FUNC_8).c $(FLAG_3) $(FLAG_2)

		$(CC) $(LIB_FLAG) $(FUNC_1).o $(FUNC_2).o $(FUNC_3).o $(FUNC_4).o $(FUNC_5).o $(FUNC_6).o $(FUNC_7).o $(FUNC_8).o $(FLAG_1) $(LIB_NAME).so$(VERS)

		$(CC) $(FLAG_3) $(FUNC_0).c

		$(CC) $(FUNC_0).o $(LIB_NAME).so$(VERS) $(FLAG_1) $(EXEC_FILE_1)


all_2: $(CHAIN_1) $(CHAIN_2) $(FUNC_0) $(HEADER)

		$(CC) $(FUNC_1).c $(FUNC_2).c $(FUNC_3).c $(FUNC_4).c $(FUNC_5).c $(FUNC_6).c $(FUNC_7).c $(FUNC_8).c $(FLAG_3) $(FLAG_2)

		$(CC) $(LIB_FLAG) $(FUNC_1).o $(FUNC_2).o $(FUNC_3).o $(FUNC_4).o $(FUNC_5).o $(FUNC_6).o $(FUNC_7).o $(FUNC_8).o $(FLAG_1) $(LIB_NAME).so

		$(CC) $(FLAG_3) $(FUNC_0).c

		$(CC) $(FUNC_0).o $(FLAG_1) $(EXEC_FILE_1) $(L_FLAG_1) $(L_FLAG_2) $(L_FLAG_3)$(L_FLAG_4)

all_3: $(CHAIN_1) $(CHAIN_2) $(FUNC_0) $(HEADER)

		$(CC) $(FUNC_1).c $(FUNC_2).c $(FUNC_3).c $(FUNC_4).c $(FUNC_5).c $(FUNC_6).c $(FUNC_7).c $(FUNC_8).c $(FLAG_3) $(FLAG_2)

		$(CC) $(LIB_FLAG) $(FUNC_1).o $(FUNC_2).o $(FUNC_3).o $(FUNC_4).o $(FUNC_5).o $(FUNC_6).o $(FUNC_7).o $(FUNC_8).o $(FLAG_1) $(LIB_NAME).so$(VERS) $(L_FLAG_3)$(L_FLAG_5)

		$(CC) $(FLAG_3) $(FUNC_0).c

		$(L_FLAG_6) $(LIB_NAME).so$(VERS) $(LIB_NAME).so

		$(L_FLAG_6) $(LIB_NAME).so$(VERS) $(LIB_NAME).so$(VERS_1)

		$(CC) $(FUNC_0).o $(FLAG_1) $(EXEC_FILE_1) $(L_FLAG_1) $(L_FLAG_2)  $(L_FLAG_3)$(L_FLAG_4)

test_calc:

		$(ENV_VAR) ./$(EXEC_FILE_1)

test_calc_2:

		./$(EXEC_FILE_1)

test_calc_3:

		./$(EXEC_FILE_1)

clean:

		rm ./$(EXEC_FILE_1)
		rm $(LIB_NAME).so$(VERS)
		rm $(CHAIN_3) $(CHAIN_4)
		rm $(FUNC_0).o

clean_2:

		rm ./$(EXEC_FILE_1)
		rm $(LIB_NAME).so
		rm $(CHAIN_3) $(CHAIN_4)
		rm $(FUNC_0).o

clean_3:

		rm ./$(EXEC_FILE_1)
		rm $(LIB_NAME).so$(VERS) $(LIB_NAME).so $(LIB_NAME).so$(VERS_1)
		rm $(CHAIN_3) $(CHAIN_4)
		rm $(FUNC_0).o
