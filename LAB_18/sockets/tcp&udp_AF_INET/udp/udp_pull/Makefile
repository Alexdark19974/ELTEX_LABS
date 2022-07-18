CC = gcc
FLAG_1 = -o
FLAG_2 = -Wall -Wextra -Wconversion -pedantic
FLAG_3 = -lpthread -lrt
FLAG_4 = -g
FILE_1 = client
FILE_2 = server

.phony: all Client Server Clean

all: $(FILE_1).c $(FILE_2).c

		$(CC) $(FILE_1).c $(FLAG_1) $(FILE_1) $(FLAG_2) $(FLAG_3) $(FLAG_4)
		$(CC) $(FILE_2).c $(FLAG_1) $(FILE_2) $(FLAG_2) $(FLAG_3) $(FLAG_4)

Client: 

		./$(FILE_1)

Server:

		./$(FILE_2)

Clean:

		rm -rf ./$(FILE_1)

		rm -rf ./$(FILE_2)
