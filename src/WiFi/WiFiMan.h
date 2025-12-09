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

# include "../drivers/WiFi/WiFiDriver.h"
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
    virtual ~WiFiMan() override;
    /**
     * @brief реакция на событие
     * 
     * @param msg сообщение
     * @param data указатель на данные сообщения
     * @note метод виртуальный, обязан быть переопредёлен в наследнике
     */
    virtual void onEvent(MsgID msg, const void* data) override;
  private:
    WiFiDriver* mDriver;
};