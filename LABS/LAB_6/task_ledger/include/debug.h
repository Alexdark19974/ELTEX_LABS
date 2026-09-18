#ifndef DEBUG_H
#define DEBUG_H
extern unsigned char debug;

#define DPRINT(...) do { \
                            if (debug) { \
                                printf(__VA_ARGS__); \
                                putchar('\n'); \
                            } \
                        } while(0)
#endif /* DEBUG_H */
