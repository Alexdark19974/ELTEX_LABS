#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "address_book.h"
#include "../include/cleanup.h"
#include "../include/address_book_internals.h"
#include "../include/debug.h"
#include "jsmn.h"
#define FIRST_NAME  "Firstname"
#define MIDDLE_NAME "Middlename"
#define LAST_NAME   "Lastname"
#define OCCUPATION  "Occupation"
#define NUMBER      "Phonenumber"
#define AGE         "Age"
#define MAX_TOKENS  32076

static int process_entry(void *entry, size_t size, TYPE data_type);
static int process_param(void *data, size_t size, TYPE data_type);
static int cmp(void *obj1, void *obj2, size_t size);
static void print_entry(Priv entry);

int add(void **obj, PrivP entry, int pos)
{
    int middle = 0, i = 0;
    LedgerP *ledger = (LedgerP *)obj;
    NodeP tmpNode = NULL, prevNode = NULL;
    NodeP newNode = calloc(1, sizeof(Node));
    if (!newNode) {
        PRINT_ERR("ERROR: calloc() failed!");
        goto fail;
    }

    if (NULL == entry &&
            ERR == process_entry((void *)&newNode->priv, sizeof(Priv), PRIV))
        goto fail;

    if (entry) {
        if (NULL == memcpy((void *)&newNode->priv, (void *)entry, sizeof(Priv))) {
            PRINT_ERR("ERROR: memcpy() failed!");
            goto fail;
        }
    }

    if (!*newNode->priv.name && !*newNode->priv.number) {
        printf("error: name or phone number have to be at least specified!");
        goto fail;
    }

    newNode->next = NULL;

    print_entry(newNode->priv);

    switch (pos) {
        case MIDDLE:
            if ((*ledger)->entry_total > 2) {
                middle = (*ledger)->entry_total / 2;
                DPRINT("middle: %d\n", middle);
                for (   i = 0, tmpNode = (*ledger)->head, prevNode = tmpNode;
                        i < middle;
                        i++, prevNode = tmpNode, tmpNode = tmpNode->next    );
                prevNode->next = newNode;                
                newNode->next = tmpNode; 
                printf("Added new entry to the middle\n");
            } else goto insert_begin;
            break;
        case END:
            if ((*ledger)->entry_total > 1) {
                for (   tmpNode = (*ledger)->head;
                        tmpNode->next;
                        tmpNode = tmpNode->next );
                tmpNode->next = newNode;
                printf("Added new entry to the end\n");

            } else goto insert_begin;
            break;
        case BEGIN:
        default:
insert_begin:
            newNode->next = (*ledger)->head;
            (*ledger)->head = newNode;
            break;
    }

    (*ledger)->entry_total++;

    return OK;

fail:
    free(newNode);
    return ERR;
}

int remove_by(void **obj,  int opt)
{
    void *obj1 = NULL, *obj2 = NULL;
    Priv entry = {0};
    size_t size = 0;
    size_t offset = 0;
    int found = FALSE;

    switch(opt) {
        case BY_ENTRY:
            size = sizeof(entry);
            if (ERR == process_entry((void *)&entry, size, PRIV)) goto fail;
            break;
        case BY_NAME:
            size = sizeof(entry.name);
            printf("Name: ");
            if (ERR == process_param((void *)entry.name, size, STRING)) goto fail;
            break;
        case BY_MIDDLE_NAME:
            size = sizeof(entry.mname);
            offset = offsetof(Priv, mname);
            printf("Middle name: ");
            if (ERR == process_param((void *)entry.mname, size, STRING)) goto fail;
            break;
        case BY_LAST_NAME:
            size = sizeof(entry.lname);
            offset = offsetof(Priv, lname);
            printf("Last name: ");
            if (ERR == process_param((void *)entry.lname, size, STRING)) goto fail;
            break;
        case BY_NUMBER:
            size = sizeof(entry.number);
            offset = offsetof(Priv, number);
            printf("Phone number: ");
            if (ERR == process_param((void *)entry.number, size, STRING)) goto fail;
            break;
        case BY_OCCUPATION:
            size = sizeof(entry.occup);
            offset = offsetof(Priv, occup);
            printf("Occupation: ");
            if (ERR == process_param((void *)entry.occup, size, STRING)) goto fail;
            break;
        case BY_AGE:
            size = sizeof(entry.age);
            offset = offsetof(Priv, age);
            printf("Age: ");
            if (ERR == process_param((void *)&entry.age, size, INT)) goto fail;
            break;
        default:
            printf("error: unknown compare action!\n");
            return OK;
    }

    obj1 = (void *)(((char *) &entry) + offset);

    for (NodeP curNode = (*(LedgerP *)obj)->head, prevNode = NULL; curNode;) {

        obj2 = (void *)((char *)curNode + offset);

        if (FOUND == cmp(obj1, obj2, size)) {

            found = TRUE;

            printf("Found and deleted customer:\n");

            print_entry(curNode->priv);

            if (curNode == (*(LedgerP *)obj)->head) {
                (*(LedgerP *)obj)->head = curNode->next;
                (*(LedgerP *)obj)->entry_total--;
                free(curNode);
                curNode = (*(LedgerP *)obj)->head;

                if (opt != BY_ENTRY && opt != BY_NUMBER)
                    continue;

            } else {
                prevNode->next = curNode->next == NULL ? NULL : curNode->next;
                free(curNode);
                (*(LedgerP *)obj)->entry_total--;
            }

            if (opt == BY_ENTRY || opt == BY_NUMBER)
                break;
        }

        prevNode = curNode;
        curNode = curNode->next;
    }

    if (found == FALSE)
        printf("error: failed to find the customer!\n");

    return OK;
fail:
    return ERR;
}

