/**
 * @file Utils.h
 * @author Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief унилиты для работы с WiFi
 * @version 0.1
 * @date 11-12-2025
 * 
 * @copyright Copyright (c) 2025
 * 
 */
# pragma once

# include <stdio.h>
# include <string.h>

/**
 * @brief структура для работы с IP адресом
 */
struct IPAddress {
  union {
    unsigned long raw; /**< адрес в виде числа */
    unsigned char addr[4];  /**< октеты адреса */
  };
  /**
   * @brief Получить IP-адрес в виде строки.
   * 
   * @param str строка формата XXX.XXX.XXX.XXX
   * @warning Аллокация памяти под строку и освобождения производятся на вызывающей стороне!
   */
  void getString(char* str) {
    sprintf(str, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
  }
  /**
   * @brief конструктор по умолчанию
   */
  IPAddress() {
    raw = 0;
  }
  /**
   * @brief конструктор из октетов
   * 
   * @param a первый октет
   * @param b второй октет
   * @param c третий октет
   * @param d четвертый октет
   */
  IPAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d) {
    addr[0] = a;
    addr[1] = b;
    addr[2] = c;
    addr[3] = d;
  }
  /**
   * @brief конструктор из октетов
   * 
   * @param octets массив октетов. Размер не менее 4 байт.
   */
  IPAddress(const uint8_t* octets) {
    memcpy(addr, octets, 4);
  }
  /**
   * @brief конструктор из числа
   * 
   * @param a raw число
   */
  IPAddress(unsigned long a) {
    raw = a;
  }
  /**
   * @brief конструктор копирования
   * 
   * @param ip копируемый адрес
   */
  IPAddress(const IPAddress& ip) {
    raw = ip.raw;
  }
  /**
   * @brief оператор присваивания
   * 
   * @param ip присваиваемый адрес
   * @return @c IPAddress& результат присваивания
   */
  IPAddress& operator=(const IPAddress& ip) {
    this->raw = ip.raw;
    return *this;
  }
  /**
   * @brief оператор присваивания
   * 
   * @param a число
   * @return @c IPAddress& результат присваивания
   */
  IPAddress& operator=(unsigned long a) {
    this->raw = a;
    return *this;
  }
  /**
   * @brief конструктор перемещения
   * 
   * @param ip перемещаемый адрес
   */
  IPAddress(const IPAddress&& ip) {
    raw = ip.raw;
  }
  /**
   * @brief оператор перемещения
   * 
   * @param ip перемещаемый адрес
   * @return @c IPAddress& результат перемещения
   */
  IPAddress& operator=(const IPAddress&& ip) {
    this->raw = ip.raw;
    return *this;
  }
  /**
   * @brief оператор сравнения
   * 
   * @param ip сравниваемый адрес
   * @return @c true адреса совпадают @n\
   * @return @c false адреса не совпадают
   */
  bool operator==(const IPAddress& ip) {
    return raw == ip.raw;
  }
  /**
   * @brief оператор неравенства
   * 
   * @param ip сравниваемый адрес
   * @return @c true адреса не совпадают @n\
   * @return @c false адреса совпадают
   */
  bool operator!=(const IPAddress& ip) {
    return !(*this == ip);
  }
  /**
   * @brief конструктор из строки. Формат XXX.XXX.XXX.XXX
   * 
   * @param address строка с адресом
   */
  IPAddress(const char* address) {
    if (!address) {
      sscanf(address, "%hhu.%hhu.%hhu.%hhu", &addr[0], &addr[1], &addr[2], &addr[3]);
    } else {
      raw = 0;
    }
  }
  /**
   * @brief оператор присваивания из строки. Формат XXX.XXX.XXX.XXX
   * 
   * @param address строка с адресом, формат XXX.XXX.XXX.XXX
   * @return @c IPAddress& результат присваивания
   */
  IPAddress& operator=(const char* address) {
    if (address) {
      sscanf(address, "%hhu.%hhu.%hhu.%hhu", &addr[0], &addr[1], &addr[2], &addr[3]);
      return *this;
    } else {
      raw = 0;
      return *this;
    }
  }
};