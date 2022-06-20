FILE_3 = task_2_s
FILE_4 = task_2_r
FLAG_1 = -o
FLAG_2 = -Wall
FLAG_3 = -Wshadow
FLAG_4 = -Wextra
FLAG_5 = -pedantic
FLAG_6 = -g
EXEC_FILE_3 = sender_2
EXEC_FILE_4 = receiver_2

PHONY: all clean

all: $(FILE_3).c $(FILE_4).c

		$(CC) $(FILE_3).c  $(FLAG_1) $(EXEC_FILE_3) $(FLAG_2) $(FLAG_3) $(FLAG_4) $(FLAG_5) $(FLAG_6)
		$(CC) $(FILE_4).c  $(FLAG_1) $(EXEC_FILE_4) $(FLAG_2) $(FLAG_3) $(FLAG_4) $(FLAG_5) $(FLAG_6)

sender2:

		./$(EXEC_FILE_3)

receiver2:

		./$(EXEC_FILE_4)

clean:

		rm -rf $(EXEC_FILE_3) $(EXEC_FILE_4)
