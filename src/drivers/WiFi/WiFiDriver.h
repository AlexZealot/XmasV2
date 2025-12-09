/**
 * @file WiFiDriver.h
 * @author Алексей Казаков Zealot (akazakov.zealot@gmail.com)
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
# include <functional>
# include "Utils/Utils.h"

/**
 * @class WiFiDriver
 * @brief драйвер для работы с WiFi
 * @note синглтон
 */
class WiFiDriver {
  public:
    static WiFiDriver& getInstance();
    /**
     * @brief прототип стандартного колбэка, без параметров
     */
    using WiFiCallback = std::function<void()>;
    /**
     * @brief прототип колбэка с одним параметром IPAddress
     */
    using WiFiIPCallback = std::function<void(const IPAddress& ip)>;
    /**
     * @brief установить колбэк на подключение к точке доступа
     * 
     * @param cb колбэк
     */
    inline void setOnAPConnected(WiFiCallback cb) {
      mOnAPConnected = cb;
    }
    /**
     * @brief установить колбэк на отключение от точки доступа
     * 
     * @param cb колбэк
     */
    inline void setOnAPDisconnected(WiFiCallback cb) {
      mOnAPDisconnected = cb;
    }
    /**
     * @brief установить колбэк на подключение к точке доступа
     * 
     * @param cb колбэк
     */
    inline void setOnSTAConnected(WiFiCallback cb) {
      mOnSTAConnected = cb;
    }
    /**
     * @brief установить колбэк на старт STA
     * 
     * @param cb колбэк
     */
    inline void setOnSTAStarted(WiFiCallback cb) {
      mOnSTAStarted = cb;
    }
    /**
     * @brief установить колбэк на отключение от точки доступа
     * 
     * @param cb колбэк
     */
    inline void setOnSTADisconnected(WiFiCallback cb) {
      mOnSTADisconnected = cb;
    }
    /**
     * @brief установить колбэк на получение IP
     * 
     * @param cb колбэк
     */
    inline void setOnGotIP(WiFiIPCallback cb) {
      mOnSTAIPReceived = cb;
    }
    /**
     * @brief установить колбэк на изменение авторизации
     * 
     * @param cb колбэк
     */
    inline void setOnAuthChanged(WiFiCallback cb) {
      mOnAuthChanged = cb;
    }
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
    /**
     * @brief установить максимальное количество попыток переподключений к точке доступа
     * 
     * @param count максимальное количество попыток
     * @warning допускается изменять это значение до включения режима станции и начала подключения, т.к. ресурс не защищён от race-condition
     */
    inline void setMaxRetryCount(int count) {
      MaxRetryCount = count;
    }
    /**
     * @brief подключиться к точке доступа
     * 
     * @param ssid ssid точки доступа
     * @param pass pass точки доступа
     * @return @c true подсистема запущена @n\
     * @return @c false запустить подсистему не удалось
     */
    bool startSTA(const char* ssid, const char* pass);
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

    esp_netif_t *mAPNetif = nullptr;  /**< netif точки доступа */
    esp_netif_t *mSTANetif = nullptr; /**< netif клиента */
    EventGroupHandle_t mWiFiEventGroup = nullptr; /**< группа событий WiFi */
    esp_event_handler_instance_t mEHInstance = nullptr; /**< обработчик событий WiFi */
    esp_event_handler_instance_t mEHInstanceIP = nullptr; /**< обработчик событий WiFi */
    int MaxRetryCount = 10; /**< максимальное количество попыток переподключений к точке доступа */

    WiFiCallback mOnAPConnected = nullptr; /**< колбэк на подключение клиента */
    WiFiCallback mOnAPDisconnected = nullptr; /**< колбэк на отключение клиента */
    WiFiCallback mOnSTAStarted = nullptr; /**< колбэк на запуск STA */
    WiFiCallback mOnSTAConnected = nullptr; /**< колбэк на подключение к STA */
    WiFiCallback mOnSTADisconnected = nullptr; /**< колбэк на отключение от STA */
    WiFiIPCallback mOnSTAIPReceived = nullptr; /**< колбэк на получение IP */
    WiFiCallback mOnAuthChanged = nullptr; /**< колбэк на изменение метода авторизации */
    /**
     * @brief базовый конструктор
     */
    WiFiDriver();
    /**
     * @brief Деструктор класса WiFiDriver
     */
    ~WiFiDriver();
    /// @cond
    WiFiDriver(WiFiDriver const&) = delete; /**< запрет конструктора копирования */
    WiFiDriver& operator= (WiFiDriver const&) = delete; /**< запрет оператора присваивания */
    WiFiDriver(WiFiDriver&&) = delete; /**< запрет конструктора перемещения */
    WiFiDriver& operator= (WiFiDriver&&) = delete; /**< запрет оператора перемещения */
    /// @endcond
};