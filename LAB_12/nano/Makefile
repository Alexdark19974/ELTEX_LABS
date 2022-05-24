CC = gcc
FLAG_1 = -lncurses
FLAG_2 = -Wall
FLAG_3 = -o
EXEC_FILE = mnano
FILE_1 = main
FILE_2 = sigwinch
FILE_3 = nano
FILE_4 = guinea_pig
KEY_1 = r
KEY_2 = w
HEADER = nano_h

.PHONY: mnano all

all: $(FILE_1).c $(FILE_2).c $(FILE_3).c $(HEADER).h

	$(CC) $(FILE_1).c $(FILE_2).c $(FILE_3).c $(FLAG_3) $(EXEC_FILE) $(FLAG_2) $(FLAG_1)

mnano:

	./$(EXEC_FILE)

mnanor:

	 ./$(EXEC_FILE) $(FILE_4).c $(KEY_1)
mnanow:

	./$(EXEC_FILE) $(FILE_4).c $(KEY_2)

clean:

	rm -rf ./$(EXEC_FILE)
