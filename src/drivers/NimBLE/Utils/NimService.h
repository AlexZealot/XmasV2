/**
 * @file NimService.h
 * @author Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief 
 * @version 0.1
 * @date 11-12-2025
 * 
 * @copyright Copyright (c) 2025
 */
# pragma once

# include "esp_log.h"
# include "nimble/nimble_port.h"
# include "host/ble_hs.h"
# include "NimCharacteristics.h"

/**
 * @class NimService
 * @brief сервис NimBLE
 */
class NimService {
  public:
    NimService(NimBLEUUID& UUID);
  private:
    /// @cond
    NimService() = delete;
    NimService(const NimService&) = delete;
    NimService(const NimService&&) = delete;
    NimService& operator = (const NimService&) = delete;
    NimService& operator = (const NimService&&) = delete;
    /// @endcond

    uint16_t mHandle = 0; /**< хэндл сервиса */
    ble_gatt_svc_def mDefinition; /**< структура определения сервиса для ESP-IDF */
};