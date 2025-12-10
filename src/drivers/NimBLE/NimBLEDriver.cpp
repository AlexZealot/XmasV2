# include "NimBLEDriver.h"

# define TAG "BLE"

void ble_host_task(void *param) {
  ESP_LOGI(TAG, "BLE Host Task Started");
  nimble_port_run();
  nimble_port_freertos_deinit();
}

#define CALL_FUNC(f)    ESP_LOGI(TAG, #f ": %s", esp_err_to_name(f))

NimBLEDriver::NimBLEDriver() {
  CALL_FUNC(nimble_port_init());
  nimble_port_freertos_init(ble_host_task);
  ESP_LOGI(TAG, "Free mem: %u", esp_get_free_heap_size());
}