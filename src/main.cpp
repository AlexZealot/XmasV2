/**
 * @file main.cpp
 * @Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief точка входа в приложение
 * @version 0.1
 * @date 07-12-2025
 * 
 * @copyright Copyright (c) 2025
 */
# include "debug.h"
# include "esp_log.h"
# include "FreeRTOS/FreeRTOS.h"

# include "SystemCore.h"
# include "WiFi/WiFiMan.h"

Listener* wifi;

extern "C" void app_main() {
  DEBUG_START();
  wifi = new WiFiMan();
}