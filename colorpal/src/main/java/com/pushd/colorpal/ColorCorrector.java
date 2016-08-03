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

    private long mNativeTransformHandle = 0;


    private native void disposeTransform(long nativeTransformHandle);
    private native long createTransform(String displayProfilePath);
    private native void correctBitmap(long nativeTransformHandle, Bitmap bm);

    static {
        System.loadLibrary("colorpal");
    }

    public ColorCorrector() {}

    /**
     * The ICC Profile must be loaded before {@code correctBitmap()} is called
     * <br/>
     * Will throw an AssertionError if there is any issue loading the profile
     * or creating the internal transform instance
     *
     * @param filename
     * @return
     */
    public void loadDisplayICCProfile(String filename) {
        if (mNativeTransformHandle != 0) {
            disposeTransform(mNativeTransformHandle);
        }

        mNativeTransformHandle = createTransform(filename);
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
            throw new IllegalStateException("Can't correct bitmap without native transform - was the profile loaded?");
        }

        if (bm == null) {
            throw new IllegalArgumentException("Can't correct null bitmap");
        }

        correctBitmap(mNativeTransformHandle, bm);
    }
}
