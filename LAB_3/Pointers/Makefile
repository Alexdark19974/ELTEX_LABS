FLAG_1 = -o
FLAG_2 = -Wall
FLAG_3 = -pedantic
FILE_1 = eltex_lab_3_ex_1
FILE_2 = eltex_lab_3_ex_2
FILE_3 = eltex_lab_3_ex_3
CC = gcc
EXEC_FILE_1 = show_bytes
EXEC_FILE_2 = change_bytes
EXEC_FILE_3 = double_pointer

all: $(FILE_1).c $(FILE_2).c $(FILE_3).c

        $(CC) $(FILE_1).c $(FLAG_1) $(EXEC_FILE_1) $(FLAG_3) $(FLAG_2)
        $(CC) $(FILE_2).c $(FLAG_1) $(EXEC_FILE_2) $(FLAG_3) $(FLAG_2)
        $(CC) $(FILE_3).c $(FLAG_1) $(EXEC_FILE_3) $(FLAG_3) $(FLAG_2)

test_show_bytes:

        ./$(EXEC_FILE_1)

tes_change_byte:

        ./$(EXEC_FILE_2)

test_double_pointer:

        ./$(EXEC_FILE_3)

clean:

        rm ./$(EXEC_FILE_1) ./$(EXEC_FILE_2) ./$(EXEC_FILE_3)



