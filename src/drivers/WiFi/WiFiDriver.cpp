/**
 * @file WiFiDriver.cpp
 * @author Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief реализация класса WiFiDriver
 * @version 0.1
 * @date 09-12-2025
 * 
 * @copyright Copyright (c) 2025
 * 
 */
# include "WiFiDriver.h"

/**
 * @namespace WIFI_EG_BITS
 * @brief пространство имен для битов событий WiFi
 */
namespace WIFI_EG_BITS {
  constexpr EventBits_t WIFI_HAS_AP_BIT = (1 << 0); /**< включена ли AP */
  constexpr EventBits_t WIFI_STA_CONNECTED_BIT = (1 << 1); /**< подключились ли к AP */
  constexpr EventBits_t WIFI_STA_FAIL_BIT = (1 << 2); /**< не удалось подключиться к AP */
  constexpr EventBits_t WIFI_HAS_STA_BIT = (1 << 3); /**< включен ли STA */
}

WiFiDriver::WiFiDriver() {
  mWiFiEventGroup = xEventGroupCreate();
  assert(mWiFiEventGroup != nullptr);
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_loop_create_default());
  ESP_ERROR_CHECK(
    esp_event_handler_instance_register(
      WIFI_EVENT,
      ESP_EVENT_ANY_ID,
      &wifiEventHandler,
      this,
      &mEHInstance
    )
  );
  ESP_ERROR_CHECK(
    esp_event_handler_instance_register(
      IP_EVENT,
      IP_EVENT_STA_GOT_IP,
      &wifiEventHandler,
      this,
      &mEHInstanceIP
    )
  );
}

WiFiDriver::~WiFiDriver() {
  stopAP();
  if (mEHInstance != nullptr) {
    esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, mEHInstance);
    mEHInstance = nullptr;
  }
  if (mEHInstanceIP != nullptr) {
    esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, mEHInstanceIP);
    mEHInstanceIP = nullptr;
  }
}


bool WiFiDriver::softAP(const char* ssid, const char* pass, uint8_t channel) {
  if (isApActive()) {
    stopAP();
  }
  // Инициализация сетевого интерфейса AP
  mAPNetif = esp_netif_create_default_wifi_ap();
  if (mAPNetif == nullptr) {
    return false;
  }
  
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  if(esp_wifi_init(&cfg) != ESP_OK) {
    esp_netif_destroy(mAPNetif);
    mAPNetif = nullptr;
    return false;
  }

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

  wifi_mode_t curMode;
  esp_wifi_get_mode(&curMode);
  wifi_mode_t oldMode = curMode;
  switch (curMode) {
    case WIFI_MODE_APSTA:
    case WIFI_MODE_STA: {
      curMode = WIFI_MODE_APSTA;
      break;
    }
    // case WIFI_MODE_NAN:
    case WIFI_MODE_NULL: {
      curMode = WIFI_MODE_AP;
      break;
    }
    default: {}
  }

  // Установка режима и конфигурации
  if(esp_wifi_set_mode(curMode) == ESP_OK) {
    if(esp_wifi_set_config(WIFI_IF_AP, &wifi_config) == ESP_OK) {
      if(esp_wifi_start() == ESP_OK) {
        xEventGroupSetBits(mWiFiEventGroup, WIFI_EG_BITS::WIFI_HAS_AP_BIT);
        return true;
      } else {
        esp_wifi_set_mode(oldMode);
        esp_netif_destroy(mAPNetif);
        mAPNetif = nullptr;
        return false;  
      }
    } else {
      esp_wifi_set_mode(oldMode);
      esp_netif_destroy(mAPNetif);
      mAPNetif = nullptr;
      return false;
    }
  } else {
    esp_wifi_set_mode(oldMode);
    esp_netif_destroy(mAPNetif);
    mAPNetif = nullptr;
    return false;
  }
}

bool WiFiDriver::isApActive() {
  return (xEventGroupGetBits(mWiFiEventGroup) & WIFI_EG_BITS::WIFI_HAS_AP_BIT) != 0;
}

