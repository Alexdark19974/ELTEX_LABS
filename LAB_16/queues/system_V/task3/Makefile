FILE_5 = task_3_rs
FLAG_1 = -o
FLAG_2 = -Wall
FLAG_3 = -Wshadow
FLAG_4 = -Wextra
FLAG_5 = -pedantic
FLAG_6 = -g
EXEC_FILE_5 = sr_3

PHONY: all clean

all:  $(FILE_5).c

		$(CC) $(FILE_5).c  $(FLAG_1) $(EXEC_FILE_5) $(FLAG_2) $(FLAG_3) $(FLAG_4) $(FLAG_5) $(FLAG_6)

sender_receiver:

		./$(EXEC_FILE_5) -s

receiver_sender:

		./$(EXEC_FILE_5) -r

clean:

		rm -rf  $(EXEC_FILE_5)
