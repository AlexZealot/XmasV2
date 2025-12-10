/**
 * @file main.cpp
 * @Алексей Казаков Zealot (akazakov.zealot@gmail.com)
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

extern "C" void app_main() {
  //DEBUG_START();
  esp_err_t ret = nvs_flash_init();

  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);   
  d = new NimBLEDriver();
}