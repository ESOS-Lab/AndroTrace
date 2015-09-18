#include <linux/kernel.h>
#include <linux/string.h>
#include "androtrace_define.h"
#include "common.h"

char get_file_type(char *fname)
{
        int len = 0;	
        len = strlen(fname);

        /* SQL File */
        if ((fname[len-3] == '.' || fname[len-3] == '_') && fname[len-2] == 'd' && fname[len-1] == 'b')
                return SQLITE_DB_FILE;

        /* SQLite-temp File(db-journal) */
        else if ((fname[len-11] == '.' || fname[len-11] == '_') && fname[len-10] == 'd' && fname[len-9] == 'b' && fname[len-8] == '-' && fname[len-7] == 'j')
                return SQLITE_JOURNAL_FILE;
	/* -journal(db name is random) */
	else if (fname[len-8] == '-' && fname[len-7] == 'j' && fname[len-6] == 'o' && fname[len-5] == 'u' && fname[len-4] == 'r' 
		&& fname[len-3] == 'n' && fname[len-2] == 'a' && fname[len-1] == 'l')
		return SQLITE_JOURNAL_FILE;

        /* SQLite-temp File(db-wal) */
        else if ((fname[len-7] == '.' || fname[len-7] == '_') && fname[len-6] == 'd' && fname[len-5] == 'b' && fname[len-4] == '-' && fname[len-3] == 'w')
                return SQLITE_WAL_FILE;
	else if (fname[len-4] == '-' && fname[len-3] == 'w' && fname[len-2] == 'a' && fname[len-1] == 'l')
		return SQLITE_WAL_FILE;
	
	 /* SQLite-temp File(db-shm) */
        else if ((fname[len-7] == '.' || fname[len-7] == '-') && fname[len-6] == 'd' && fname[len-5] == 'b' && fname[len-4] == '-' && fname[len-3] == 's')
                return SQLITE_TEMP_FILE;
	else if (fname[len-4] == '-' && fname[len-3] == 's' && fname[len-2] == 'h' && fname[len-1] == 'm')
		return SQLITE_TEMP_FILE;

	/* SQLite-temp File(db-mjxxxx(8numbers or 9numbers) */
        else if ((fname[len-15] == '.' || fname[len-15] == '-') && fname[len-14] == 'd' && fname[len-13] == 'b' && fname[len-12] == '-' && fname[len-11] == 'm' && fname[len-10] == 'j')
                return SQLITE_TEMP_FILE;
	else if ((fname[len-16] == '.' || fname[len-16] == '-') && fname[len-15] == 'd' && fname[len-14] == 'b' && fname[len-13] == '-' && fname[len-12] == 'm' && fname[len-11] == 'j')
                return SQLITE_TEMP_FILE;
       
        /*  Multimedia File */
        else if((fname[len-4] == '.' || fname[len-4] == '-') && fname[len-3] == 'j' && fname[len-2] == 'p' && fname[len-1] == 'g')
                return MULTI_FILE;
        else if ((fname[len-4] == '.' || fname[len-4] == '-') && fname[len-3] == '3' && fname[len-2] == 'p' && fname[len-1] == 'g')
                return MULTI_FILE;
        else if ((fname[len-4] == '.' || fname[len-4] == '-') && fname[len-3] == 'm' && fname[len-2] == 'p' && fname[len-1] == '3')
                return MULTI_FILE;
	else if ((fname[len-4] == '.' || fname[len-4] == '-') && fname[len-3] == 'm' && fname[len-2] == 'p' && fname[len-1] == '4')  /* add 140912 */
                return MULTI_FILE;
	else if ((fname[len-4] == '.' || fname[len-4] == '-') && fname[len-3] == 'M' && fname[len-2] == 'O' && fname[len-1] == 'V')  /* add 140912 */
                return MULTI_FILE;
	else if ((fname[len-4] == '.' || fname[len-4] == '-') && fname[len-3] == 'a' && fname[len-2] == 'v' && fname[len-1] == 'i')  /* add 140912 */
                return MULTI_FILE;
        else if ((fname[len-6] == '.' || fname[len-6] == '-') && fname[len-5] == 't' && fname[len-4] == 'h' && fname[len-3] == 'u' && fname[len-2] == 'm' && fname[len-1] == 'b')
                return MULTI_FILE;
        else if ((fname[len-6] == '.' || fname[len-6] == '-') && fname[len-5] == 'l' && fname[len-4] == 'o' && fname[len-3] == 'c' && fname[len-2] == 'a' && fname[len-1] == 'l')
                return MULTI_FILE;
 
        /* Execute File */
        else if ((fname[len-3] == '.' || fname[len-3] == '-') && fname[len-2] == 's' && fname[len-1] == 'o')
                return EXEC_FILE;
        else if ((fname[len-4] == '.' || fname[len-4] == '-') && fname[len-3] == 'd' && fname[len-2] == 'e' && fname[len-1] == 'x')
                return EXEC_FILE;
        else if ((fname[len-4] == '.' || fname[len-4] == '-') && fname[len-3] == 'a' && fname[len-2] == 'p' && fname[len-1] == 'k')
                return EXEC_FILE;
        else if ((fname[len-5] == '.' || fname[len-5] == '-') && fname[len-4] == 'o'  && fname[len-3] == 'd' && fname[len-2] == 'e' && fname[len-1] == 'x')
                return EXEC_FILE;

        /* Resource File */
        else if ((fname[len-4] == '.' || fname[len-4] == '-') && fname[len-3] == 'd' && fname[len-2] == 'a' && fname[len-1] == 't')
                return RESOURCE_FILE;
        else if ((fname[len-4] == '.' || fname[len-4] == '-') && fname[len-3] == 'x' && fname[len-2] == 'm' && fname[len-1] == 'l')
                return RESOURCE_FILE;
        else if ((fname[len-6] == '.' || fname[len-6] == '-') && fname[len-5] == 'c' && fname[len-4] == 'a' && fname[len-3] == 'c' && fname[len-2] == 'h' && fname[len-1] == 'e')
                return RESOURCE_FILE;
        else if((fname[len-13] == '.' || fname[len-13] == '-') && fname[len-12] == 'l' && fname[len-11] == 'o' && fname[len-10] == 'c' && fname[len-9] == 'a' && fname[len-8] == 'l' && fname[len-7] == 's')
                return RESOURCE_FILE;
        else if((fname[len-11] == '.' || fname[len-11] == '-') && fname[len-10] == 't' && fname[len-9] == 'h' && fname[len-8] == 'u' &&
                fname[len-7] == 'm' && fname[len-6] == 'b' && fname[len-5] == 'd' && fname[len-4] == 'a' && fname[len-3] == 't' && fname[len-2] == 'a' && fname[len-1] == '3')
               return RESOURCE_FILE;

	/* Temp File(+140818 ryoung) */
	else if(fname[len-4] == '.' && fname[len-3] == 'b' && fname[len-2] =='a' && fname[len-1] =='k')
		return TEMP_FILE;
	else if(fname[len-4] == '.' && fname[len-3] == 't' && fname[len-2] == 'm' && fname[len-1] =='p')
		return TEMP_FILE;
	else if(fname[len-5] == '.' && fname[len-4] == 't' && fname[len-3] == 'e' && fname[len-2] == 'm' && fname[len-1] == 'p')
		return TEMP_FILE;

        /* 140425 add ryoung, no need to collect IO trace made by uiolog file */
        /* Udroid log file  ->to be except */
        else if(fname[len-4] == '.' && fname[len-3] == 'u' && fname[len-2] == 'i' && fname[len-1] =='o')
                return UDROID_IO_FILE;
        else if(fname[len-5] == '.' && fname[len-4] == 'u' && fname[len-3] == 'f' && fname[len-2] =='l' && fname[len-1] =='s')
                return UDROID_FILELS_FILE;
	else if(fname[len-3] == '.' && fname[len-2] == 'u' && fname[len-1] =='m')
		return UDROID_MEM_FILE;
	else if(fname[len-4] =='.' && fname[len-3] == 'u' && fname[len-2] == 'm' && fname[len-1] =='d')
		return UDROID_MEMDETAIL_FILE;

       /* Other File */
        else
                return ETC_FILE;
}

