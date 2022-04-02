
FLAG_1 = -o
FLAG_2 = -Wall
FILE_1 = eltex_ex_1
FILE_2 = eltex_ex_2
CC = gcc
EXEC_FILE_1 = bit_wise_1
EXEC_FILE_2 = bit_wise_2

all: $(FILE_1).c

	$(CC) $(FILE_1).c $(FLAG_1) $(EXEC_FILE_1) $(FLAG_2)
	$(CC) $(FILE_2).c $(FLAG_1) $(EXEC_FILE_2) $(FLAG_2)

test_bit_wise_1:

	./$(EXEC_FILE_1)

test_bit_wise_2:

	./$(EXEC_FILE_2)

clean:
	rm ./$(EXEC_FILE_1) ./$(EXEC_FILE_2)
