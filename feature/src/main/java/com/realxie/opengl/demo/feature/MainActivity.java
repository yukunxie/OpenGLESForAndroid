package com.realxie.opengl.demo.feature;

import android.Manifest;
import android.app.Activity;
import android.app.Application;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.widget.Toast;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.View.OnClickListener;
import android.util.Log;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

public class MainActivity extends Activity implements SurfaceHolder.Callback {

    private static String TAG = "EglSample";
    private static String mDocumentPath;






    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
        //System.loadLibrary("cxx_jni_interface");
    }

    private MyRender glRenderer;


    public  boolean isReadStoragePermissionGranted() {
        if (Build.VERSION.SDK_INT >= 23) {
            if (checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE)
                    == PackageManager.PERMISSION_GRANTED) {
                Log.v(TAG,"Permission is granted1");
                return true;
            } else {

                Log.v(TAG,"Permission is revoked1");
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 3);
                return false;
            }
        }
        else { //permission is automatically granted on sdk<23 upon installation
            Log.v(TAG,"Permission is granted1");
            return true;
        }
    }

    public  boolean isWriteStoragePermissionGranted() {
        if (Build.VERSION.SDK_INT >= 23) {
            if (checkSelfPermission(android.Manifest.permission.WRITE_EXTERNAL_STORAGE)
                    == PackageManager.PERMISSION_GRANTED) {
                Log.v(TAG,"Permission is granted2");
                return true;
            } else {

                Log.v(TAG,"Permission is revoked2");
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 2);
                return false;
            }
        }
        else { //permission is automatically granted on sdk<23 upon installation
            Log.v(TAG,"Permission is granted2");
            return true;
        }
    }

    private void createDocumentPath()
    {
        mDocumentPath = Environment.getExternalStorageDirectory().getPath() + "/Android/data/"
                + BuildConfig.APPLICATION_ID + "/files/Documents/";

        File tmp = new File(mDocumentPath);
        if (false == tmp.exists())
        {
            tmp.mkdirs();
        }
        Log.i("MainActivity", "document path:" + mDocumentPath);
        nativeSetDocumentPath(mDocumentPath);
    }

    public static String getDocumentPath()
    {
        return mDocumentPath;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        isWriteStoragePermissionGranted();
        isReadStoragePermissionGranted();
        createDocumentPath();

        SurfaceView surfaceView = (SurfaceView) findViewById(R.id.gl_view);
        surfaceView.getHolder().addCallback(this);
        surfaceView.setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                Toast toast = Toast.makeText(MainActivity.this,
                        "This demo combines Java UI and native EGL + OpenGL renderer",
                        Toast.LENGTH_LONG);
                toast.show();
            }});



        //        // Example of a call to a native method
        //        TextView tv = (TextView) findViewById(R.id.sample_text);
        //        tv.setText(stringFromJNI());

//        SurfaceView sv = (SurfaceView) findViewById(R.id.gl_view);
//
//        glRenderer = new MyRender();
//        glRenderer.start();
//
//        sv.getHolder().addCallback(new SurfaceHolder.Callback() {
//            @Override
//            public void surfaceCreated(SurfaceHolder surfaceHolder) {
//
//            }
//
//            @Override
//            public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int width, int height) {
//                glRenderer.render(surfaceHolder.getSurface(), width, height);
//            }
//
//            @Override
//            public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
//
//            }
//        });

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    @Override
    protected void onStart() {
        super.onStart();
        Log.i(TAG, "onStart()");
        nativeOnStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.i(TAG, "onResume()");
        nativeOnResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.i(TAG, "onPause()");
        nativeOnPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.i(TAG, "onStop()");
        nativeOnStop();
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        nativeSetSurface(holder.getSurface());
    }

    public void surfaceCreated(SurfaceHolder holder) {
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        nativeSetSurface(null);
    }

    public static native void nativeSetDocumentPath(String path);
    public static native void nativeOnStart();
    public static native void nativeOnResume();
    public static native void nativeOnPause();
    public static native void nativeOnStop();
    public static native void nativeSetSurface(Surface surface);

//    static {
//        System.loadLibrary("nativeegl");
//    }

}
