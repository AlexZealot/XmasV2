/**
 * @file NimScan.h
 * @Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief колбеки скана
 * @version 0.1
 * @date 11-12-2025
 * 
 * @copyright Copyright (c) 2025
 * 
 */
# pragma once

# include "host/ble_gap.h"

/**
 * @class NimScan
 * @brief класс-колбэк сканирования
 */
class NimScan {
	public:
		/**
		 * @brief метод, подлежащий переопределению наследниками. Реализация колбэка, вызываемая при сканировании
		 * 
		 * @param scanResult результат сканирования
		 */
		virtual void onScanResult(const ble_gap_disc_desc* scanResult) = 0;
};