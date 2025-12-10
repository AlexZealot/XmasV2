# include "NimBLEDriver.h"

# define TAG "BLE"

void ble_host_task(void *param) {
  ESP_LOGI(TAG, "BLE Host Task Started");
  nimble_port_run();
  nimble_port_freertos_deinit();
}

#define CALL_FUNC(f)    ESP_LOGI(TAG, #f ": %s", esp_err_to_name(f))

NimBLEDriver::NimBLEDriver() {
  // // ESP_LOGI(TAG, "Heap: %u", esp_get_free_heap_size());
  // // CALL_FUNC(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));  

  // // 1. Проверяем состояние
  // esp_bt_controller_status_t status = esp_bt_controller_get_status();
  // ESP_LOGI(TAG, "Controller status: %d", status);
  
  // if (status == ESP_BT_CONTROLLER_STATUS_IDLE) {
  //   // Контроллер не инициализирован - инициализируем
  //   ESP_LOGI(TAG, "Initializing controller from idle...");
  //   CALL_FUNC(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    
  //   esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  //   bt_cfg.mode = ESP_BT_MODE_BLE;
  //   bt_cfg.ble_max_conn = 3;
    
  //   CALL_FUNC(esp_bt_controller_init(&bt_cfg));
  //   CALL_FUNC(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    
  // } else if (status == ESP_BT_CONTROLLER_STATUS_ENABLED) {
  //   // Контроллер уже включен - ПРОПУСКАЕМ инициализацию!
  //   ESP_LOGI(TAG, "Controller already enabled, skipping init");
    
  // } else if (status == ESP_BT_CONTROLLER_STATUS_INITED) {
  //   // Контроллер инициализирован, но не включен
  //   ESP_LOGI(TAG, "Controller initialized but not enabled");
  //   CALL_FUNC(esp_bt_controller_enable(ESP_BT_MODE_BLE));
  // }

  // // CALL_FUNC(esp_nimble_hci_init());
  // // ESP_LOGI(TAG, "Heap (esp_nimble_hci_init): %u", esp_get_free_heap_size());
  // // vTaskDelay(pdMS_TO_TICKS(100));

  // ESP_LOGI(TAG, "BT controller status: %u", esp_bt_controller_get_status());

  // // Инициализируем NimBLE host
  // // CALL_FUNC(nimble_port_init());
  // ESP_LOGI(TAG, "Heap (nimble_port_init): %u", esp_get_free_heap_size());

  // // Настраиваем параметры хоста
  // ble_hs_cfg.sm_bonding = 0;      // No bonding
  // ble_hs_cfg.sm_mitm = 0;         // No MITM protection
  // ble_hs_cfg.sm_sc = 0;           // No secure connections
  // ble_hs_cfg.sm_our_key_dist = 0;
  // ble_hs_cfg.sm_their_key_dist = 0;

  // // Устанавливаем device name
  // ble_svc_gap_device_name_set("ESP32-Scanner");

  // // Запускаем NimBLE порт
  // nimble_port_freertos_init(ble_host_task);
  // ESP_LOGI(TAG, "Heap (nimble_port_freertos_init): %u", esp_get_free_heap_size());
  CALL_FUNC(nimble_port_init());
  nimble_port_freertos_init(ble_host_task);
  ESP_LOGI(TAG, "Free mem: %u", esp_get_free_heap_size());
}