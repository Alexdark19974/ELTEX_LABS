#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <malloc.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

typedef enum act {
	FAIL = -1,
	OK = 0,
	QUIT = 1,
	CHDIR = 2,
} act_t;

#define CMD_SIZE (128)
#define ARG_NUM 24
#define NEWLINE ('\n')
#define IS_NEWLINE(x) ((x) == NEWLINE)
#define DELIMS ";&&"

int pseudo_bash(void);
int parse_cmd(char *buf);
int smart_space_cnt(const char *buf);
void sighandler(int signo, siginfo_t *info, void *context);

int main(void)
{
	struct sigaction act = {0};
	act.sa_flags = SA_SIGINFO | SA_RESTART;
	act.sa_sigaction = &sighandler;
	if (sigaction(SIGINT, &act, NULL) == FAIL) {
		fprintf(stderr, "[%s:%d] sigaction() failed: %s\n", __func__, __LINE__, strerror(errno));
		return EXIT_FAILURE;		
	}
	return pseudo_bash();
}

int pseudo_bash(void)
{
	char *chp = NULL;
	const char *user = NULL;
	char hostname[HOST_NAME_MAX + 1] = {0};
	int ret = EXIT_FAILURE;
	char buf[BUFSIZ + 1] = {0}, dir[BUFSIZ + 1] = {0};
	bool loop = true;

	if (!(user = getenv("USER"))) {
		fprintf(stderr, "[%s:%d] getenv() failed: %s\n", __func__, __LINE__, strerror((errno)));
		goto err;
	} else if (gethostname(hostname, sizeof(hostname)) < 0) {
		fprintf(stderr, "[%s:%d] gethostname() failed: %s\n", __func__, __LINE__, strerror((errno)));
		goto err;		
	}
 
	while (loop) {
		if (!*dir && !getcwd(dir, sizeof(dir)))
				goto err;

    	printf("[%s@%s:%s] $ ", user, hostname, dir);
    	fflush(stdout); // fgets (stdin) usually triggers fflush() but, on some systems, it doesn't
    	if (!fgets(buf, sizeof(buf), stdin)) {
	    	if (ferror(stdin)) {
	    		fprintf(stderr, "\n[%s:%d] an error ocurred: %s\n", __func__, __LINE__, strerror((errno)));
	    		break;
	    	} else if (feof(stdin)) { // leave at the 1st sight of EOF
	    		fprintf(stderr, "\n[%s:%d] received EOF from input\n", __func__, __LINE__);
	    		loop = false;
	    	}
	    	continue;
	    }

    	if (IS_NEWLINE(*buf))
			continue;

		if (!(chp = strchr(buf, NEWLINE))) { // input sanitization: discard if buf is not large enough
			printf("[%s:%d] WARNING: input was truncated because it's larger than %d bytes. Discarding the rest\n", __func__, __LINE__, BUFSIZ);
			while (getchar() != '\n') ;
			continue;
		}
		*chp = '\0'; // parse_cmd needs no newlines

		switch ((ret = parse_cmd(buf))) {
			case QUIT:
				ret = EXIT_SUCCESS;
				loop = false;
				break;
			case FAIL:
				ret = EXIT_FAILURE;
				loop = false;
				break;
			case CHDIR:
				*dir = '\0';
				break;
			default:
				break;
		}

		memset(buf, 0x0, sizeof(buf));
	}

err:
	return ret;
}

