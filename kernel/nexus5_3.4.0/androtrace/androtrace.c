/*
 * author: eunryoung lim 
 * date: 15/6/19
 * desc:
 *  -I/O traces: from block/blk-core.c
 *  -suspend: from kernel/power
 *  -SQLite: from SQLite Lib platform
 */

#include "androtrace.h"
#include "androtrace_module.h"

#define CONFIG_ANDROTRACE_PRINT

extern struct cbuf *io_buf_hand;
extern raw_spinlock_t io_buf_lock;
extern struct cbuf *fls_buf_hand;
extern raw_spinlock_t fls_buf_lock;

extern wait_queue_head_t wait_queue;

bool __get_io_buffer_left(unsigned char size){
	int idx = 0;

	size += 20; /*header */

	/*--->find buffer & save log data */
	for (idx = 0; idx < IO_BUF_COUNT; ++idx) {
		if (io_buf_hand->full) { /* user daemon has not been sent data to storage */
			raw_spin_lock_irq(&io_buf_lock);
			io_buf_hand = io_buf_hand->next;
			raw_spin_unlock_irq(&io_buf_lock);
		} else if (io_buf_hand->offset + size > IO_BUF_SIZE) { /* buffer has no space to save log */
			raw_spin_lock_irq(&io_buf_lock);
			io_buf_hand->full = true;
			raw_spin_unlock_irq(&io_buf_lock);
			wake_up_interruptible(&wait_queue); /* wake up user daemon */
			raw_spin_lock_irq(&io_buf_lock);
			io_buf_hand = io_buf_hand->next;
			raw_spin_unlock_irq(&io_buf_lock);
		} else {
			return true;
		}
	}

	/* if (!find){ */
	printk("\t[udroid] buffer full!\n");
	return 0;
}

void __check_io_buffer_left(unsigned char size)
{
	/* 140523, add ryoung, if file will be over: */
	if (io_buf_hand->offset + size > IO_BUF_SIZE) {
		raw_spin_lock_irq(&io_buf_lock);
		io_buf_hand->full = true;
		raw_spin_unlock_irq(&io_buf_lock);
		wake_up_interruptible(&wait_queue);
		raw_spin_lock_irq(&io_buf_lock);
		io_buf_hand = io_buf_hand->next;
		raw_spin_unlock_irq(&io_buf_lock);
	}
}

bool __get_filels_buffer_left(unsigned char size){
	int idx = 0;

	size += 20; /*header */

	/*--->find buffer & save log data */
	for (idx = 0; idx < FILELS_BUF_COUNT; ++idx) {
		if (fls_buf_hand->full) { /* user daemon has not been sent data to storage */
			raw_spin_lock_irq(&fls_buf_lock);
			fls_buf_hand = fls_buf_hand->next;
			raw_spin_unlock_irq(&fls_buf_lock);
		} else if (fls_buf_hand->offset + size > FILELS_BUF_SIZE) { /* buffer has no space to save log */
			raw_spin_lock_irq(&fls_buf_lock);
			fls_buf_hand->full = true;
			raw_spin_unlock_irq(&fls_buf_lock);
			wake_up_interruptible(&wait_queue); /* wake up user daemon */
			raw_spin_lock_irq(&fls_buf_lock);
			fls_buf_hand = fls_buf_hand->next;
			raw_spin_unlock_irq(&fls_buf_lock);
		} else {
			return true;
		}
	}

	/* if (!find){ */
	printk("\t[udroid] buffer full!\n");
	return 0;
}

void __check_filels_buffer_left(unsigned char size)
{
	size += 21; /*header, offset */
	/* 140523, add ryoung, if file will be over: */
	if (fls_buf_hand->offset + size > FILELS_BUF_SIZE) {
		raw_spin_lock_irq(&fls_buf_lock);
		fls_buf_hand->full = true;
		raw_spin_unlock_irq(&fls_buf_lock);
		wake_up_interruptible(&wait_queue);
		raw_spin_lock_irq(&fls_buf_lock);
		fls_buf_hand = io_buf_hand->next;
		raw_spin_unlock_irq(&fls_buf_lock);
	}
}

