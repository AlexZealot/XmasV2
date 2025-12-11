/**
 * @file NimBLEDriver.h
 * @author Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief драйвер для работы с NimBLE.
 * @version 0.1
 * @date 11-12-2025
 * 
 * @copyright Copyright (c) 2025
 * 
 */
# pragma once

# include "esp_log.h"
# include "nimble/nimble_port.h"
# include "nimble/nimble_port_freertos.h"
# include "host/ble_hs.h"
# include "services/gap/ble_svc_gap.h"

# include "Utils/NimScan.h"

/**
 * @namespace NIM_BLE_BITS
 * @brief пространство имён для драйвера NimBLE
 */
namespace NIM_BLE_BITS {
  constexpr EventBits_t SCAN_ACTIVE = (1 << 0); /**< сканирование в процессе */
};

constexpr int SCAN_DURATION_INFINITE = BLE_HS_FOREVER; /**< бесконечное сканирование */

/**
 * @class NimBLEDriver
 * @brief драйвер для работы с NimBLE
 */
class NimBLEDriver {
  public:
    /**
     * @brief получить ссылку на экземпляр драйвера
     * 
     * @param deviceName имя устройства. Имеет значение только при первом вызове. Залее - игнорируется
     * @return @c NimBLEDriver& ссылка на экземпляр драйвера
     */
    static NimBLEDriver& getInstance(const char* deviceName = nullptr);
    /**
     * @brief задать колбэк для сканирования
     * 
     * @param callback колбэк для сканирования
     */
    void setScanCallback(NimScan *callback);
    /**
     * @brief установить интервал сканирования в мс
     * 
     * @param interval интервал в мс
     */
    void setScanInterval(uint16_t interval);
    /**
     * @brief установить размер окна сканирования в мс
     * 
     * @param window окно в мс
     */
    void setScanWindow(uint16_t window);
    /**
     * @brief производить ли активное сканирование
     * 
     * @param active true - активное сканирование, false - пассивное
     */
    void setActiveScan(bool active);
    /**
     * @brief включить фильтр дубликатов
     * 
     * @param filter true - включить, false - выключить
     */
    void setFilterDuplicates(bool filter);
    /**
     * @brief запущено ли сканирование
     * 
     * @return @c true запущено @n\
     * @return @c false не запущено
     */
    inline bool isScanning() {
      if (mBLEEventGroup) {
        return xEventGroupGetBits(mBLEEventGroup) & NIM_BLE_BITS::SCAN_ACTIVE;
      } else {
        return false;
      }
    }
    /**
     * @brief Запустить сканирование
     * 
     * @param duration_ms длительность в мс.
     * @return @c int 0 в случае успеха, иначе код ошибки
     */
    int startScan(int duration_ms = SCAN_DURATION_INFINITE);
    /**
     * @brief остановить сканирование
     */
    void stopScan();
  private:
    /**
     * @brief Конструктор, инициализирующий драйвер
     * 
     * @param deviceName имя устройства
     */
    NimBLEDriver(const char* deviceName);

    /// @cond
    NimBLEDriver() = delete; /**< запретить создание экземпляра без параметров */
    NimBLEDriver(const NimBLEDriver&) = delete; /**< запретить копирование */
    NimBLEDriver& operator=(const NimBLEDriver&) = delete; /**< запретить присваивание */
    NimBLEDriver(NimBLEDriver&&) = delete; /**< запретить перемещение */
    NimBLEDriver& operator=(NimBLEDriver&&) = delete; /**< запретить перемещение */
    /// @endcond

    NimScan *mScanCallback = nullptr; /**< колбэк, отвечающий за сканирование */
    ble_gap_disc_params mScanParams; /**< параметры сканирования */
    EventGroupHandle_t mBLEEventGroup = nullptr; /**< группа событий для синхронизации */

    /**
     * @brief колбэк для сканирования
     * 
     * @param event событие колбэка
     * @param arg указатель на класс-родитель
     * @return @c int результат выполнения колбэка @b 0
     */
    static int scanCallback(ble_gap_event *event, void *arg);
};