void WiFiDriver::wifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  auto wifiDriver = static_cast<WiFiDriver*>(arg);
  static int sta_retry_count = 0;
  
  if (event_base == WIFI_EVENT) {
    switch (event_id) {
      case WIFI_EVENT_AP_STACONNECTED: {
        if (wifiDriver->mOnAPConnected) {
          wifiDriver->mOnAPConnected();
        }
        break;
      }
      case WIFI_EVENT_AP_STADISCONNECTED: {
        if (wifiDriver->mOnAPDisconnected) {
          wifiDriver->mOnAPDisconnected();
        }
        break;
      }
      case WIFI_EVENT_STA_START: {
        if (wifiDriver->mOnSTAStarted) {
          wifiDriver->mOnSTAStarted();
        }
        break;
      }
          
      case WIFI_EVENT_STA_CONNECTED: {
        if (wifiDriver->mOnSTAConnected) {
          wifiDriver->mOnSTAConnected();
        }
        sta_retry_count = 0;
        break;
      }
          
      case WIFI_EVENT_STA_DISCONNECTED: {
        if (wifiDriver->mOnSTADisconnected) {
          wifiDriver->mOnSTADisconnected();
        }
        if (sta_retry_count < wifiDriver->MaxRetryCount) {
          esp_wifi_connect();
          sta_retry_count++;
        } else {
          xEventGroupSetBits(wifiDriver->mWiFiEventGroup, WIFI_EG_BITS::WIFI_STA_FAIL_BIT);
          xEventGroupClearBits(wifiDriver->mWiFiEventGroup, WIFI_EG_BITS::WIFI_STA_CONNECTED_BIT);
        }
        break;
      }
          
      case WIFI_EVENT_STA_AUTHMODE_CHANGE: {
        if (wifiDriver->mOnAuthChanged) {
          wifiDriver->mOnAuthChanged();
        }
        break;
      }
    }
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    if (wifiDriver->mOnSTAIPReceived) {
      ip_event_got_ip_t* event = static_cast<ip_event_got_ip_t*>(event_data);
      IPAddress ip = event->ip_info.ip.addr;
      wifiDriver->mOnSTAIPReceived(ip);
    }
    // ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    // ESP_LOGI(TAG, "STA получила IP: " IPSTR, IP2STR(&event->ip_info.ip));
    sta_retry_count = 0;
    xEventGroupClearBits(wifiDriver->mWiFiEventGroup, WIFI_EG_BITS::WIFI_STA_FAIL_BIT);
    xEventGroupSetBits(wifiDriver->mWiFiEventGroup, WIFI_EG_BITS::WIFI_STA_CONNECTED_BIT);
  }  
}

bool WiFiDriver::stopAP() {
  // Проверяем, существует ли AP
  wifi_mode_t currentMode;
  esp_wifi_get_mode(&currentMode);
  
  if (currentMode != WIFI_MODE_AP && currentMode != WIFI_MODE_APSTA) {
    xEventGroupClearBits(mWiFiEventGroup, WIFI_EG_BITS::WIFI_HAS_AP_BIT);
    return true;
  }

  // Останавливаем WiFi
  esp_err_t err = esp_wifi_stop();
  if (err != ESP_OK) {
    return false;
  }

  // Ждем завершения остановки
  vTaskDelay(pdMS_TO_TICKS(100));

  // Устанавливаем новый режим
  wifi_mode_t newMode = (currentMode == WIFI_MODE_APSTA) ? WIFI_MODE_STA : WIFI_MODE_NULL;
  err = esp_wifi_set_mode(newMode);
  if (err != ESP_OK) {
    return false;
  }

  // Уничтожаем интерфейс AP
  if (mAPNetif != nullptr) {
    esp_netif_destroy(mAPNetif);
    mAPNetif = nullptr;
    ESP_LOGI("WiFiDriver", "AP netif destroyed");
    xEventGroupClearBits(mWiFiEventGroup, WIFI_EG_BITS::WIFI_HAS_AP_BIT);    
  }

  // Запускаем WiFi снова, если остался STA
  if (newMode == WIFI_MODE_STA) {
    err = esp_wifi_start();
    if (err != ESP_OK) {
      return false;
    }
  }

  return true;
}

