package com.gameparkstudio.wkcocos.lib;

import android.app.Activity;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.content.Intent;
import android.net.Uri;
import android.os.Handler;
import android.os.Messenger;
import android.os.SystemClock;
import android.provider.Settings;
import android.util.Log;

import com.android.vending.expansion.zipfile.ZipResourceFile;
import com.google.android.vending.expansion.downloader.Constants;
import com.google.android.vending.expansion.downloader.DownloadProgressInfo;
import com.google.android.vending.expansion.downloader.DownloaderClientMarshaller;
import com.google.android.vending.expansion.downloader.DownloaderServiceMarshaller;
import com.google.android.vending.expansion.downloader.Helpers;
import com.google.android.vending.expansion.downloader.IDownloaderClient;
import com.google.android.vending.expansion.downloader.IDownloaderService;
import com.google.android.vending.expansion.downloader.IStub;
import com.soomla.cocos2dx.common.ServiceManager;
import com.soomla.cocos2dx.store.StoreService;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.cocos2dx.lib.Cocos2dxHelper;

import java.io.DataInputStream;
import java.io.File;
import java.io.IOException;
import java.util.zip.CRC32;

public abstract class MainActivity extends Cocos2dxActivity {

    private final static String TAG = MainActivity.class.getSimpleName();

    private Cocos2dxWebViewHelper mWebViewHelper = null;

    private static Activity me = null;

    WkDownloaderInfo.XAPKFile mainXAPK = null;
    WkDownloaderInfo.XAPKFile patchXAPK = null;

    static Boolean mainXAPKValid = null;
    static Boolean patchXAPKValid = null;

    WkAd ad;

    protected static WkDownloaderInfo DLinfo = null;
	
    /**
     * Associate the download Activity with an implementation of WkDownloaderInfo providing
     * information by the client app required for this activity.
     * @param i DownloaderInfo implementation
     */
    static public void setInfo(WkDownloaderInfo i){
        DLinfo = i;
    };

    /**
     * Check the main APK Expansion files and determine if the files are present and match the required size.
     * Free applications should definitely consider doing this, as this allows the
     * application to be launched for the first time without having a network
     * connection present. Paid applications that use LVL should probably do at
     * least one LVL check that requires the network to be present, so this is
     * not as necessary.
     *
     * @return XAPKFile with its filepath set to proper path if it is present. it is set null if missing ( may be not needed ).
     */
    protected WkDownloaderInfo.XAPKFile expansionFilePath(boolean main) {
        try {
            if ( DLinfo == null ) {
                throw new NullPointerException(" ERROR : DLinfo is not set ! ");
            }
            else {
                WkDownloaderInfo.XAPKFile xf = main? DLinfo.getMainXAPK() : DLinfo.getPatchXAPK();
                if (xf != null) {
                    String expFileName = Helpers.getExpansionAPKFileName(this, true, xf.mFileVersion); //only filename
                    String expFilePath = Helpers.generateSaveFileName(this, expFileName); //with directory added
                    if ( (xf.mCheckEnabled && Helpers.doesFileExist(this, expFileName, xf.mFileSize, false)) //if check enabled we check for size
                            || (!xf.mCheckEnabled && new File(expFilePath).exists()) )//if check disabled we only check it exists
                    {
                        xf.setFilePath(expFilePath);
                    }else{
                        Log.e(TAG, "Missing " + (main?"Main":"Patch") + " XAPK at : " + expFilePath );
                        if (xf.mCheckEnabled) Log.e(TAG, "Expected Size = " + xf.mFileSize);
                    }
                }
                return xf; // we return null if no XAPK file needed ( null returned by app DLinfo implementation ).
            }
        } catch (NullPointerException e) {
            Log.e(TAG, "NullPointerException in WkDownloaderActivity.expansionFileDelivered()");
            e.printStackTrace();
        }
        return null; //we reach here only if error
    }

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        me = this;