void __fill_header(unsigned char type, struct at_header *header)
{
	struct timespec ts_now;		/* date */
	struct tm tm;			/* time_to_tm(timeconv) */

	header->type = type;
	header->size = 0;

	/* get current time */
	getnstimeofday(&ts_now);
	ts_now.tv_sec += 60 * 60 * 9;   /* GMT +9hour(Korea);*/
	time_to_tm(ts_now.tv_sec, 0, &tm);

	/*----colloect log data */
	header->year = tm.tm_year + 1900 - YEAR_BASE /*2000*/;
	header->month = tm.tm_mon + 1;
	header->day = tm.tm_mday;
	header->hour = tm.tm_hour;
	header->min = tm.tm_min;
	header->sec = tm.tm_sec;
	header->nsec = ts_now.tv_nsec;

}

bool androtrace_add_io(struct request *rq, bool is_issue)
{
	struct at_header header;
	struct uio log;
	struct uio_ws log_ws; 		/* +ryoung, 150129 synch write */

	unsigned char log_size = 0;
	struct timespec ts_now;		/* date */
	struct tm tm;			/* time_to_tm(timeconv) */
	struct bio *bio = NULL;		/* the first bio of request */
	char *bio_buf = NULL;		/* check jouranl header */
	struct address_space *mapping = NULL;   /* flags for sync/fdatasync */
	struct inode *inode = NULL;	/* bio's address_space->inode */
	struct list_head *next = NULL;	/* dentry.next */
	struct dentry *dentry = NULL;	/* dentry, to get file name */
	struct task_struct *bio_task = NULL;    /* process who submit bio */
	int pname_len = 0;
	char pname[PNAME_MAX];
	char *fname = NULL;
	int fname_len = 0;

#ifdef CONFIG_ANDROTRACE_PRINT
	char *str_rwbs = NULL;
	char *str_bt = NULL;
	char *str_fsync = NULL;
	char *str_fdatasync = NULL;
	char *str_fsync_sqlite = NULL;
	char str_log[200];

	memset(str_log, 0, 200);
#endif

	
	log_size = sizeof(struct uio);
	memset(&log, 0, sizeof(struct uio));
	memset(&log_ws, 0, sizeof(struct uio_ws));
	memset(&pname, 0, PNAME_MAX);

	getnstimeofday(&ts_now);
	ts_now.tv_sec += 60 * 60 * 9;   /* GMT +9hour(Korea); */
	time_to_tm(ts_now.tv_sec, 0, &tm);

	/* check buffer is allocated? */
	if (IS_ERR_OR_NULL(io_buf_hand))
		return 0;

	/* header information (time) */
	__fill_header(TRACE_IO, &header);

	/* io information */
	bio = rq->bio;	
	if (bio && bio->bi_io_vec && bio->bi_io_vec->bv_page && bio->bi_io_vec->bv_page->mapping) {
		if (bio_has_data(bio))
			bio_buf = bio_data(bio);
		if (PageAnon(bio->bi_io_vec->bv_page)) { /* page->mapping points to anon_vma object, not address_space */
			log.block_type = BLOCK_ANON;
			log.file_type = ANON_FILE;
		} else { /* page->mappings points to address_space object */

			mapping = bio->bi_io_vec->bv_page->mapping;
			inode = mapping->host;
			if (inode && inode->i_ino != 0 && &inode->i_dentry) {
				log.block_type = BLOCK_DATA;
				/* to get file name, file type */
				next = inode->i_dentry.next;
				dentry = list_entry(next, struct dentry, d_alias);
				if (dentry->d_name.len > 0) {
					fname = (char*)dentry->d_name.name;
					fname_len = strlen(fname);
					log.file_type = get_file_type(fname);
				}
				log.major_dev = MAJOR(inode->i_sb->s_dev);
				log.minor_dev = MINOR(inode->i_sb->s_dev);
			} else {
				/* joruanl or meta block */
				if (!IS_ERR_OR_NULL(bio_buf)) {
					/* journal signature(magic number) */
					if (*(bio_buf) == 0xc0 && *(bio_buf+1) == 0x3b &&
					    *(bio_buf+2) == 0x39 && *(bio_buf+3) == 0x98)
						log.block_type = BLOCK_JOURNAL;
					else
						log.block_type = BLOCK_META;
				} else {
					log.block_type = BLOCK_NONE;
				}
				log.major_dev = MAJOR(inode->i_rdev);
				log.minor_dev = MINOR(inode->i_rdev);
			}
		}

		/*----collects I/O data */
		log.action = is_issue;
		/* operation type */
		if (rq->cmd_flags & REQ_WRITE) { 
			if (rq->cmd_flags & REQ_SYNC)
				log.rwbs = WRITE_SYNC_MODE;
			else
				log.rwbs = WRITE_NONE_MODE;
		} else { 
			if (rq->cmd_flags & REQ_SYNC) 
				log.rwbs = READ_SYNC_MODE;
			else
				log.rwbs = READ_NONE_MODE;
		}
		/* synchronous write  */
		if (log.rwbs == WRITE_SYNC_MODE) {
			log_size += sizeof(struct uio_ws);
			log.ws_info = 1;
			if ( !IS_ERR_OR_NULL(mapping)){
				log_ws.fs_id = mapping->fs_id;
				if (mapping->fs_flags & AS_FS_FSYNC)
					log_ws.fsync = 1;
				if (mapping->fs_flags & AS_FS_FDSYNC)
					log_ws.fdatasync = 1;
				if (mapping->fs_flags & AS_FS_SQLITE)
					log_ws.fsync_sqlite = 1;
			}
		}
		/* @todo. discard, flush has no data */
/*		if (rq->cmd_flags & REQ_DISCARD)
			log.discard = 1;
	 	if (rq->cmd_flags & REQ_FLUSH)
			log.flush = 1;
*/
		log.sector_nb = (int)blk_rq_pos(rq);
		log.block_len = (int)blk_rq_bytes(rq) / 512;
		if (bio->bi_pid != 0) {
			bio_task = find_task_by_vpid(bio->bi_pid);
			if (!IS_ERR_OR_NULL(bio_task) &&
				!IS_ERR_OR_NULL(bio_task->comm)) {
				while (bio_task->comm[pname_len] != '\0') {
					pname[pname_len] = bio_task->comm[pname_len];
					pname_len++;
				}
				log.pname_len = pname_len;
			}
		}
		
		/* to get journal type, tranasction id */
		if (log.block_type == BLOCK_JOURNAL && !IS_ERR_OR_NULL(bio_buf))  {
			memcpy( &log_ws.journal_type, (char*)bio_buf+7, 1); 
			/* memcpy & shift ( big-endian, litten-endian) */
			memcpy(&log_ws.t_id, (char*)bio_buf+8, 4);
			log_ws.t_id = ((log_ws.t_id << 24) & 0xFF000000) | ((log_ws.t_id << 8) & 0x00FF0000)
					| ((log_ws.t_id >> 8) & 0x0000FF00) | ((log_ws.t_id >> 24) & 0x000000FF);
		}

#ifdef DEBUG_RYOUNG
		{
			int i = 0;
			if (log.block_type == BLOCK_JOURNAL) {
				bio_buf = bio_data(bio);
				for(i=0; i< 12; ++i) {
					printk("%02x", *(bio_buf+i));
					if ( (i+1) % 4 ==0 )
						printk("\t");
					if( (i+1) % 16 ==0)
						printk("\n\t");
				}
				printk("\t");
			}
		}
#endif
		/* log_size = pname + file name + file name length */
		log_size += pname_len + 1 + fname_len;

		/*--->find buffer & save log data */
		if (!__get_io_buffer_left(log_size))	
			return 0;
		/* save I/O information to buffer */
		raw_spin_lock_irq(&io_buf_lock);
		header.size = log_size;
		memcpy(io_buf_hand->data + io_buf_hand->offset, &header, sizeof(struct at_header));
		io_buf_hand->offset += sizeof(struct at_header);
		//memcpy(io_buf_hand->data + io_buf_hand->offset, &log_size, 1);
		//io_buf_hand->offset += 1;
		memcpy(io_buf_hand->data + io_buf_hand->offset, &log, sizeof(struct uio));
		io_buf_hand->offset += sizeof(struct uio);
		if (pname_len > 0) {
			memcpy(io_buf_hand->data + io_buf_hand->offset, pname, pname_len);
			io_buf_hand->offset += pname_len;
		}
		memcpy(io_buf_hand->data + io_buf_hand->offset, &fname_len, 1);
		io_buf_hand->offset += 1;
		if (fname_len > 0) {
			memcpy(io_buf_hand->data + io_buf_hand->offset, fname, fname_len);
			io_buf_hand->offset += fname_len;
		}
		if (log.ws_info) {
			memcpy(io_buf_hand->data + io_buf_hand->offset, &log_ws, sizeof(struct uio_ws));
			io_buf_hand->offset += sizeof(struct uio_ws);
		}
		raw_spin_unlock_irq(&io_buf_lock);
		// }

#ifdef CONFIG_ANDROTRACE_PRINT
		if (log_ws.fsync_sqlite)
			str_fsync_sqlite = "S";
		if (log_ws.fsync)
			str_fsync = "F";
		if (log_ws.fdatasync)
			str_fdatasync ="D"; 
/*
		log_ws.fsync_sqlite ? str_fsync_sqlite = "S" : "";
		log_ws.fsync ? str_fsync = "F" : "";
		log_ws.fdatasync ? str_fdatasync = "D" : "";
*/
		if (log.rwbs == READ_NONE_MODE)
			str_rwbs = "R";
		else if (log.rwbs == WRITE_NONE_MODE)
			str_rwbs = "W";
		else if (log.rwbs == WRITE_SYNC_MODE)
			str_rwbs = "WS";

		if (log.block_type == BLOCK_META)
			str_bt = "M";
		else if (log.block_type == BLOCK_JOURNAL)
			str_bt = "J";
		else if (log.block_type == BLOCK_DATA)
			str_bt = "D";
		else if (log.block_type == BLOCK_ANON)
			str_bt = "A";
		else if (log.block_type == BLOCK_NONE)
			str_bt = "N";

		snprintf(str_log, sizeof(str_log), "[%d] %d%d\t%d\t%s\t%s%s%s\t%s\t%d\t%d\t%s\t%s\ttid:%d\tfid:%d\t",
		is_issue,
		header.min, header.sec, header.nsec,
		str_rwbs, str_fsync_sqlite, str_fsync, str_fdatasync,
		str_bt, log.sector_nb, log.block_len,
		pname, fname, log_ws.t_id, log_ws.fs_id);


	/*	
		if (log.ws_info){
			//snprintf(str_log, sizeof(str_log), "%d%d%d%d%d%d\t %d %s\t%s%s%s\t %d:%d\t %s\t %d %d\t %s\t %s",
			snprintf(str_log, sizeof(str_log), "[%d] %d%d%d%d%d%d\t%d\t%s\t%s%s%s\t%s\t%d\t%d\t%s\t%s\t%d\t tid:%d\t%d\t fid:%d\t",
			is_issue,
			header.year, header.month, header.day,
			header.hour, header.min, header.sec, header.nsec,
			str_rwbs, str_fsync_sqlite, str_fsync, str_fdatasync,
			str_bt, log.sector_nb, log.block_len,
			pname, fname, log.file_type,
			log_ws.t_id, log_ws.journal_type, 
			log_ws.fs_id);
		} 
		else {
			snprintf(str_log, sizeof(str_log), "[%d] %d%d%d%d%d%d\t %d\t %s\t \t\t\t\t %s\t %d\t%d\t %s\t%s\t%d\t",
			is_issue,
			header.year, header.month, header.day,
			header.hour, header.min, header.sec, header.nsec,
			str_rwbs, 
			str_bt, log.sector_nb, log.block_len,
			pname, fname, log.file_type);
		}
	*/

		printk(KERN_INFO "\t%s\n", str_log);
#endif
		__check_io_buffer_left(log_size+24 /*header*/);
	}

	return true;
}


