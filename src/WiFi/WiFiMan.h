/**
 * @file WiFiMan.h
 * @Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief подсистема для работы с WiFi
 * @version 0.1
 * @date 08-12-2025
 * 
 * @copyright Copyright (c) 2025
 */
# pragma once

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
# include "../debug.h"
# include "../SystemCore.h"

/**
 * @class WiFiMan
 * @brief менеджер сетевых подключений
 */
class WiFiMan: public Listener {
  public:
		/**
		 * @brief базовый конструктор
		 */
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
		/**
		 * @brief запустить AP
		 * 
		 * @param ssid ssid soft AP
		 * @param pass pass soft AP
		 * @param channel номер канала
		 */
    void softAP(const char* ssid, const char* pass, uint8_t channel = 1);	
		/**
		 * @brief обработчик event-loop
		 * 
		 * @param arg указатель на класс-родитель
		 * @param event_base подсистема, сгенерировавшая событие
		 * @param event_id ID события
		 * @param event_data данные события
		 */
    static void wifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
};