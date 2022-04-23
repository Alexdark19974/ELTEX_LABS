FLAG_1 = -o
FLAG_2 = -c
S_LIB = ar rc
LIB_NAME = libmy_static_calc
L_FLAG_1 = -L.
L_FLAG_2 = -lmy_static_calc
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

.PHONY: all test_calc clean

all: $(CHAIN_1) $(CHAIN_2) $(FUNC_0).c $(HEADER)

	$(CC) $(FLAG_2) $(FUNC_1).c $(FUNC_2).c $(FUNC_3).c $(FUNC_4).c $(FUNC_5).c $(FUNC_6).c $(FUNC_7).c $(FUNC_8).c

	$(S_LIB) $(LIB_NAME).a  $(FUNC_1).o $(FUNC_2).o $(FUNC_3).o $(FUNC_4).o $(FUNC_5).o $(FUNC_6).o $(FUNC_7).o $(FUNC_8).o

	$(CC) $(FLAG_2) $(FUNC_0).c

	$(CC) $(FUNC_0).o $(FLAG_1) $(EXEC_FILE_1)  $(L_FLAG_1) $(L_FLAG_2)

test_calc:

	./$(EXEC_FILE_1)

clean:

	rm ./$(EXEC_FILE_1)
	rm $(LIB_NAME).a
	rm $(CHAIN_3) $(CHAIN_4)
	rm $(FUNC_0).o

