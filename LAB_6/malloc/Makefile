.PHONY: all addr_book debug clean
OBJECT_FLAG := -o
DBUG_FLAG := -DDMALLOC -ldmalloc -g3
CC := gcc
HEADER = ./include/address_book.h
CFILES := ./src/*.c
TARGET := dynamic_address_book
CFLAGS := -Wall -pedantic -Wextra -O2 -I./include

all: $(HEADER) $(CFILES)
	$(CC) $(CFILES) $(OBJECT_FLAG) $(TARGET) $(CFLAGS)
	mv $(EXEC_FILE) ./bin

addr_book:
	./bin/$(EXEC_FILE)

debug:
	$(CC) $(CFILES) $(OBJECT_FLAG) $(TARGET) $(CFLAGS) $(DBUG_FLAG)
	mv $(TARGET) ./bin
	dmalloc -l ./logs/logfile -i 100 high
clean:
	rm -rf ./bin/*
	rm -rf ./logs/*

