#ifndef ADRESS_BOOK_H
/* Public data */
#define ADRESS_BOOK_H	
typedef struct priv Priv, *PrivP;
typedef struct node Node, *NodeP;
typedef struct ledger Ledger, *LedgerP;
enum { ADD = 1, REMOVE, FIND, DISPLAY, LOAD_FILE, USAGE, EXIT };
enum { BY_START, BY_ENTRY , BY_NAME, BY_MIDDLE_NAME, BY_LAST_NAME, BY_NUMBER, BY_OCCUPATION, BY_AGE, BY_END };
enum { START = 0, BEGIN = 1, MIDDLE, END, END_END };
typedef enum TYPE { BYTE = 1, INT = 2, POINTER = 4, STRING = 8, PRIV = 16 } TYPE;
#define BUFSIZE 256
#define ERR -1
#define OK 0
#define FOUND 0
#define TRUE 1
#define FALSE 0
extern int address_book(void);
#endif /* ADRESS_BOOK_H */
