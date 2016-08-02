   LOCAL_PATH := $(call my-dir)

   include $(CLEAR_VARS)

   LCMS_ROOT 		:= ../../lcms2
   LCMS_SRC_PATH 	:= $(LCMS_ROOT)/src

   LOCAL_MODULE     := colorpal
   LOCAL_LDFLAGS 	+= \
   		-llog \
   		-ljnigraphics

   LOCAL_C_INCLUDES := ../lcms2/include/

   LOCAL_SRC_FILES  := \
        $(LCMS_SRC_PATH)/cmsalpha.c \
        $(LCMS_SRC_PATH)/cmscam02.c \
        $(LCMS_SRC_PATH)/cmscgats.c \
        $(LCMS_SRC_PATH)/cmscnvrt.c \
        $(LCMS_SRC_PATH)/cmserr.c \
        $(LCMS_SRC_PATH)/cmsgamma.c \
        $(LCMS_SRC_PATH)/cmsgmt.c \
        $(LCMS_SRC_PATH)/cmshalf.c \
        $(LCMS_SRC_PATH)/cmsintrp.c \
        $(LCMS_SRC_PATH)/cmsio0.c \
        $(LCMS_SRC_PATH)/cmsio1.c \
        $(LCMS_SRC_PATH)/cmslut.c \
        $(LCMS_SRC_PATH)/cmsmd5.c \
        $(LCMS_SRC_PATH)/cmsmtrx.c \
        $(LCMS_SRC_PATH)/cmsnamed.c \
        $(LCMS_SRC_PATH)/cmsopt.c \
        $(LCMS_SRC_PATH)/cmspack.c \
        $(LCMS_SRC_PATH)/cmspcs.c \
        $(LCMS_SRC_PATH)/cmsplugin.c \
        $(LCMS_SRC_PATH)/cmsps2.c \
        $(LCMS_SRC_PATH)/cmssamp.c \
        $(LCMS_SRC_PATH)/cmssm.c \
        $(LCMS_SRC_PATH)/cmstypes.c \
        $(LCMS_SRC_PATH)/cmsvirt.c \
        $(LCMS_SRC_PATH)/cmswtpnt.c \
        $(LCMS_SRC_PATH)/cmsxform.c

   LOCAL_SRC_FILES += com_pushd_ColorPal.cpp

   include $(BUILD_SHARED_LIBRARY)
