package com.asmodehn.wkcocos.testapp;

import android.app.Activity;
import android.app.NotificationManager;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import com.asmodehn.wkcocos.lib.MainActivity;
import com.asmodehn.wkcocos.lib.WkDownloaderActivity;
import com.asmodehn.wkcocos.lib.WkDownloaderInfo;
import com.google.android.vending.expansion.downloader.Helpers;

public class AppActivity extends MainActivity implements WkDownloaderInfo {

    private final static String TAG = AppActivity.class.getSimpleName();

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        this.setInfo(this);
        super.onCreate(savedInstanceState);
    }


    @Override
    public String getVersionName()
    {
        //gradle
        String versionName = BuildConfig.VERSION_NAME;
        return versionName;
    }

    @Override
    public int getVersionCode()
    {
        //gradle // careful : current package !
        int versionCode = BuildConfig.VERSION_CODE;
        return versionCode;
    }

    @Override
    public XAPKFile getMainXAPK() {
        return null;
//
//        To test this you need to put main.<versionCode>.com.asmodehn.wkcocos.testapp(.debug).obb
//        containing zipped assets/ into Android/obb/com.asmodehn.wkcocos.testapp(.debug)/
//
//        return new XAPKFile(
//                BuildConfig.VERSION_CODE, // the version of the APK that the file was uploaded against
//                // it should be the version code added to the obb filename
//                // TODO : CHECK => so it s possible that it doesn't match current BuildConfig.VERSION_CODE ?
//                654654L,//BuildConfig.OBB_SIZE, // the length of the file in bytes
//                false //false ignores the size check and the zip CRC checks.
//        );
    }

    @Override
    public XAPKFile getPatchXAPK() {
        //return null if no XAPK is needed
        return null;
    }

    @Override
    public String getLVLKey()
    {
        return "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAl+3XzvgIDy5FnT47omyrAtNBs3cVLJEq5GQQjHOIiF7xAqPJM5Toi9ejhRJuxqqrPgERLLJgqGO8Lv/tYMMvm2Tona0PE+6+yfkxdMfSIBQnqSHyyyPsnqCO7jXdLYYmn904UwPpamdoaR724RNAfJgcSEtmOBcw760kvcVG62SHeaK0DjWjgouW1XO5lWvvUGlh+tEV8T4e/7LDyXFwRlTA8++HjvTB4xH9fK7AXphJLLMuEEwNczBpBkztk8mW+6DAwfDESGz2hRP/XyvYZGuH9T/9H6BFabmXxSUuIIZZeVycF1/2U3rOOBh19E2xTtCeMCG/QvYc4Rk2EeoqaQIDAQAB";
    }

    @Override
    public byte[] getSALT()
    {
        return new byte[] { 1, 42, -12, -1, 54, 98, -100, -12, 43, 2, -8, -4, 9, 5, -106, -42, -33, 45, -1, 84 };
    }

}
