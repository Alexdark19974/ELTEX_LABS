FLAG_1 = -o
FLAG_2 = -Wall
FLAG_3 = -pedantic
FILE_1 = address_book
CC = gcc
EXEC_FILE_1 = addr_book

all: $(FILE_1).c

	$(CC) $(FILE_1).c $(FLAG_1) $(EXEC_FILE_1) $(FLAG_3) $(FLAG_2)

test_addr_book:

	./$(EXEC_FILE_1)
clean:

	rm ./$(EXEC_FILE_1)
