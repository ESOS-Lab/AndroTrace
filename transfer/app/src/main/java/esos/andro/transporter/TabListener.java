package esos.andro.transporter;

import android.app.ActionBar;
import android.app.ActionBar.Tab;
import android.app.Fragment;
import android.app.FragmentTransaction;

import esos.andro.transporter.R;

public class TabListener implements ActionBar.TabListener {

	private Fragment fragment;
	
	// The contructor.
	public TabListener(Fragment fragment) {
		this.fragment = fragment;
	}

	// When a tab is tapped, the FragmentTransaction replaces
	// the content of our main layout with the specified fragment;
	// that's why we declared an id for the main layout.
	@Override
	public void onTabSelected(Tab tab, FragmentTransaction ft) {
		ft.replace(R.id.pager, fragment);
	}

	// When a tab is unselected, we have to hide it from the user's view. 
	@Override
	public void onTabUnselected(Tab tab, FragmentTransaction ft) {
		ft.remove(fragment);
	}

	// Nothing special here. Fragments already did the job.
	@Override
	public void onTabReselected(Tab tab, FragmentTransaction ft) {
		
	}
}
