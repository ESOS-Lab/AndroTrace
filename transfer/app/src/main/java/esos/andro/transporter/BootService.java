package esos.andro.transporter;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.SystemClock;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;


@SuppressLint("HandlerLeak")
public class BootService extends Service {
	public static final String ACTION_RESTART_PERSISTENTSERVICE = "ACTION.Restart.PersistentService";
	boolean end_flag = false;
	private static String TAG="UD";
	public static final boolean UDEBUG = false;
	
	InputStream is = null;
	BufferedReader br = null;
	
	public IBinder onBind(Intent intent){
		return null;	
	}
	
	public int onStartCommand(Intent intent, int flags,int startId)
	{
		super.onStartCommand(intent, flags, startId);
		end_flag=false;
		CheckThread thread=new CheckThread(this, mHandler);
		thread.start();
		return START_STICKY;
	}
	
    @Override
    public void onCreate() {
        super.onCreate();
        Log.d("PersistentService", "onCreate()");
        unregisterRestartAlarm();
    }
    
    @Override
	public void onDestroy(){
		super.onDestroy();
		end_flag=true;
        registerRestartAlarm();

	}
	
	class CheckThread extends Thread {
		BootService parenT;
		Handler mHandler;

		CheckThread(BootService parenT, Handler handler){
			this.parenT=parenT;
			this.mHandler=handler;
		}
		public void run(){
			for(int i=0; end_flag == false; i++){
				Message msg=new Message();
				msg.what=0;

				mHandler.sendMessage(msg);
				try{
					Thread.sleep(5000);}catch(Exception e){}
			}
		}
		
	}
	
	Handler mHandler = new Handler(){
		public void handleMessage(Message msg){
			if(msg.what==0){
				
				ArrayList<Integer> pid = new ArrayList<Integer>();
				ArrayList<String> p_name = new ArrayList<String>();
				
				ActivityManager atm = (ActivityManager)getSystemService(ACTIVITY_SERVICE);
				List<RunningAppProcessInfo> runningAppProcesses = atm.getRunningAppProcesses();
				for( RunningAppProcessInfo runningAppProcessInfo : runningAppProcesses){
					
					String tmpStr = runningAppProcessInfo.processName;
					if( tmpStr.equals("com.android.chrome") 
							|| tmpStr.equals("com.android.systemui")
							|| tmpStr.equals("com.android.phone")
							|| tmpStr.equals("com.android.providers.calendar")
							|| tmpStr.equals("com.kakao.talk")
							|| tmpStr.equals("com.google.process.location")
							|| tmpStr.equals("com.facebook.katana")
							|| tmpStr.equals("android.process.media")
							|| tmpStr.equals("com.android.camera2")
							|| tmpStr.equals("com.rovio.angrybirdsseasons")
							|| tmpStr.equals("com.gamelion.speedx3d:mcServiceProcess"))
					{
						pid.add(runningAppProcessInfo.pid);	
			        	if(UDEBUG){
			        		System.out.println("pid add " + runningAppProcessInfo.pid + " / " + runningAppProcessInfo.processName);
			        	}
					}
				}			
				
				try{
					run_cmd("dumpsys", "meminfo");
					
					for( int j =0; j < pid.size(); j++){
						int n_pid = pid.get(j);
						run_cmd("dumpsys", "meminfo", "-a", Integer.toString(n_pid));
					}								
				}catch(Exception e){
					Log.e("Process Manager", "Unable to execute dumpsys command");
				}
			}
		}
	};
	
	public static void run_cmd(String... command){
        try {
            ProcessBuilder pb = new ProcessBuilder(command);
            pb.redirectErrorStream(true);
            Process p = pb.start();
            InputStream is = p.getInputStream();
        	if(UDEBUG){
        		System.out.println("\nCommand " + Arrays.asList(command) + " reported");
        	}
            int b;
            while ((b = is.read()) >= 0){
            	if(UDEBUG){
            		System.out.write(b);
            	}
            }
            is.close();
            p.destroy();
        } catch (IOException e) {
            System.err.println("\nCommand " + Arrays.asList(command) + " reported " + e);
        }
	}
    /**
     * register Restart Alarm
     *
     */
	   private void registerRestartAlarm() {
	        Intent intent = new Intent(BootService.this, BootReceiver.class);
	        intent.setAction(BootReceiver.ACTION_RESTART_PERSISTENTSERVICE);
	        PendingIntent sender = PendingIntent.getBroadcast(BootService.this, 0, intent, 0);
	 
	        long firstTime = SystemClock.elapsedRealtime();
	        firstTime += 10000;
	 
	        AlarmManager am = (AlarmManager) getSystemService(ALARM_SERVICE);
	        am.setRepeating(AlarmManager.ELAPSED_REALTIME_WAKEUP, firstTime,10000, sender);
	    }
	 
	 
	    /**
	     * unregister Restart Alarm
	     */
	    private void unregisterRestartAlarm() {
	        Intent intent = new Intent(BootService.this, BootReceiver.class);
	        intent.setAction(BootReceiver.ACTION_RESTART_PERSISTENTSERVICE);
	        PendingIntent sender = PendingIntent.getBroadcast(BootService.this, 0, intent, 0);
	 
	        AlarmManager am = (AlarmManager) getSystemService(ALARM_SERVICE);
	        am.cancel(sender);
	    }


}

