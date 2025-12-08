# pragma once

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
# include "../debug.h"
# include "../SystemCore.h"

class WiFiMan: public Listener {
  public:
    WiFiMan();
		/**
     * @brief реакция на событие
     * 
     * @param msg сообщение
     * @param data указатель на данные сообщения
     * @note метод виртуальный, обязан быть переопредёлен в наследнике
     */
    virtual void onEvent(MsgID msg, const void* data) override;
  private:
    void softAP(const char* ssid, const char* pass, uint8_t channel = 1);	
    static void wifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
};