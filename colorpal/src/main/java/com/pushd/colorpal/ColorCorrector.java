package com.pushd.colorpal;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.SystemClock;
import android.util.Log;

/**
 * Created by james on 8/2/16.
 */
public class ColorCorrector {
    private static final String TAG = "ColorPal.ColorCorrector";
    private static ColorCorrector mInstance;
    private static Context mContext;

    private long mNativeTransformHandle = 0;


    private native void disposeTransform(long nativeTransformHandle);
    private native long createTransform(String displayProfilePath);
    private native void correctBitmap(long nativeTransformHandle, Bitmap bm);

    static {
        System.loadLibrary("colorpal");
    }

    public ColorCorrector() {}

    /**
     * Initialize the singleton ColorCorrector with the given context and
     * icc profile path.
     * @param context
     * @param filename
     * @return
     */
    public static boolean init(Context context, String filename) {
        mContext = context.getApplicationContext();
        return getInstance().loadDisplayICCProfile(filename);
    }

    public static ColorCorrector getInstance() {
        if(mInstance == null) {
            mInstance = new ColorCorrector();
        }
        return mInstance;
    }

    /**
     * The ICC Profile must be loaded before {@code correctBitmap()} is called
     * <br/>
     * Note: Do this on a background thread
     *
     * @param filename
     * @return
     */
    public boolean loadDisplayICCProfile(String filename) {
        if (mNativeTransformHandle != 0) {
            disposeTransform(mNativeTransformHandle);
        }

        long start = SystemClock.uptimeMillis();
        mNativeTransformHandle = createTransform(filename);
        Log.d(TAG, "loadDisplayICCProfile took " + String.valueOf(SystemClock.uptimeMillis() - start) + "ms");

        return mNativeTransformHandle != 0;
    }

    /**
     * Apply the loaded ICC profile in-place to the passed Bitmap instance
     * <br/>
     * {@code loadDisplayICCProfile()} must be called prior to this.
     * <br/>
     * Note: Do this on a background thread
     *
     * @param bm
     */
    public void correctBitmap(Bitmap bm) {
        if (mNativeTransformHandle == 0) {
            Log.e(TAG, "Can't correct bitmap without native transform");
            return;
        }

        long start = SystemClock.uptimeMillis();
        correctBitmap(mNativeTransformHandle, bm);
        Log.d(TAG, "correctBitmap took " + String.valueOf(SystemClock.uptimeMillis() - start) + "ms");
    }
}
