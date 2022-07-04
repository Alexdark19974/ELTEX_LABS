FILE_1 = customer_store_problem
FLAG_1 = -o
FLAG_2 = -Wall
FLAG_3 = -Wshadow
FLAG_4 = -Wextra
FLAG_5 = -pedantic
FLAG_6 = -g
EXEC_FILE = cstmstr

PHONY: all clean cstmstr

all:  $(FILE_1).c

		$(CC) $(FILE_1).c $(FLAG_1) $(EXEC_FILE) $(FLAG_2) $(FLAG_3) $(FLAG_4) $(FLAG_5) $(FLAG_6)

cstm_str:

		./$(EXEC_FILE)

clean:

		rm -rf ./$(EXEC_FILE)
