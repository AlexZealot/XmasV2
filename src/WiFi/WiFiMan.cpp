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
  auto &mDriver = WiFiDriver::getInstance();
  mDriver.setOnAPConnected([](){
    DEBUG_PRINT_I_WIFI("AP connected event");
  });
  mDriver.setOnAPDisconnected([](){
    DEBUG_PRINT_I_WIFI("AP disconnected event");
  });
  mDriver.setOnAuthChanged([](){
    DEBUG_PRINT_I_WIFI("AUTH changed event");
  });
  mDriver.setOnGotIP([](const IPAddress& ip){
    DEBUG_PRINT_I_WIFI("Got IP event [%03u.%03u.%03u.%03u]", ip.addr[0], ip.addr[1], ip.addr[2], ip.addr[3]);
  });
  mDriver.setOnSTAConnected([](){
    DEBUG_PRINT_I_WIFI("STA connected event");
  });
  mDriver.setOnSTADisconnected([](){
    DEBUG_PRINT_I_WIFI("STA disconnected event");
  });
  mDriver.setOnSTAStarted([](){
    DEBUG_PRINT_I_WIFI("STA started event");
  });
  DEBUG_CHECK_BOOL_WIFI(mDriver.softAP(DEFAULT_SSID, DEFAULT_PASS));
  vTaskDelay(pdMS_TO_TICKS(10000));
  DEBUG_CHECK_BOOL_WIFI(mDriver.startSTA("Tech", "mIBpW0ov"));
}

WiFiMan::~WiFiMan() {

}

void WiFiMan::onEvent(MsgID msg, const void* data) {

}