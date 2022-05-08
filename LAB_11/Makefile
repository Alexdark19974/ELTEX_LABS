FLAG_1 = -o
FLAG_2 = -Wall
FLAG_3 = -g
CC = gcc
FILE = fio
FILE_2 = result
STRING = string
EXEC_FILE = fio

.PHONY: all test_fio clean

all: $(FILE).c

	$(CC) $(FILE).c $(FLAG_1) $(EXEC_FILE) $(FLAG_2) $(FLAG_3)

test_fio:

	./$(EXEC_FILE) $(fiLE_2).txt $(STRING)

clean:

	rm -rf ./$(EXEC_FILE)