        if (mWebViewHelper == null) {
            mWebViewHelper = new Cocos2dxWebViewHelper(mFrameLayout);
        }

        ad = new WkAd(this);
        mFrameLayout.addView(ad.getUI());

        WkJniHelper.getInstance().setActivity(this);

        //Needed for download XAPK
        WkDownloaderService.setPublicKey(getLVLKey());
        WkDownloaderService.setSALT(getSALT());
        //this will call Cocos2dxHelper.nativeSetMainXApkPath and Cocos2dxHelper.nativeSetPatchXApkPath
        //needs to be done after cocos2d-x app creation
        if ( mainXAPK == null ) mainXAPK = expansionFilePath(true);
        if ( patchXAPK == null ) patchXAPK = expansionFilePath(false);

        // we activate download only if one XAPK is needed and DL hasn't been validated yet.
        if ( ( mainXAPK != null && mainXAPKValid == null)
          || ( patchXAPK != null && patchXAPKValid == null)
        ) {
            //setup cocos values to access resources from XAPK
            if ( mainXAPK != null ) Cocos2dxHelper.nativeSetMainXApkPath(mainXAPK.getFilePath());
            if ( patchXAPK != null) Cocos2dxHelper.nativeSetPatchXApkPath(patchXAPK.getFilePath());

            // Fire the intent that launches the DL screen.
            Intent dl = new Intent(this, WkDownloaderActivity.class);
            dl.putExtra("mainXAPK",mainXAPK);
            dl.putExtra("patchXAPK",patchXAPK);
            this.startActivity(dl);
        }
    }

    @Override
    public Cocos2dxGLSurfaceView onCreateView() {

        Cocos2dxGLSurfaceView glSurfaceView = super.onCreateView();

        // initialize services
        final ServiceManager serviceManager = ServiceManager.getInstance();
        serviceManager.setActivity(this);
        serviceManager.setGlSurfaceView(glSurfaceView);
        serviceManager.registerService(StoreService.getInstance());

        return glSurfaceView;
    }

    @Override protected void onPause() {
        super.onPause();
        ad.pause();
    }

    @Override protected void onResume() {

        //TODO : JNI binding to remove this from here and make it doable in C++ testapp
        NotificationManager WKNM = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        WKNM.cancelAll();

        super.onResume();

        //if XAPK are not valid when we resume, we  just exit.
        if ( ( mainXAPK != null && mainXAPKValid != null && ! mainXAPKValid)
            || ( patchXAPK != null && patchXAPKValid != null && ! patchXAPKValid)
        ) {
            finish();
        }

        ad.resume();
    }

    /**
     * returns the version name of the main packaged app
     * needs to be implemented in the main package
     * @return string holding the main package app version Name
     */
    public abstract String getVersionName();
    //{
        //gradle
        //String version = BuildConfig.VERSION_NAME;
        //not gradle
        //String versionName = getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
        //return versionName;
    //}

    /**
     * returns the version code of the main packaged app
     * needs to be implemented in the main package
     * @return string holding the main package app version Code
     */
    public abstract int getVersionCode();
    //{
        //gradle // careful : current package !
        //int vcode = BuildConfig.VERSION_CODE;
        //not gradle
        //int versionCode = getPackageManager().getPackageInfo(getPackageName(), 0).versionCode;
        //return versionCode;
    //}

    /**
     * returns the Google play license key
     * needs to be implemented in the main package
     * @return string holding the license key
     */
    public abstract String getLVLKey();

    /**
     * returns some SALT to mix with the license key
     * needs to be implemented in the main package
     * @return byte[] holding the salt
     */
    public abstract byte[] getSALT();

    //TODO : test it ! maybe use new JniHelper for this...
    public static void openURL(String url) {
        Intent i = new Intent(Intent.ACTION_VIEW);
        i.setData(Uri.parse(url));
        me.startActivity(i);
    }

    public WkAd getAd()
    {
        return ad;
    }
}