int find_by(void **obj, int opt)
{
    void *obj1 = NULL, *obj2 = NULL;
    Priv entry = {0};
    size_t size = 0, offset = 0;
    int found = FALSE;

    switch(opt) {
        case BY_ENTRY:
            size = sizeof(entry);
            if (ERR == process_entry((void *)&entry, size, PRIV)) goto fail;
            break;
        case BY_NAME:
            size = sizeof(entry.name);
            printf("Name: ");
            if (ERR == process_param((void *)entry.name, size, STRING)) goto fail;
            break;
        case BY_MIDDLE_NAME:
            size = sizeof(entry.mname);
            offset = offsetof(Priv, mname);
            printf("Middle name: ");
            if (ERR == process_param((void *)entry.mname, size, STRING)) goto fail;
            break;
        case BY_LAST_NAME:
            size = sizeof(entry.lname);
            offset = offsetof(Priv, lname);
            printf("Last name: ");
            if (ERR == process_param((void *)entry.lname, size, STRING)) goto fail;
            break;
        case BY_NUMBER:
            size = sizeof(entry.number);
            offset = offsetof(Priv, number);
            printf("Phone number: ");
            if (ERR == process_param((void *)entry.number, size, STRING)) goto fail;
            break;
        case BY_OCCUPATION:
            size = sizeof(entry.occup);
            offset = offsetof(Priv, occup);
            printf("Occupation: ");
            if (ERR == process_param((void *)entry.occup, size, STRING)) goto fail;
            break;
        case BY_AGE:
            size = sizeof(entry.age);
            offset = offsetof(Priv, age);
            printf("Age: ");
            if (ERR == process_param((void *)&entry.age, size, INT)) goto fail;
            break;
        default:
            printf("error: unknown compare action!\n");
            return OK;
    }

    obj1 = (void *)(((char *)&entry) + offset);

    for (NodeP curNode = (*(LedgerP *)obj)->head; curNode; curNode = curNode->next) {

        obj2 = (void *)((char *)curNode + offset);

        if (FOUND == cmp(obj1, obj2, size)) {

            found = TRUE;

            printf("Found customer:\n");

            print_entry(curNode->priv);

            if (opt == BY_ENTRY || opt == BY_NUMBER)
                break;
        }
    }

    if (found == FALSE)
        printf("error: failed to find the customer!");

    return OK;
fail:
    return ERR;
}

void display(void **obj)
{
    printf("\ntotal num of entries: %d\n", (*(LedgerP *)obj)->entry_total);
    for (NodeP curNode = (*(LedgerP *)obj)->head; curNode; curNode = curNode->next) {

        puts("----------------------------");

        print_entry(curNode->priv);

        puts("----------------------------");
    }
}

void print_entry(Priv entry)
{
    printf(
            "Customer:\n"
            " 1) Name:%s\n"
            " 2) Middle name:%s\n"
            " 3) Last name:%s\n"
            " 4) Phone number:%s\n"
            " 5) Occupation:%s\n"
            " 6) Age:%d\n",
            entry.name,
            entry.mname,
            entry.lname,
            entry.number,
            entry.occup,
            entry.age
        );
}

