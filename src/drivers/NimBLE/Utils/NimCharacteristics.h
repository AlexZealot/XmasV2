/**
 * @file NimCharacteristics.h
 * @author Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief Характеристика NimBLE
 * @version 0.1
 * @date 11-12-2025
 * 
 * @copyright Copyright (c) 2025
 */
# pragma once

# include "esp_log.h"
# include "nimble/nimble_port.h"
# include "host/ble_hs.h"
# include "NimBLEUUID.h"

/**
 * @brief тип характеристики (запись/чтение)
 * 
 */
enum class NimCharType: uint8_t {
	NIM_CHAR_WRITE = 0, /**< запись */
	NIM_CHAR_READ = 1 /**< чтение */
};

/**
 * @class NimCharacteristics
 * @brief характеристика NimBLE
 */
class NimCharacteristics {
	public:
		NimCharacteristics(NimBLEUUID &UUID, NimCharType charType);
	private:
		uint16_t mHandle;
		ble_gatt_chr_def mDefinition;
		NimCharType mCharType;

		/// @cond
		NimCharacteristics() = delete;
		NimCharacteristics(const NimCharacteristics&) = delete;
		NimCharacteristics(const NimCharacteristics&&) = delete;
		NimCharacteristics& operator = (const NimCharacteristics&) = delete;
		NimCharacteristics& operator = (const NimCharacteristics&&) = delete;
		/// @endcond

		/**
		 * @brief колбэк работы характеристики
		 * 
		 * @param conn_handle хэндл характеристики
		 * @param attr_handle хэндл аттрибута
		 * @param ctxt контекст колбэка
		 * @param arg указатель на класс-родитель
		 * @return @c int результат работы колбэка
		 */
		static int characteristicsCallback(uint16_t conn_handle, uint16_t attr_handle, ble_gatt_access_ctxt *ctxt, void *arg);
};