int parse_cmd(char *buf)
{
	char *tok = NULL, *cmd = NULL;
	int ret = OK, cmd_num = 0;
	pid_t pid = 0;

	tok = strtok(buf, DELIMS);
	while (tok) { // while loop is more appropriate than do-while for strtok: redunancy, clean code, etc.
		cmd_num++;
		printf("[%s:%d] tok=%s, cmd_num=%d\n", __func__, __LINE__, tok, cmd_num);

		while (isspace(*tok)) tok++;

		if (!strcmp(tok, "exit")) {
			ret = QUIT;
			break;
		} else if (!strncmp(tok, "cd", 2)) {
			char *home = NULL;
			tok += 2;
			if (!*tok) {
home:
				home = getenv("HOME");
				if (!home) {
					fprintf(stderr, "[%s:%d] $HOME var is missing!\n", __func__, __LINE__);
					ret = FAIL;
					break;
				}
			} else {
				char *sub_tok = NULL;
				while (isspace(*tok)) tok++;
				if (!*tok)
					goto home;
				sub_tok = tok;
				while (*sub_tok && !isspace(*sub_tok)) sub_tok++;
				*sub_tok = '\0';
			}
			printf("[%s:%d] arg=%s\n", __func__, __LINE__, tok);
			ret = chdir(*tok ? tok : home) < 0 ? FAIL : CHDIR;
			if (ret == FAIL)
				fprintf(stderr, "[%s:%d] chdir() failed: %s\n", __func__, __LINE__, strerror(errno));
			goto next;
		}

		free(cmd);
		cmd = strdup(tok);
		if (!cmd) {
			fprintf(stderr, "[%s:%d] strdup() failed: %s\n", __func__, __LINE__, strerror(errno));
			return FAIL;
		}

		printf("[%s:%d] cmd: %s\n", __func__, __LINE__, cmd);

		if ((pid = fork()) > 0) {
			int wstatus = 0;
			(void)wait(&wstatus);
			printf("[%s:%d] waiting for %d done\n", __func__, __LINE__, pid);
			if (WIFEXITED(wstatus)) {
				int exit_code = WEXITSTATUS(wstatus);
				printf("[%s:%d] child process %d exited %s\n", __func__, __LINE__, pid, exit_code == 0 ? "normally" : "with an error");
			} else if (WIFSIGNALED(wstatus)) {
				printf("[%s:%d] child process %d killed by signal %d\n", __func__, __LINE__, pid, WTERMSIG(wstatus));
			}
		} else if (pid == FAIL) {
			fprintf(stderr, "[%s:%d] fork() failed: %s\n", __func__, __LINE__, strerror(errno));
		} else if (pid == 0) {
			char **argv = NULL;
			int arg_num = smart_space_cnt(cmd) + 1;

			(void)signal(SIGINT, SIG_DFL); 

			printf("[%s:%d] arg_num=%d\n", __func__, __LINE__, arg_num);
			if ((argv = calloc(arg_num + 1 /* extra argv[i] for execvp */, sizeof(char *)))) {
				int i = 0;
				char *cmd_tok = strtok(cmd, " ");
				while (cmd_tok) {
					argv[i++] = cmd_tok;
					printf("[%s:%d] i=%d\n", __func__, __LINE__, i);
					cmd_tok = strtok(NULL, " ");
				}

				if (execvp(argv[0], argv) == FAIL) // the code below should never be executed...
					fprintf(stderr, "[%s:%d] execvp() failed: %s\n", __func__, __LINE__, strerror(errno));
			} else
				fprintf(stderr, "[%s:%d] calloc() failed: %s\n", __func__, __LINE__, strerror(errno));

			free(argv);
			free(cmd);
			exit(EXIT_FAILURE);
		}
next:
		tok = strtok(NULL, DELIMS);
	}

	free(cmd);

	return ret;
}

int smart_space_cnt(const char *buf)
{
	int cnt = 0;

	if (!buf || !*buf)
		return cnt;

	/* we skip the first trailing spaces before counting */
	while (*buf && (isspace(*buf))) buf++;

	while (*buf) {
		if (isspace(*buf)) {
			while (*buf && (isspace(*buf))) buf++;
			if (!*buf)
				break;
			cnt++;
		}
		buf++;
	}

	printf("[%s:%d] cnt=%d\n", __func__, __LINE__, cnt);
	return cnt;
}

void sighandler(int signo, siginfo_t *info, void *context)
{
	(void)info;
	(void)context;

	printf("\n[%s:%d] received sig%d\n", __func__, __LINE__, signo);
}