static int process_entry(void *entry, size_t size, TYPE data_type)
{
    if (!entry || !size) {
        PRINT_ERR("ERROR: unknown entry or its size!");
        goto fail;
    }

    switch (data_type) {
        case PRIV:
            printf("1) Name: ");
            if (ERR == process_param((void *)((PrivP)entry)->name, BUFSIZE, STRING)) {
                goto fail;
            }

            printf("1) Middle name: ");
            if (ERR == process_param((void *)((PrivP)entry)->mname, BUFSIZE, STRING)) {
                goto fail;
            }

            printf("1) Last name: ");
            if (ERR == process_param((void *)((PrivP)entry)->lname, BUFSIZE, STRING)) {
                goto fail;
            }

            printf("1) Phone number: ");
            if (ERR == process_param((void *)((PrivP)entry)->number, BUFSIZE, STRING)) {
                goto fail;
            }

            printf("1) Occupation: ");
            if (ERR == process_param((void *)((PrivP)entry)->occup, BUFSIZE, STRING)) {
                goto fail;
            }

            printf("1) Age: ");
            if (ERR == process_param((void *)&((PrivP)entry)->age, BUFSIZE, INT)) {
                goto fail;
            }
            break;
        default:
            DPRINT("WARNING: unknown entry type = %d", data_type);
            break;
    }
    return OK;
fail:
    return ERR;
}

static int process_param(void *data, size_t size, TYPE data_type)
{
    char *subStr = NULL;

    switch (data_type) {

        case STRING:
            if (!fgets((char *)data, (int) size, stdin)) {
                PRINT_ERR("ERROR: fgets() failed!");
                return ERR;
            }

            if ((subStr = strchr((char *)data, '\n')) == NULL) {
                PRINT_ERR("ERROR: strchr() failed!");
                goto fail;
            }

            *subStr = '\0';

            if ((char *)data == subStr)
                DPRINT("WARNING: no input. This field is empty");
            break;
        case INT:
            if (scanf("%d", (int *)data) <= 0) {
                PRINT_ERR("ERROR: scanf() failed!");
                goto fail;
            }
            clear_stdin_buffer();
            break;
        default:
            DPRINT("WARNING: unknown data type = %d", data_type);
            break;
    }
    return OK;
fail:
    return ERR;
}

void clear_stdin_buffer(void)
{
    while (getchar() != '\n') ;
}

static int cmp(void *obj1, void *obj2, size_t size)
{
    return memcmp(obj1, obj2, size);
}

