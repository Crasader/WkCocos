package com.gameparkstudio.wkcocos.lib;
/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import android.app.Activity;
import android.app.PendingIntent;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.Messenger;
import android.os.SystemClock;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.android.vending.expansion.zipfile.ZipResourceFile;
import com.google.android.vending.expansion.downloader.Constants;
import com.google.android.vending.expansion.downloader.DownloadProgressInfo;
import com.google.android.vending.expansion.downloader.DownloaderClientMarshaller;
import com.google.android.vending.expansion.downloader.DownloaderServiceMarshaller;
import com.google.android.vending.expansion.downloader.Helpers;
import com.google.android.vending.expansion.downloader.IDownloaderClient;
import com.google.android.vending.expansion.downloader.IDownloaderService;
import com.google.android.vending.expansion.downloader.IStub;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.zip.CRC32;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;

/**
 * This is sample code for a project built against the downloader library. It
 * implements the IDownloaderClient that the client marshaler will talk to as
 * messages are delivered from the DownloaderService.
 */
public class WkDownloaderActivity extends Activity implements IDownloaderClient {
    private static final String TAG = "WkDownloaderActivity";
    private ProgressBar mPB;

    private TextView mStatusText;
    private TextView mProgressFraction;
    private TextView mProgressPercent;
    private TextView mAverageSpeed;
    private TextView mTimeRemaining;

    private View mDashboard;
    private View mCellMessage;

    private Button mPauseButton;
    private Button mWiFiSettingsButton;

    private boolean mStatePaused;
    private int mState;

    private IDownloaderService mRemoteService;

    private IStub mDownloaderClientStub;

    private void setState(int newState) {
        if (mState != newState) {
            mState = newState;
            mStatusText.setText(Helpers.getDownloaderStringResourceIDFromState(newState));
        }
    }

    private void setButtonPausedState(boolean paused) {
        mStatePaused = paused;
        int stringResourceID = paused ? R.string.text_button_resume :
                R.string.text_button_pause;
        mPauseButton.setText(stringResourceID);
    }

    private static WkDownloaderInfo DLinfo = null;

    /**
     * Associate the download Activity with an implementation of WkDownloaderInfo providing
     * information by the client app required for this activity.
     * @param i DownloaderInfo implementation
     */
    static public void setInfo(WkDownloaderInfo i){
        DLinfo = i;
    };

    /**
     * Calculating a moving average for the validation speed so we don't get
     * jumpy calculations for time etc.
     */
    static private final float SMOOTHING_FACTOR = 0.005f;

    /**
     * Used by the async task
     */
    private boolean mCancelValidation;

