# Каналы
## Задание:
### Написать программу, которая эмулирует работу "bash". В исходниках в файле "string_parser.c" можно добавить другие программы в список, в противном случае может появится сообщение о том, что сделать, чтобы программа работала". Программа является минимальной версией того, что может делать "bash".

### Можно вводить комманды до 2-х каналов: 
#### 1) "ls -la", "pwd", "pstree", etc.
#### 2) "ls -la | grep task" | "ls -la | tail -n10", etc.
#### 3) "ls -la | grep task | nl", "ls -la" | nl | grep task", etc.
#### 4) "CTRL + D для выхода из программы и очистки памяти без утечек"
#### 5) Ввод "exit" приводит к выходу из программы, но без предварительной очистки памяти
____

#### Сборка проекта:

'~$ make all'

#### Запуск проекта:

'~$ make task_5'

#### Удаление проекта:

'~$ make clean'
____
