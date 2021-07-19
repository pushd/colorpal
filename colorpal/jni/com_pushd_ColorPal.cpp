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

#include "lcms2.h"

#include <stdlib.h>
#include <jni.h>
#include <time.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <endian.h>
#include <stdarg.h>
#include <math.h>

#define  LOG_TAG    "ColorPal.CorrectorJNI"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

// AssertionErrors are fancy, ThrowNew won't work since it has an Object ctor, not a String ctor
void throwAssertionError(JNIEnv* env, const char *msgFormat, ...) {
    char msg[128];
    va_list args;
    va_start(args, msgFormat);
    vsprintf(msg, msgFormat, args);
    va_end(args);

    jclass cls = env->FindClass("java/lang/AssertionError");
    jmethodID constructor = env->GetMethodID(cls, "<init>", "(Ljava/lang/Object;)V");
    jstring jmsg = env->NewStringUTF(msg);
    jthrowable throwable = (jthrowable)env->NewObject(cls, constructor, (jobject)jmsg);
    env->Throw(throwable);
}

JNIEXPORT jintArray JNICALL Java_com_pushd_colorpal_ColorCorrector_create3DLUT(JNIEnv *env, jobject obj, jlong longHandle) {
    static const int DIM = 64; // 8MB LUT

    if (!longHandle) {
        throwAssertionError(env, "No tansform handle available");
        return NULL;
    }

    cmsHTRANSFORM hTransform = (cmsHTRANSFORM)longHandle;

    jintArray ret = env->NewIntArray(DIM * DIM * DIM);
    jint *lut = env->GetIntArrayElements(ret, NULL);

    for (uint16_t r = 0; r < DIM; ++r) {
        for (uint16_t g = 0; g < DIM; ++g) {
            for (uint16_t b = 0; b < DIM; ++b) {
                int r_scaled = floor((double) r / (DIM - 1) * 255.0 + 0.5);
                int b_scaled = floor((double) b / (DIM - 1) * 255.0 + 0.5);
                int g_scaled = floor((double) g / (DIM - 1) * 255.0 + 0.5);

                // little-endian RGBA -> ABGR
                uint32_t pixel =  (0xFF << 24) | (b_scaled << 16) | (g_scaled << 8) | (r_scaled << 0);
                uint32_t mapped = 0;
                cmsDoTransform(hTransform, &pixel, &mapped, 1);

                // expects [b][g][r] "flat" index
                lut[b*DIM*DIM + g*DIM + r] = mapped;
                //LOGD("pixel: 0x%08x, transformed: 0x%08x", pixel, mapped);
            }
        }
    }

    env->ReleaseIntArrayElements(ret, lut, 0);
    return ret;
}

JNIEXPORT jlong JNICALL Java_com_pushd_colorpal_ColorCorrector_createTransform(JNIEnv *env, jobject obj, jstring javaString) {

    cmsHPROFILE hInputProfile = cmsCreate_sRGBProfile();

    if (!hInputProfile) {
        throwAssertionError(env, "Could not open SRGB Profile");
        return 0;
    }

    // this will give us java's "modified" UTF-8 but for icc profile paths this shouldn't matter
    const char *iccPath = env->GetStringUTFChars(javaString, JNI_FALSE);

    cmsHPROFILE hOutputProfile = cmsOpenProfileFromFile(iccPath, "r");

    if (!hOutputProfile) {
        throwAssertionError(env, "Could not load ICC Profile at %s", iccPath);
        env->ReleaseStringUTFChars(javaString, iccPath);
        return 0;
    }

    // create the transform for concurrent use (cmsFLAGS_NOCACHE)
    // http://littlecms2.blogspot.com/2010/12/multithreading-question.html
    cmsHTRANSFORM hTransform = cmsCreateTransform(hInputProfile,
                                                  TYPE_RGBA_8,
                                                  hOutputProfile,
                                                  TYPE_RGBA_8,
                                                  INTENT_PERCEPTUAL,
                                                  cmsFLAGS_NOCACHE|cmsFLAGS_NOOPTIMIZE);

    if (!hTransform) {
        throwAssertionError(env, "Could not create transform");
        env->ReleaseStringUTFChars(javaString, iccPath);
        return 0;
    }

    // once the transform is created we can free the profiles
    cmsCloseProfile(hInputProfile);
    cmsCloseProfile(hOutputProfile);
    hInputProfile = hOutputProfile = NULL;

    env->ReleaseStringUTFChars(javaString, iccPath);

    return (jlong)(hTransform);
}

JNIEXPORT void JNICALL Java_com_pushd_colorpal_ColorCorrector_disposeTransform(JNIEnv *env, jobject obj, jlong longHandle) {
    if (!longHandle) {
        env->ThrowNew(env->FindClass("java/lang/IllegalArgumentException"), "No tansform handle to dispose");
        return;
    }

    cmsHTRANSFORM hTransform = (cmsHTRANSFORM)longHandle;
    cmsDeleteTransform(hTransform);
}


JNIEXPORT void JNICALL Java_com_pushd_colorpal_ColorCorrector_correctBitmap(JNIEnv *env, jobject /*obj*/, jlong longHandle, jobject bitmap) {
    AndroidBitmapInfo info;
    void *pixels = NULL;
    int ret = 0;
    char excmsg[128];

    if (!longHandle) {
        env->ThrowNew(env->FindClass("java/lang/IllegalArgumentException"), "No tansform handle provided");
        return;
    }

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        throwAssertionError(env, "AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        throwAssertionError(env, "Bitmap format is not RGBA_8888 !");
        return;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        throwAssertionError(env, "AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return;
    }

    // do the thing
    cmsHTRANSFORM hTransform = (cmsHTRANSFORM)longHandle;
    cmsDoTransform(hTransform, pixels, pixels, info.width * info.height);

    if ((ret = AndroidBitmap_unlockPixels(env, bitmap)) < 0) {
        throwAssertionError(env, "AndroidBitmap_unlockPixels() failed ! error=%d", ret);
        return;
    }

}

JNIEXPORT jint JNICALL Java_com_pushd_colorpal_ColorCorrector_correctedColor(JNIEnv */*env*/, jobject /*obj*/, jlong longHandle, jint argb) {

    // ARGB -> ABGR
    uint8_t a = (argb >> 24) & 0xFF;
    uint8_t r = (argb >> 16) & 0xFF;
    uint8_t g = (argb >> 8) & 0xFF;
    uint8_t b = argb & 0xFF;

    // do not map alpha channel, so replace with 0xFF
    uint32_t pixel = 0xFF << 24 | b << 16 | g << 8 | r;

    uint32_t mapped = 0;
    cmsHTRANSFORM hTransform = (cmsHTRANSFORM)longHandle;
    cmsDoTransform(hTransform, &pixel, &mapped, 1);

    // ABGR -> ARGB
    b = (mapped >> 16) & 0xFF;
    g = (mapped >> 8) & 0xFF;
    r = mapped & 0xFF;

    // use original alpha
    return a << 24 | r << 16 | g << 8 | b;
}

#ifdef __cplusplus
}
#endif