    /**
     * Go through each of the Expansion APK files and open each as a zip file.
     * Calculate the CRC for each file and return false if any fail to match.
     *
     * @return true if XAPKZipFile is successful
     */
    void validateXAPKZipFiles() {
        AsyncTask<Object, DownloadProgressInfo, Boolean> validationTask = new AsyncTask<Object, DownloadProgressInfo, Boolean>() {

            @Override
            protected void onPreExecute() {
                mDashboard.setVisibility(View.VISIBLE);
                mCellMessage.setVisibility(View.GONE);
                mStatusText.setText(R.string.text_verifying_download);
                mPauseButton.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        mCancelValidation = true;
                    }
                });
                mPauseButton.setText(R.string.text_button_cancel_verify);
                super.onPreExecute();
            }

            @Override
            protected Boolean doInBackground(Object... params) {

                boolean mainXAPKextracted = false;
                boolean patchXAPKextracted = false;

                WkDownloaderInfo.XAPKFile mainXAPK = expansionFilePath(true);
                if ( mainXAPK != null ) {
                    if (mainXAPK.getFilePath() != "") {
                        mainXAPKextracted = bgExtract(mainXAPK.getFilePath(), Environment.getDataDirectory().getAbsolutePath(), mainXAPK.mCheckEnabled);
                    } // else extraction has failed
                }
                else
                {  //no expansion file : no extract same as extract empty patch file
                    mainXAPKextracted = true;
                }

                WkDownloaderInfo.XAPKFile patchXAPK = expansionFilePath(false);
                if ( patchXAPK != null ) {
                    if (patchXAPK.getFilePath() != "") {
                        patchXAPKextracted = bgExtract(patchXAPK.getFilePath(), Environment.getDataDirectory().getAbsolutePath(), patchXAPK.mCheckEnabled);
                    } // else extraction has failed
                }
                else
                {  //no expansion file : no extract same as extract empty patch file
                    patchXAPKextracted = true;
                }

                return mainXAPKextracted && patchXAPKextracted;
            }

            protected Boolean bgExtract(String fileName, String unzipLocation, boolean checkCRC)
            {
                ZipResourceFile zrf;
                byte[] buf = new byte[1024 * 256];
                try {
                    //To access Zip files directly
                    zrf = new ZipResourceFile(fileName);
                    ZipResourceFile.ZipEntryRO[] entries = zrf.getAllEntries();
                    /**
                     * First calculate the total compressed length
                     */
                    long totalCompressedLength = 0;
                    for (ZipResourceFile.ZipEntryRO entry : entries) {
                        totalCompressedLength += entry.mCompressedLength;
                    }
                    float averageVerifySpeed = 0;
                    long totalBytesRemaining = totalCompressedLength;
                    long timeRemaining;
                    /**
                     * Then calculate a CRC for every file in the Zip file,
                     * comparing it to what is stored in the Zip directory.
                     * Note that for compressed Zip files we must extract
                     * the contents to do this comparison.
                     */
                    for (ZipResourceFile.ZipEntryRO entry : entries) {
                        if (-1 != entry.mCRC32) {
                            long length = entry.mUncompressedLength;
                            CRC32 crc = new CRC32();
                            DataInputStream dis = null;
                            ZipInputStream zis = null;
                            try {
                                Log.e(Constants.TAG, "uncompressing " + entry.mFileName + "...");

                                //creating parent directories if needed
                                File f = new File(unzipLocation + entry.mFileName);
                                if ( !f.isDirectory()){
                                    f.mkdirs();
                                }

                                // write the inputStream to a FileOutputStream
                                FileOutputStream fos = new FileOutputStream(f);

                                dis = new DataInputStream(zrf.getInputStream(entry.mFileName));
                                long startTime = SystemClock.uptimeMillis();
                                while (length > 0) {
                                    int seek = (int) (length > buf.length ? buf.length : length);
                                    dis.readFully(buf, 0, seek);
                                    //write to file
                                    fos.write(buf, 0, seek);
                                    if ( checkCRC ) {
                                        //check crc
                                        crc.update(buf, 0, seek);
                                    }
                                    length -= seek;
                                    long currentTime = SystemClock.uptimeMillis();
                                    long timePassed = currentTime - startTime;
                                    if (timePassed > 0) {
                                        float currentSpeedSample = (float) seek / (float) timePassed;
                                        if (0 != averageVerifySpeed) {
                                            averageVerifySpeed = SMOOTHING_FACTOR * currentSpeedSample + (1 - SMOOTHING_FACTOR) * averageVerifySpeed;
                                        } else {
                                            averageVerifySpeed = currentSpeedSample;
                                        }
                                        totalBytesRemaining -= seek;
                                        timeRemaining = (long) (totalBytesRemaining / averageVerifySpeed);
                                        this.publishProgress(
                                                new DownloadProgressInfo(
                                                        totalCompressedLength,
                                                        totalCompressedLength - totalBytesRemaining,
                                                        timeRemaining,
                                                        averageVerifySpeed)
                                        );
                                    }
                                    startTime = currentTime;
                                    if (mCancelValidation) {
                                        return true;
                                    }
                                }
                                if (checkCRC && crc.getValue() != entry.mCRC32) {
                                    Log.e(Constants.TAG, "CRC does not match for entry: " + entry.mFileName);
                                    Log.e(Constants.TAG, "In file: " + entry.getZipFileName());
                                    return false;
                                }
                            } finally {
                                if (null != dis) {
                                    dis.close();
                                }
                            }
                        }
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                    return false;
                }
                return true;
            }

            @Override
            protected void onProgressUpdate(DownloadProgressInfo... values) {
                onDownloadProgress(values[0]);
                super.onProgressUpdate(values);
            }

            @Override
            protected void onPostExecute(Boolean result) {
                if (result) {
                    mDashboard.setVisibility(View.VISIBLE);
                    mCellMessage.setVisibility(View.GONE);
                    mStatusText.setText(R.string.text_validation_complete);
                    mPauseButton.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            finish();
                        }
                    });
                    mPauseButton.setText(android.R.string.ok);
                } else {
                    mDashboard.setVisibility(View.VISIBLE);
                    mCellMessage.setVisibility(View.GONE);
                    mStatusText.setText(R.string.text_validation_failed);
                    mPauseButton.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            finish();
                        }
                    });
                    mPauseButton.setText(android.R.string.cancel);
                }
                super.onPostExecute(result);
            }

        };
        validationTask.execute(new Object());
    }

    /**
     * If the download isn't present, we initialize the download UI. This ties
     * all of the controls into the remote service calls.
     */
    private void initializeDownloadUI() {
        mDownloaderClientStub = DownloaderClientMarshaller.CreateStub
                (this, WkDownloaderService.class);
        setContentView(R.layout.downloader);

        mPB = (ProgressBar) findViewById(R.id.progressBar);
        mStatusText = (TextView) findViewById(R.id.statusText);
        mProgressFraction = (TextView) findViewById(R.id.progressAsFraction);
        mProgressPercent = (TextView) findViewById(R.id.progressAsPercentage);
        mAverageSpeed = (TextView) findViewById(R.id.progressAverageSpeed);
        mTimeRemaining = (TextView) findViewById(R.id.progressTimeRemaining);
        mDashboard = findViewById(R.id.downloaderDashboard);
        mCellMessage = findViewById(R.id.approveCellular);
        mPauseButton = (Button) findViewById(R.id.pauseButton);
        mWiFiSettingsButton = (Button) findViewById(R.id.wifiSettingsButton);

        mPauseButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mRemoteService != null) {
                    if (mStatePaused) {
                        mRemoteService.requestContinueDownload();
                    } else {
                        mRemoteService.requestPauseDownload();
                    }
                    setButtonPausedState(!mStatePaused);
                }
            }
        });

        mWiFiSettingsButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                startActivity(new Intent(Settings.ACTION_WIFI_SETTINGS));
            }
        });

        Button resumeOnCell = (Button) findViewById(R.id.resumeOverCellular);
        resumeOnCell.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mRemoteService.setDownloadFlags(IDownloaderService.FLAGS_DOWNLOAD_OVER_CELLULAR);
                mRemoteService.requestContinueDownload();
                mCellMessage.setVisibility(View.GONE);
            }
        });

    }

    /**
     * Check the main APK Expansion files and determine if the files are present and match the required size.
     * Free applications should definitely consider doing this, as this allows the
     * application to be launched for the first time without having a network
     * connection present. Paid applications that use LVL should probably do at
     * least one LVL check that requires the network to be present, so this is
     * not as necessary.
     *
     * @return XAPKFile with its filepath set to proper path if it is present. it is set null if still missing, or default empty String if not needed.
     */
    protected WkDownloaderInfo.XAPKFile expansionFilePath(boolean main) {
        try {
            if ( DLinfo == null ) {
                throw new NullPointerException(" ERROR : DLinfo is not set ! ");
            }
            else {
                WkDownloaderInfo.XAPKFile xf = main? DLinfo.getMainXAPK() : DLinfo.getPatchXAPK();
                if (xf != null) {
                    String expFileName = Helpers.getExpansionAPKFileName(WkDownloaderActivity.this, true, xf.mFileVersion); //only filename
                    String expFilePath = Helpers.generateSaveFileName(WkDownloaderActivity.this, expFileName); //with directory added
                    if ( (xf.mCheckEnabled && Helpers.doesFileExist(WkDownloaderActivity.this, expFileName, xf.mFileSize, false)) //if check enabled we check for size
                        || (!xf.mCheckEnabled && new File(expFilePath).exists()) )//if check disabled we only check it exists
                    {
                        xf.setFilePath(expFilePath);
                    }else{
                        Log.e(TAG, "Missing " + (main?"Main":"Patch") + " XAPK at : " + expFilePath );
                        if (xf.mCheckEnabled) Log.e(TAG, "Expected Size = " + xf.mFileSize);
                    }
                }
                return xf; // we return null if no XAPK file needed ( null returned by app DLinfo implementation.
            }
        } catch (NullPointerException e) {
            Log.e(TAG, "NullPointerException in WkDownloaderActivity.expansionFileDelivered()");
            e.printStackTrace();
        }
        return null; //we reach here only if error
    }

    /**
     * Called when the activity is first create; we wouldn't create a layout in
     * the case where we have the file and are moving to another activity
     * without downloading.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        /**
         * Before we do anything, are the files we expect already here and
         * delivered (presumably by Market) For free titles, this is probably
         * worth doing. (so no Market request is necessary)
         */
        if ( (expansionFilePath(true) != null && expansionFilePath(true).getFilePath() == "")
          || (expansionFilePath(false) != null && expansionFilePath(false).getFilePath() == "")
        ) { // we download only if one XAPK is needed but missing

            try {
                Intent launchIntent = WkDownloaderActivity.this.getIntent();
                Intent intentToLaunchThisActivityFromNotification = new Intent(WkDownloaderActivity.this, WkDownloaderActivity.this.getClass());
                intentToLaunchThisActivityFromNotification.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);
                intentToLaunchThisActivityFromNotification.setAction(launchIntent.getAction());

                if (launchIntent.getCategories() != null) {
                    for (String category : launchIntent.getCategories()) {
                        intentToLaunchThisActivityFromNotification.addCategory(category);
                    }
                }

                // Build PendingIntent used to open this activity from
                // Notification
                PendingIntent pendingIntent = PendingIntent.getActivity(WkDownloaderActivity.this,0, intentToLaunchThisActivityFromNotification,PendingIntent.FLAG_UPDATE_CURRENT);
                // Request to start the download
                int startResult = DownloaderClientMarshaller.startDownloadServiceIfRequired(this,pendingIntent, WkDownloaderService.class);

                if (startResult != DownloaderClientMarshaller.NO_DOWNLOAD_REQUIRED) {
                    // The DownloaderService has started downloading the files,
                    // show progress
                    initializeDownloadUI();
                    return;
                } // otherwise, download not needed so we fall through to
                // starting the movie
            } catch (PackageManager.NameNotFoundException e) {
                Log.e(TAG, "Cannot find own package! MAYDAY!");
                e.printStackTrace();
            }

        } else {
        // Both downloading and validation make use of the "download" UI
            initializeDownloadUI();
            validateXAPKZipFiles();
        }

    }

    /**
     * Connect the stub to our service on start.
     */
    @Override
    protected void onStart() {
        if (null != mDownloaderClientStub) {
            mDownloaderClientStub.connect(this);
        }
        super.onStart();
    }

    /**
     * Disconnect the stub from our service on stop
     */
    @Override
    protected void onStop() {
        if (null != mDownloaderClientStub) {
            mDownloaderClientStub.disconnect(this);
        }
        super.onStop();
    }

    /**
     * Critical implementation detail. In onServiceConnected we create the
     * remote service and marshaller. This is how we pass the client information
     * back to the service so the client can be properly notified of changes. We
     * must do this every time we reconnect to the service.
     */
    @Override
    public void onServiceConnected(Messenger m) {
        mRemoteService = DownloaderServiceMarshaller.CreateProxy(m);
        mRemoteService.onClientUpdated(mDownloaderClientStub.getMessenger());
    }

    /**
     * The download state should trigger changes in the UI --- it may be useful
     * to show the state as being indeterminate at times. This sample can be
     * considered a guideline.
     */
    @Override
    public void onDownloadStateChanged(int newState) {
        setState(newState);
        boolean showDashboard = true;
        boolean showCellMessage = false;
        boolean paused;
        boolean indeterminate;
        switch (newState) {
            case IDownloaderClient.STATE_IDLE:
                // STATE_IDLE means the service is listening, so it's
                // safe to start making calls via mRemoteService.
                paused = false;
                indeterminate = true;
                break;
            case IDownloaderClient.STATE_CONNECTING:
            case IDownloaderClient.STATE_FETCHING_URL:
                showDashboard = true;
                paused = false;
                indeterminate = true;
                break;
            case IDownloaderClient.STATE_DOWNLOADING:
                paused = false;
                showDashboard = true;
                indeterminate = false;
                break;

            case IDownloaderClient.STATE_FAILED_CANCELED:
            case IDownloaderClient.STATE_FAILED:
            case IDownloaderClient.STATE_FAILED_FETCHING_URL:
            case IDownloaderClient.STATE_FAILED_UNLICENSED:
                paused = true;
                showDashboard = false;
                indeterminate = false;
                break;
            case IDownloaderClient.STATE_PAUSED_NEED_CELLULAR_PERMISSION:
            case IDownloaderClient.STATE_PAUSED_WIFI_DISABLED_NEED_CELLULAR_PERMISSION:
                showDashboard = false;
                paused = true;
                indeterminate = false;
                showCellMessage = true;
                break;

            case IDownloaderClient.STATE_PAUSED_BY_REQUEST:
                paused = true;
                indeterminate = false;
                break;
            case IDownloaderClient.STATE_PAUSED_ROAMING:
            case IDownloaderClient.STATE_PAUSED_SDCARD_UNAVAILABLE:
                paused = true;
                indeterminate = false;
                break;
            case IDownloaderClient.STATE_COMPLETED:
                showDashboard = false;
                paused = false;
                indeterminate = false;
                validateXAPKZipFiles();
                return;
            default:
                paused = true;
                indeterminate = true;
                showDashboard = true;
        }
        int newDashboardVisibility = showDashboard ? View.VISIBLE : View.GONE;
        if (mDashboard.getVisibility() != newDashboardVisibility) {
            mDashboard.setVisibility(newDashboardVisibility);
        }
        int cellMessageVisibility = showCellMessage ? View.VISIBLE : View.GONE;
        if (mCellMessage.getVisibility() != cellMessageVisibility) {
            mCellMessage.setVisibility(cellMessageVisibility);
        }

        mPB.setIndeterminate(indeterminate);
        setButtonPausedState(paused);
    }

    /**
     * Sets the state of the various controls based on the progressinfo object
     * sent from the downloader service.
     */
    @Override
    public void onDownloadProgress(DownloadProgressInfo progress) {
        mAverageSpeed.setText(getString(R.string.kilobytes_per_second,
                Helpers.getSpeedString(progress.mCurrentSpeed)));
        mTimeRemaining.setText(getString(R.string.time_remaining,
                Helpers.getTimeRemaining(progress.mTimeRemaining)));

        progress.mOverallTotal = progress.mOverallTotal;
        mPB.setMax((int) (progress.mOverallTotal >> 8));
        mPB.setProgress((int) (progress.mOverallProgress >> 8));
        mProgressPercent.setText(Long.toString(progress.mOverallProgress
                * 100 /
                progress.mOverallTotal) + "%");
        mProgressFraction.setText(Helpers.getDownloadProgressString
                (progress.mOverallProgress,
                        progress.mOverallTotal));
    }

    @Override
    protected void onDestroy() {
        this.mCancelValidation = true;
        super.onDestroy();
    }

}
