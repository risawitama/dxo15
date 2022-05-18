/*
 * Copyright (C) 2017-2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <dlfcn.h>

#define LOG_TAG "vendor.lineage.livedisplay@2.0-service.xiaomi_onc"

#include <android-base/logging.h>
#include <binder/ProcessState.h>
#include <hidl/HidlTransportSupport.h>

#include "PictureAdjustment.h"

using android::OK;
using android::sp;
using android::status_t;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

using ::vendor::lineage::livedisplay::V2_0::sdm::PictureAdjustment;

int main() {
  // Vendor backend
  void *libHandle = nullptr;
  const char *libName = "libsdm-disp-vndapis.so";
  int32_t (*disp_api_init)(uint64_t *, uint32_t) = nullptr;
  int32_t (*disp_api_deinit)(uint64_t, uint32_t) = nullptr;
  uint64_t cookie = 0;

  // HIDL frontend
  sp<PictureAdjustment> pa;

  status_t status = OK;

  android::ProcessState::initWithDriver("/dev/vndbinder");

  LOG(INFO) << "LiveDisplay HAL service is starting.";

  libHandle = dlopen(libName, RTLD_NOW);

  if (libHandle == nullptr) {
    LOG(ERROR) << "Failed to load SDM display lib, exiting.";
    goto shutdown;
  }

  disp_api_init = reinterpret_cast<int32_t (*)(uint64_t *, uint32_t)>(
      dlsym(libHandle, "disp_api_init"));
  if (disp_api_init == nullptr) {
    LOG(ERROR) << "Can not get disp_api_init from " << libName << " ("
               << dlerror() << ")";
    goto shutdown;
  }

  disp_api_deinit = reinterpret_cast<int32_t (*)(uint64_t, uint32_t)>(
      dlsym(libHandle, "disp_api_deinit"));
  if (disp_api_deinit == nullptr) {
    LOG(ERROR) << "Can not get disp_api_deinit from " << libName << " ("
               << dlerror() << ")";
    goto shutdown;
  }

  status = disp_api_init(&cookie, 0);
  if (status != OK) {
    LOG(ERROR) << "Can not initialize " << libName << " (" << status << ")";
    goto shutdown;
  }

  pa = new PictureAdjustment(libHandle, cookie);
  if (pa == nullptr) {
    LOG(ERROR) << "Can not create an instance of LiveDisplay HAL "
                  "PictureAdjustment Iface, "
                  "exiting.";
    goto shutdown;
  }

  if (!pa->isSupported()) {
    // Backend isn't ready yet, so restart and try again
    goto shutdown;
  }

  configureRpcThreadpool(1, true /*callerWillJoin*/);

  if (pa->isSupported()) {
    status = pa->registerAsService();
    if (status != OK) {
      LOG(ERROR) << "Could not register service for LiveDisplay HAL "
                    "PictureAdjustment Iface ("
                 << status << ")";
      goto shutdown;
    }
  }

  LOG(INFO) << "LiveDisplay HAL service is ready.";
  joinRpcThreadpool();
  // Should not pass this line

shutdown:
  // Cleanup what we started
  if (disp_api_deinit != nullptr) {
    disp_api_deinit(cookie, 0);
  }

  if (libHandle != nullptr) {
    dlclose(libHandle);
  }

  // In normal operation, we don't expect the thread pool to shutdown
  LOG(ERROR) << "LiveDisplay HAL service is shutting down.";
  return 1;
}
