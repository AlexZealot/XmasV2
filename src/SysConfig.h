/**
 * @file SysConfig.h
 * @Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief базовая конфигурация системы
 * @version 0.1
 * @date 07-12-2025
 * 
 * @copyright Copyright (c) 2025
 */
# pragma once

# include "SysTypes.h"

/**
 * @namespace STACK_SIZES
 * @brief Конфигурация стеков
 */
namespace STACK_SIZES {
	constexpr sz MANAGER_STACK = 1024 * 4;
}

/**
 * @namespace TASK_PRIO
 * @brief Конфигурация приоритетов задач
 */
namespace TASK_PRIO {
	constexpr sz MANAGER_PRIO = 3;
}