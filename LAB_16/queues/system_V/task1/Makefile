FILE = task_1
FILE_2 = task_1_receiver
FLAG_1 = -o
FLAG_2 = -Wall
FLAG_3 = -Wshadow
FLAG_4 = -Wextra
FLAG_5 = -pedantic
FLAG_6 = -g
EXEC_FILE_1 = sender_1
EXEC_FILE_2 = receiver_1

PHONY: all clean

all:  $(FILE).c $(FILE_2).c

		$(CC) $(FILE).c $(FLAG_1) $(EXEC_FILE_1) $(FLAG_2) $(FLAG_3) $(FLAG_4) $(FLAG_5) $(FLAG_6)
		$(CC) $(FILE_2).c  $(FLAG_1) $(EXEC_FILE_2) $(FLAG_2) $(FLAG_3) $(FLAG_4) $(FLAG_5) $(FLAG_6)

sender1:

		./$(EXEC_FILE_1)

receiver1:

		./$(EXEC_FILE_2)

clean:

		rm -rf $(EXEC_FILE_1) $(EXEC_FILE_2)
