#ifndef ADRESS_BOOK_H
#define ADRESS_BOOK_H

#define BUFSIZE 256
#define ERR -1
#define OK 0
#define DECIDE(message, c) do { printf(message); c = getc(stdin); } while(0)
#define MAX_PRIV    64
#define NAMESIZ     64
#define NUMSIZ      64
#define OCCUPSIZ    32

extern unsigned char debug;

#define DPRINT(fmt, ...) do { \
							if (debug) \
								fprintf(stderr, fmt, ##args); \
						} while(0)

extern int address_book(void);
extern int add(void);
extern int remove_(void);
extern int find(void);
extern void display(void);
extern void clear_stdin_buffer(void);

enum { ADD = 1, REMOVE, FIND, DISPLAY, USAGE, EXIT };
typedef enum TYPE { BYTE = 1, INT = 4, POINTER = 8, STRING = 12 } TYPE;

typedef struct priv Priv;
typedef struct ledger Ledger;

struct priv {
		char name[NAMESIZ];
		char middle_name[NAMESIZ];
		char last_name[NAMESIZ];
		char phone_number[NUMSIZ];
		char occupation[OCCUPSIZ];
		int age;
};

struct ledger {
		int (*add)(void);
		int (*remove_)(void);
		int (*find)(void);
		void (*display)(void);
        Priv priv[MAX_PRIV];
};

#endif /* ADRESS_BOOK_H */
