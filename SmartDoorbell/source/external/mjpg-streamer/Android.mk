LOCAL_PATH:= $(call my-dir)

# build input_control.so
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    plugins/input_control/dynctrl.c \
    plugins/input_control/input_uvc.c

LOCAL_C_INCLUDES := \
    mjpg_streamer.h \
    utils.h \
    plugins/output.h \
    plugins/input.h \
    plugins/input_control/uvcvideo.h \
    plugins/input_control/uvc_compat.h \
    plugins/input_control/v4l2uvc.h \
    plugins/input_control/dynctrl.h
    
LOCAL_CFLAGS := -O2 -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC

LOCAL_MODULE := input_control

include $(BUILD_SHARED_LIBRARY)

# build input_file.so
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    plugins/input_file/input_file.c

    
LOCAL_C_INCLUDES := \
    mjpg_streamer.h \
    utils.h \
    plugins/output.h \
    plugins/input.h
    
LOCAL_CFLAGS := -O2 -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC

LOCAL_MODULE := input_file

include $(BUILD_SHARED_LIBRARY)

# build input_http.so
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    plugins/input_http/misc.c \
    plugins/input_http/mjpg-proxy.c \
    plugins/input_http/input_http.c
    
LOCAL_C_INCLUDES := \
    mjpg_streamer.h \
    utils.h \
    plugins/output.h \
    plugins/input.h \
    plugins/input_http/misc.h \
    plugins/input_http/mjpg-proxy.h \
    plugins/input_http/version.h
    
LOCAL_CFLAGS := -O2 -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC

LOCAL_MODULE := input_http

include $(BUILD_SHARED_LIBRARY)

# build input_uvc.so
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    utils.c \
    plugins/input_uvc/v4l2uvc.c \
    plugins/input_uvc/jpeg_utils.c \
    plugins/input_uvc/dynctrl.c \
    plugins/input_uvc/input_uvc.c \

    
LOCAL_C_INCLUDES := \
    mjpg_streamer.h \
    utils.h \
    plugins/output.h \
    plugins/input.h \
    plugins/input_uvc/huffman.h \
    plugins/input_uvc/uvc_compat.h \
    plugins/input_uvc/v4l2uvc.h \
    plugins/input_uvc/jpeg_utils.h \
    plugins/input_uvc/dynctrl.h \
    ../jpeg/jpeglib.h
    
#LOCAL_CFLAGS := -O1 -DLINUX -DUSE_LIBV4L2 -D_GNU_SOURCE -Wall -shared -fPIC
LOCAL_CFLAGS := -O1 -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC

LOCAL_SHARED_LIBRARIES := libjpeg

LOCAL_MODULE := input_uvc

include $(BUILD_SHARED_LIBRARY)

# build output_autofocus.so
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    plugins/output_autofocus/processJPEG_onlyCenter.c \
    plugins/output_autofocus/output_autofocus.c
    
LOCAL_C_INCLUDES := \
    mjpg_streamer.h \
    utils.h \
    plugins/output.h \
    plugins/input.h \
    plugins/output_autofocus/processJPEG_onlyCenter.h
    
LOCAL_CFLAGS := -O2 -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC

LOCAL_MODULE := output_autofocus

include $(BUILD_SHARED_LIBRARY)

# build output_file.so
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    plugins/output_file/output_file.c 

LOCAL_C_INCLUDES := \
    mjpg_streamer.h \
    utils.h \
    plugins/output.h \
    plugins/input.h \
    plugins/output_file/output_file.h
    
LOCAL_CFLAGS := -O2 -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC

LOCAL_MODULE := output_file

include $(BUILD_SHARED_LIBRARY)

# build output_http.so
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    plugins/output_http/httpd.c \
    plugins/output_http/output_http.c

    
LOCAL_C_INCLUDES := \
    mjpg_streamer.h \
    utils.h \
    plugins/output.h \
    plugins/input.h \
    plugins/output_http/httpd.h
    
LOCAL_CFLAGS := -O1 -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC

LOCAL_MODULE := output_http

include $(BUILD_SHARED_LIBRARY)

# build output_rtsp.so
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    plugins/output_rtsp/output_rtsp.c

    
LOCAL_C_INCLUDES := \
    mjpg_streamer.h \
    utils.h \
    plugins/output.h \
    plugins/input.h \
    plugins/output_rtsp/httpd.h
    
LOCAL_CFLAGS := -O1 -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC

LOCAL_MODULE := output_rtsp

include $(BUILD_SHARED_LIBRARY)

# build output_udp.so
include $(CLEAR_VARS)

LOCAL_SRC_FILES := plugins/output_udp/output_udp.c

LOCAL_C_INCLUDES := \
    mjpg_streamer.h \
    utils.h \
    plugins/output.h \
    plugins/input.h \
    
LOCAL_CFLAGS := -O2 -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC

LOCAL_MODULE := output_udp

include $(BUILD_SHARED_LIBRARY)

# build mjpg_streamer
include $(CLEAR_VARS)

LOCAL_MODULE := mjpg_streamer

LOCAL_SRC_FILES := utils.c mjpg_streamer.c

LOCAL_C_INCLUDES := utils.h mjpg_streamer.h

LOCAL_CFLAGS := -O2 -DLINUX -D_GNU_SOURCE -Wall

LOCAL_SHARED_LIBRARIES := libjpeg

include $(BUILD_EXECUTABLE)
