#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define ANDROTRACE_DEVICE_FD  "/dev/androtrace_device"
#define ANDROTRACE_IO_DIR 	  "/data/androtrace/io"
#define ANDROTRACE_FILELS_DIR "/data/androtrace/filels"

#include "androtrace_define.h"
#include "androtrace_module.h"

static int dev_fd;  /*char device(kernel module)*/

static int io_fd;
static int io_f_offset;
static char io_buf_data[IO_BUF_SIZE];
static int fls_fd;
static int fls_f_offset;
static char fls_buf_data[FILELS_BUF_SIZE];

/*return value: file descriptor, after creating a new log file*/
static int log_file_creat(int type)
{
	char path[300];
	int ret_fd;
	struct timeval tv_now;
	struct tm *tm;
	gettimeofday(&tv_now);
	tv_now.tv_sec += 60 * 60 * 9;
	tm = localtime(&tv_now.tv_sec);

	if(type == IO_LOG){
		sprintf(path, "%s/%.2d%.2d%.2d%.2d%.2d%.2d.uio",
				ANDROTRACE_IO_DIR, tm->tm_year+1900-YEAR_BASE, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	}else if(type == FILELS_LOG){
		sprintf(path, "%s/%.2d%.2d%.2d%.2d%.2d%.2d.ufls",
			ANDROTRACE_FILELS_DIR, tm->tm_year+1900-YEAR_BASE, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);	
	}
	
	ret_fd = creat(path,  0777);
	chmod(path, 0777);
	if (ret_fd < 0) {
		printf("[androtrace] %s() failed to open file:%s\n", __func__, path);
		return 0;
	}
	return ret_fd;
}

static void io_file_write()
{
	int read_bytes, write_bytes;
	memset(io_buf_data, 0, IO_BUF_SIZE);
	read_bytes = read(dev_fd, io_buf_data, IO_LOG); /*type: what buffer user want to read from kernel */
	if (read_bytes > 0 && io_buf_data != NULL) {
		if (io_f_offset + read_bytes <= IO_FILE_SIZE) {
			write_bytes = write(io_fd, io_buf_data, read_bytes);
			io_f_offset += write_bytes;
		} else{
			close(io_fd);
			fflush(stdin);
			io_f_offset = 0;
			io_fd = log_file_creat(IO_LOG);
			write_bytes = write(io_fd, io_buf_data, read_bytes);
			io_f_offset += write_bytes;
		}
	}
}

static void fls_file_write()
{
	int read_bytes, write_bytes;
	memset(fls_buf_data, 0, FILELS_BUF_SIZE);
	read_bytes = read(dev_fd, fls_buf_data, FILELS_LOG); /* type: what buffer user want to read from kernel	*/
	if (read_bytes > 0 && fls_buf_data != NULL) {
		if (fls_f_offset + read_bytes <= FILELS_FILE_SIZE) {
			write_bytes = write(fls_fd, fls_buf_data, read_bytes);
			fls_f_offset += write_bytes;
		} else {
			close(fls_fd);
			fflush(stdin);
			fls_f_offset = 0;
			fls_fd = log_file_creat(FILELS_LOG);
			write_bytes = write(fls_fd, fls_buf_data, read_bytes);
			fls_f_offset += write_bytes;
		}
	}
}

int main (int argc, char **argv)
{
	int retval = 0;
	struct pollfd events[1];
	struct timeval tv_now;
	struct tm *tm;
	io_fd = 0;
	io_f_offset = 0;
	memset(io_buf_data, 0, IO_BUF_SIZE);
	fls_fd = 0;
	fls_f_offset = 0;
	memset(fls_buf_data, 0, FILELS_BUF_SIZE);
	sleep(20); /*16~17*/

	/*sleep until date sync*/
	while (1) {
		gettimeofday(&tv_now);
		tv_now.tv_sec += 60 * 60 * 9;
		tm = localtime(&tv_now.tv_sec);
		if (tm->tm_year + 1900-YEAR_BASE < YEAR_RELEASE)
			sleep(2);
		else
			break;
	};
	/*open device file */
	dev_fd = open(ANDROTRACE_DEVICE_FD, O_RDWR | O_NOCTTY);
	memset(events, 0, sizeof(events));
	events[0].fd = dev_fd;
	events[0].events = ALL_LOG | IO_LOG | FILELS_LOG | POLLERR;
	if (!dev_fd) {
		printf("[androtrace] %s() failed to open device file\n", __func__);
		return 0;
	}
	io_fd = log_file_creat(IO_LOG);
	fls_fd = log_file_creat(FILELS_LOG);
	while (1) {
		retval = poll((struct pollfd *)&events, 1, 1000);
		if (retval < 0)
			printf("[androtrace] %s() poll error\n", __func__);
		else if (retval == 0)
			printf("[androtrace] %s() no event\n", __func__);
		else {

			if (ALL_LOG & events[0].revents) {
				io_file_write();
				fls_file_write();
			} else if (IO_LOG & events[0].revents) {
				io_file_write();
			} else if (FILELS_LOG & events[0].revents) {
				fls_file_write();
			}
		}
	}
	close(io_fd);
	close(fls_fd);
	close(dev_fd);
	return 0;
}


