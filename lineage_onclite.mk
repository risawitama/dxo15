#
# Copyright (C) 2019-2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/product_launched_with_p.mk)

# Inherit from onclite device
$(call inherit-product, device/xiaomi/onclite/device.mk)

# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_NAME := lineage_onclite
PRODUCT_DEVICE := onclite
PRODUCT_BRAND := Xiaomi
PRODUCT_MODEL := Redmi 7
PRODUCT_MANUFACTURER := Xiaomi
PRODUCT_GMS_CLIENTID_BASE := android-xiaomi

MATRIXX_MAINTAINER := Hadad
MATRIXX_CHIPSET := MSM8953
MATRIXX_BATTERY := 4000mAh
MATRIXX_DISPLAY := 720X1520

TARGET_BOOT_ANIMATION_RES := 720

SELINUX_IGNORE_NEVERALLOWS := true
