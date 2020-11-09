LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS) 
LOCAL_SRC_FILES := Check_Condition_caller.c	# your source code 
LOCAL_MODULE := CCcallerARM	# output file name 
LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE	# these two lines let the program complied based on PIE 
LOCAL_FORCE_STATIC_EXECUTABLE := true

include $(BUILD_EXECUTABLE)

