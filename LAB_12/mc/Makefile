CC = gcc
FLAG_1 = -lncurses
FLAG_2 = -Wall
FLAG_3 = -o
EXEC_FILE = mc
FILE_1 = main
FILE_2 = sigwinch
FILE_3 = init_wnd
FILE_4 = myselect
FILE_5 = mc
FILE_6 = isDirectory
HEADER = midc

.PHONY: mc

all: $(FILE_1).c $(FILE_2).c $(FILE_3).c $(FILE_4).c $(FILE_5).c $(FILE_6).c $(HEADER).h

	$(CC) $(FILE_1).c $(FILE_2).c $(FILE_3).c $(FILE_4).c $(FILE_5).c $(FILE_6).c $(FLAG_3) $(EXEC_FILE) $(FLAG_2) $(FLAG_1)

mc:

	./$(EXEC_FILE)

clean:

	rm -rf ./$(EXEC_FILE)
