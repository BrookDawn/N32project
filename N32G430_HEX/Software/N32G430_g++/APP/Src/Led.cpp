/**
 * @file Led.cpp
 * @brief LED控制类的具体实现
 * @author AI Assistant
 * @version v1.0.0
 * @date 2025-09-18
 */

#include "Led.h"
#include <string.h>

/* ==================== 私有变量 ==================== */

static LedController_t led_controllers[MAX_LED_COUNT];
static const LedInterface_t* hardware_interface = nullptr;
static bool system_initialized = false;

/* ==================== 私有函数声明 ==================== */

static bool is_valid_led_id(uint8_t led_id);
static LedController_t* get_led_controller(uint8_t led_id);

/* ==================== 公共API函数实现 ==================== */

/**
 * @brief 注册LED硬件接口
 */
LedError_t Led_RegisterInterface(const LedInterface_t* interface)
{
    if (interface == nullptr) {
        return LED_ERROR;
    }
    
    // 检查接口函数指针是否有效
    if (interface->init == nullptr || 
        interface->set_state == nullptr || 
        interface->get_state == nullptr) {
        return LED_ERROR;
    }
    
    hardware_interface = interface;
    
    // 初始化控制器数组
    memset(led_controllers, 0, sizeof(led_controllers));
    
    system_initialized = true;
    return LED_OK;
}

/**
 * @brief 初始化LED控制器
 */
LedError_t Led_Init(uint8_t led_id, const LedConfig_t* config)
{
    if (!system_initialized || hardware_interface == nullptr) {
        return LED_ERROR_NOT_INIT;
    }
    
    if (!is_valid_led_id(led_id) || config == nullptr) {
        return LED_ERROR_INVALID_ID;
    }
    
    LedController_t* controller = get_led_controller(led_id);
    
    // 配置LED控制器
    controller->interface           = hardware_interface;
    controller->config              = *config;
    controller->current_state       = LED_STATE_OFF;
    controller->last_toggle_time    = 0;
    controller->is_initialized      = false;
    
    // 调用硬件初始化
    LedError_t result = hardware_interface->init(led_id, config);
    if (result == LED_OK) {
        controller->is_initialized = true;
    }
    
    return result;
}

/**
 * @brief 设置LED状态
 */
LedError_t Led_SetState(uint8_t led_id, LedState_t state)
{
    if (!is_valid_led_id(led_id)) {
        return LED_ERROR_INVALID_ID;
    }
    
    LedController_t* controller = get_led_controller(led_id);
    if (!controller->is_initialized) {
        return LED_ERROR_NOT_INIT;
    }
    
    LedError_t result = controller->interface->set_state(led_id, state);
    if (result == LED_OK) {
        if (state != LED_STATE_TOGGLE) {
            controller->current_state = state;
        } else {
            // 切换状态
            controller->current_state = (controller->current_state == LED_STATE_ON) ? 
                                       LED_STATE_OFF : LED_STATE_ON;
        }
    }
    
    return result;
}

/**
 * @brief 获取LED状态
 */
LedState_t Led_GetState(uint8_t led_id)
{
    if (!is_valid_led_id(led_id)) {
        return LED_STATE_OFF;
    }
    
    LedController_t* controller = get_led_controller(led_id);
    if (!controller->is_initialized) {
        return LED_STATE_OFF;
    }
    
    return controller->current_state;
}

/**
 * @brief LED开启
 */
LedError_t Led_On(uint8_t led_id)
{
    return Led_SetState(led_id, LED_STATE_ON);
}

/**
 * @brief LED关闭
 */
LedError_t Led_Off(uint8_t led_id)
{
    return Led_SetState(led_id, LED_STATE_OFF);
}

/**
 * @brief LED切换状态
 */
LedError_t Led_Toggle(uint8_t led_id)
{
    return Led_SetState(led_id, LED_STATE_TOGGLE);
}

/**
 * @brief LED闪烁控制更新
 */
LedError_t Led_BlinkUpdate(uint8_t led_id, uint32_t current_time)
{
    if (!is_valid_led_id(led_id)) {
        return LED_ERROR_INVALID_ID;
    }
    
    LedController_t* controller = get_led_controller(led_id);
    if (!controller->is_initialized) {
        return LED_ERROR_NOT_INIT;
    }
    
    // 检查是否需要切换状态
    if (controller->config.blink_period > 0) {
        uint32_t elapsed = current_time - controller->last_toggle_time;
        
        if (elapsed >= controller->config.blink_period) {
            Led_Toggle(led_id);
            controller->last_toggle_time = current_time;
        }
    }
    
    return LED_OK;
}

/**
 * @brief 设置LED闪烁周期
 */
LedError_t Led_SetBlinkPeriod(uint8_t led_id, uint32_t period_ms)
{
    if (!is_valid_led_id(led_id)) {
        return LED_ERROR_INVALID_ID;
    }
    
    LedController_t* controller = get_led_controller(led_id);
    if (!controller->is_initialized) {
        return LED_ERROR_NOT_INIT;
    }
    
    controller->config.blink_period = period_ms;
    return LED_OK;
}

/**
 * @brief 去初始化LED
 */
LedError_t Led_Deinit(uint8_t led_id)
{
    if (!is_valid_led_id(led_id)) {
        return LED_ERROR_INVALID_ID;
    }
    
    LedController_t* controller = get_led_controller(led_id);
    if (!controller->is_initialized) {
        return LED_ERROR_NOT_INIT;
    }
    
    LedError_t result = LED_OK;
    
    // 调用硬件去初始化（如果接口提供）
    if (controller->interface->deinit != nullptr) {
        result = controller->interface->deinit(led_id);
    }
    
    // 清理控制器状态
    memset(controller, 0, sizeof(LedController_t));
    
    return result;
}

/* ==================== 私有函数实现 ==================== */

/**
 * @brief 检查LED ID是否有效
 */
static bool is_valid_led_id(uint8_t led_id)
{
    return (led_id < MAX_LED_COUNT);
}

/**
 * @brief 获取LED控制器指针
 */
static LedController_t* get_led_controller(uint8_t led_id)
{
    if (is_valid_led_id(led_id)) {
        return &led_controllers[led_id];
    }
    return nullptr;
}