bool WiFiDriver::startSTA(const char* ssid, const char* pass) {
  esp_wifi_stop();
  vTaskDelay(pdMS_TO_TICKS(1000));
  if (mSTANetif == nullptr) {
    mSTANetif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (!mSTANetif) {
        mSTANetif = esp_netif_create_default_wifi_sta();
        if (!mSTANetif) {
            return false;
        }
    }
  }

  wifi_config_t sta_config = {
    .sta = {
      .ssid = {0,},
      .password = {0,},
      .scan_method = wifi_scan_method_t::WIFI_FAST_SCAN,
      .bssid_set = false,
      .bssid = {0,},
      .channel = 0,
      .listen_interval = 0,
      .sort_method = wifi_sort_method_t::WIFI_CONNECT_AP_BY_SIGNAL,
      .threshold = {
        .rssi = -127, 
        .authmode = wifi_auth_mode_t::WIFI_AUTH_WPA_WPA2_PSK,
        .rssi_5g_adjustment = 0,
      },
      .pmf_cfg = {
        .capable = true, 
        .required = false
      },
      .rm_enabled = 0,
      .btm_enabled = 0,
      .mbo_enabled = 0,
      .ft_enabled = 0,
      .owe_enabled = 0,
      .transition_disable = 0,
      .reserved1 = 0,
      .sae_pwe_h2e = wifi_sae_pwe_method_t::WPA3_SAE_PWE_UNSPECIFIED,
      .sae_pk_mode = wifi_sae_pk_mode_t::WPA3_SAE_PK_MODE_AUTOMATIC,
      .failure_retry_cnt = 0,
      .he_dcm_set = 0,
      .he_dcm_max_constellation_tx = 0,
      .he_dcm_max_constellation_rx = 0,
      .he_mcs9_enabled = 0,
      .he_su_beamformee_disabled = 0,
      .he_trig_su_bmforming_feedback_disabled = 0,
      .he_trig_mu_bmforming_partial_feedback_disabled = 0,
      .he_trig_cqi_feedback_disabled = 0,
      .vht_su_beamformee_disabled = 0,
      .vht_mu_beamformee_disabled = 0,
      .vht_mcs8_enabled = 0,
      .reserved2 = 0,
      .sae_h2e_identifier = {0,}
    },
  };

  memcpy(sta_config.sta.ssid, ssid, strlen(ssid));
  if ((pass == nullptr) || (strlen(pass) == 0)) {
    sta_config.sta.threshold.authmode = wifi_auth_mode_t::WIFI_AUTH_OPEN;
  } else {
    memcpy(sta_config.sta.password, pass, strlen(pass));
  }
    
  wifi_mode_t currentMode;
  esp_wifi_get_mode(&currentMode);
  if (currentMode == WIFI_MODE_AP) {
    if (esp_wifi_set_mode(WIFI_MODE_APSTA) != ESP_OK) {
      esp_netif_destroy(mSTANetif);
      mSTANetif = nullptr;
      return false;
    }
  } else {
    if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK) {
      esp_netif_destroy(mSTANetif);
      mSTANetif = nullptr;
      return false;
    }
  }

  if (esp_wifi_set_config(WIFI_IF_STA, &sta_config) != ESP_OK) {
    esp_wifi_set_mode(currentMode);
    if (mSTANetif != nullptr) {
      esp_netif_destroy(mSTANetif);
      mSTANetif = nullptr;
    }
    return false;    
  } else {
    if (esp_wifi_start() != ESP_OK) {
      esp_wifi_set_mode(currentMode);
      if (mSTANetif != nullptr) {
        esp_netif_destroy(mSTANetif);
        mSTANetif = nullptr;
      }
      return false;        
    }
  }

  return esp_wifi_connect() == ESP_OK;
}

WiFiDriver& WiFiDriver::getInstance() {
  static WiFiDriver instance;
  return instance;
}