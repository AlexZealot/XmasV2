/**
 * @file SystemCore.h
 * @Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief ядро системы, реализующее event-driven архитектуру
 * @details база для архитектуры Listener
 * @version 0.1
 * @date 07-12-2025
 * 
 * @copyright Copyright (c) 2025
 */
# pragma once

# include "SysConfig.h"
# include "debug.h"
# include "freertos/FreeRTOS.h"
# include "freertos/task.h"
# include "nvs_flash.h"
# include <vector>

# ifndef DATA_BLOCK_MAX_SIZE
/**
 * @var DATA_BLOCK_MAX_SIZE
 * @brief максимальный размер блока данных, передаваемый через сообщения менеджера
 */
constexpr sz DATA_BLOCK_MAX_SIZE = 8;
# endif
# ifndef MANAGER_QUEUE_SIZE
/**
 * @var MANAGER_QUEUE_SIZE
 * @brief длина очереди сообщений менеджера
 */
constexpr sz MANAGER_QUEUE_SIZE = 16;
# endif
# ifndef DATA_POOL_SIZE
/**
 * @var DATA_POOL_SIZE
 * @brief количество блоков памяти для встроенного менеджера
 */
constexpr sz DATA_POOL_SIZE = 16;
# endif

enum class MsgID: u8 {
  MSG_ID_DUMMY = 0xFF /**< заглушка */
};

class Listener;

/**
 * @class Manager
 * @brief центральный менеджер системы. Обеспечивает доставку сообщений между Listener
 * @note синглтон
 */
class Manager {
  public:
    /**
     * @brief Получить ссылку на экземпляр синглтона
     * 
     * @return Manager& ссылка на экземпляр синглтона
     */
    static Manager& getInstance();
    /**
     * @brief уведомить слушателей о событии
     * 
     * @param msg ID сообщения
     * @param data указатель на данные
     * @param dataSize размер данных
     */
    void notify(MsgID msg, const void* data = nullptr, sz dataSize = 0);
    /**
     * @brief уведомить слушателей о событии
     * 
     * @tparam T тип передаваемого параметра
     * @param msg ID сообщения
     * @param data данные
     */
    template<class T>
    void notify(MsgID msg, const T& data) {
      static_assert(sizeof(T) <= DATA_BLOCK_MAX_SIZE);
      static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable (no pointers, no classes)");
      static_assert(std::is_standard_layout_v<T>, "T must be standard layout for safe memcpy");
      notify(msg, &data, sizeof(T));
    }
  private:
    /**
     * @brief Базовый конструктор. Приватный, это - архитектурное решение
     */
    Manager();
    /**
     * @brief Добавить слушателя в список
     * 
     * @param lisneter новый слушатель
     */
    void addListener(Listener* lisneter);
    /// @cond
    Manager(const Manager&) = delete; /**< запрет копирования */
    Manager& operator=(const Manager&) = delete; /**< запрет присваивания */
    Manager(Manager&&) = delete; /**< запрет перемещения */
    Manager& operator=(Manager&&) = delete; /**< запрет перемещения */
    /// @endcond
    TaskHandle_t mNotifyTask; /**< хэндл таска, который будет уведомлять о поступлении сообщений */
    /**
     * @brief таск для уведомлений
     * 
     * @param arg указатель на класс-родитель
     */
    static void notifyTask(void* arg);

    /**
     * @struct Message
     * @brief сообщение для очереди
     */
    struct Message {
      MsgID msgID;
      void* data;
    };

    /**
     * @struct DataBlock
     * @brief блок данных для статического выделения памяти
     */
    struct DataBlock {
      bool isAllocated;
      uint8_t data[DATA_BLOCK_MAX_SIZE];
    };

    void* safeAlloc();
    void safeFree(void* data);

    QueueHandle_t mQueue; /**< очередь сообщений */
    StaticQueue_t mQueueStatic; /**< служебная информация для очереди */
    uint8_t mQueueBuffer[MANAGER_QUEUE_SIZE * sizeof(Message)]; /**< буфер для очереди */
    std::vector<Listener*> mListeners; /**< cписок слушателей */
    DataBlock mDataPool[DATA_POOL_SIZE]; /**< пул данных для безопасного выделения */
    SemaphoreHandle_t mMemMutex; /**< мьютекс для доступа к пулу данных */
    friend Listener;
};

/**
 * @class Listener
 * @brief базовый класс слушателя архитектуры Listener
 */
class Listener {
  public:
    /**
     * @brief Базовый конструктор. Должен быть вызван каждым наследником
     */
    Listener();
    /**
     * @brief дефолтный виртуальный деструктор
     */
    virtual ~Listener() = default;
    /**
     * @brief реакция на событие
     * 
     * @param msg сообщение
     * @param data указатель на данные сообщения
     * @note метод виртуальный, обязан быть переопредёлен в наследнике
     */
    virtual void onEvent(MsgID msg, const void* data) = 0;
  private:
    /// @cond
    Listener(const Listener&) = delete; /**< запрет копирования */
    Listener& operator=(const Listener&) = delete; /**< запрет присваивания */
    Listener(Listener&&) = delete; /**< запрет перемещения */
    Listener& operator=(Listener&&) = delete; /**< запрет перемещения */
    /// @endcond
};