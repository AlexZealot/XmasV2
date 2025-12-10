# include "NimBLEDriver.h"

# define TAG "BLE"

// Callback при обнаружении устройства
static int ble_gap_event_cb(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
        case BLE_GAP_EVENT_DISC: {
            // Обнаружено BLE устройство
            struct ble_gap_disc_desc *disc = &event->disc;
            
            // Базовая информация
            char addr_str[18];
            sprintf(addr_str, "%02x:%02x:%02x:%02x:%02x:%02x",
                    disc->addr.val[5], disc->addr.val[4], disc->addr.val[3],
                    disc->addr.val[2], disc->addr.val[1], disc->addr.val[0]);
            
            ESP_LOGI(TAG, "Device found: %s, RSSI: %d dBm", 
                    addr_str, disc->rssi);
            
            // Парсим advertisement data для имени
            char name[32] = {0};
            bool name_found = false;
            
            const uint8_t *data = disc->data;
            uint8_t len = disc->length_data;
            uint8_t index = 0;
            
            struct ble_hs_adv_fields fields;
            int rc = ble_hs_adv_parse_fields(&fields, event->disc.data, 
                                            event->disc.length_data);
            if (rc == 0) {
                if (fields.name != NULL) {
                    ESP_LOGI(TAG, "  Name: %.*s", fields.name_len, fields.name);
                }
                if (fields.tx_pwr_lvl_is_present) {
                    ESP_LOGI(TAG, "  TX Power: %d dBm", fields.tx_pwr_lvl);
                }
            }
            // while (index < len) {
            //     uint8_t field_len = data[index];
            //     if (field_len == 0) break;
                
            //     uint8_t field_type = data[index + 1];
                
            //     if (field_type == BLE_HCI_ADV_TYPE_COMP_NAME) {
            //         uint8_t name_len = field_len - 1;
            //         if (name_len > 31) name_len = 31;
            //         memcpy(name, &data[index + 2], name_len);
            //         name[name_len] = '\0';
            //         name_found = true;
            //         break;
            //     } else if (field_type == BLE_HCI_ADV_TYPE_INCOMP_NAME) {
            //         uint8_t name_len = field_len - 1;
            //         if (name_len > 31) name_len = 31;
            //         memcpy(name, &data[index + 2], name_len);
            //         name[name_len] = '\0';
            //         name_found = true;
            //         break;
            //     }
                
            //     index += field_len + 1;
            // }
            
            // if (name_found) {
            //     ESP_LOGI(TAG, "  Name: %s", name);
            // }
            
            // Можно получить дополнительные данные
            ESP_LOGI(TAG, "  Data length: %d bytes", disc->length_data);
            
            break;
        }
        
        case BLE_GAP_EVENT_DISC_COMPLETE:
            ESP_LOGI(TAG, "Scan complete");
            // Можно перезапустить сканирование
            // ble_gap_disc(0, BLE_HS_FOREVER, NULL, ble_gap_event_cb, NULL);
            break;
    }
    
    return 0;
}

// Запуск сканирования
void start_ble_scan() {
    struct ble_gap_disc_params disc_params = {0};
    
    // Настройки сканирования
    disc_params.filter_duplicates = 0;  // 0 = показывать все дубликаты
    disc_params.passive = 0;           // 0 = активное сканирование (запрос scan response)
    disc_params.itvl = 0x50;           // Scan interval (0.625ms units)
    disc_params.window = 0x30;         // Scan window (0.625ms units)
    disc_params.filter_policy = 0;     // Accept all advertisements
    disc_params.limited = 0;           // Not limited discovery
    
    // Начинаем сканирование
    int rc = ble_gap_disc(0, BLE_HS_FOREVER, &disc_params, 
                          ble_gap_event_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error starting scan: %d", rc);
    } else {
        ESP_LOGI(TAG, "BLE scan started");
    }
}

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
  vTaskDelay(pdMS_TO_TICKS(1000));
  start_ble_scan();
}