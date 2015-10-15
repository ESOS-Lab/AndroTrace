package esos.andro.transporter;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Proxy;
import android.os.AsyncTask;
import android.util.Log;

import org.apache.http.HttpHost;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.conn.ClientConnectionManager;
import org.apache.http.conn.params.ConnRouteParams;
import org.apache.http.entity.ByteArrayEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.impl.conn.tsccm.ThreadSafeClientConnManager;
import org.apache.http.params.HttpParams;
import org.apache.http.util.EntityUtils;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

public class Http extends AsyncTask<Void, Void, int []>
{       
	private static boolean HTTP_DEBUG = true;
	private static boolean FILE_DELETE = true;
	
	private Context ctx;
	
	private static final int MEM_RECODE_SIZE = 112;

	
	private static final String HTTP_TAG = "HTTP_TAG";

	
	private static final boolean SUCCESS = true;
	private static final boolean FAIL = false;
	
	public static final int IO = 700;
	public static final int FILELS = 701;
	public static final int MEM = 703;
	public static final int MINFO = 704;
	
	public static final int TRANS_SUCCESS = 555;
	public static final int HTTP_FAIL = 554;
	public static final int IO_EXCEPTION = 553;

	
	protected void onPreExecute() {		
		super.onPreExecute();
	} 

    protected void onPostExecute(boolean page) {

    }

    void SetContext(Context ctx) {
        this.ctx = ctx;
    }
	@Override
	protected int[] doInBackground(Void... unused) {
		if( HTTP_DEBUG ) Log.i(HTTP_TAG, "       >>>" + Info.sDevNum);
		if(HTTP_DEBUG){ 
			Log.i(HTTP_TAG, "[doInB] ******* START ASYNC SERVICE ****** listpos:  " + Info.list_pos + " list_length : " + Info.file_list.length);
			Log.i(HTTP_TAG, "[doInB] ******* listpos,listlength,listmax : " + Info.list_pos + " / " + Info.file_list.length + " / " + Info.list_max);
			for( int i = 0 ; i< Info.file_list.length; i++){
				Log.i(HTTP_TAG, " File list[" + i + "] : " + Info.file_list[i].getName());
			}		
		}
		
		if(Info.list_max == 0){
			Log.i(HTTP_TAG, "[doInB] return doInBackground, listmax is : " + Info.list_max);
			Info.start_flag = true;
			return null;
		}


		for( int i = Info.list_pos; i< Info.list_max; i++){			

			String File_name = Info.file_list[i].getName();
			int ext_pos = File_name.lastIndexOf('.');
			String file_ext = File_name.substring(ext_pos+1);
			
			if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] File name and ext : " + File_name + " // " + file_ext);
			if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] File pos and max : " + Info.list_pos + " // " + Info.list_max);
			
