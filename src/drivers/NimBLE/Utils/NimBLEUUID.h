# pragma once

# include "host/ble_uuid.h"
# include <stdio.h>

/**
 * @class NimBLEUUID
 * @brief Стандартный UUID 128 бит
 */
class NimBLEUUID {
	public:
		/**
		 * @brief Конструктор класса NimBLEUUID
		 */
		NimBLEUUID() = default;
		/**
		 * @brief Конструктор класса NimBLEUUID
		 * 
		 * @param UUID строка в формате 00112233-4455-6677-8899-AABBCCDDEEFF
		 */
		NimBLEUUID(const char *UUID);
		/**
		 * @brief Конструктор копирования
		 * 
		 * @param uuid копируемый объект
		 */
		NimBLEUUID(const NimBLEUUID& uuid);
		/**
		 * @brief Конструктор перемещения
		 * 
		 * @param uuid перемещаемый объект
		 */
		NimBLEUUID(const NimBLEUUID&& uuid);
		/**
		 * @brief оператор присвоения
		 * 
		 * @param UUID строка в формате 00112233-4455-6677-8899-AABBCCDDEEFF
		 * @return @c NimBLEUUID& результат присвоения
		 */
		NimBLEUUID& operator = (const char* UUID);
		/**
		 * @brief оператор копирования
		 * 
		 * @param uuid копируемый объект
		 * @return @c NimBLEUUID& результат копирования
		 */
		NimBLEUUID& operator = (const NimBLEUUID& uuid);
		/**
		 * @brief оператор перемещения
		 * 
		 * @param uuid перемещаемый объект
		 * @return @c NimBLEUUID& результат перемещения
		 */
		NimBLEUUID& operator = (const NimBLEUUID&& uuid);
		/**
		 * @brief получить значение, подходящее для работы с ESP-IDF
		 * 
		 * @return @c ble_uuid128_t& значение, подходящее для работы с ESP-IDF
		 */
		inline ble_uuid128_t& getIDFValue() {
			return mUUID;
		}
		/**
		 * @brief оператор доступа к UUID по индексу
		 * 
		 * @param idx индекс
		 * @return @c uint8_t& значение
		 */
		uint8_t& operator [] (int idx) {
		  constexpr int UUID_LEN = sizeof(mUUID.value) / sizeof(mUUID.value[0]); //compile-time вычисление размера массива
			assert(idx >= 0 && idx < UUID_LEN);
			return mUUID.value[idx];
		}
	private:
		/**
		 * @var mUUID
		 * @brief базовая переменная, вокруг которой сделана обёртка в виде класса
		 */
		ble_uuid128_t mUUID = {
			.u = {
				.type = BLE_UUID_TYPE_128,
			},
			.value = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
		};
		/**
		 * @brief выполнить сброс mUUID к значению по-умолчанию
		 */
		inline void reset() {
			mUUID.u.type = BLE_UUID_TYPE_128;
			mUUID.value[ 0] = mUUID.value[ 1] = mUUID.value[ 2] = mUUID.value[ 3] =
			mUUID.value[ 4] = mUUID.value[ 5] = mUUID.value[ 6] = mUUID.value[ 7] =
			mUUID.value[ 8] = mUUID.value[ 9] = mUUID.value[10] = mUUID.value[11] =
			mUUID.value[12] = mUUID.value[13] = mUUID.value[14] = mUUID.value[15] =
			0;
		}
};