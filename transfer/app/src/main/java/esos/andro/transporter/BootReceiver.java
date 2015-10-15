package esos.andro.transporter;


import android.annotation.SuppressLint;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import esos.andro.transporter.R;

public class BootReceiver extends BroadcastReceiver {
	public static final int US_NOTIBAR = 900;
    public static final String ACTION_RESTART_PERSISTENTSERVICE = "ACTION.Restart.PersistentService";
    
	private static final String UDROID = "UDROID";
	
	@SuppressWarnings("deprecation")
	@SuppressLint("ServiceCast")
	@Override
	public void onReceive(Context context, Intent intent) {
		// TODO Auto-generated method stub

		
		String action = intent.getAction();
		Log.i(UDROID, "Received action: " + action);
		
		if (action.equals("android.intent.action.BOOT_COMPLETED") || action.equals(ACTION_RESTART_PERSISTENTSERVICE))
        {
        	/* Notibar Action */
        	String ns = Context.NOTIFICATION_SERVICE;
        	NotificationManager mNotificationManager = (NotificationManager)context.getSystemService(ns);
        	int icon = R.drawable.ic_launcher;
        	CharSequence tickerText = "UDROID_S";
        	long when = System.currentTimeMillis();
        	Notification notification = new Notification(icon, tickerText, when);
        	notification.flags = notification.FLAG_NO_CLEAR;
        	CharSequence contentTitle = "UDROID_S";
        	CharSequence contentText = "����� ������ ������ ��ġ�ϼ���.";
        	Intent notificationIntent = new Intent(context, MainActivity.class);
        	PendingIntent contentIntent = PendingIntent.getActivity(context, 0, notificationIntent, 0);
        	notification.setLatestEventInfo(context, contentTitle, contentText, contentIntent);            	
        	mNotificationManager.notify( US_NOTIBAR, notification);
        	
	
			Intent service = new Intent(context, BootService.class);
			context.startService(service); 		
        }	
	

	}
}
