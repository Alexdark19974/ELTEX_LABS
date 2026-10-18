#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
#define PRINT_DBG(fmt, ...) do { \
                                fprintf(stderr, fmt, __VA_ARGS__); \
                            } while(0)
#else
#define PRINT_DBG(fmt, ...) ((void) 0)
#endif /* DEBUG */

#endif /* DEBUG_H */
