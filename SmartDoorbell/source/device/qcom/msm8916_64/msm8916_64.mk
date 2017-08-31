DEVICE_PACKAGE_OVERLAYS := device/qcom/msm8916_64/overlay

TARGET_USES_QCOM_BSP := true
# Add QC Video Enhancements flag
TARGET_ENABLE_QC_AV_ENHANCEMENTS := true

DEVICE_PACKAGE_OVERLAYS := device/qcom/msm8916_64/overlay

#QTIC flag
-include $(QCPATH)/common/config/qtic-config.mk

# media_profiles and media_codecs xmls for 8916
ifeq ($(TARGET_ENABLE_QC_AV_ENHANCEMENTS), true)
PRODUCT_COPY_FILES += device/qcom/msm8916_32/media/media_profiles_8916.xml:system/etc/media_profiles.xml \
                      device/qcom/msm8916_32/media/media_codecs_8916.xml:system/etc/media_codecs.xml \
                      device/qcom/msm8916_32/media/media_codecs_performance_8916_64.xml:system/etc/media_codecs_performance.xml \
                      device/qcom/msm8916_32/media/media_codecs_performance_8929.xml:system/etc/media_codecs_performance_8929.xml \
                      device/qcom/msm8916_32/media/media_codecs_8939.xml:system/etc/media_codecs_8939.xml \
                      device/qcom/msm8916_32/media/media_codecs_performance_8916_64_8939.xml:system/etc/media_codecs_performance_8939.xml \
                      device/qcom/msm8916_32/media/media_codecs_8929.xml:system/etc/media_codecs_8929.xml
endif

TARGET_USES_QCA_NFC := other
TARGET_USES_NQ_NFC := false

PRODUCT_PROPERTY_OVERRIDES += \
           dalvik.vm.heapgrowthlimit=128m
$(call inherit-product, device/qcom/common/common64.mk)

PRODUCT_NAME := msm8916_64
PRODUCT_DEVICE := msm8916_64
PRODUCT_BRAND := Android
PRODUCT_MODEL := MSM8916 for arm64

ifeq ($(strip $(TARGET_USES_QTIC)),true)
# font rendering engine feature switch
-include $(QCPATH)/common/config/rendering-engine.mk
ifneq (,$(strip $(wildcard $(PRODUCT_RENDERING_ENGINE_REVLIB))))
    MULTI_LANG_ENGINE := REVERIE
#    MULTI_LANG_ZAWGYI := REVERIE
endif
endif

PRODUCT_BOOT_JARS += qcom.fmradio

#PRODUCT_BOOT_JARS += vcard
PRODUCT_BOOT_JARS += tcmiface
PRODUCT_BOOT_JARS += qcmediaplayer

ifneq ($(strip $(QCPATH)),)
PRODUCT_BOOT_JARS += com.qti.dpmframework
PRODUCT_BOOT_JARS += dpmapi
PRODUCT_BOOT_JARS += oem-services
PRODUCT_BOOT_JARS += com.qti.location.sdk
endif

PRODUCT_BOOT_JARS += WfdCommon

# default is nosdcard, S/W button enabled in resource
PRODUCT_CHARACTERISTICS := nosdcard

#Android EGL implementation
PRODUCT_PACKAGES += libGLES_android

PRODUCT_PACKAGES += \
    libqcomvisualizer \
    libqcompostprocbundle \
    libqcomvoiceprocessing

# Audio configuration file
PRODUCT_COPY_FILES += \
    device/qcom/msm8916_32/audio_policy.conf:system/etc/audio_policy.conf \
    device/qcom/msm8916_32/audio_effects.conf:system/vendor/etc/audio_effects.conf \
    device/qcom/msm8916_32/mixer_paths_mtp.xml:system/etc/mixer_paths_mtp.xml \
    device/qcom/msm8916_32/mixer_paths_sbc.xml:system/etc/mixer_paths_sbc.xml \
    device/qcom/msm8916_32/mixer_paths_qrd_skuh.xml:system/etc/mixer_paths_qrd_skuh.xml \
    device/qcom/msm8916_32/mixer_paths_qrd_skui.xml:system/etc/mixer_paths_qrd_skui.xml \
    device/qcom/msm8916_32/mixer_paths_qrd_skuhf.xml:system/etc/mixer_paths_qrd_skuhf.xml \
    device/qcom/msm8916_32/mixer_paths_wcd9306.xml:system/etc/mixer_paths_wcd9306.xml \
    device/qcom/msm8916_32/mixer_paths_skuk.xml:system/etc/mixer_paths_skuk.xml \
    device/qcom/msm8916_32/mixer_paths_skul.xml:system/etc/mixer_paths_skul.xml \
    device/qcom/msm8916_32/mixer_paths.xml:system/etc/mixer_paths.xml \
    device/qcom/msm8916_32/sound_trigger_mixer_paths.xml:system/etc/sound_trigger_mixer_paths.xml \
    device/qcom/msm8916_32/sound_trigger_mixer_paths_wcd9306.xml:system/etc/sound_trigger_mixer_paths_wcd9306.xml \
    device/qcom/msm8916_32/sound_trigger_platform_info.xml:system/etc/sound_trigger_platform_info.xml \
    device/qcom/msm8916_64/aanc_tuning_mixer.txt:system/etc/aanc_tuning_mixer.txt \
    device/qcom/msm8916_64/bootanimation.zip:system/media/bootanimation.zip