int load_from(void **obj, const char *filename)
{
    char *buf = NULL;
    jsmn_parser parser;
    jsmntok_t tokens[MAX_TOKENS];
    Priv entry = {0};
    struct stat statbuf = {0};
    ssize_t filesize = 0, nbytes = 0;
    int fd = 0, i = 0, j = 0, n = 0, arraysize = 0, objectsize = 0, ret = ERR;

    memset(&parser, 0x0, sizeof(jsmn_parser));
    memset(tokens, 0x0, sizeof(jsmntok_t) * MAX_TOKENS);


    if (NULL == filename) {
        PRINT_ERR("ERROR: filename is NULL!");
        goto fail;        
    }

    if (ERR == stat(filename, &statbuf)) {
        PRINT_ERR("ERROR: stat() failed!");
        goto fail;
    }

    if ((filesize = statbuf.st_size) <= 0) {
        printf("error: nbytes read from '%s' is %ld!\n", filename, filesize);
        goto ok;
    }

    if (!(buf = calloc(1, (size_t)filesize + 1))) {
        PRINT_ERR("ERROR: calloc() failed!");
        goto fail;
    }

    if (ERR == (fd = open(filename, O_RDONLY, 0))) {
        PRINT_ERR("ERROR: open() failed!");
        goto fail;
    }

    if (ERR == (nbytes = read(fd, (void *)buf, (size_t)filesize))) {
        PRINT_ERR("ERROR: read() failed!");
        goto fail;
    }

    if (!nbytes) {
        DPRINT("WARNING: 0 bytes from '%s' is read\n", filename);
        goto ok;
    }

    DPRINT("nbytes read = %zu from '%s'\n", nbytes, filename);

    jsmn_init(&parser);

    ret = jsmn_parse(&parser, (const char *)buf, (const size_t)filesize, tokens, (const unsigned int)(sizeof(tokens) / sizeof(tokens[0])));
    if (ret < 0) {
        PRINT_ERR("ERROR: jsmn_parse() failed!");
        goto fail;
    }

    if (ret < 1 || tokens[i++].type != JSMN_OBJECT) {
        PRINT_ERR("ERROR: JSON Object expected!");
        goto fail;
    }

    if (!(FOUND == cmp((void *)(buf + tokens[i].start), "Employees", (size_t)(tokens[i].end - tokens[i].start)))) {
            PRINT_ERR("ERROR: \"Employees\" object name is missing!");
            goto fail;
    }

    i++;

    if (tokens[i].type != JSMN_ARRAY) {
        PRINT_ERR("ERROR: JSON ARRAY expected!");
        goto fail;
    }

    arraysize = tokens[i].size;
    i++;

    for (j = 0; j < arraysize; j++) { // array object size
        if (tokens[i].type != JSMN_OBJECT) {
            DPRINT("JSMN OBJECT not found. Leaving.");
            break;
        }

        objectsize = tokens[i].size;

        i++;

        for (n = 0; n < objectsize; n++, i++) { // object size
            if (tokens[i].type == JSMN_STRING && tokens[i].size) {

                i++;

                if (!*entry.name && tokens[i].type == JSMN_STRING) {
                    if (tokens[i].start == tokens[i].end) {
                        DPRINT("value of key \"%.*s\" s empty! Writing \"nil\"", tokens[i - 1].end - tokens[i - 1].start, buf + tokens[i - 1].start);
                        strcpy(entry.name, "nil");
                    } else
                        memcpy((void *)entry.name, (void *)(buf + tokens[i].start), (size_t)(tokens[i].end - tokens[i].start));
                } else if (!*entry.mname && tokens[i].type == JSMN_STRING) {
                    if (tokens[i].start == tokens[i].end) {
                        DPRINT("value of key \"%.*s\" s empty! Writing \"nil\"", tokens[i - 1].end - tokens[i - 1].start, buf + tokens[i - 1].start);
                        strcpy(entry.mname, "nil");
                    } else
                        memcpy((void *)entry.mname, (void *)(buf + tokens[i].start), (size_t)(tokens[i].end - tokens[i].start));
                } else if (!*entry.lname && tokens[i].type == JSMN_STRING) {
                    if (tokens[i].start == tokens[i].end) {
                        DPRINT("value of key \"%.*s\" s empty! Writing \"nil\"", tokens[i - 1].end - tokens[i - 1].start, buf + tokens[i - 1].start);
                        strcpy(entry.lname, "nil");
                    } else
                        memcpy((void *)entry.lname, (void *)(buf + tokens[i].start), (size_t)(tokens[i].end - tokens[i].start)); 
                } else if (!*entry.occup && tokens[i].type == JSMN_STRING) {
                    if (tokens[i].start == tokens[i].end) {
                        DPRINT("value of key \"%s\" s empty! Writing \"nil\"", buf + tokens[i - 1].start);
                        strcpy(entry.occup, "nil");
                    } else
                        memcpy((void *)entry.occup, (void *)(buf + tokens[i].start), (size_t)(tokens[i].end - tokens[i].start));
                } else if (!*entry.number && tokens[i].type == JSMN_STRING) {
                    if (tokens[i].start == tokens[i].end) {
                        DPRINT("value of key \"%s\" s empty! Writing \"nil\"", buf + tokens[i - 1].start);
                        strcpy(entry.number, "nil");
                    } else
                        memcpy((void *)entry.number, (void *)(buf + tokens[i].start), (size_t)(tokens[i].end - tokens[i].start));
                } else if (tokens[i].type == JSMN_PRIMITIVE) {
                    if (sscanf(buf + tokens[i].start, "%d\n", &entry.age) == ERR) {
                        PRINT_ERR("ERROR: Failed to convert number from string to integer!\n");
                    }
                    if (entry.age < 0) {
                        PRINT_ERR("ERROR: age cannot be a negative integer!\n");
                        goto fail;
                    }
                } else {
                    PRINT_ERR("ERROR: unknown JSON type inside the object");
                    goto fail;
                }
            } else {
                PRINT_ERR("ERROR: icorrect key:value pair in the object");
                goto fail;
            }
        }

        /* DPRINT("\nname: %s;\nmiddle name: %s;\nlast name: %s;\nphone number: %s;\noccupation: %s;\nage:%d\n\n",
        entry.name, entry.mname, entry.lname, entry.number, entry.occup, entry.age); */
        if (ERR == add(obj, &entry, BEGIN))
            goto fail;
        memset(&entry, 0x0, sizeof(entry));
    }

ok:
    ret = OK;
    free(buf);
    close(fd);

    return ret;

fail:

    if (buf) free(buf);
    if (fd > 0 && fd != ERR) close(fd);

    return ret;
}

