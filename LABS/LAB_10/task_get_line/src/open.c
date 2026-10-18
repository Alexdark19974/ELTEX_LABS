#include "common.h"
#include <fcntl.h>
#include "open.h"
#include "debug.h"
#include "get_line.h"

int open_mode(void)
{
	char *ch = NULL;
	size_t len = 0;
	ssize_t nbytes = 0, recv_nbytes = 0;
	int fd = 0, status = OK;
	char buf[BUFSIZE] = {0};

	if (ERR == (status = get_line(buf, sizeof(buf))))
		goto fail;
	
	if (ERR == (fd = status = open(FILE_TXT, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU))) {
		PRINT_ERR("error opening %s: %s", FILE_TXT, strerror(errno));
		goto fail;
	}
	
	len = strlen(buf);

	if (ERR == (nbytes = status = write(fd, buf, len + 1))) {
		PRINT_ERR("error writing to %s: %s", FILE_TXT, strerror(errno));
		goto fail;
	}

	PRINT_DBG("written %ld bytes", nbytes);

	if (ERR == (status = fsync(fd))) {
		PRINT_ERR("error syncing %s: %s", FILE_TXT, strerror(errno));
		goto fail;
	}

	lseek(fd, 0L, SEEK_SET);

	memset(buf, 0x0, sizeof(buf));

	recv_nbytes = read(fd, buf, nbytes);

	if ((recv_nbytes = read(fd, buf, nbytes)) <= 0) {
        PRINT_ERR("error during read(): bytes read (%zu:%s)", recv_nbytes, strerror(errno));
        status = ERR;
        goto fail;
    }
	PRINT_DBG("read %zd bytes", recv_nbytes);

	len = strlen(buf);

	ch = buf + len;

	PRINT_N("%s", "reversed string: ");

	for (ch = buf + len; ch >= buf; ch--) {
		if (write(STDOUT_FILENO, ch, 1) < 0) {
	        PRINT_ERR("error during write(): %s", strerror(errno));
	        status = ERR;
    	    goto fail;
		}		
	}

	if (write(STDOUT_FILENO, "\n", 1) < 0) {
        PRINT_ERR("error during write(): %s", strerror(errno));
        status = ERR;
	}

fail:
	if (fd > 0)
		close(fd);

	return status;
}
