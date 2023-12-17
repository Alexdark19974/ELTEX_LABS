.PHONY: all snake clean
OBJECT_FLAG := -o
CFLAGS := -Wall -Wextra --pedantic -O2
SRC := ./*.c
CC := gcc
TARGET := snake

all: $(SRC)
	$(CC) $(SRC) $(OBJECT_FLAG) $(TARGET) $(CFLAGS)
snake:
	./$(EXEC_FILE_1)
clean:
	rm -rf ./$(EXEC_FILE_1)