#ifdef CONFIG_ANDROTRACE_FILELS
bool androtrace_add_filels(struct inode *inode)
{
	struct at_header header;
        struct ufilels log;

	//140619 modify ryoung /*log_size*/
	unsigned char log_size = 0;
#ifdef CONFIG_ANDROTRACE_PRINT
	char str_log[200]; 		
#endif
	struct tm tm;
	struct list_head *next = NULL;
	struct dentry *dentry = NULL;
	int pname_len = 0;
	char pname[PNAME_MAX];
	int fname_len = 0;
	char *fname = NULL;		
	memset(&log, 0, sizeof(struct ufilels));
#ifdef CONFIG_ANDROTRACE_PRINT
	memset(str_log, 0, 200);
#endif
	memset(pname,0, PNAME_MAX);

	if (IS_ERR_OR_NULL(fls_buf_hand))
        	return 0;

	/* header information (time) */
	__fill_header(TRACE_FILELS, &header);

        if (inode && inode->i_ino != 0) {
		if (!IS_ERR(inode->i_dentry.next)) {
			next = inode->i_dentry.next;
			dentry = list_entry(next, struct dentry, d_alias);
			if (!IS_ERR(dentry)) {
				if (dentry->d_name.len > 0) {
					fname = (char*)dentry->d_name.name;
					fname_len = strlen(fname);
					log.ext = get_file_type(fname);
				}
			}	
		}
		/* get create time from inode */
		if (inode->i_crtime.tv_sec != 0)
			time_to_tm(inode->i_crtime.tv_sec, 0, &tm);		
		log.c_year = tm.tm_year + 1900 - YEAR_BASE/*2000*/;
		log.c_month = tm.tm_mon + 1;
		log.c_day = tm.tm_mday;
		log.c_hour = tm.tm_hour;
		log.c_min = tm.tm_min;
		log.c_sec = tm.tm_sec;
		log.c_nsec = inode->i_crtime.tv_nsec;

		/* 150617 dtime -> header */
		/* delete time(current) */
		/*
		memset(&tm, 0, sizeof(struct tm));
		time_to_tm(ts_delete.tv_sec, 0, &tm);
		log.d_year = tm.tm_year + 1900 - YEAR_BASE;
		log.d_month = tm.tm_mon + 1;
		log.d_day = tm.tm_mday;
		log.d_hour = tm.tm_hour;
		log.d_min = tm.tm_min;
		log.d_sec = tm.tm_sec;
		log.d_nsec = ts_delete.tv_nsec;
		*/

		/* diff time(lifespan) */
		/*
		memset(&tm, 0, sizeof(struct tm));
		if (inode->i_crtime.tv_sec != 0) {
			ts_diff = timespec_sub(ts_delete, inode->i_crtime);
			time_to_tm(ts_diff.tv_sec, 0, &tm);
		}
		// time_to_tm(){ first day:january 1, 1970; result.year = 1970-1900 base year:70); }
		if (tm.tm_year == 70)
			log.di_year = 0;
		else
			log.di_year = tm.tm_year;
		log.di_month = tm.tm_mon;
		// time_to_tm(){ result.mday =days+1;} 
		log.di_day = tm.tm_mday-1;
		log.di_hour = tm.tm_hour;
		log.di_min = tm.tm_min;
		log.di_sec = tm.tm_sec;
		if (inode->i_crtime.tv_sec != 0)
			log.di_nsec = ts_diff.tv_nsec;
		else
			log.di_nsec = 0;
		*/

		/* other information, 140523 add ryoung */
		log.major_dev = MAJOR(inode->i_sb->s_dev);
		log.minor_dev = MINOR(inode->i_sb->s_dev);
		log.filesize = inode->i_size;
		pname_len = strlen(current->comm);
		log.pname_len = pname_len;
		strcpy(pname, current->comm);
		log_size = sizeof(struct ufilels);
		log_size += pname_len + 1 /*fname size*/ + fname_len;
		header.size = log_size;
		
		/*--->find buffer & save log data */
		if (!__get_filels_buffer_left(log_size))	
			return 0;
	
		/* save filels information */
		raw_spin_lock_irq(&fls_buf_lock);
		memcpy(fls_buf_hand->data + fls_buf_hand->offset, &log_size, 1 /*char*/);
		fls_buf_hand->offset += 1;
		memcpy(fls_buf_hand->data + fls_buf_hand->offset, &log, sizeof(struct ufilels));
		fls_buf_hand->offset += sizeof(struct ufilels);
		if (pname_len > 0) {
			memcpy(fls_buf_hand->data + fls_buf_hand->offset, pname, pname_len);
			fls_buf_hand->offset += pname_len;
		}
		memcpy(fls_buf_hand->data + fls_buf_hand->offset, &fname_len, 1);
		fls_buf_hand->offset += 1;
		if (fname_len > 0) {
			memcpy(fls_buf_hand->data + fls_buf_hand->offset, fname, fname_len);
			fls_buf_hand->offset += fname_len;
		}
		raw_spin_unlock_irq(&fls_buf_lock);
#ifdef CONFIG_ANDROTRACE_PRINT
/*		snprintf(str_log, strlen(str_log), "[%d/%d] %d%d%d%d%d%d %d\t %d%d%d%d%d%d %d\t %d%d%d%d%d%d %d\t %d\t %d %d\t %lld\t %s",
				  log_size, fls_buf_hand->offset,
				  log.c_year, log.c_month, log.c_day, log.c_hour, log.c_min, log.c_sec, log.c_nsec,
				  log.d_year, log.d_month, log.d_day, log.d_hour, log.d_min, log.d_sec, log.d_nsec,
				  log.di_year, log.di_month, log.di_day, log.di_hour, log.di_min, log.di_sec, log.di_nsec,
				  log.ext, log.major_dev, log.minor_dev,
				  log.filesize, fname);
*/

		//printk(KERN_INFO, "[udroid] %s\n", str_log);
#endif
		__check_filels_buffer_left(log_size);
       	}
	return true;
}
#endif

bool androtrace_add_sqlite(struct at_sqlite* sqlite)
{
	struct at_header header;

	if (IS_ERR_OR_NULL(io_buf_hand))
	        return 0;

	/* sqlite information */
	__fill_header(TRACE_SQLITE, &header);
	
	return true;
}
bool androtrace_add_suspend(unsigned char type)
{
	struct at_header header;
	int size;

	if (IS_ERR_OR_NULL(io_buf_hand))
        	return 0;

	__fill_header(TRACE_SUSPEND, &header);

	size = sizeof(struct at_header);
	/*--->find buffer & save log data */
	if (!__get_io_buffer_left(size+1))	
		return 0;

	//{ /* save suspend information to buffer */
	raw_spin_lock_irq(&io_buf_lock);
	memcpy(io_buf_hand->data + io_buf_hand->offset, &header, size);
	io_buf_hand->offset += size;
	memcpy(io_buf_hand->data + io_buf_hand->offset, &type, 1);
	io_buf_hand->offset += 1;
	raw_spin_unlock_irq(&io_buf_lock);
	// }

	__check_io_buffer_left(size +1);

	return true;
}
