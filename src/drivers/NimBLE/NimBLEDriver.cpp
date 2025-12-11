/**
 * @file NimBLEDriver.cpp
 * @@author Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief реализация класса NimBLEDriver
 * @version 0.1
 * @date 11-12-2025
 * 
 * @copyright Copyright (c) 2025
 */
# include "NimBLEDriver.h"

int NimBLEDriver::scanCallback(ble_gap_event *event, void *arg) {
  auto driver = static_cast<NimBLEDriver*>(arg);
  switch (event->type) {
    case BLE_GAP_EVENT_DISC: {
      if (driver->mScanCallback != nullptr) {
        driver->mScanCallback->onScanResult(&event->disc);
      }
      break;
    }
      
    case BLE_GAP_EVENT_DISC_COMPLETE: {
      xEventGroupClearBits(driver->mBLEEventGroup, NIM_BLE_BITS::SCAN_ACTIVE);
      break;
    }
  }

  return 0;
}

int NimBLEDriver::startScan(int duration_ms) {
  constexpr int MAX_RETRY = 10;
  int res = ble_gap_disc(0, duration_ms, &mScanParams, scanCallback, this);
  int cnt = 0;
  while (res != 0 && cnt < MAX_RETRY) {
    vTaskDelay(pdMS_TO_TICKS(100));
    res = ble_gap_disc(0, duration_ms, &mScanParams, scanCallback, this);
  }
  if (res == 0) {
    xEventGroupSetBits(mBLEEventGroup, NIM_BLE_BITS::SCAN_ACTIVE);
  }
  return res;
}

void NimBLEDriver::stopScan() {
  ble_gap_disc_cancel();
  xEventGroupClearBits(mBLEEventGroup, NIM_BLE_BITS::SCAN_ACTIVE);
}

static void ble_host_task(void *param) {
  nimble_port_run();
  nimble_port_freertos_deinit();
}

NimBLEDriver::NimBLEDriver(const char* deviceName) {
  mBLEEventGroup = xEventGroupCreate();
  mScanParams = {
    .itvl = 0x50,
    .window = 0x30,
    .filter_policy = 0,
    .limited = 0,
    .passive = 0,
    .filter_duplicates = 0,
    .disable_observer_mode = 0,
  };
  nimble_port_init();
  nimble_port_freertos_init(ble_host_task);
  if (deviceName != nullptr) {
    ble_svc_gap_device_name_set(deviceName);
  }
}

NimBLEDriver& NimBLEDriver::getInstance(const char* deviceName) {
  static NimBLEDriver instance(deviceName);
  return instance;
}

void NimBLEDriver::setScanCallback(NimScan *callback) {
  mScanCallback = callback;
}

void NimBLEDriver::setScanInterval(uint16_t interval) {
  mScanParams.itvl = interval / 0.625;
}

void NimBLEDriver::setScanWindow(uint16_t window) {
  mScanParams.window = window / 0.625;
}

void NimBLEDriver::setActiveScan(bool active) {
  mScanParams.passive = active ? 0 : 1;
}

void NimBLEDriver::setFilterDuplicates(bool filter) {
  mScanParams.filter_duplicates = filter ? 1 : 0;
}