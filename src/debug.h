/**
 * @file debug.h
 * @Алексей Казаков Zealot (akazakov.zealot@gmail.com)
 * @brief подсистема отладочных сообщений
 * @version 0.1
 * @date 07-12-2025
 * 
 * @copyright Copyright (c) 2025
 * 
 */
# pragma once

# ifndef SYS_DEBUG
#   define SYS_DEBUG 0
# endif

# define DEBUG_MAN                              1   /**< отладка менеджера устройств */
# define DEBUG_WIFI                             1   /**< отладка от менеджера WiFi */

# if SYS_DEBUG

# include "esp_log.h"

#   define DEBUG_START()                        ESP_LOGI("DEBUG","-----=====Start debug=====-----");  esp_log_level_set("*", esp_log_level_t::ESP_LOG_WARN)
#   define DEBUG_ENABLE_TAG(TAG)                esp_log_level_set(TAG, esp_log_level_t::ESP_LOG_MAX)
#   define DEBUG_PRINT_I(TAG, MSG, ...)         ESP_LOGI(TAG, "[Heap: %06u] " MSG, esp_get_free_heap_size() ,##__VA_ARGS__)
#   define DEBUG_PRINT_W(TAG, MSG, ...)         ESP_LOGW(TAG, "[Heap: %06u] " MSG, esp_get_free_heap_size() , ##__VA_ARGS__)
#   define DEBUG_PRINT_E(TAG, MSG, ...)         ESP_LOGE(TAG, "[Heap: %06u] " MSG, esp_get_free_heap_size() , ##__VA_ARGS__)
#   define DEBUG_CHECK_BOOL(TAG, val)           ESP_LOGI(TAG, "[Heap: %06u] " "%s: %s", esp_get_free_heap_size(), #val, val ? "TRUE" : "FALSE")
#   define DEBUG_CHECK_ESP_VAL(TAG, val)        ESP_LOGI(TAG, "[Heap: %06u] " "%s: %s", esp_get_free_heap_size(), #val, esp_err_to_name(val))

# else

#   define DEBUG_START()                        esp_log_level_set("*", esp_log_level_t::ESP_LOG_NONE)
#   define DEBUG_ENABLE_TAG(TAG)
#   define DEBUG_PRINT_I(TAG, MSG, ...)
#   define DEBUG_PRINT_W(TAG, MSG, ...)
#   define DEBUG_PRINT_E(TAG, MSG, ...)
#   define DEBUG_CHECK_BOOL(TAG, val)           val
#   define DEBUG_CHECK_ESP_VAL(TAG, val)        val

# endif

# if DEBUG_MAN
#   define DEBUG_ENABLE_TAG_MAN()               DEBUG_ENABLE_TAG("MAN")
#   define DEBUG_PRINT_I_MAN(MSG, ...)          DEBUG_PRINT_I("MAN", MSG, ##__VA_ARGS__)
#   define DEBUG_PRINT_W_MAN(MSG, ...)          DEBUG_PRINT_W("MAN", MSG, ##__VA_ARGS__)
#   define DEBUG_PRINT_E_MAN(MSG, ...)          DEBUG_PRINT_E("MAN", MSG, ##__VA_ARGS__)
#   define DEBUG_CHECK_BOOL_MAN(val)            DEBUG_CHECK_BOOL("MAN", val)
#   define DEBUG_CHECK_ESP_VAL_MAN(val)         DEBUG_CHECK_ESP_VAL("MAN", val)
# else
#   define DEBUG_ENABLE_TAG_MAN()
#   define DEBUG_PRINT_I_MAN(MSG, ...)
#   define DEBUG_PRINT_W_MAN(MSG, ...)
#   define DEBUG_PRINT_E_MAN(MSG, ...)
#   define DEBUG_CHECK_BOOL_MAN(val)            val
#   define DEBUG_CHECK_ESP_VAL_MAN(val)         val
# endif

# if DEBUG_WIFI
#   define DEBUG_ENABLE_TAG_WIFI()               DEBUG_ENABLE_TAG("WF")
#   define DEBUG_PRINT_I_WIFI(MSG, ...)          DEBUG_PRINT_I("WF", MSG, ##__VA_ARGS__)
#   define DEBUG_PRINT_W_WIFI(MSG, ...)          DEBUG_PRINT_W("WF", MSG, ##__VA_ARGS__)
#   define DEBUG_PRINT_E_WIFI(MSG, ...)          DEBUG_PRINT_E("WF", MSG, ##__VA_ARGS__)
#   define DEBUG_CHECK_BOOL_WIFI(val)            DEBUG_CHECK_BOOL("WF", val)
#   define DEBUG_CHECK_ESP_VAL_WIFI(val)         DEBUG_CHECK_ESP_VAL("WF", val)
# else
#   define DEBUG_ENABLE_TAG_WIFI()
#   define DEBUG_PRINT_I_WIFI(MSG, ...)
#   define DEBUG_PRINT_W_WIFI(MSG, ...)
#   define DEBUG_PRINT_E_WIFI(MSG, ...)
#   define DEBUG_CHECK_BOOL_WIFI(val)            val
#   define DEBUG_CHECK_ESP_VAL_WIFI(val)         val
# endif