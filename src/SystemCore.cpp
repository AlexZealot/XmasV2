/**
 * @file SystemCore.cpp
 * @Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief реализация классов Manager и Listener
 * @version 0.1
 * @date 08-12-2025
 * 
 * @copyright Copyright (c) 2025
 */
# include "SystemCore.h"

constexpr TickType_t memMutexAwait = pdMS_TO_TICKS(200);

Manager& Manager::getInstance() {
  static Manager instance;
  return instance;
}

Manager::Manager(): mNotifyTask(nullptr) {
  DEBUG_ENABLE_TAG_MAN();  
	esp_err_t ret = nvs_flash_init();

  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    DEBUG_CHECK_ESP_VAL_MAN(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  DEBUG_CHECK_ESP_VAL_MAN(ret);
  mQueue = xQueueCreateStatic(MANAGER_QUEUE_SIZE, sizeof(Message), mQueueBuffer, &mQueueStatic);
  mMemMutex = xSemaphoreCreateMutex();
  if (mQueue == nullptr) {
    DEBUG_PRINT_E_MAN("Queue create error");
  }
  if (mMemMutex == nullptr) {
    DEBUG_PRINT_E_MAN("Mutex create error");
  }
  DEBUG_CHECK_BOOL_MAN(xTaskCreate(notifyTask, "notify", STACK_SIZES::MANAGER_STACK, this, TASK_PRIO::MANAGER_PRIO, &mNotifyTask));
  if (mNotifyTask == nullptr) {
    DEBUG_PRINT_E_MAN("Notify task create error");
  }
  memset(mDataPool, 0, sizeof(mDataPool));
}

void Manager::notifyTask(void* arg) {
  auto man = static_cast<Manager*>(arg);
  while (1) {
    Message msg;
    if (xQueueReceive(man->mQueue, &msg, portMAX_DELAY)) {
      for (auto it: man->mListeners) {
        it->onEvent(msg.msgID, msg.data);
      }
      man->safeFree(msg.data);
    }
  }
}

void Manager::addListener(Listener* lisneter) {
  mListeners.push_back(lisneter);
  DEBUG_PRINT_I_MAN("Got new listener; Now it's count is %zu", mListeners.size());
}

void* Manager::safeAlloc() {
  if (xSemaphoreTake(mMemMutex, memMutexAwait)) {
    for (int i = 0; i < DATA_POOL_SIZE; i++) { 
      if (!mDataPool[i].isAllocated) {
        mDataPool[i].isAllocated = true;
        xSemaphoreGive(mMemMutex);
        return mDataPool[i].data;
      }
    }
    DEBUG_PRINT_E_MAN("Error on memory allocation");
    xSemaphoreGive(mMemMutex);
  } else {
    DEBUG_PRINT_E_MAN("Memory allocation deadlock");
  }
  return nullptr;

}

void Manager::safeFree(void* data) {
  if (xSemaphoreTake(mMemMutex, memMutexAwait)) {
    for (int i = 0; i < DATA_POOL_SIZE; i++) {
      if (mDataPool[i].data == data) {
        mDataPool[i].isAllocated = false;
      }
    }
    xSemaphoreGive(mMemMutex);
  } else {
    DEBUG_PRINT_E_MAN("Memory free deadlock");
  }
}

void Manager::notify(MsgID msg, const void* data, sz dataSize) {
  Message m = {
    .msgID = msg,
    .data = nullptr
  };
  if (data != nullptr) {
    m.data = safeAlloc();
    memcpy(m.data, data, dataSize);
  }
  DEBUG_CHECK_BOOL_MAN(xQueueSend(mQueue, &m, pdMS_TO_TICKS(200)));
}

Listener::Listener() {
  auto &man = Manager::getInstance();
  man.addListener(this);
}