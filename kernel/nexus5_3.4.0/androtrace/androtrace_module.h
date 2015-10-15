#ifndef _UDROID_IO_LOG_H_
#define _UDROID_IO_LOG_H_

/* buffer type */
#define ALL_LOG 1
#define IO_LOG 2
#define FILELS_LOG 4

/* -->IO Trace */
#define IO_BUF_COUNT 3
#define IO_BUF_SIZE_SHIFT  18
#define IO_BUF_SIZE 1 << IO_BUF_SIZE_SHIFT /* 0.25mb */
#define IO_FILE_SIZE_SHIFT 20 //20 /* 1024* 1024 */  
#define IO_FILE_SIZE  1 << IO_FILE_SIZE_SHIFT /* 1mb */  

/* -->FIle life-span */
#define FILELS_BUF_COUNT 3
#define FILELS_BUF_SIZE_SHIFT 16
#define FILELS_BUF_SIZE 1 << FILELS_BUF_SIZE_SHIFT //63336b
#define FILELS_FILE_SIZE_SHIFT 19
#define FILELS_FILE_SIZE 1 << FILELS_FILE_SIZE_SHIFT //0.5mb

#pragma pack(push,1)
struct cbuf
{
	int id;
	bool full;
	char *data;
	int offset;
	struct cbuf *next;
};
#pragma pack(pop)

#endif
