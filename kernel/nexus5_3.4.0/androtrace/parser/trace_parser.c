#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include "../androtrace_define.h"

static char input_file[100];
static char output_file[100];
static FILE *g_parse_file; 
static int g_idx = 0;

static int io_parser()
{
	FILE *read_file, *parse_file;
	/* data */
	struct at_header header;
	struct uio log;
	struct uio_ws log_ws;
	
	int log_size = 0;
	char pname[16];
	int fname_len = 0;
	char fname[100];
	char s_log[400];
	int idx = 0;
	char *str_rwbs = NULL;
	char *str_bt = NULL;
	char *str_fsync_sqlite = NULL;
	char *str_fsync = NULL;
	char *str_fdatasync = NULL;
	char *str_action = NULL;

	memset(pname, 0, 16);
	memset(fname, 0, 100);
	memset(s_log, 0, 400);

	read_file = fopen(input_file, "rb");
	if (read_file == NULL) {
		printf("failed to open original binary file\n");
		return -1;
	}
/*
	parse_file = fopen(output_file, "w+");
	if (parse_file == NULL) {
		printf("failed to open parse_file\n");
		return -1;
	}
*/
	while (!feof(read_file)) {
		idx += 1;
		g_idx += 1;
		memset(s_log, 0, 400);
		memset(pname, 0, 16);
		memset(fname, 0, 100);
		fread(&header, sizeof(struct at_header), 1, read_file);

		if (header.type == TRACE_IO){
			//{ read io trace 
			fread(&log, sizeof(struct uio), 1, read_file);
			fread(pname, log.pname_len, 1, read_file);
			fread(&fname_len, 1, 1, read_file);
			fread(fname, fname_len, 1, read_file);
			str_fsync_sqlite = "-";
			str_fsync = "-";
			str_fdatasync = "-";
			if(log.ws_info) {
				fread(&log_ws, sizeof(struct uio_ws), 1, read_file);
				if (log_ws.fsync_sqlite)
					str_fsync_sqlite = "s";
				if (log_ws.fsync)
					str_fsync = "f";
				if (log_ws.fdatasync)
					str_fdatasync = "d";

				//log_ws.fsync_sqlite == 1 ? str_fsync_sqlite = "s" : "-";
				//log_ws.fsync == 1 ? str_fsync = "f" : "-";
				//log_ws.fdatasync == 1 ? str_fdatasync = "d" : "-";
			}
			// }
			//log.action == 1 ? str_action = "I" : "C";
			if(log.action)
				str_action = "I";
			else
				str_action = "C";
			
			switch (log.rwbs) {
			case READ_NONE_MODE :
				str_rwbs = "R";
				break;
			case READ_SYNC_MODE :
 				str_rwbs = "RS";
				break;
			case WRITE_NONE_MODE :
				str_rwbs = "W";
				break;
			case WRITE_SYNC_MODE :
				str_rwbs = "WS";
				break;
			default :
				break;
			}
			switch (log.block_type) { 
			case BLOCK_META : 
				str_bt = "M";
				break;
			case BLOCK_JOURNAL : 
				str_bt = "J";
				break;
			case BLOCK_DATA :
				str_bt = "D";
				break;
			case BLOCK_ANON :
				str_bt = "A"; 
				break;
			default : 
				str_bt = "N";
			}

			if(log.rwbs == WRITE_SYNC_MODE){
				sprintf(s_log, "[%04d]\t%1s\t%02d\t%02d\t%02d\t%03d\t%2s\t%1s%1s%1s\t%1s\t%10d\t%6d\t%05d\t%05d\t%-16s%s\n", 
					g_idx, str_action, header.hour,header.min, header.sec, (header.nsec)/1000000,
					str_rwbs, str_fsync_sqlite, str_fsync, str_fdatasync, 
					str_bt, log.sector_nb, log.block_len, 
					log_ws.t_id, log_ws.fs_id,
					pname, fname);
			}

/*
			if(log.ws_info)
				sprintf(s_log, "[%04d]  %02d/%02d/%02d %02d:%02d:%02d %09d  %02d  %2s  %1s %1s %1s  %3d %2d  %1s  %10d %6d  %1s jt:%1d  tid:%05d  fid:%05d  %-16s  %s\n", 
					idx, header.year, header.month, header.day, header.hour, header.min, header.sec, header.nsec,
					log.file_type, str_rwbs, str_fsync_sqlite, str_fsync, str_fdatasync, 
					log.major_dev, log.minor_dev, str_bt, log.sector_nb, log.block_len, 
					str_action, log_ws.journal_type, log_ws.t_id, log_ws.fs_id,
					pname, fname);
			else
				sprintf(s_log, "[%04d]  %02d/%02d/%02d %02d:%02d:%02d %09d  %02d  %2s  %1s %1s %1s  %3d %2d  %1s  %10d %6d  %-16s  %s\n", 
					idx, header.year, header.month, header.day, header.hour, header.min, header.sec, header.nsec,
					log.file_type, str_rwbs, str_fsync_sqlite, str_fsync, str_fdatasync, 
					log.major_dev, log.minor_dev, str_bt, log.sector_nb, log.block_len, 
					pname, fname);
*/
		} else if (header.type == TRACE_SUSPEND) {
			unsigned char mesg;
			char *str_mesg = NULL;
			fread(&mesg, sizeof(unsigned char), 1, read_file);

			switch (mesg) {
			case TRACE_PM_SUSPEND_ON :
				str_mesg = "PM_SUPEND_ON";
				break;
			case TRACE_PM_SUSPEND_MEM :
				str_mesg = "PM_SUPEND_MEM";
				break;
			case TRACE_SUSPEND_ROUTINE_START :
				str_mesg = "SUSPEND_ROUTINE_START";
				break;
			case TRACE_SUSPEND_ROUTINE_END :
				str_mesg = "SUPEND_ROUTINE_END";
				break;
			case TRACE_SYNCING_FS :
				str_mesg = "SYNCING_FS";
				break;
			case TRACE_SYNCING_DONE :
				str_mesg = "SYNCING_DONE";
				break;
			case TRACE_SUSPEND_FAILED :
				str_mesg = "SUSPEND_FAILED";
				break;
			case TRACE_SUSPEND_ENTER :
				str_mesg = "SUSPEND_ENTER";
				break;
			case TRACE_WAKEUP :
				str_mesg = "WAKEUP";
				break;
			default: 
				break;
			}
			sprintf(s_log, "[%04d]  %02d/%02d/%02d %02d:%02d:%02d %09d %s\n", 
				idx, header.year, header.month, header.day, header.hour, header.min, header.sec, header.nsec,
				str_mesg);
		}	
	
		//fwrite(s_log, 1, strlen(s_log), parse_file);
		if(log.rwbs  == WRITE_NONE_MODE || log.rwbs == WRITE_SYNC_MODE){
			fwrite(s_log, 1, strlen(s_log), g_parse_file);
		}
	}

	fclose(read_file);
//	fclose(parse_file);

	return 0;
}

