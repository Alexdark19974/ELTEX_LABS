FILE = pthread_once
FLAG_1 = -o
FLAG_2 = -Wall
FLAG_3 = -pthread
FLAG_4 = -g
EXEC_FILE = once

.PHONY: once all clean

all: $(FILE).c

	$(CC) $(FILE).c $(FLAG_1) $(EXEC_FILE) $(FLAG_2) $(FLAG_3) $(FLAG_4)

once:

	./$(EXEC_FILE)

clean:
	rm -rf ./$(EXEC_FILE)
