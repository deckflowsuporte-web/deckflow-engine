package com.deckflow.engine;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.content.pm.ActivityInfo;

/**
 * DeckFlowLib - Main Java interface to the native C engine
 */
public class DeckFlowLib extends Activity {
    
    static {
        System.loadLibrary("native-lib");
    }
    
    // Native methods
    public static native void nativeInit();
    public static native void nativeSetSurface(Object surface);
    public static native void nativeRender();
    public static native void nativeTouch(int action, int id, float x, float y);
    public static native void nativeDestroy();
    
    private SurfaceView surfaceView;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // Keep screen on
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        
        // Fullscreen
        getWindow().setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        );
        
        // Initialize native engine
        nativeInit();
        
        // Create surface view
        surfaceView = new SurfaceView(this);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                nativeSetSurface(holder.getSurface());
            }
            
            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            }
            
            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
            }
        });
        
        setContentView(surfaceView);
    }
    
    @Override
    protected void onResume() {
        super.onResume();
    }
    
    @Override
    protected void onPause() {
        super.onPause();
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        nativeDestroy();
    }
}
