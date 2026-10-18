#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdbool.h>
#define DEFAULT_QUEUE_PATHNAME "../src/sender_receiver.c"
#define PROG_NAME "sender_receiver"
#define SENDER_STR "sender"
#define RECEIVER_STR "receiver"
#define EXAMPLE_PATH "/path/to/file"
#define FILE_OPT 'o'
#define SENDER_OPT 's'
#define RECEIVER_OPT 'r'
#define TYP_OPT 't'
#define QUEUE_OPT 'q'
#define MESSAGE "./feedlot/message"
#define MESSAGE_2 "./feedlot/message2"
#define MAGIC_ID 'A'
#define BUF_SIZ 256
#define MSG_SIZE 2048 // is MSGMAX is usually 8192
#define PERMS 0666 // -rw-rw-rw
#define MSG_TYPE 1 // can be consedered a priority field
#define MSG_NUM 2
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)
#define handle_error_msg(fmt, ...) do { \
                            fprintf(stderr, fmt, __VA_ARGS__); \
                        } while(0)

typedef struct {
    long mtype;
    char mtext[1];
} Msg;

typedef enum prog_e {
    NONE = 0,
    RECEIVER = 1,
    SENDER = 2
} Prog_t;

void print_usage(void);
bool options_bundled(int opt, const char *optarg);

