# include "WiFiDriver.h"

constexpr size_t SSID_MAX_LEN = 64 - 1;
constexpr size_t PASS_MAX_LEN = 32 - 1;

bool WiFiDriver::configureAP(bool enable, const char* SSID, const char* PASS, uint8_t channel) {
  if (!enable) {
    if (mAPNetif != nullptr) {
      esp_netif_destroy_default_wifi(mAPNetif);
      mAPNetif = nullptr;
    }
    if (cur_mode == wifi_mode_t::WIFI_MODE_APSTA) {
      cur_mode = wifi_mode_t::WIFI_MODE_STA;
    } else {
      cur_mode = wifi_mode_t::WIFI_MODE_NULL;
    }
    xEventGroupClearBits(mWiFiEventGroup, WIFI_EG_BITS::WIFI_HAS_AP_BIT);
    return true;
  }
  if ((SSID == nullptr) || (SSID[0] == 0)) {
    configureAP(false);
    return false;
  }
  size_t len = strlen(SSID);
  if (len > SSID_MAX_LEN) {
    configureAP(false);
    return false;
  }
  strncpy(reinterpret_cast<char*>(ap_config.ap.ssid), SSID, len);
  ap_config.ap.ssid[len] = 0;
  if (PASS != nullptr) {
    len = strlen(PASS);
    if (len > PASS_MAX_LEN) {
      configureAP(false);
      return false;
    }
    strncpy(reinterpret_cast<char*>(ap_config.ap.password), PASS, len);
    ap_config.ap.password[len] = 0;
    ap_config.ap.authmode = wifi_auth_mode_t::WIFI_AUTH_WPA_WPA2_PSK;
  } else {
    ap_config.ap.authmode = WIFI_AUTH_OPEN;
    ap_config.ap.password[0] = 0;
  }
  ap_config.ap.channel = channel;
  if (mAPNetif == nullptr) {
    mAPNetif = esp_netif_create_default_wifi_ap();
  }
  xEventGroupSetBits(mWiFiEventGroup, WIFI_EG_BITS::WIFI_HAS_AP_BIT);
  if (cur_mode == wifi_mode_t::WIFI_MODE_NULL) {
    cur_mode = wifi_mode_t::WIFI_MODE_AP;
  } else
  if (cur_mode == wifi_mode_t::WIFI_MODE_STA) {
    cur_mode = wifi_mode_t::WIFI_MODE_APSTA;
  }
  return true;
}

bool WiFiDriver::configureSTA(bool enable, const char* SSID, const char* PASS) {
  if (!enable) {
    if (mSTANetif != nullptr) {
      esp_netif_destroy_default_wifi(mSTANetif);
      mSTANetif = nullptr;
    }
    if (cur_mode == wifi_mode_t::WIFI_MODE_APSTA) {
      cur_mode = wifi_mode_t::WIFI_MODE_AP;
    } else {
      cur_mode = wifi_mode_t::WIFI_MODE_NULL;
    }
    xEventGroupClearBits(mWiFiEventGroup, WIFI_EG_BITS::WIFI_HAS_STA_BIT);
    return true;
  }
  if (SSID == nullptr || SSID[0] == 0) {
    configureSTA(false);
    return false;
  }
  size_t len = strlen(SSID);
  if (len > SSID_MAX_LEN) {
    configureSTA(false);
    return false;
  }
  strncpy(reinterpret_cast<char*>(sta_config.sta.ssid), SSID, len);
  sta_config.sta.ssid[len] = 0;
  if (PASS != nullptr) {
    len = strlen(PASS);
    if (len > PASS_MAX_LEN) {
      configureSTA(false);
      return false;
    }
    strncpy(reinterpret_cast<char*>(sta_config.sta.password), PASS, len);
    sta_config.sta.password[len] = 0;
    sta_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
  } else {
    sta_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
    sta_config.sta.password[0] = 0;
  }
  if (mSTANetif == nullptr) {
    mSTANetif = esp_netif_create_default_wifi_sta();
  }
  xEventGroupSetBits(mWiFiEventGroup, WIFI_EG_BITS::WIFI_HAS_STA_BIT);
  if (cur_mode == wifi_mode_t::WIFI_MODE_NULL) {
    cur_mode = wifi_mode_t::WIFI_MODE_STA;
  } else
  if (cur_mode == wifi_mode_t::WIFI_MODE_AP) {
    cur_mode = wifi_mode_t::WIFI_MODE_APSTA;
  }
  return true;
}

void WiFiDriver::stopWiFiOnConfigure() {
  if ((xEventGroupGetBits(mWiFiEventGroup) & WIFI_EG_BITS::WIFI_HAS_ANY_INTERFACE) != 0) {
    esp_wifi_stop();
  }
}

void WiFiDriver::startWiFiOnConfigure() {
  EventBits_t bits = xEventGroupGetBits(mWiFiEventGroup);
  if ((bits & WIFI_EG_BITS::WIFI_HAS_ANY_INTERFACE) != 0) {
    esp_wifi_set_mode(cur_mode);
    esp_wifi_set_config(wifi_interface_t::WIFI_IF_AP, &ap_config);
    esp_wifi_set_config(wifi_interface_t::WIFI_IF_STA, &sta_config);
    esp_wifi_start();
    if ((bits & WIFI_EG_BITS::WIFI_HAS_STA_BIT) != 0) {
      esp_wifi_connect();
    }
  }
}