#
# Copyright 2016 pushd inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LCMS_ROOT 		:= ../../lcms2
LCMS_SRC_PATH 	:= $(LCMS_ROOT)/src

LOCAL_MODULE     := colorpal
LOCAL_LDFLAGS 	+= \
		-llog \
		-ljnigraphics

LOCAL_C_INCLUDES := $(LCMS_ROOT)/include/

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
