/**
 * @file main.cpp
 * @author Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief точка входа в приложение
 * @version 0.1
 * @date 07-12-2025
 * 
 * @copyright Copyright (c) 2025
 */

# include "SystemCore.h"
# include "WiFi/WiFiMan.h"

# include "drivers/NimBLE/NimBLEDriver.h"

NimBLEDriver *d;
class TestScan: public NimScan {
  public:
    void onScanResult(const ble_gap_disc_desc *disc) override {
      static char data[21*3+2+21] = {0,};
      if (disc->length_data == 21 && disc->data[2] == 0x95 && disc->data[3] == 0xFE) {
        sprintf(
          data, 
          "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X [%02X:%02X:%02X:%02X:%02X:%02X]",
          disc->data[0],
          disc->data[1],
          disc->data[2],
          disc->data[3],
          disc->data[4],
          disc->data[5],
          disc->data[6],
          disc->data[7],
          disc->data[8],
          disc->data[9],
          disc->data[10],
          disc->data[11],
          disc->data[12],
          disc->data[13],
          disc->data[14],
          disc->data[15],
          disc->data[16],
          disc->data[17],
          disc->data[18],
          disc->data[19],
          disc->data[20],
          disc->addr.val[5],
          disc->addr.val[4],
          disc->addr.val[3],
          disc->addr.val[2],
          disc->addr.val[1],
          disc->addr.val[0]);
        ESP_LOGI("BLE", "[%06u] %s", esp_get_free_heap_size(), data);
      }
    }
} *testScan;

extern "C" void app_main() {
  //DEBUG_START();
  esp_err_t ret = nvs_flash_init();

  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);   
  auto &d = NimBLEDriver::getInstance("ESP");
  testScan = new TestScan();
  d.setScanCallback(testScan);
  d.startScan();
}