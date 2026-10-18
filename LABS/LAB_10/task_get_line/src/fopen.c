#include "common.h"
#include "fopen.h"
#include "debug.h"
#include "get_line.h"

int fopen_mode(void)
{
	FILE *fp = NULL;
	char *ch = NULL;
	size_t nbytes = 0, recv_nbytes = 0, len = 0;
	int fd = 0, status = OK;
	char buf[BUFSIZE] = {0};

	if (ERR == (status = get_line(buf, sizeof(buf))))
		goto fail;

	if (NULL == (fp = fopen(FILE_TXT, "w+"))) {
		PRINT_ERR("error writing to '%s`: %s", FILE_TXT, strerror(errno));
		status = ERR;
		goto fail;
	}
	
	len = strlen(buf);

	if (len + 1 != (nbytes = fwrite(buf, 1, len + 1, fp))) {
		PRINT_ERR("error writing to '%s`: %s", FILE_TXT, strerror(errno));
		status = ERR;
		goto fail;
	}
	
	PRINT_DBG("written %ld bytes", nbytes);

	if (ERR == (fd = fileno(fp))) {
		PRINT_ERR("error getting file descriptor of '%s': %s", FILE_TXT, strerror(errno));
		status = ERR;
		goto fail;	
	}
	
	if (ERR == fsync(fd)) {
		PRINT_ERR("error syncing '%s`: %s", FILE_TXT, strerror(errno));
		status = ERR;
		goto fail;
	}

	fseek(fp, 0, SEEK_SET);

	memset(buf, 0x0, sizeof(buf));

	if ((recv_nbytes = fread(buf, 1, nbytes, fp) < sizeof(char))) {
        PRINT_ERR("error during fread()`: no bytes read (%zu)", recv_nbytes);
        status = ERR;
        goto fail;
    }
	
	PRINT_DBG("read %zu bytes", recv_nbytes);

	len = strlen(buf);

	ch = buf + len;
	
	PRINT_N("%s", "reversed string: ");

	for (ch = buf + len; ch >= buf; ch--) {
		fputc(*ch, stdout);		
	}

	fputc('\n', stdout);

fail:
	if (fp)
		fclose(fp);

	return status;	
}

