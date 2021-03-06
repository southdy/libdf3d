package org.flaming0.df3d;

import android.app.Activity;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.Keep;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

public class Df3dActivity extends Activity {
    private Df3dSurfaceView m_glSurfaceView = null;
    private Runnable setUiVisibilityRunnable = null;
    private boolean m_appRunning = false;
    private GamePadHelper mGamePadHelper = null;

//    private boolean mQuittingApp = false;

    private static Df3dActivity m_sharedActivity = null;

    private native void nativeHardwareBackPressed(boolean pressed);

//    public void setShouldQuit() {
//        mQuittingApp = true;
//        finish();
//        System.exit(0);
//    }

    public static Df3dActivity getSharedActivity() {
        return m_sharedActivity;
    }

    public void runOnMainThread(Runnable runnable)
    {
        if (m_glSurfaceView != null) {
            m_glSurfaceView.queueEvent(runnable);
        }
    }

    public boolean isAppRunning() {
        return m_appRunning;
    }

    public GamePadHelper getGamePadHelper() {
        return mGamePadHelper;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        m_sharedActivity = this;

        mGamePadHelper = new GamePadHelper(this);

        enableFullscreen();

        NativeBindings.servicesInitialized(new Df3dAndroidServices(this));
        NativeBindings.setAssetManager(getAssets());

        setContentView(m_glSurfaceView = new Df3dSurfaceView(this));

        final Window window = getWindow();
        if (window != null)
            window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    public View getMainView() {
        return m_glSurfaceView;
    }

    @Override
    protected void onPause() {
        m_appRunning = false;

        m_glSurfaceView.onPause();

        super.onPause();

        mGamePadHelper.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();

        mGamePadHelper.onResume();
        m_glSurfaceView.onResume();

        m_appRunning = true;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

//        if (mQuittingApp) {
//            System.runFinalization();
//            System.exit(0);
//        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus && setUiVisibilityRunnable != null) {
            setUiVisibilityRunnable.run();
        }
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event)
    {
        switch (keyCode) {
            case KeyEvent.KEYCODE_BACK:
                runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeHardwareBackPressed(false);
                    }
                });
                return true;
        }

        return super.onKeyUp(keyCode, event);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        switch (keyCode) {
            case KeyEvent.KEYCODE_BACK:
                runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeHardwareBackPressed(true);
                    }
                });
                return true;
        }

        return super.onKeyDown(keyCode, event);
    }

    private void enableFullscreen()
    {
        if (Build.VERSION.SDK_INT >= 16) {
            int uiOptions = getWindow().getDecorView().getSystemUiVisibility();

            uiOptions |= View.SYSTEM_UI_FLAG_FULLSCREEN;

            if (Build.VERSION.SDK_INT >= 19) {
                uiOptions |= View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN;
                uiOptions |= View.SYSTEM_UI_FLAG_LAYOUT_STABLE;
                uiOptions |= View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
                uiOptions |= View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION;
                uiOptions |= View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
            }

            final int opts = uiOptions;
            setUiVisibilityRunnable = new Runnable() {
                @Override
                public void run() {
                    getWindow().getDecorView().setSystemUiVisibility(opts);
                }
            };

            setUiVisibilityRunnable.run();
        } else {
            requestWindowFeature(Window.FEATURE_NO_TITLE);
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        }
    }

    protected void onGameInitializationFailed()
    {
        MyLog.e("df3d_activity", "Game init failed!");
    }
}
