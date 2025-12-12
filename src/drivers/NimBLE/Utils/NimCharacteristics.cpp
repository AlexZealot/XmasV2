/**
 * @file NimCharacteristics.cpp
 * @author Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief реализация класса NimCharacteristics
 * @version 0.1
 * @date 11-12-2025
 * 
 * @copyright Copyright (c) 2025
 * 
 */
# include "NimCharacteristics.h"

NimCharacteristics::NimCharacteristics(NimBLEUUID &UUID, NimCharType charType) {
	mCharType = charType;
}

int NimCharacteristics::characteristicsCallback(uint16_t conn_handle, uint16_t attr_handle, ble_gatt_access_ctxt *ctxt, void *arg) {
  // ESP_LOGI(TAG, "Read request on handle %d", attr_handle);
  
  // if (attr_handle == write_chr_handle) {
  //     // Возвращаем текущее значение
  //     const char *value = "Current value";
  //     os_mbuf_append(ctxt->om, value, strlen(value));
  //     return 0;
  // }
  
  // return BLE_ATT_ERR_UNLIKELY;
}