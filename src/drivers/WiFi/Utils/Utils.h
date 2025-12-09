# pragma once

# include <stdio.h>

/**
 * @brief структура для работы с IP адресом
 */
struct IPAddress {
	union {
		unsigned long raw; /**< адрес в виде числа */
		unsigned char addr[4];	/**< октеты адреса */
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
	IPAddress() {
		raw = 0;
	}
};