static int filels_parser()
{
	FILE *read_file, *parse_file;
	struct ufilels log;
	int log_size = 0;
	char pname[16];
	int fname_len;
	char fname[100];
	char s_log[200];
	int idx =1;

	memset(pname, 0, 16);
	memset(fname, 0, 100);
	memset(s_log, 0, 100);

	read_file = fopen(input_file, "rb");
	if (read_file == NULL) {
		printf("failed to open original binary file\n");
		return -1;
	}
	
	parse_file = fopen(output_file, "aw");
	if (parse_file == NULL) {
		printf("failed to open parse_file\n");
		return -1;
	}
/*	while (!feof(read_file)) {
		memset(s_log, 0, 200);
		memset(pname, 0, 16);
		memset(fname, 0 ,100);
		fread(&log_size, 1, 1,read_file);
		fread(&log, sizeof(struct ufilels), 1, read_file);
		fread(pname, log.pname_len, 1, read_file);
		fread(&fname_len, 1, 1, read_file);
		fread(fname, fname_len, 1, read_file);		
		sprintf(s_log, "[%d] %d%d%d%d%d%d %d\t %d%d%d%d%d%d %d\t %d%d%d%d%d%d %d\t %d\t %lld\t %s\t %s\n", 
			idx, log.c_year, log.c_month, log.c_day, log.c_hour, log.c_min, log.c_sec, log.c_nsec,
			log.d_year, log.d_month, log.d_day, log.d_hour, log.d_min, log.d_sec, log.d_nsec,
			log.di_year, log.di_month, log.di_day, log.di_hour, log.di_min, log.di_sec, log.di_nsec,
			log.ext, log.filesize, pname, fname);
		printf("%s\n", s_log);	
		idx +=1;
	}
*/

	fclose(read_file);
	fclose(parse_file);

	return 0;
}

int main(int argc, char **argv)
{
	int slen = 0;

	if(argc < 2){
		printf("usage: ./udroid_parser <input file>\n");
		exit(0);
	}

	g_parse_file = fopen("./all_io_parse.dat", "a");

	if (strstr(argv[1], "all")){
		DIR *d;
		struct dirent *dir;
		d = opendir("./data");
		if (d)
		{
			while ((dir = readdir(d)) != NULL)
			{
				//printf("%s\n", dir->d_name);

				sprintf(input_file, "./data/%s", dir->d_name);
				sprintf(output_file, "./data/%s_parse.txt", dir->d_name);
				slen = strlen(input_file);
				printf("path: %s\n", input_file);

				if (input_file[slen-4] == '.' && input_file[slen-3] == 'u' && input_file[slen-2] =='i' && input_file[slen-1] == 'o')
					io_parser();
				else if (input_file[slen-5] == '.' && input_file[slen-4] == 'u' && input_file[slen-3] == 'f' && input_file[slen-2] == 'l' &&  input_file[slen-1] == 's')
					filels_parser();

			}
			closedir(d);
		}

	} else{
		sprintf(input_file, "./%s", argv[1]);
		sprintf(output_file, "./%s_parse.txt", argv[1]);
		slen = strlen(input_file);
		printf("path: %s\n", input_file);
			
		if (input_file[slen-4] == '.' && input_file[slen-3] == 'u' && input_file[slen-2] =='i' && input_file[slen-1] == 'o')
			io_parser();
		else if (input_file[slen-5] == '.' && input_file[slen-4] == 'u' && input_file[slen-3] == 'f' && input_file[slen-2] == 'l' &&  input_file[slen-1] == 's')
			filels_parser();
	}
	fclose(g_parse_file);
	return true;
}

