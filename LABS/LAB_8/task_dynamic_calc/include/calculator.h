#ifndef CALCULATOR_H
#define CALCULATOR_H
extern int calculator(void);
extern double ADD_F(double val1, double val2);
extern double SUB_F(double val1, double val2);
extern double DIV_F(double val1, double val2);
extern double MUL_F(double val1, double val2);
extern int REM_F(int val1, int val2);
typedef struct stack Stack, *StackP;
typedef enum op { NONE = -2, FATAL_ERR = -1, RESET = '0', NUM = '1', ADD = '+', SUB = '-', MUL = '*', DIV = '/', REM = '%', RES = '=' } Op;
typedef enum type { INT = 4, DOUBLE = 8 } Type;
#endif /* CALCULATOR_H */
