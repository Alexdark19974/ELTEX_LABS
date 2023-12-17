.PHONY: all addr_book debug clean
OBJFILE := -o
DBUG_FLAG := -DDMALLOC -ldmalloc -g3
CC := gcc
PREREQ = ./include/address_book.h
CFILES := ./src/*.c
EXEC_FILE = dynamic_address_book
FLAG_2 := -Wall -pedantic -Wextra -O2 -I./include

all: $(PREREQ) ./src/*.c
	$(CC) $(CFILES) $(OBJFILE) $(EXEC_FILE) $(FLAG_2)
	mv $(EXEC_FILE) ./bin

addr_book:
	./bin/$(EXEC_FILE)

debug:
	$(CC) $(CFILES) $(OBJFILE) $(EXEC_FILE) $(FLAG_2) $(DBUG_FLAG)
	mv $(EXEC_FILE) ./bin
	dmalloc -l ./logs/logfile -i 100 high
clean:
	rm -rf ./bin/*
	rm -rf ./logs/*

