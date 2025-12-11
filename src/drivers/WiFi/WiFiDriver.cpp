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

WiFiDriver::WiFiDriver() {
  ap_config = {
    .ap = {
      .ssid = {0,},
      .password = {0,},
      .ssid_len = 0,
      .channel = 0,      
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
  sta_config = {
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
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
}

WiFiDriver::~WiFiDriver() {
  stopWiFiOnConfigure();
  if (mEHInstance != nullptr) {
    esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, mEHInstance);
    mEHInstance = nullptr;
  }
  if (mEHInstanceIP != nullptr) {
    esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, mEHInstanceIP);
    mEHInstanceIP = nullptr;
  }
  esp_wifi_deinit();
  esp_netif_deinit();
}


bool WiFiDriver::softAP(const char* ssid, const char* pass, uint8_t channel) {
  stopWiFiOnConfigure();
  if (!configureAP(true, ssid, pass, channel)) {
    startWiFiOnConfigure();
    return false;
  }
  startWiFiOnConfigure();
  return true;
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
    sta_retry_count = 0;
    xEventGroupClearBits(wifiDriver->mWiFiEventGroup, WIFI_EG_BITS::WIFI_STA_FAIL_BIT);
    xEventGroupSetBits(wifiDriver->mWiFiEventGroup, WIFI_EG_BITS::WIFI_STA_CONNECTED_BIT);
  }

  if (wifiDriver->mOnWiFiEventCallback) {
    wifiDriver->mOnWiFiEventCallback(event_base, event_id, event_data);
  }
}

bool WiFiDriver::stopAP() {
  stopWiFiOnConfigure();
  if (!configureAP(false)) {
    startWiFiOnConfigure();
    return false;
  }
  startWiFiOnConfigure();
  return true;
}

bool WiFiDriver::startSTA(const char* ssid, const char* pass) {
  stopWiFiOnConfigure();
  if (!configureSTA(true, ssid, pass)) {
    startWiFiOnConfigure();
    return false;
  }
  startWiFiOnConfigure();
  return true;
}

bool WiFiDriver::stopSTA() {
  stopWiFiOnConfigure();
  if (!configureSTA(false)) {
    startWiFiOnConfigure();
    return false;
  }
  startWiFiOnConfigure();
  return true;  
}

WiFiDriver& WiFiDriver::getInstance() {
  static WiFiDriver instance;
  return instance;
}