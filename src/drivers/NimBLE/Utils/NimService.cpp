/**
 * @file NimService.cpp
 * @author Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief реализация класса NimService
 * @version 0.1
 * @date 11-12-2025
 * 
 * @copyright Copyright (c) 2025
 */
# include "NimService.h"

NimService::NimService(NimBLEUUID& UUID) {
	mDefinition.type = BLE_GATT_SVC_TYPE_PRIMARY;
	mDefinition.uuid = &(UUID.getIDFValue().u);
}