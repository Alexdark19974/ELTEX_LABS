CC = gcc
FILE = matrix_multiplication
FLAG_1 = -o
FLAG_2 = -Wall
FLAG_3 = -pthread
FLAG_4 = -g
EXEC_FILE = matrix

.PHONY: matrix all clean

all: $(FILE).c

	$(CC) $(FILE).c $(FLAG_1) $(EXEC_FILE) $(FLAG_2) $(FLAG_3) $(FLAG_4)

matrix_1:

	 ./$(EXEC_FILE) 3 3 3 3

matrix_2:

	./$(EXEC_FILE) 2 4 4 2

matrix_3:

	./$(EXEC_FILE) 5 5 5 5


clean:

	rm -rf ./$(EXEC_FILE)

