/**
 * @file WiFiMan.cpp
 * @Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief реализация класса WiFiMan
 * @version 0.1
 * @date 08-12-2025
 * 
 * @copyright Copyright (c) 2025
 */
# include "WiFiMan.h"

WiFiMan::WiFiMan() {
  DEBUG_ENABLE_TAG_WIFI();
  DEBUG_CHECK_ESP_VAL_WIFI(esp_netif_init());
  DEBUG_CHECK_ESP_VAL_WIFI(esp_event_loop_create_default());

  softAP(DEFAULT_SSID, DEFAULT_PASS);
}

void WiFiMan::softAP(const char* ssid, const char* pass, uint8_t channel) {
  // Инициализация сетевого интерфейса AP
  esp_netif_create_default_wifi_ap();
  
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  DEBUG_CHECK_ESP_VAL_WIFI(esp_wifi_init(&cfg));

  DEBUG_CHECK_ESP_VAL_WIFI(
    esp_event_handler_instance_register(
      WIFI_EVENT,
      ESP_EVENT_ANY_ID,
      &wifiEventHandler,
      this,
      nullptr
    )
  );

  // Конфигурация точки доступа
  wifi_config_t wifi_config = {
	  .ap = {
      .ssid = {0,},
      .password = {0,},
      .ssid_len = 0,
      .channel = channel,			
      .authmode = WIFI_AUTH_WPA2_PSK,
      .ssid_hidden = 0,				
      .max_connection = 4,	
      .beacon_interval = 100,
      .csa_count = 3,
      .dtim_period = 1,
      .pairwise_cipher = WIFI_CIPHER_TYPE_TKIP_CCMP,
      .ftm_responder = false,
      .pmf_cfg = {
        .capable = false,
        .required = false,
      },
      .sae_pwe_h2e = WPA3_SAE_PWE_UNSPECIFIED,
      .transition_disable = 0,
      .sae_ext = 0,
      .bss_max_idle_cfg = {
        .period = 0,
        .protected_keep_alive = false
      },
      .gtk_rekey_interval = 0,
	  },
  };
	memcpy(wifi_config.ap.ssid, ssid, strlen(ssid));
  
  // Если пароль пустой, то открытая сеть
  if (pass == nullptr || strlen(pass) == 0) {
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  } else {
		memcpy(wifi_config.ap.password, pass, strlen(pass));
	}

  // Установка режима и конфигурации
  DEBUG_CHECK_ESP_VAL_WIFI(esp_wifi_set_mode(WIFI_MODE_AP));
  DEBUG_CHECK_ESP_VAL_WIFI(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  DEBUG_CHECK_ESP_VAL_WIFI(esp_wifi_start());
}

void WiFiMan::wifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  if (event_base == WIFI_EVENT) {
      switch (event_id) {
        case WIFI_EVENT_AP_STACONNECTED: {
          DEBUG_PRINT_I_WIFI("Device connected");
          break;
        }
        case WIFI_EVENT_AP_STADISCONNECTED: {
          DEBUG_PRINT_I_WIFI("Device disconnected");
          break;
        }
        case WIFI_EVENT_AP_START:
          DEBUG_PRINT_I_WIFI("AP start");
          break;
        case WIFI_EVENT_AP_STOP:
          DEBUG_PRINT_I_WIFI("AP stop");
          break;
        default:
          break;
    }
  }
}

void WiFiMan::onEvent(MsgID msg, const void* data) {

}