int main(int argc, char **argv)
{
    Msg *msg = NULL;
    const char *file = NULL;
    const char *queue_pname = NULL;
    struct stat sb;
    struct msqid_ds ds;
    ssize_t nbytes = -1;
    size_t payload_size = 0;
    key_t key = 0;
    int qid = 0;
    int opt = 0, fd = -1, ret = EXIT_SUCCESS;
    int error_num = 0;
    long mtype = 1; // default mtype value
    Prog_t prog_type = NONE;

    /* : at the beginning suppresses default error message from getopt() */
    while ((opt = getopt(argc, argv, ":hsrt:o:q:")) != -1) { // NEVER type "sr: with ':' unless you want some option added after it!!!"
        switch (opt) {
            case 's':
                prog_type |= SENDER;
                break;
            case 'r':
                prog_type |= RECEIVER;
                break;
            case 'q': // regular file used to generate a key to create a queue
                if (true == options_bundled(opt, optarg))
                    goto print;
                queue_pname = optarg;
                printf("queue path name = %s\n", queue_pname);
                break;
            case 'o':
                if (true == options_bundled(opt, optarg))
                    goto print;
                file = optarg;
                printf("file = %s\n", file);
                break;
            case 't': // regular file to form a message
                if (true == options_bundled(opt, optarg))
                    goto print;
                errno = 0;
                mtype = strtol(optarg, NULL, 10);
                if (errno != 0) {
                    handle_error("strtol", errno);
                    ret = EXIT_FAILURE;
                    goto print;
                } else if (mtype <= 0) {
                    handle_error_msg("mtype <= 0. -%c must be follwed by a strictly positive integer\n", TYP_OPT);
                    goto print;
                }
                break;
            case 'h':
                goto print;
            case ':':
                if (optopt == 'o')
                    handle_error_msg("-%c requires an argument, e.g. %s\n", optopt, EXAMPLE_PATH);
                else if (optopt == 't')
                    handle_error_msg("-%c requires an argument, e.g. %ld\n", optopt, mtype);
                else if (optopt == 'q')
                    handle_error_msg("-%c requires an argument, e.g. %s\n", optopt, DEFAULT_QUEUE_PATHNAME);
                goto print;
            default: // '?'
                handle_error_msg("-%c is an unknown option\n", optopt);
                goto print;
        }
    }

    if (prog_type == NONE) {
        handle_error_msg("Program type (%d) not chosen. Is -%c or -%c missing?\n", prog_type, SENDER_OPT, RECEIVER_OPT);
        goto print;
    } else if ((prog_type & SENDER) && (prog_type & RECEIVER)) {
        handle_error_msg("Choosing both prog types (%s and %s) is forbidden\n", SENDER_STR, RECEIVER_STR);
        goto print;
    } else if (prog_type & SENDER && !file) {
        handle_error_msg("path to file is missing. Is -%c missing?\n", FILE_OPT);
        goto print;
    } else if (!queue_pname) {
        handle_error_msg("error: queue pathname is not specified. Perhaps -%c is missing?\n", QUEUE_OPT);
        goto print;
    } else if (prog_type & RECEIVER && file)
        printf("warning: using options -%c when -%c is selected is useless\n", FILE_OPT, RECEIVER_OPT);

    printf("program type = %s\n", prog_type == SENDER ? SENDER_STR : RECEIVER_STR);

    memset(&sb, 0x0, sizeof(struct stat));    
    if ((ret = stat(queue_pname, &sb)) < 0) {
        handle_error("stat", errno);
        goto end;        
    }

    if (prog_type & SENDER) {
        memset(&sb, 0x0, sizeof(struct stat));    
        if ((ret = stat(file, &sb)) < 0) {
            handle_error("stat", errno);
            goto end;        
        }

        if (((sb.st_mode & S_IFMT) != S_IFREG)) {
            switch (sb.st_mode & S_IFMT) {
                case S_IFDIR:
                    handle_error_msg("error: %s is a directory\n", file);
                    break;
                case S_IFIFO:
                    handle_error_msg("error: %s is a FIFO/pipe\n", file);
                    break;
                case S_IFBLK:
                    handle_error_msg("error: %s is a block device\n", file);
                    break;
                case S_IFSOCK:
                    handle_error_msg("error: %s is a socket\n", file);
                    break;
                case S_IFCHR:
                    handle_error_msg("error: %s is a character device\n", file);
                    break;
                case S_IFLNK:
                    handle_error_msg("error: %s is a symlink\n", file);
                    break;
                default:
                    handle_error_msg("error: %s is unknown\n", file);
                    break;
            }
            goto end;
        }
    }

    if ((key = ftok(queue_pname, MAGIC_ID)) < 0) {
        handle_error("ftok", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    if ((qid = msgget(key, IPC_CREAT | PERMS)) < 0) {
        handle_error("msgget", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    if (prog_type & SENDER)
        payload_size = sb.st_size;
    else {
        if ((ret = msgctl(qid, IPC_STAT, &ds)) < 0) {
            handle_error("msgctl", errno);
            goto end;
        }

        if (ds.msg_qnum > 0)
            payload_size = (size_t)ds.msg_cbytes;
        else {
            printf("No messages in the queue.\n");
            error_num = ENOMSG;
            ret = EXIT_SUCCESS;
            goto end;
        }
    }

    printf("payload_size = %zu\n", payload_size);

    /* 8 bytes for mtype and st_size for payload */
    /* sizeof(MSG) + sb.st_size OR sizeof(MSG) -1 + sb.st_size would still give us and 8 byte overhead */
    if (!(msg = calloc(1, sizeof(long) + payload_size + 1))) { /* null-terminating byte as a safety check */
        handle_error("calloc", errno);
        ret = EXIT_FAILURE;
        goto end;        
    }

    msg->mtype = mtype;

    if (prog_type & SENDER) {
        printf("file to read = %s\n", file);
        if ((fd = open(file, O_RDONLY)) < 0) {
            handle_error("open", errno);
            ret = EXIT_FAILURE;
            goto end;
        }

        if ((nbytes = read(fd, msg->mtext, payload_size)) < 0) {
            handle_error("read", errno);
            ret = EXIT_FAILURE;
            goto end;
        }

        if ((ret = msgsnd(qid, msg, payload_size, 0)) < 0) {
            handle_error("msgsnd", errno);
            goto end;
        }

        printf("Sent message of %ld with mtype %ld\n", nbytes, msg->mtype);
    } else {
        if ((nbytes = msgrcv(qid, msg, payload_size, msg->mtype, IPC_NOWAIT)) < 0) {
            error_num = errno; // we retain errno because it is highly volatile between function calls
            if (error_num != ENOMSG) {
                ret = EXIT_FAILURE;
                handle_error("msgrcv", error_num);
                goto end;
            /* ENOMSG is not a critical error, it's just there are no message is all */
            } else
                handle_error_msg("No messages in queue%d\n", qid);
        } else {
            printf("Read message of %ld bytes\n", nbytes);
            printf("Message:\n%s\n", msg->mtext);
        }
    }
end:
    if (msg)
        free(msg);
    if (fd != -1)
        (void)close(fd);
    if (prog_type & RECEIVER && qid != -1) {
        memset(&ds, 0x0, sizeof(ds));
        /* If one receiver fails while there are 50 valid messages on the queue, we shouldn't delete it */
        if ((ret = msgctl(qid, IPC_STAT, &ds)) < 0) {
            handle_error("msgctl", errno);
            return ret;
        }        
        if (((nbytes >= 0 && ds.msg_qnum <= 0) || (ret == EXIT_FAILURE && error_num != ENOMSG)))
            (void)msgctl(qid, IPC_RMID, NULL);
    }
    /* Make a failed state explicit if it exists */
    ret = ret != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return ret;
print:
    print_usage();
    return ret;
}

void print_usage(void)
{
    printf("Usage:\n");
    printf("1) %s -%c OR -ro\n", PROG_NAME, RECEIVER_OPT);
    printf("2) %s -%c -%c %s OR -so %s\n", PROG_NAME, SENDER_OPT, FILE_OPT, EXAMPLE_PATH, EXAMPLE_PATH);
    printf("3) %s -%c -%c %s -%c 4 OR -ro %s\n", PROG_NAME, RECEIVER_OPT, FILE_OPT, EXAMPLE_PATH, TYP_OPT, EXAMPLE_PATH);
    printf("4) %s -%c -%c %s -%c 4 OR -so %s\n", PROG_NAME, SENDER_OPT, FILE_OPT, EXAMPLE_PATH, TYP_OPT, EXAMPLE_PATH);
    printf("5) %s -%c -%c %s -%c 4 OR -rq %s\n", PROG_NAME, RECEIVER_OPT, QUEUE_OPT, DEFAULT_QUEUE_PATHNAME, TYP_OPT, DEFAULT_QUEUE_PATHNAME);
}

bool options_bundled(int opt, const char *optarg)
{
    bool bundled = false;

    if (opt <= 0 || !optarg)
        return false;

    if (*optarg == 'o'
        || *optarg == 't'
        || *optarg == 'q') {
        handle_error_msg("error: bundled options, i.e \"-%c%c\" are not allowed\n", opt, *optarg);
        bundled = true;
    }

    return bundled;
}