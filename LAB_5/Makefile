FLAG_1 = -o
FLAG_2 = -fno-stack-protector
FILE_1 = Pass
FILE_2 = test
CC = gcc
EXEC_FILE_1 = PSWD

all: $(FILE_1).c

	$(CC) $(FILE_1).c $(FLAG_1) $(EXEC_FILE_1) $(FLAG_2)

test_PSWD:

	./$(EXEC_FILE_1) < $(FILE_2).bin

clean:

	rm ./$(EXEC_FILE_1)
