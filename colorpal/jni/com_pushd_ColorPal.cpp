#include "lcms2.h"

#include <stdlib.h>
#include <jni.h>
#include <time.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <endian.h>

#define  LOG_TAG    "ColorPal.CorrectorJNI"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_com_pushd_colorpal_ColorCorrector_createTransform(JNIEnv *env, jobject obj, jstring javaString) {

    // this will give us java's "modified" UTF-8 but for icc profile paths this shouldn't matter
    const char *iccPath = env->GetStringUTFChars(javaString, JNI_FALSE);

    cmsHPROFILE hInputProfile = cmsCreate_sRGBProfile();

    if (!hInputProfile) {
        LOGE("Could not open SRGB Profile");
        return 0;
    }

    cmsHPROFILE hOutputProfile = cmsOpenProfileFromFile(iccPath, "r");

    if (!hOutputProfile) {
        LOGE("Could not open ICC Profile at %s", iccPath);
        return 0;
    }

    // create the transform for concurrent use (cmsFLAGS_NOCACHE)
    // http://littlecms2.blogspot.com/2010/12/multithreading-question.html
    cmsHTRANSFORM hTransform = cmsCreateTransform(hInputProfile,
                                                  TYPE_RGBA_8,
                                                  hOutputProfile,
                                                  TYPE_RGBA_8,
                                                  INTENT_PERCEPTUAL,
                                                  cmsFLAGS_NOCACHE);

    if (!hTransform) {
        LOGE("Could not create multi profile xform");
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
    cmsHTRANSFORM hTransform = (cmsHTRANSFORM)longHandle;
    cmsDeleteTransform(hTransform);
}


JNIEXPORT void JNICALL Java_com_pushd_colorpal_ColorCorrector_correctBitmap(JNIEnv *env, jobject obj, jlong longHandle, jobject bitmap) {
    AndroidBitmapInfo info;
    void *pixels = NULL;
    int ret = 0;
    char excmsg[128];

    if (!longHandle) {
        env->ThrowNew(env->FindClass("java/lang/AssertionError"), "No tansform handle available");
        return;
    }

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        sprintf(excmsg, "AndroidBitmap_getInfo() failed ! error=%d", ret);
        env->ThrowNew(env->FindClass("java/lang/AssertionError"), excmsg);
        return;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        env->ThrowNew(env->FindClass("java/lang/AssertionError"), "Bitmap format is not RGBA_8888 !");
        return;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        sprintf(excmsg, "AndroidBitmap_lockPixels() failed ! error=%d", ret);
        env->ThrowNew(env->FindClass("java/lang/AssertionError"), excmsg);
        return;
    }

    // do the thing
    cmsHTRANSFORM hTransform = (cmsHTRANSFORM)longHandle;
    cmsDoTransform(hTransform, pixels, pixels, info.width * info.height);

    if ((ret = AndroidBitmap_unlockPixels(env, bitmap)) < 0) {
        sprintf(excmsg, "AndroidBitmap_unlockPixels() failed ! error=%d", ret);
        env->ThrowNew(env->FindClass("java/lang/AssertionError"), excmsg);
        return;
    }
    return;
}

#ifdef __cplusplus
}
#endif

