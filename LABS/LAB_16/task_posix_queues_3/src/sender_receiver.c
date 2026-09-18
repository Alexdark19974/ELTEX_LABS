#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdbool.h>
#define MSG "./feedlot/message"
#define MSG_2 "./feedlot/message2"
#define MSG_QUEUE "/message"
#define PROG_NAME "sender_receiver"
#define SENDER_STR "sender"
#define RECEIVER_STR "receiver"
#define EXAMPLE_PATH "/path/to/file"
#define SLASH_CHAR '/'
#define FILE_OPT 'o'
#define SENDER_OPT 's'
#define RECEIVER_OPT 'r'
#define TYP_OPT 't'
#define QUEUE_OPT 'q'
#define MSG_MAX_SIZE 8192 // standard MSG_MAX. CANNOT be changed by using mq_setattr() after a queue beging initialized
#define MSG_MAX_NUM 8
#define BUF_SIZ 256
#define PRI_NUM 7
#define MSG_NUM 2
#define PERMS 0666 // -rw-rw-rw
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)
#define handle_error_msg(fmt, ...) do { \
                            fprintf(stderr, fmt, __VA_ARGS__); \
                        } while(0)

typedef enum prog_e {
    NONE = 0,
    RECEIVER = 1,
    SENDER = 2
} Prog_t;

void print_usage(void);
bool options_bundled(int opt, const char *optarg);

int main(int argc, char **argv)
{
    const char *file = NULL;
    const char *qname = NULL;
    char *payload = NULL;
    struct stat sb;
    struct mq_attr attr = {0};
    size_t payload_size = 0;
    ssize_t nbytes = -1, rbytes = -1;
    long mtype = 1; // default mtype value
    unsigned int prio = 1;
    int mode = 0;
    Prog_t prog_type = NONE;
    mqd_t qfd = -1;
    int fd = -1, i;
    int opt = 0, ret = EXIT_SUCCESS;
    int msg_num, msg_read = 0, msg_sent = 0;

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
                qname = optarg;
                if (*qname != '/') {
                    handle_error_msg("POSIX message queue name must start with '%c'\n", SLASH_CHAR);
                    goto print;
                }
                printf("queue path name = %s\n", qname);
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
                prio = (unsigned int)mtype;
                break;
            case 'h':
                goto print;
            case ':':
                if (optopt == 'o')
                    handle_error_msg("-%c requires an argument, e.g. %s\n", optopt, EXAMPLE_PATH);
                else if (optopt == 't')
                    handle_error_msg("-%c requires an argument, e.g. %ld\n", optopt, mtype);
                else if (optopt == 'q') {
                    handle_error_msg("-%c requires an argument, e.g. %s\n", optopt, MSG_QUEUE);
                    break;
                }
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
    } else if (!qname) {
        handle_error_msg("error: queue pathname is not specified. Perhaps -%c is missing?\n", QUEUE_OPT);
        goto print;
    } else if (prog_type & RECEIVER && file)
        printf("warning: using options -%c when -%c is selected is useless\n", FILE_OPT, RECEIVER_OPT);

    printf("program type = %s\n", prog_type == SENDER ? SENDER_STR : RECEIVER_STR); 

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

    attr.mq_maxmsg = MSG_MAX_NUM;
    attr.mq_msgsize = MSG_MAX_SIZE;
    mode = O_CREAT;
    if (prog_type & SENDER)
        mode |= O_WRONLY;
    else
        mode |= O_RDONLY;
    /* Create or open a POSIX message queue. Signature is similar to open() */
    if ((qfd = mq_open(qname, mode, PERMS, &attr)) < 0) {
        ret = EXIT_FAILURE;
        handle_error("mq_open", errno);
        goto end;
    }

    printf("queue num = %d\n", (int)qfd);

    if (prog_type & SENDER) {
        payload_size = sb.st_size;
        if (payload_size > MSG_MAX_SIZE) {
            printf("warning: file is too big (%zu), message max size = %d. Needs chunking\n", payload_size, MSG_MAX_SIZE);
            payload_size = MSG_MAX_SIZE;
        }
    }
    else {
        if ((ret = mq_getattr(qfd, &attr)) < 0) {
            handle_error("mq_getattr", errno);
            goto end;
        }

        printf("messages in queue: %ld\n", attr.mq_curmsgs);
        if (attr.mq_curmsgs == 0) {
            printf("No messages. Leaving.\n");
            goto end;
        }
        payload_size = attr.mq_msgsize;
    }

    printf("payload_size = %zu\n", payload_size);
    if (!(payload = calloc(1, payload_size + 1))) { /* null-terminating byte as a safety check */
        handle_error("calloc", errno);
        ret = EXIT_FAILURE;
        goto end;        
    }

    if (prog_type & SENDER) {
        printf("file to read = %s\n", file);
        if ((fd = open(file, O_RDONLY)) < 0) {
            handle_error("open", errno);
            ret = EXIT_FAILURE;
            goto end;
        }

        printf("priority is %u\n", prio);
        /* Read up to rbytes == 0 which is either EOF or an empty message (also EOF, but an empty payload is valid) */
        while ((rbytes = read(fd, payload, payload_size)) >= 0) {
            /* msg_sent == 0 and rbytes == 0 is an empty payload which is valid */
            if (msg_sent > 0 && rbytes == 0) {
                printf("End of file\n");
                break;
            }

            if ((ret = mq_send(qfd, payload, payload_size, prio)) < 0) {
                handle_error("mq_send", errno);
                goto end;
            }

            nbytes += rbytes; // accumulate bytes
            msg_sent++;
            printf("Sent message %d of %ld with prio %u\n", msg_sent, rbytes, prio);
            memset(payload, 0x0, payload_size);
        }

        if (rbytes < 0) {
            handle_error("read", errno);
            ret = EXIT_FAILURE;
            goto end;
        }

        printf("Messages sent in total: %d\n", msg_sent);
        printf("bytes sent in total: %ld \n", nbytes);

    } else {
again:
        for (i = 0, msg_num = attr.mq_curmsgs; msg_num > 0; i++, msg_num--) {
            if ((rbytes = mq_receive(qfd, payload, payload_size, &prio)) < 0) {
                handle_error("mq_receive", errno);
                ret = EXIT_FAILURE;
                break; // if something bad happens and we have managed to read at least once, it's still a success
            }

            printf("Read %ld bytes from message queue\n", rbytes);
            printf("priority is %u\n", prio);
            printf("Message %d:\n", i + 1);

            if (rbytes > 0) {
                nbytes += rbytes; // accumulate bytes
                printf("%s\n", payload);
            } else
                printf("No payload\n");

            msg_read++;
            memset(payload, 0x0, payload_size);
        }

        /* Update the message queue status */
        if ((ret = mq_getattr(qfd, &attr)) < 0)
            handle_error("mq_getattr", errno);
        else {
            msg_num = attr.mq_curmsgs;
            if (msg_num > 0)
                goto again;
        }

        printf("Read messages in total: %d\n", msg_read);
        printf("Bytes read in total: %ld\n", nbytes);
    }
end:
    if (payload)
        free(payload);
    if (fd != -1)
        (void)close(fd);
    if (qfd != -1)
        (void)mq_close(qfd);
    if (prog_type & RECEIVER && nbytes >= 0 && msg_num <= 0) {
        (void)mq_unlink(qname);
        printf("Message queue '%s' unlinked successfully.\n", qname);
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
    printf("5) %s -%c -%c %s -%c 4 OR -rq %s\n", PROG_NAME, RECEIVER_OPT, QUEUE_OPT, MSG_QUEUE, TYP_OPT, MSG_QUEUE);
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