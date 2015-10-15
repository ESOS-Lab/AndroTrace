package esos.andro.transporter;

import android.content.BroadcastReceiver;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;
import java.util.Arrays;
import java.util.Comparator;

import esos.andro.transporter.R;

public class OnStorage extends Fragment{

	public static final String IO_PATH = "/data/androtrace/io";
	public static final String FILELS_PATH = "/data/androtrace/filels";
	public static final String MEM_PATH = "/data/androtrace/mem";
	public static final String MDETAIL_PATH = "/data/androtrace/mdetail";
	
	public static File[] io_list = null;
	public static File[] filels_list = null;
	public static File[] mem_list = null;
	public static File[] mdetail_list = null;
	 
	public static TextView tv_io = null;
	public static TextView tv_filels = null;
	public static TextView tv_mem = null;
	public static TextView tv_mdetail = null;
	
	public static TextView title = null;
	public static TextView title_io = null;
	public static TextView title_filels = null;
	public static TextView title_mem = null;
	public static TextView title_mdetail = null;
	

	
    BroadcastReceiver receiver;
    Intent intentMyService;

	public OnStorage() {

	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
							 Bundle savedInstanceState) {
		View rootView = inflater.inflate(R.layout.ud_s_layout, container, false);
        tv_io = (TextView)rootView.findViewById(R.id.io_txt);
        tv_filels = (TextView)rootView.findViewById(R.id.filels_txt);
        tv_mem = (TextView)rootView.findViewById(R.id.mem_txt);
        tv_mdetail = (TextView)rootView.findViewById(R.id.mdetail_txt);
        
        title = (TextView)rootView.findViewById(R.id.title);
        title_io = (TextView)rootView.findViewById(R.id.io_title);
        title_filels = (TextView)rootView.findViewById(R.id.filels_title);
        title_mem = (TextView)rootView.findViewById(R.id.mem_title);
        title_mdetail = (TextView)rootView.findViewById(R.id.mdetail_title);
                
        io_list = GetFileList(IO_PATH);
        filels_list = GetFileList(FILELS_PATH);
        mem_list = GetFileList(MEM_PATH);
        mdetail_list = GetFileList( MDETAIL_PATH );
        

    	
        
        long total_size = 0;
        long io_total =0;        
        for( int i = 0 ; i<io_list.length; i++){
        	int size =(int)( io_list[i].length() /1024);         	
        	if( i == 0 ){
        		tv_io.setText(io_list[i].getName());
        		tv_io.append("  :  " + Integer.toString(size));
        		tv_io.append(" KB");
        		tv_io.append("\n"); 
        	}else if( i == io_list.length-1){
            	tv_io.append(io_list[i].getName());
        		tv_io.append("  :  " + Integer.toString(size));
        		tv_io.append(" KB");   
        	}else{
            	tv_io.append(io_list[i].getName());
        		tv_io.append("  :  " + Integer.toString(size));
        		tv_io.append(" KB");
            	tv_io.append("\n");
        	}        	
        	total_size += (long)size;
        	io_total += (long)size;
        }        
        title_io.append(" ( ");
        title_io.append(Long.toString(io_total));
        title_io.append(" KB / " + Integer.toString(io_list.length) + " files )");
        
        io_total = 0;
     
        long filels_total =0;
        for( int i = 0 ; i<filels_list.length; i++){
        	int size =(int)( filels_list[i].length() /1024);         	
        	if( i == 0 ){
        		tv_filels.setText(filels_list[i].getName());
        		tv_filels.append("  :  " + Integer.toString(size));
        		tv_filels.append(" KB");
        		tv_filels.append("\n"); 
        	}else if( i == filels_list.length-1){
            	tv_filels.append(filels_list[i].getName());
        		tv_filels.append("  :  " + Integer.toString(size));
        		tv_filels.append(" KB");   
        	}else{
            	tv_filels.append(filels_list[i].getName());
        		tv_filels.append("  :  " + Integer.toString(size));
        		tv_filels.append(" KB");
            	tv_filels.append("\n");
        	}        	
        	total_size += (long)size;
        	filels_total += (long)size;
        }        
        title_filels.append(" ( ");
        title_filels.append(Long.toString(filels_total));
        title_filels.append(" KB / " + Integer.toString(filels_list.length) + " files )");
        filels_total = 0;
        
        long mem_total =0;
        for( int i = 0 ; i<mem_list.length; i++){
        	int size =(int)( mem_list[i].length() /1024);         	
        	if( i == 0 ){
        		tv_mem.setText(mem_list[i].getName());
        		tv_mem.append("  :  " + Integer.toString(size));
        		tv_mem.append(" KB");
        		tv_mem.append("\n"); 
        	}else if( i == mem_list.length-1){
            	tv_mem.append(mem_list[i].getName());
        		tv_mem.append("  :  " + Integer.toString(size));
        		tv_mem.append(" KB");   
        	}else{
            	tv_mem.append(mem_list[i].getName());
        		tv_mem.append("  :  " + Integer.toString(size));
        		tv_mem.append(" KB");
            	tv_mem.append("\n");
        	}        	
        	total_size += (long)size;
        	mem_total += (long)size;
        }        
        title_mem.append(" ( ");
        title_mem.append(Long.toString(mem_total));
        title_mem.append(" KB / " + Integer.toString(mem_list.length) + " files )");
        mem_total = 0;
        
        long mdetail_total =0;
        for( int i = 0 ; i<mdetail_list.length; i++){
        	int size =(int)( mdetail_list[i].length() /1024);         	
        	if( i == 0 ){
        		tv_mdetail.setText(mdetail_list[i].getName());
        		tv_mdetail.append("  :  " + Integer.toString(size));
        		tv_mdetail.append(" KB");
        		tv_mdetail.append("\n"); 
        	}else if( i == mdetail_list.length-1){
            	tv_mdetail.append(mdetail_list[i].getName());
        		tv_mdetail.append("  :  " + Integer.toString(size));
        		tv_mdetail.append(" KB");   
        	}else{
            	tv_mdetail.append(mdetail_list[i].getName());
        		tv_mdetail.append("  :  " + Integer.toString(size));
        		tv_mdetail.append(" KB");
            	tv_mdetail.append("\n");
        	}        	
        	total_size += (long)size;
        	mdetail_total += (long)size;
        }        
        title_mdetail.append(" ( ");
        title_mdetail.append(Long.toString(mdetail_total));
        title_mdetail.append(" KB / " + Integer.toString(mdetail_list.length) + " files )");
        mdetail_total = 0;
        
        total_size /= 1024;
        title.append(Long.toString(total_size));
        title.append(" MB");

		rootView.findViewById(R.id.btn).setOnClickListener(mClickListener);

		return rootView;
        
    }
    
