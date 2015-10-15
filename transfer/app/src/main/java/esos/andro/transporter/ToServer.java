package esos.andro.transporter;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;

import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;
import android.net.ConnectivityManager;
import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.HttpHost;
import android.net.NetworkInfo;
import org.apache.http.conn.params.ConnRouteParams;
import android.net.Proxy;

import org.apache.http.client.methods.HttpPost;

import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import esos.andro.transporter.R;


public class ToServer extends Fragment {

	protected static final String TAG = "UD_T";
	private static final String HTTP_TAG = "HTTP_TAG";
	private static TextView tv_io = null;
	private static TextView tv_filels = null;
	private static TextView tv_mem = null;
	private static TextView tv_minfo = null;
	private static Button btn_refresh = null;
	public static Context ctx = null;

	private SharedPreferences prefs = null;
	private SharedPreferences.Editor editor = null;

	public ToServer() {

	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
							 Bundle savedInstanceState) {
		View rootView = inflater.inflate(R.layout.ud_t_layout, container, false);
		
		tv_io = (TextView)rootView.findViewById(R.id.io_txt);
		tv_filels = (TextView)rootView.findViewById(R.id.filels_txt);
		tv_mem = (TextView)rootView.findViewById(R.id.mem_txt);
		tv_minfo = (TextView)rootView.findViewById(R.id.minfo_txt);
		btn_refresh = (Button)rootView.findViewById(R.id.refresh_btn);
		
		tv_io.setText(Long.toString(Info.size_io / 1024));
		tv_filels.setText(Long.toString(Info.size_filels / 1024));
		tv_mem.setText(Long.toString(Info.size_mem / 1024));
		tv_minfo.setText(Long.toString(Info.size_minfo / 1024));
		
		btn_refresh.setOnClickListener(btnOnClickListener);

		ctx = getActivity().getApplicationContext();
		prefs = getActivity().getSharedPreferences("UDROID", 0);

		editor = prefs.edit();
		Info.sDevNum = getMD5Hash(getDeviceId(ctx));
		editor.putString("devname", Info.sDevNum);

		Log.i(HTTP_TAG, "Set Device Number " + Info.sDevNum);

		editor.commit();

		Info.sModelName = getModelName(ctx);
		editor.putString("modelname", Info.sModelName);

		Log.i(HTTP_TAG, "Set Model Name " + Info.sModelName);

		editor.commit();
		return rootView;
	}
	
	private static Button.OnClickListener btnOnClickListener = new Button.OnClickListener(){
		@Override
		public void onClick(View arg0) {
			// TODO Auto-generated method stub
				tv_io.setText(Long.toString(Info.size_io / 1024));
				tv_filels.setText(Long.toString(Info.size_filels / 1024));
				tv_mem.setText(Long.toString(Info.size_mem / 1024));
				tv_minfo.setText(Long.toString(Info.size_minfo / 1024));
				
				Toast.makeText(ctx, "Refresh!", Toast.LENGTH_SHORT).show();
		}
		};
		
   @Override
    public void onResume(){
        super.onResume();
		tv_io.setText(Long.toString(Info.size_io / 1024));
		tv_filels.setText(Long.toString(Info.size_filels / 1024));
		tv_mem.setText(Long.toString(Info.size_mem / 1024));
		tv_minfo.setText(Long.toString(Info.size_minfo / 1024));
        
          
    }
   
    @Override
    public void onPause(){
        super.onPause();  
        
    }

	public static String getMD5Hash(String s) {
		MessageDigest m = null;
		String hash = null;

		try {  
			m = MessageDigest.getInstance("MD5");
			m.update(s.getBytes(),0,s.length());  
			hash = new BigInteger(1, m.digest()).toString(16);
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();  
		}  
		return hash;  
	} 
	
	
	@SuppressLint("NewApi")
	public String getDeviceId(Context ctx)
	{ 
		TelephonyManager mgr = (TelephonyManager)ctx.getSystemService(Context.TELEPHONY_SERVICE);
		String dev_id = mgr.getDeviceId();
		final String dev_id_final = String.format("%1$"+32+ "s", dev_id);
		return dev_id_final;
	}

	@SuppressLint("NewApi")
	public String getModelName(Context ctx)
	{
		Build bd = new Build();
		String serialNum = bd.SERIAL;
		final String serialNum_final = String.format("%1$"+22+ "s", serialNum);

		return serialNum_final;
	}

	private static void get(final Context context, final String url) {
		new Thread() {
			@Override
			public void run() {
				HttpClient client = new DefaultHttpClient();
				HttpPost request = new HttpPost(url);
				setProxyIfNecessary(context, request);
				try {
					HttpResponse response = client.execute(request);
					Log.v("Test", "StatusCode: " + response.getStatusLine().getStatusCode() + ", Entity: " + EntityUtils.toString(response.getEntity()));
				} catch (Exception e) {
					// Oh, crash
				}
			}
		}.start();
	}

	private static void setProxyIfNecessary(Context context, HttpUriRequest request) {
		ConnectivityManager connectivity = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo networkInfo = connectivity == null ? null : connectivity.getActiveNetworkInfo();
		if (networkInfo == null || networkInfo.getType() == ConnectivityManager.TYPE_WIFI) {
			return;
		}

		String proxyHost = Proxy.getHost(context);
		if (proxyHost == null) {
			return;
		}

		int proxyPort = Proxy.getPort(context);
		if (proxyPort < 0) {
			return;
		}

		HttpHost proxy = new HttpHost(proxyHost, proxyPort);
		ConnRouteParams.setDefaultProxy(request.getParams(), proxy);
	}
}
