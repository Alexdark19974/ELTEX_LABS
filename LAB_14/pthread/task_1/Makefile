CC = gcc
FILE_1 = test_thread_no_mutex
FILE_2 = test_thread_with_mutex
FLAG_1 = -o
FLAG_2 = -Wall
FLAG_3 = -pthread
FLAG_4 = -g
EXEC_FILE_1 = test_thread_1
EXEC_FILE_2 = test_thread_2

.PHONY: test_thread_1 test_thread_2 all clean

all: $(FILE_1).c $(FILE_2).c

	$(CC) $(FILE_1).c $(FLAG_1) $(EXEC_FILE_1) $(FLAG_2) $(FLAG_3) $(FLAG_4)
	$(CC) $(FILE_2).c $(FLAG_1) $(EXEC_FILE_2) $(FLAG_2) $(FLAG_3) $(FLAG_4)

test_thread_1:

	./$(EXEC_FILE_1)

test_thread_2:

	./$(EXEC_FILE_2)

clean_1:

	rm -rf ./$(EXEC_FILE_1)

clean_2:

	rm -rf ./$(EXEC_FILE_2)
