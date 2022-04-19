FLAG_1 = -o
FLAG_2 = -Wall -pedantic
CC = gcc
PREREQ = dynamic_address_book
EXEC_FILE = addr_book

all: $(PREREQ).c

	$(CC) $(PREREQ).c $(FLAG_1) $(EXEC_FILE) $(FLAG_2)

test_addr_book:

	./$(EXEC_FILE)
clean:

	rm ./$(EXEC_FILE)
