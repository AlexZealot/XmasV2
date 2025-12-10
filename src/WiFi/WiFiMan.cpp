/**
 * @file WiFiMan.cpp
 * @author Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief реализация класса WiFiMan
 * @version 0.1
 * @date 08-12-2025
 * 
 * @copyright Copyright (c) 2025
 */
# include "WiFiMan.h"

WiFiMan::WiFiMan() {
  DEBUG_ENABLE_TAG_WIFI();
}

WiFiMan::~WiFiMan() {

}

void WiFiMan::onEvent(MsgID msg, const void* data) {

}