# Boot animation
PRODUCT_COPY_FILES += \
    device/qcom/msm8916_64/bootanimation.zip:system/media/bootanimation.zip

# MIDI feature
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.software.midi.xml:system/etc/permissions/android.software.midi.xml

#ANT+ stack
PRODUCT_PACKAGES += \
    AntHalService \
    libantradio \
    antradio_app
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    camera2.portability.force_api=1
PRODUCT_PACKAGES += wcnss_service

# MSM IRQ Balancer configuration file
PRODUCT_COPY_FILES += \
    device/qcom/msm8916_64/msm_irqbalance.conf:system/vendor/etc/msm_irqbalance.conf

#wlan driver
PRODUCT_COPY_FILES += \
    device/qcom/msm8916_64/WCNSS_qcom_cfg.ini:system/etc/wifi/WCNSS_qcom_cfg.ini \
    device/qcom/msm8916_64/WCNSS_wlan_dictionary.dat:persist/WCNSS_wlan_dictionary.dat \
    device/qcom/msm8916_64/WCNSS_qcom_wlan_nv.bin:persist/WCNSS_qcom_wlan_nv.bin

PRODUCT_PACKAGES += \
    wpa_supplicant_overlay.conf \
    p2p_supplicant_overlay.conf

ifeq ($(TARGET_USES_QCA_NFC),true)
NFC_D := true

ifeq ($(NFC_D), true)
    PRODUCT_PACKAGES += \
        libqnfc-nci \
        libqnfc_nci_jni \
        nfc_nci.msm8916 \
        QNfc \
        Tag \
        com.android.nfc_extras \
        com.android.nfc.helper \
        SmartcardService \
        org.simalliance.openmobileapi \
        org.simalliance.openmobileapi.xml \
        libassd
else
    PRODUCT_PACKAGES += \
    libnfc-nci \
    libnfc_nci_jni \
    nfc_nci.msm8916 \
    NfcNci \
    Tag \
    com.android.nfc_extras
endif

# file that declares the MIFARE NFC constant
# Commands to migrate prefs from com.android.nfc3 to com.android.nfc
# NFC access control + feature files + configuration
PRODUCT_COPY_FILES += \
        packages/apps/Nfc/migrate_nfc.txt:system/etc/updatecmds/migrate_nfc.txt \
        frameworks/native/data/etc/com.nxp.mifare.xml:system/etc/permissions/com.nxp.mifare.xml \
        frameworks/native/data/etc/com.android.nfc_extras.xml:system/etc/permissions/com.android.nfc_extras.xml \
        frameworks/native/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml \
        frameworks/native/data/etc/android.hardware.nfc.hce.xml:system/etc/permissions/android.hardware.nfc.hce.xml
else
ifeq ($(TARGET_USES_NQ_NFC),true)
PRODUCT_PACKAGES += \
    nfc.msm8916_64 \
    NfcNci \
    libnfc-nci \
    libnfc_nci_jni \
    nfc_nci.nqx.default \
    Tag \
    com.android.nfc_extras

PRODUCT_COPY_FILES += \
    packages/apps/Nfc/migrate_nfc.txt:system/etc/updatecmds/migrate_nfc.txt \
    frameworks/native/data/etc/com.nxp.mifare.xml:system/etc/permissions/com.nxp.mifare.xml \
    frameworks/native/data/etc/com.android.nfc_extras.xml:system/etc/permissions/com.android.nfc_extras.xml \
    frameworks/native/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml \
    frameworks/native/data/etc/android.hardware.nfc.hce.xml:system/etc/permissions/android.hardware.nfc.hce.xml
