FILE_1 = shmwriter
FILE_2 = shmreader
FLAG_1 = -o
FLAG_2 = -Wall
FLAG_3 = -Wshadow
FLAG_4 = -Wextra
FLAG_5 = -pedantic
FLAG_6 = -lrt 
FLAG_7 = -g
EXEC_FILE_1 = shmwriter
EXEC_FILE_2 = shmreader

PHONY: all clean shmwriter shmreader shm_writer shm_reader

all:  $(FILE_1).c $(FILE_2).c

		$(CC) $(FILE_1).c $(FLAG_1) $(EXEC_FILE_1) $(FLAG_2) $(FLAG_3) $(FLAG_4) $(FLAG_5) $(FLAG_6) $(FLAG_7)
		$(CC) $(FILE_2).c $(FLAG_1) $(EXEC_FILE_2) $(FLAG_2) $(FLAG_3) $(FLAG_4) $(FLAG_5) $(FLAG_6) $(FLAG_7)

shm_writer:

		./$(EXEC_FILE_1)

shm_reader:

		./$(EXEC_FILE_2)

clean:

		rm -rf  $(EXEC_FILE_1)
		rm -rf  $(EXEC_FILE_2)

