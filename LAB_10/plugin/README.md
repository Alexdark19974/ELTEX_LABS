# Плагиновый калькулятор
## Задание:
### Написать плагиновый калькулятор при помощи динамических библиотек.
### Библиотека должна включать все базовые операции: сложение, вычитание, умножение, деление.
### Каждая библиотека должна включать одну операцию.
### Библиотеки расположены в папках, извлекаются сочетанием функций "opendir", "readidr" и "dlopen", "dlsym"
____

##### Сборка проекта:

### Сборка выполняется в три этапа: 

### 1) компиляция исходных файлов до объектных модулей с ключом -fPIC;

### 2) компиляция библиотек;

### 3) компиляция главной программы;

### 1) сборка до объектных модулей с использованием позиционно независимого кода:

'~$ make pic'

### 2) сборка всех безверсионных библиотек

	### --- необходима команда "make pic" для успешной сборки

'~$ make lib_all'

### 3) компиляция файла программы до исполняемой программы 

	### --- с ключами для линкера
	### --- без переменной среды окружения

'~$ make all'

#### Запуск программы:

'~$ make test_plugin'

#### Очистка собранных файлов:

'~$ clean'
____