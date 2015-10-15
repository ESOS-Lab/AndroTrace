package esos.andro.transporter;

import java.io.File;

public class Info {
	
	public static final String IO_PATH = "/data/androtrace/io";
	public static final String IND_PATH = "/data/androtrace/filels";
	public static final String MEM_PATH = "/data/androtrace/mem";
	public static final String MEMINFO_PATH = "/data/androtrace/mdetail";

	public static final String TEST_RESPONSE_URL = "http://166.104.115.206/androtrace/response.php";
	public static final String IO_URL = "http://166.104.115.206/androtrace/io.php";
	public static final String IND_URL = "http://166.104.115.206/androtrace/upd_filels.php";
	public static final String MEM_URL = "http://166.104.115.206/androtrace/upd_mem.php";
	public static final String MINFO_URL = "http://166.104.115.206/androtrace/upd_memdetail.php";

	public static long data_size = 0;

	public static boolean begin_flag = true;
	/* Get device IO : once!! :) */
	
	public static boolean start_flag = true; 
	/* For file list refreshing :( */
	
	
	public static int list_pos = 0;
	public static int list_max = 0;
	
	public static File[] file_list = null;
	
	public static String sDevNum = null;
	public static byte[] bDevNum = null;

	public static String sModelName = null;
	public static byte[] bModelName = null;
	
	public static long size_io = 0;
	public static long size_filels = 0;
	public static long size_mem = 0;
	public static long size_minfo = 0;

	public static int TRANS_BUFFER_SIZE = 1250000;

}
