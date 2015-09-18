/* -->data information */
/* date */
#define YEAR_BASE 2000
#define YEAR_RELEASE 14
/* file_type based on file extension */
#define NONE_FILE 0
#define SQLITE_DB_FILE 1
#define SQLITE_JOURNAL_FILE 2
#define SQLITE_WAL_FILE 3
#define SQLITE_TEMP_FILE 4
#define MULTI_FILE 5
#define EXEC_FILE 6
#define RESOURCE_FILE 7
#define TEMP_FILE 8
#define ETC_FILE 9
#define ANON_FILE 10 /* anoymous page, PageAnon(page) ->to be discuss */
#define UDROID_IO_FILE 11
#define UDROID_FILELS_FILE 12
#define UDROID_MEM_FILE 13
#define UDROID_MEMDETAIL_FILE 14

/* rwbs(read,write, synchronous)*/
#define READ_NONE_MODE 0
#define WRITE_NONE_MODE 1
#define READ_SYNC_MODE 2
#define WRITE_SYNC_MODE 3

/* block type */ 
#define BLOCK_META 0
#define BLOCK_JOURNAL 1  
#define BLOCK_DATA 2
#define BLOCK_ANON 3 /*anonymous page, PageAnon(page) ->to be discuss */
#define BLOCK_NONE 4 

/* process name*/
#define PNAME_MAX 16

/* trace type */
#define TRACE_IO 1
#define TRACE_FILELS 2
#define TRACE_SQLITE 3
#define TRACE_SUSPEND 4

/*SUSPEND trace type */
#define TRACE_PM_SUSPEND_ON 1
#define TRACE_PM_SUSPEND_MEM 2
#define TRACE_SUSPEND_ROUTINE_START 3
#define TRACE_SUSPEND_ROUTINE_END 4
#define TRACE_SYNCING_FS 5 
#define TRACE_SYNCING_DONE 6
#define TRACE_SUSPEND_FAILED 7
#define TRACE_SUSPEND_ENTER 8
#define TRACE_WAKEUP 9
#define TRACE_SUSPEND_FAILED_FREEZE 10



#pragma pack(push,1)
struct at_header{
	unsigned char type;
	unsigned char size;
	/* day & time */
	unsigned int year:4;  //14,15
	unsigned int month:4;
	unsigned int day:5;
 	unsigned int hour:5;
	unsigned int min:6;
	unsigned int sec:6;
	unsigned int reserved:2; 
	unsigned int nsec;	
};

struct uio_ws{
	unsigned char fs_id;
	unsigned char fsync :1;
	unsigned char fdatasync :1;
	unsigned char fsync_sqlite :1;
	unsigned char reserved :5;
	unsigned int t_id;
	unsigned char journal_type;
};

struct uio
{
	/* day & time */
	/* -150511 ryoung, date ->at_header */
/*
	unsigned int year:4;  //14,15
	unsigned int month:4;
	unsigned int day:5;
 	unsigned int hour:5;
	unsigned int min:6;
	unsigned int sec:6;
	unsigned int reserved:2; 
	unsigned int nsec;
*/
	/* 140428, 140813 modify ryoung (fsync, fdatasync) */
	unsigned char file_type; //150124 -->at server
	unsigned char action :1; /* D, C */
	unsigned char rwbs :2;
	unsigned char ws_info :1; /* synchronous write informatio */
	unsigned char discard :1;
	unsigned char flush :1;
	unsigned char reserved2 :2;

	/* 20140523 add ryoung major,minor device number */
	unsigned char major_dev;
	unsigned char minor_dev;
	unsigned char block_type; 
	unsigned int sector_nb; 
	unsigned int block_len; 
	unsigned char pname_len;
	/*
	char* pname 
	unsigned char fname_len;
	char* fname;
	*/
};

struct ufilels
{
	/* create day&time */
	unsigned int c_year:4;
	unsigned int c_month:4;
	unsigned int c_day:5;
	unsigned int c_hour:5;
	unsigned int c_min:6;
	unsigned int c_sec:6;
	unsigned int c_reserved:2;
	unsigned int c_nsec;
	/* delete  day&time */
	/*
	unsigned int d_year:4;
	unsigned int d_month:4;
	unsigned int d_day:5;
	unsigned int d_hour:5;
	unsigned int d_min:6;
	unsigned int d_sec:6;
	unsigned int d_reserved:2;
	unsigned int d_nsec;
	*/
	/* diff day & time */
	/*
	unsigned int di_year:4;
	unsigned int di_month:4;
	unsigned int di_day:5;
	unsigned int di_hour:5;
	unsigned int di_min:6;
	unsigned int di_sec:6;
	unsigned int di_reserved:2;
	unsigned int di_nsec;
	*/
	unsigned char ext :4;
	unsigned char reserved :4;
	unsigned char major_dev;
	unsigned char minor_dev;
	long long filesize;
	unsigned char pname_len;
	/* char* pname 
	unsigned char fname_len;
	char* fname;
	char* filename; //dynamic size */
};

struct at_suspend{
	unsigned char msg_type; 
};

struct at_sqlite{
	char id;
	unsigned int action :1;
	unsigned int j_mode :3;
	unsigned int s_mode :2;
	unsigned int operation : 4;
	unsigned int reserved : 22;
	unsigned char path_len;
	/*
	char* path;
	unsigned char name_len;
	char *db_name;
	*/
};

/*
struct at_str{
	unsigned char len;
	unsigned char str;

}
*/
#pragma pack(pop)