			if( file_ext.equals("uio")  ){
				
				int trans_result = file_transfer(Info.file_list[i], IO);
				if( trans_result == TRANS_SUCCESS || trans_result == IO_EXCEPTION){
					// file transfer is success.
					// 1. Delete file.
					// 2. Plus "Info.list_pos"
					// 3. Check file list is end
					if( FILE_DELETE ){
						if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] File delete : " + Info.file_list[i].getName() + " " + Info.file_list[i].getPath() + " "
								+ Info.file_list[i].delete());
					}
					Info.list_pos++;
					if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] File_transfer is SUCCESS, pos/max : " + Info.list_pos + "/" + Info.list_max);
					
					if( check_file_end() ){
						Info.start_flag = true;			
						if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] file list is end, pos/max/start_flag : " + Info.list_pos + "/" + Info.list_max + "/" + Info.start_flag);
						try{
							Thread.sleep(1800000);}catch(Exception e){}
						return null;
					}	
				}else if ( trans_result == HTTP_FAIL ){ // it will return, for retry.. and doing nothing
					if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] Http [IO] transfer failed  :( ");
					//return null;
				}else{
					if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] Something wrong is happened. It never occur");
					//return null;
				}
			}else if( file_ext.equals("ufls") ){
				int trans_result = file_transfer(Info.file_list[i], FILELS);
				if( trans_result == TRANS_SUCCESS || trans_result == IO_EXCEPTION){
					// file transfer is success.
					// 1. Delete file.
					// 2. Plus "Info.list_pos"
					// 3. Check file list is end
					if( FILE_DELETE ){
						if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] File delete : " + Info.file_list[i].getName());
						Info.file_list[i].delete();							
					}
					
					Info.list_pos++;
					if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] File_transfer is SUCCESS, pos/max : " + Info.list_pos + "/" + Info.list_max);
					
					if( check_file_end() ){
						Info.start_flag = true;			
						if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] file list is end, pos/max/start_flag : " + Info.list_pos + "/" + Info.list_max + "/" + Info.start_flag);
						try{
							Thread.sleep(1800000);}catch(Exception e){}
						return null;
					}	
				}else if ( trans_result == HTTP_FAIL ){ // it will return, for retry.. and doing nothing
					if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] Http [IO] transfer failed  :( ");
					return null;
				}else{
					if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] Something wrong is happened. It never occur");
					return null;
				}	
			}else if( file_ext.equals("um")){
				int trans_result = file_transfer(Info.file_list[i], MEM);
				if( trans_result == TRANS_SUCCESS || trans_result == IO_EXCEPTION){
					// file transfer is success.
					// 1. Delete file.
					// 2. Plus "Info.list_pos"
					// 3. Check file list is end
					if( FILE_DELETE ){
						if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] File delete : " + Info.file_list[i].getName());
						Info.file_list[i].delete();							
					}
					
					Info.list_pos++;
					if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] File_transfer is SUCCESS, pos/max : " + Info.list_pos + "/" + Info.list_max);
					
					if( check_file_end() ){
						Info.start_flag = true;			
						if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] file list is end, pos/max/start_flag : " + Info.list_pos + "/" + Info.list_max + "/" + Info.start_flag);
						try{
							Thread.sleep(1800000);}catch(Exception e){}
						return null;
					}	
				}else if ( trans_result == HTTP_FAIL ){ // it will return, for retry.. and doing nothing
					if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] Http [IO] transfer failed  :( ");
					return null;
				}else{
					if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] Something wrong is happened. It never occur");
					return null;
				}	
			}else if( file_ext.equals("umd")){
				if( Info.file_list[i].length() > 1000 ){
				//if( Info.file_list[i].length() > 1 ){
					int trans_result = file_transfer(Info.file_list[i], MINFO);
					if( trans_result == TRANS_SUCCESS || trans_result == IO_EXCEPTION){
						// file transfer is success.
						// 1. Delete file.
						// 2. Plus "Info.list_pos"
						// 3. Check file list is end
						if( FILE_DELETE ){
							if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] File delete : " + Info.file_list[i].getName());
							Info.file_list[i].delete();							
						}
						
						Info.list_pos++;
						if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] File_transfer is SUCCESS, pos/max : " + Info.list_pos + "/" + Info.list_max);
						
						if( check_file_end() ){
							Info.start_flag = true;			
							if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] file list is end, pos/max/start_flag : " + Info.list_pos + "/" + Info.list_max + "/" + Info.start_flag);
							try{
								Thread.sleep(1800000);}catch(Exception e){}
							return null;
						}	
					}else if ( trans_result == HTTP_FAIL ){ // it will return, for retry.. and doing nothing
						if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] Http [IO] transfer failed  :( ");
						return null;
					}else{
						if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] Something wrong is happened. It never occur");
						return null;
					}	
				}else{
					if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] MD File skipping : " + Info.file_list[i]);
					Info.list_pos++;
					if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] list_pos[SKIP] : " + Info.list_pos + " MAX : " + Info.list_max);
					if( check_file_end() ){
						Info.start_flag = true;
						try{
							Thread.sleep(1800000);}catch(Exception e){}
						return null;
					}	
				}
			}else{
				// just skip this file
				if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] File skipping : " + Info.file_list[i]);
				Info.list_pos++;
				if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] list_pos[SKIP] : " + Info.list_pos + " MAX : " + Info.list_max);
				if( check_file_end() ){
					Info.start_flag = true;
					try{
						Thread.sleep(1800000);}catch(Exception e){}
					return null;
				}
			}			
		}
		return null;
	}
	
	// return true : no more files,
	public boolean check_file_end(){
		if( Info.list_pos == Info.list_max){ // file list is end
			if(HTTP_DEBUG) Log.i(HTTP_TAG, "[doInB] reset");
			Info.list_pos = 0;
			Info.list_max = 0;			
			return true;
		}else{
			return false;
		}		
	}

	// [ file_transfer(file,type) ]      
	//  1 : Transfer is success, whole file transfer
	//  0 : Http exception occur, file is still remaining
	// -1 : Transfer is failed, file problem or file is empty	
	public int file_transfer(File file, int type ){
 		
		byte[] content = new byte[Info.TRANS_BUFFER_SIZE];
		try{				
			DataInputStream dis = new DataInputStream(new FileInputStream(file));
			boolean trans_result = false;
			while( true ){	
				int read_size = dis.read(content);
				
				if( read_size > 0){
					if( HTTP_DEBUG )
						Log.i(HTTP_TAG, "[FT] dis.read(content)size : " + read_size);
					
					switch(type){
					case IO:
						trans_result = http_transfer(content, IO, read_size);
						break;
					case FILELS:
						trans_result = http_transfer(content, FILELS, read_size);
						break;
					case MEM:
						trans_result = http_transfer(content, MEM, read_size);
						break;
					case MINFO:
						trans_result = http_transfer(content, MINFO, read_size);
						break;
					}
								
					if (trans_result){ // success transfer
						Log.i(HTTP_TAG, "[FT] SUCCESS : file transfer ");
						dis.close();
						return TRANS_SUCCESS;
					}else{// http transfer is failed	
						Log.i(HTTP_TAG, "[FT] FAIL : file transfer ");
						dis.close();
						return HTTP_FAIL;
					}
					
				}else{
					dis.close();
					if( HTTP_DEBUG ) Log.i(HTTP_TAG, "[FT] FAIL : file.read_size <= 0");
					return IO_EXCEPTION;
				}
			}
		}catch (IOException e){
			if( HTTP_DEBUG )
				Log.i(HTTP_TAG, "IO exception occur in file_transfer(File file, int type )");

			e.printStackTrace();
			return IO_EXCEPTION;
		}		
		
	}
	
	
	// true : success
	// false : fail to transfer
	public boolean http_transfer( byte [] data, int type, int size){
		
		byte [] bTotalRC = new byte[ Info.bDevNum.length + Info.bModelName.length + size ];
		if(HTTP_DEBUG) Log.i(HTTP_TAG, ">>>>>>>>>>>: " + Info.bModelName);
		System.arraycopy(Info.bModelName, 0, bTotalRC, 0, Info.bModelName.length);

		if(HTTP_DEBUG) Log.i(HTTP_TAG, ">>>>>>>>>>>: " + Info.bDevNum);
		System.arraycopy(Info.bDevNum, 0, bTotalRC, Info.bModelName.length, Info.bDevNum.length);

		System.arraycopy(data, 0, bTotalRC, Info.bDevNum.length + Info.bModelName.length, size);
		
		int statusCode = -9030;	    		
		try {
			HttpClient http = getThreadSafeClient();
			HttpPost httpPost = null;
			
			switch(type){
			case IO :
				httpPost = new HttpPost(Info.IO_URL);
				if( HTTP_DEBUG ) Log.i(HTTP_TAG, "       [http_transfer] type IO");
				break;
				
			case FILELS :
				httpPost = new HttpPost(Info.IND_URL);
				if( HTTP_DEBUG ) Log.i(HTTP_TAG, "       [http_transfer] type FILELS");
				break;
				
			case MEM :
				httpPost = new HttpPost(Info.MEM_URL);
				if( HTTP_DEBUG ) Log.i(HTTP_TAG, "       [http_transfer] type MEM");
				break;
				
			case MINFO :
				httpPost = new HttpPost(Info.MINFO_URL);
				if( HTTP_DEBUG ) Log.i(HTTP_TAG, "       [http_transfer] type MINFO");
				break;
			}
			if( HTTP_DEBUG ) Log.i(HTTP_TAG, "       [http_transfer] HTTP START");
			httpPost.setEntity(new ByteArrayEntity(bTotalRC));
			HttpResponse responsePost = http.execute(httpPost);
			statusCode = responsePost.getStatusLine().getStatusCode();				
			if( HTTP_DEBUG ) Log.i(HTTP_TAG, "       [http_transfer] HTTP END  -  response code (" + statusCode + ") ");
			
			if( statusCode == HttpStatus.SC_OK || statusCode == HttpStatus.SC_NO_CONTENT){ // Maybe Server is down
				switch(type){
				case IO :
					Info.size_io += size;	
					break;					
				case FILELS :
					Info.size_filels += size;	
					break;					
				case MEM :
					Info.size_mem += size;	
					break;					
				case MINFO :
					Info.size_minfo += size;	
					break;
				}
				Log.i(HTTP_TAG, "       [http_transfer] HTTP SUCCESS, code : " + statusCode);
				return SUCCESS;	
			}else{
				if( HTTP_DEBUG )
					Log.i(HTTP_TAG, "       [http_transfer] HTTP failed, code : " + statusCode);
				return FAIL;
			}
		}catch(Exception e){
			if( HTTP_DEBUG )
				Log.i(HTTP_TAG, "       [http_transfer] HTTP Exception occur");
			return FAIL;					
		}
	}
	
	public static DefaultHttpClient getThreadSafeClient()  {

        DefaultHttpClient client = new DefaultHttpClient();
        ClientConnectionManager mgr = client.getConnectionManager();
        HttpParams params = client.getParams();
        client = new DefaultHttpClient(new ThreadSafeClientConnManager(params,

        mgr.getSchemeRegistry()), params);
        return client;
	}

	private static void get(final Context context, final String url) {
		new Thread() {
			@Override
			public void run() {
				HttpClient client = new DefaultHttpClient();
				HttpGet request = new HttpGet(url);
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
