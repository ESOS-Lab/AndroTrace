package esos.andro.transporter;

import android.annotation.SuppressLint;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;

import java.io.File;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Enumeration;

import esos.andro.transporter.R;

public class WifiReceiver extends BroadcastReceiver {

	private static final int UT_NOTIBAR = 999;

	private static int batteryPercent;

	public static final String ACTION_RESTART_PERSISTENTSERVICE = "ACTION.Restart.PersistentService";
	
	private static boolean UT_DEBUG = false;
	private static boolean UT_HTTP_DEBUG = false;
	private static final String HTTP_TAG = "HTTP_TAG";

	public static File[] io_list = null;
	public static File[] ind_list = null;
	public static File[] mem_list = null;
	public static File[] minfo_list = null;

	private SharedPreferences prefs = null;
	private SharedPreferences.Editor editor = null;
	@SuppressWarnings("deprecation")
	@SuppressLint("ServiceCast")
	@Override
	public void onReceive(Context context, Intent intent) {
		// TODO Auto-generated method stub

		prefs = context.getSharedPreferences("UDROID", context.MODE_PRIVATE);
		editor = prefs.edit();
		
		
		Info.sDevNum = prefs.getString("devname", "JWGOM");
		Info.sModelName = prefs.getString("modelname", "JWGOM");

		batteryPercent = prefs.getInt("battery_percentage", 0);

		Log.i(HTTP_TAG, "WifiRecevier DevID: " + Info.sDevNum);
		Log.i(HTTP_TAG, "WifiRecevier ModelName: " + Info.sModelName);

		if( Info.begin_flag ){

        	Info.bDevNum = Info.sDevNum.getBytes();
			Info.bModelName = Info.sModelName.getBytes();
        	Info.begin_flag = false;
		}

		if(batteryPercent > 10) {
			boolean isWifiConn = false;
			
			ConnectivityManager cm = (ConnectivityManager)context.getSystemService(Context.CONNECTIVITY_SERVICE);
			NetworkInfo ni = cm.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
			isWifiConn = ni.isConnected();
			if( UT_HTTP_DEBUG )
			{
				getWifiApIpAddress();
			}

	        if (isWifiConn){

	            // wifi is enabled	            	
            	if( UT_HTTP_DEBUG ){
            		Log.i(HTTP_TAG, "Wifi con");
            		Log.i(HTTP_TAG, "Start flag : " + Info.start_flag);
            	}
            	
            	if(Info.start_flag){ // start_flag == 1
            		io_list = GetFileList(Info.IO_PATH);
            		ind_list = GetFileList(Info.IND_PATH);
            		mem_list = GetFileList(Info.MEM_PATH);
            		minfo_list = GetFileList(Info.MEMINFO_PATH);
            		
            		int file_length = 0;
            		if( io_list.length > 1 ){
            			file_length += (io_list.length -1);            			
            		}
            		if( ind_list.length > 1 ){
            			file_length += (ind_list.length -1);            			
            		}
            		if( mem_list.length > 1 ){
            			file_length += (mem_list.length -1);            			
            		}
            		if( minfo_list.length > 1 ){
            			file_length += (minfo_list.length -1);            			
            		}
            		
            		Info.file_list = new File[file_length];
            		Info.list_max = file_length;
            		Info.list_pos = 0;
            		
            		int pos = 0;
            		for(int i = 0; i< io_list.length -1; i++){
            			Info.file_list[pos] = io_list[i];
            			pos++;
            			         
            		}
            		for(int i = 0; i< ind_list.length -1; i++){
            			Info.file_list[pos] = ind_list[i];
            			pos++;
  
            		}
            		for(int i = 0; i< mem_list.length -1; i++){
            			Info.file_list[pos] = mem_list[i];
            			pos++;
  
            		}
            		for(int i = 0; i< minfo_list.length -1; i++){
            			Info.file_list[pos] = minfo_list[i];
            			pos++;
  
            		}
            		pos = 0;
            		if(UT_DEBUG){
            			Log.i(HTTP_TAG, "[UDEBUG] Total file length : " + Info.file_list.length);
            			for( int i = 0 ; i < Info.file_list.length; i++){
            				Log.i(HTTP_TAG, "[UDEBUG] Send File_list [" + i + "] : " + Info.file_list[i].getName());
            			}
            		}
        			
            	}
            	
            	/* Notibar Action */
            	String ns = Context.NOTIFICATION_SERVICE;
            	NotificationManager mNotificationManager = (NotificationManager)context.getSystemService(ns);
            	int icon = R.drawable.ic_launcher;
            	CharSequence tickerText = "Androtrace";
            	long when = System.currentTimeMillis();
            	Notification notification = new Notification(icon, tickerText, when);
            	notification.flags = notification.FLAG_NO_CLEAR;
            	CharSequence contentTitle = "Androtrace";
            	CharSequence contentText = "Collect and Transfer I/O traces.";
            	Intent notificationIntent = new Intent(context, MainActivity.class);
            	PendingIntent contentIntent = PendingIntent.getActivity(context, 0, notificationIntent, 0);
            	notification.setLatestEventInfo(context, contentTitle, contentText, contentIntent);            	
            	mNotificationManager.notify(UT_NOTIBAR, notification);
            	
            	
    			Http myAsyncTask = new Http();
    			myAsyncTask.SetContext(context);
    		    myAsyncTask.execute();
	            	
	        }else{
	        }
        }
	}
	
	public void init_flag(){
		if(UT_DEBUG){
			Log.i(HTTP_TAG, "[UDEBUG] Flag initialization ");
		}	
		
		  
		
	}
	
	public File[] GetFileList(String Path){
		File[] tmp_list = null;
		File f = new File(Path);
		Log.i(HTTP_TAG, "GetFileList : file path [" + Path + "]");
	
		if( f.exists() && f.isDirectory() ){ 
			tmp_list = f.listFiles();
			if( tmp_list == null ){
				return null;
			}
			/* File sorting : Modified date */
			if(tmp_list.length > 1){							
				/* Sort file List by Modified date*/ 
				Arrays.sort(tmp_list, new Comparator<File>() {
					public int compare(File f1, File f2) {
						return Long.valueOf(f1.lastModified()).compareTo(f2.lastModified());
					}
				});
			}	
			if( UT_DEBUG ){
				Log.i(HTTP_TAG, "Load : file list path [" + Path + "]");
				for( int i = 0; i < tmp_list.length; i++)
					Log.i(HTTP_TAG, "[" + i + "] : " + tmp_list[i].getName());
			}
		}
		return tmp_list;
	}
	
	public String getWifiApIpAddress() {
	    try {
	        for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en
	                .hasMoreElements();) {
	            NetworkInterface intf = en.nextElement();
	            if (intf.getName().contains("wlan")) {
	                for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr
	                        .hasMoreElements();) {
	                    InetAddress inetAddress = enumIpAddr.nextElement();
	                    if (!inetAddress.isLoopbackAddress()
	                            && (inetAddress.getAddress().length == 4)) {
	                        Log.d(HTTP_TAG, inetAddress.getHostAddress());
	                        return inetAddress.getHostAddress();
	                    }
	                }
	            }
	        }
	    } catch (SocketException ex) {
	        Log.e(HTTP_TAG, ex.toString());
	    }
	    return null;
	}


}