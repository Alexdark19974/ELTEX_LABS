FILE_1 = shm_writer
FILE_2 = shm_reader
FLAG_1 = -o
FLAG_2 = -Wall
FLAG_3 = -Wshadow
FLAG_4 = -Wextra
FLAG_5 = -pedantic
FLAG_6 = -g
EXEC_FILE_1 = shm_writer
EXEC_FILE_2 = shm_reader

PHONY: all clean chatroom client

all:  $(FILE_1).c $(FILE_2).c

		$(CC) $(FILE_1).c $(FLAG_1) $(EXEC_FILE_1) $(FLAG_2) $(FLAG_3) $(FLAG_4) $(FLAG_5) $(FLAG_6)

		$(CC) $(FILE_2).c $(FLAG_1) $(EXEC_FILE_2) $(FLAG_2) $(FLAG_3) $(FLAG_4) $(FLAG_5) $(FLAG_6)

shm1_writer:

		./$(EXEC_FILE_1)

shm1_reader:

		./$(EXEC_FILE_2)
clean:

		rm -rf  $(EXEC_FILE_1)
		rm -rf $(EXEC_FILE_2)                       
