package com.deckflow.engine;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;

/**
 * NativeLib - Bridge between Java Android and C Native Engine
 * 
 * This activity initializes the native C engine and renders
 * to a SurfaceView using OpenGL ES 2.0.
 */
public class NativeLib extends Activity implements SurfaceHolder.Callback {
    
    private SurfaceView surfaceView;
    private SurfaceHolder surfaceHolder;
    private Thread renderThread;
    private boolean isRunning = false;
    
    // Native methods
    public static native int nativeInit(Surface surface);
    public static native void nativeResize(int width, int height);
    public static native void nativeStep();
    public static native void nativeTouch(int action, float x, float y);
    public static native void nativeShutdown();
    
    // Load native library
    static {
        System.loadLibrary("deckflow_main");
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // Keep screen on
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        
        // Create SurfaceView for OpenGL rendering
        surfaceView = new SurfaceView(this);
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);
        
        setContentView(surfaceView);
    }
    
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Surface surface = holder.getSurface();
        if (surface != null && surface.isValid()) {
            int result = nativeInit(surface);
            if (result == 0) {
                startRenderLoop();
            }
        }
    }
    
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        nativeResize(width, height);
    }
    
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        stopRenderLoop();
        nativeShutdown();
    }
    
    private void startRenderLoop() {
        isRunning = true;
        renderThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (isRunning) {
                    try {
                        nativeStep();
                        Thread.sleep(16); // ~60 FPS
                    } catch (InterruptedException e) {
                        break;
                    }
                }
            }
        });
        renderThread.start();
    }
    
    private void stopRenderLoop() {
        isRunning = false;
        if (renderThread != null) {
            try {
                renderThread.join(1000);
            } catch (InterruptedException e) {
                // Ignore
            }
            renderThread = null;
        }
    }
    
    @Override
    protected void onPause() {
        super.onPause();
        stopRenderLoop();
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        if (surfaceHolder.getSurface() != null && surfaceHolder.getSurface().isValid()) {
            startRenderLoop();
        }
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopRenderLoop();
        nativeShutdown();
    }
}