    Button.OnClickListener mClickListener = new View.OnClickListener() {
		public void onClick(View v) {
			switch(v.getId()){
			
			// Play button�� ������ ����� ����
			case R.id.btn:
				
		        io_list = GetFileList(IO_PATH);
		        filels_list = GetFileList(FILELS_PATH);
		        mem_list = GetFileList(MEM_PATH);
		        mdetail_list = GetFileList( MDETAIL_PATH );
		        
		        long total_size = 0;
		        long io_total =0;        
		        for( int i = 0 ; i<io_list.length; i++){
		        	int size =(int)( io_list[i].length() /1024);         	
		        	if( i == 0 ){
		        		tv_io.setText(io_list[i].getName());
		        		tv_io.append("  :  " + Integer.toString(size));
		        		tv_io.append(" KB");
		        		tv_io.append("\n"); 
		        	}else if( i == io_list.length-1){
		            	tv_io.append(io_list[i].getName());
		        		tv_io.append("  :  " + Integer.toString(size));
		        		tv_io.append(" KB");   
		        	}else{
		            	tv_io.append(io_list[i].getName());
		        		tv_io.append("  :  " + Integer.toString(size));
		        		tv_io.append(" KB");
		            	tv_io.append("\n");
		        	}        	
		        	total_size += (long)size;
		        	io_total += (long)size;
		        }        
		        title_io.setText("/io ( ");
		        title_io.append(Long.toString(io_total));
		        title_io.append(" KB / " + Integer.toString(io_list.length) + " files )");
		        
		        io_total = 0;
		     
		        long filels_total =0;
		        for( int i = 0 ; i<filels_list.length; i++){
		        	int size =(int)( filels_list[i].length() /1024);         	
		        	if( i == 0 ){
		        		tv_filels.setText(filels_list[i].getName());
		        		tv_filels.append("  :  " + Integer.toString(size));
		        		tv_filels.append(" KB");
		        		tv_filels.append("\n"); 
		        	}else if( i == filels_list.length-1){
		            	tv_filels.append(filels_list[i].getName());
		        		tv_filels.append("  :  " + Integer.toString(size));
		        		tv_filels.append(" KB");   
		        	}else{
		            	tv_filels.append(filels_list[i].getName());
		        		tv_filels.append("  :  " + Integer.toString(size));
		        		tv_filels.append(" KB");
		            	tv_filels.append("\n");
		        	}        	
		        	total_size += (long)size;
		        	filels_total += (long)size;
		        }        
		        title_filels.setText("/filels ( ");
		        title_filels.append(Long.toString(filels_total));
		        title_filels.append(" KB / " + Integer.toString(filels_list.length) + " files )");
		        filels_total = 0;
		        
		        long mem_total =0;
		        for( int i = 0 ; i<mem_list.length; i++){
		        	int size =(int)( mem_list[i].length() /1024);         	
		        	if( i == 0 ){
		        		tv_mem.setText(mem_list[i].getName());
		        		tv_mem.append("  :  " + Integer.toString(size));
		        		tv_mem.append(" KB");
		        		tv_mem.append("\n"); 
		        	}else if( i == mem_list.length-1){
		            	tv_mem.append(mem_list[i].getName());
		        		tv_mem.append("  :  " + Integer.toString(size));
		        		tv_mem.append(" KB");   
		        	}else{
		            	tv_mem.append(mem_list[i].getName());
		        		tv_mem.append("  :  " + Integer.toString(size));
		        		tv_mem.append(" KB");
		            	tv_mem.append("\n");
		        	}        	
		        	total_size += (long)size;
		        	mem_total += (long)size;
		        }        
		        title_mem.setText("/mem ( ");
		        title_mem.append(Long.toString(mem_total));
		        title_mem.append(" KB / " + Integer.toString(mem_list.length) + " files )");
		        mem_total = 0;
		        
		        long mdetail_total =0;
		        for( int i = 0 ; i<mdetail_list.length; i++){
		        	int size =(int)( mdetail_list[i].length() /1024);         	
		        	if( i == 0 ){
		        		tv_mdetail.setText(mdetail_list[i].getName());
		        		tv_mdetail.append("  :  " + Integer.toString(size));
		        		tv_mdetail.append(" KB");
		        		tv_mdetail.append("\n"); 
		        	}else if( i == mdetail_list.length-1){
		            	tv_mdetail.append(mdetail_list[i].getName());
		        		tv_mdetail.append("  :  " + Integer.toString(size));
		        		tv_mdetail.append(" KB");   
		        	}else{
		            	tv_mdetail.append(mdetail_list[i].getName());
		        		tv_mdetail.append("  :  " + Integer.toString(size));
		        		tv_mdetail.append(" KB");
		            	tv_mdetail.append("\n");
		        	}        	
		        	total_size += (long)size;
		        	mdetail_total += (long)size;
		        }        
		        title_mdetail.setText("/mdetail ( ");
		        title_mdetail.append(Long.toString(mdetail_total));
		        title_mdetail.append(" KB / " + Integer.toString(mdetail_list.length) + " files )");
		        mdetail_total = 0;
		        
		        total_size /= 1024;
		        title.setText("/data/udroid : Total "+ Long.toString(total_size) + " MB");
				break;

			}
		}
    };
    
    
	public void OnDestroy() {
        
        // ���ù� �輼�� ���� ������ ����
        Log.d("MpMainActivity", "Service Destroy");
      //  unregisterReceiver(receiver);
        
        super.onDestroy();
    }
	
	public File[] GetFileList(String Path){
		File[] tmp_list = null;
		File f = new File(Path);
		
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

		}
		return tmp_list;
	}
	

}