endif # TARGET_USES_NQ_NFC
endif # TARGET_USES_QCA_NFC

# Defined the locales
PRODUCT_LOCALES += th_TH vi_VN tl_PH hi_IN ar_EG ru_RU tr_TR pt_BR bn_IN mr_IN ta_IN te_IN zh_HK \
        in_ID my_MM km_KH sw_KE uk_UA pl_PL sr_RS sl_SI fa_IR kn_IN ml_IN ur_IN gu_IN or_IN

# Add the overlay path
ifeq ($(strip $(TARGET_USES_QTIC)),true)
PRODUCT_PACKAGE_OVERLAYS := $(QCPATH)/qrdplus/Extension/res-overlay \
        $(PRODUCT_PACKAGE_OVERLAYS)
endif

PRODUCT_SUPPORTS_VERITY := true
PRODUCT_SYSTEM_VERITY_PARTITION := /dev/block/bootdevice/by-name/system

# Sensor HAL conf file
PRODUCT_COPY_FILES += \
    device/qcom/msm8916_64/sensors/hals.conf:system/etc/sensors/hals.conf

GMS_ENABLE_OPTIONAL_MODULES := false

# TS Chengx 20170705 add for smart doorbell begin 
PRODUCT_COPY_FILES += \
	external/mjpg-streamer/www/bodybg.gif:data/www/bodybg.gif \
	external/mjpg-streamer/www/cambozola.jar:data/www/cambozola.jar \
	external/mjpg-streamer/www/control.htm:data/www/control.htm \
	external/mjpg-streamer/www/example.jpg:data/www/example.jpg \
	external/mjpg-streamer/www/favicon.ico:data/www/favicon.ico \
	external/mjpg-streamer/www/favicon.png:data/www/favicon.png \
	external/mjpg-streamer/www/fix.css:data/www/fix.css \
	external/mjpg-streamer/www/functions.js:data/www/functions.js \
	external/mjpg-streamer/www/index.html:data/www/index.html \
	external/mjpg-streamer/www/java_control.html:data/www/java_control.html \
	external/mjpg-streamer/www/java.html:data/www/java.html \
	external/mjpg-streamer/www/javascript.html:data/www/javascript.html \
	external/mjpg-streamer/www/javascript_motiondetection.html:data/www/javascript_motiondetection.html \
	external/mjpg-streamer/www/javascript_simple.html:data/www/javascript_simple.html \
	external/mjpg-streamer/www/java_simple.html:data/www/java_simple.html \
	external/mjpg-streamer/www/jquery.js:data/www/jquery.js \
	external/mjpg-streamer/www/jquery.rotate.js:data/www/jquery.rotate.js \
	external/mjpg-streamer/www/JQuerySpinBtn.css:data/www/JQuerySpinBtn.css \
	external/mjpg-streamer/www/JQuerySpinBtn.js:data/www/JQuerySpinBtn.js \
	external/mjpg-streamer/www/jquery.ui.core.min.js:data/www/jquery.ui.core.min.js \
	external/mjpg-streamer/www/jquery.ui.custom.css:data/www/jquery.ui.custom.css \
	external/mjpg-streamer/www/jquery.ui.tabs.min.js:data/www/jquery.ui.tabs.min.js \
	external/mjpg-streamer/www/jquery.ui.widget.min.js:data/www/jquery.ui.widget.min.js \
	external/mjpg-streamer/www/LICENSE.txt:data/www/LICENSE.txt \
	external/mjpg-streamer/www/rotateicons.png:data/www/rotateicons.png \
	external/mjpg-streamer/www/sidebarbg.gif:data/www/sidebarbg.gif \
	external/mjpg-streamer/www/spinbtn_updn.gif:data/www/spinbtn_updn.gif \
	external/mjpg-streamer/www/static.html:data/www/static.html \
	external/mjpg-streamer/www/static_simple.html:data/www/static_simple.html \
	external/mjpg-streamer/www/stream.html:data/www/stream.html \
	external/mjpg-streamer/www/stream_simple.html:data/www/stream_simple.html \
	external/mjpg-streamer/www/style.css:data/www/style.css \
	external/mjpg-streamer/www/videolan.html:data/www/videolan.html
# TS Chengx 20170705 add for smart doorbell end 

