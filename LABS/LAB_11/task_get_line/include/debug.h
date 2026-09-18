#ifndef DEBUG_H
#define DEBUG_H

#define PRINT(fmt, ...) do { \
                                printf(fmt, __VA_ARGS__); \
								putchar('\n'); \
                            } while(0)

#define PRINT_N(fmt, ...) do { \
								printf(fmt, __VA_ARGS__); \
							} while(0)

#ifdef DEBUG
#define PRINT_DBG(fmt, ...) do { \
                                fprintf(stderr, "[%s:%d] DEBUG: " fmt, __func__, __LINE__, __VA_ARGS__); \
								fputc('\n', stderr); \
                            } while(0)
#else
#define PRINT_DBG(fmt, ...) ((void) 0)
#endif /* DEBUG */
#define PRINT_ERR(fmt, ...) do { \
								fprintf(stderr, "[%s:%d] ERROR: " fmt, __func__, __LINE__, __VA_ARGS__); \
								fputc('\n', stderr); \
							} while(0)


#endif /* DEBUG_H */
