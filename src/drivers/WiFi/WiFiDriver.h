/**
 * @file WiFiDriver.h
 * @author AlexSibZavod (a.kazakov@sibirzavod.ru)
 * @brief драйвер для работы с WiFi
 * @version 0.1
 * @date 09-12-2025
 * 
 * @copyright Copyright (c) 2025
 */
# pragma once

# include "esp_system.h"
# include "esp_wifi.h"
# include "esp_event.h"
# include "esp_log.h"
# include "Utils/Utils.h"

/**
 * @class WiFiDriver
 * @brief драйвер для работы с WiFi
 */
class WiFiDriver {
  public:
    /**
     * @brief базовый конструктор
     */
    WiFiDriver();
    /**
     * @brief Деструктор класса WiFiDriver
     */
    ~WiFiDriver();
    /**
     * @brief запустить AP
     * 
     * @param ssid ssid soft AP
     * @param pass pass soft AP
     * @param channel номер канала
     * @return @c true успешное создание точки доступа @n\
     * @return @c false точку доступа создать не удалось
     */
    bool softAP(const char* ssid, const char* pass, uint8_t channel = 1);
    /**
     * @brief остановить AP
     * 
     * @return @c true AP остановлена, никаких ошибок не произошло @n\
     * @return @c false остановить AP не удалось, либо произошла ошибка переключения режима (например, на STA с APSTA)
     */
    bool stopAP();
    /**
     * @brief существует ли активная точка доступа
     * 
     * @return @c true существует @n\
     * @return @c false не существует
     */
    bool isApActive();
  private:
    /**
     * @brief обработчик event-loop
     * 
     * @param arg указатель на класс-родитель
     * @param event_base подсистема, сгенерировавшая событие
     * @param event_id ID события
     * @param event_data данные события
     */
    static void wifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

    esp_netif_t *mAPNetif;  /**< netif точки доступа */
    esp_netif_t *mSTANetif; /**< netif клиента */
    EventGroupHandle_t mWiFiEventGroup; /**< группа событий WiFi */
    esp_event_handler_instance_t mEHInstance; /**< обработчик событий WiFi */
};