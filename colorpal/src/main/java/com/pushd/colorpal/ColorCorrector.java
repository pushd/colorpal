/**
 * Copyright 2016 pushd inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.pushd.colorpal;

import android.graphics.Bitmap;
import android.renderscript.Allocation;
import android.renderscript.Element;
import android.renderscript.RenderScript;
import android.renderscript.ScriptIntrinsic3DLUT;
import android.renderscript.Type;

public class ColorCorrector {
    private static final String TAG = "ColorPal.ColorCorrector";

    private long mNativeTransformHandle = 0;

    private native void disposeTransform(long nativeTransformHandle);
    private native long createTransform(String displayProfilePath);
    private native void correctBitmap(long nativeTransformHandle, Bitmap bm);
    private native int[] create3DLUT(long nativeTransformHandle);

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

    public ScriptIntrinsic3DLUT create3DLUT(RenderScript rs) {
        Type.Builder tb = new Type.Builder(rs, Element.U8_4(rs));
        tb.setX(64);
        tb.setY(64);
        tb.setZ(64);

        Type t = tb.create();
        Allocation allocation = Allocation.createTyped(rs, t);

        int dat[] = create3DLUT(mNativeTransformHandle);
        allocation.copyFromUnchecked(dat);

        ScriptIntrinsic3DLUT lut = ScriptIntrinsic3DLUT.create(rs, Element.U8_4(rs));
        lut.setLUT(allocation);
        return lut;
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
