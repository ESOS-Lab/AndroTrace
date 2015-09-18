#include <linux/string.h>
#include "common.h"

#include <linux/time.h>
#include <linux/file.h>
#include <linux/fcntl.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/syscalls.h>
#include <linux/namei.h>
#include <linux/fs.h>
#include <linux/blkdev.h>
#include "androtrace_define.h"

bool __get_io_buffer_left(unsigned char size);
void __check_io_buffer_left(unsigned char size);
bool __get_filels_buffer_left(unsigned char size);
void __check_filels_buffer_left(unsigned char size);

void __file_header(unsigned char type, struct at_header *header);

bool androtrace_add_io(struct request *rq, bool is_issue);
bool androtrace_add_filels(struct inode *inode);
bool androtrace_add_sqlite(struct at_sqlite *sqlite);
bool androtrace_add_suspend(unsigned char type);


