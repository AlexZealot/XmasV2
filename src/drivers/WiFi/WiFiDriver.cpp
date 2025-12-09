/**
 * @file WiFiDriver.cpp
 * @author AlexSibZavod (a.kazakov@sibirzavod.ru)
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
  constexpr EventBits_t WIFI_HAS_AP_BIT = (1 << 0);       /**< включена ли AP */
}

WiFiDriver::WiFiDriver(): mAPNetif(nullptr), mSTANetif(nullptr), mWiFiEventGroup(nullptr), mEHInstance(nullptr) {
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
}

WiFiDriver::~WiFiDriver() {
  stopAP();
  if (mEHInstance != nullptr) {
    esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, mEHInstance);
    mEHInstance = nullptr;
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
  if (event_base == WIFI_EVENT) {
      switch (event_id) {
        case WIFI_EVENT_AP_STACONNECTED: {
          break;
        }
        case WIFI_EVENT_AP_STADISCONNECTED: {
          break;
        }
        case WIFI_EVENT_AP_START:
          break;
        case WIFI_EVENT_AP_STOP:
          break;
        default:
          break;
    }
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