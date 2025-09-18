/**
 * @file Led.h
 * @brief 抽象LED控制类，与具体硬件解耦
 * @author AI Assistant
 * @version v1.0.0
 * @date 2025-09-18
 * 
 * 该文件定义了LED控制的抽象接口，支持不同MCU平台的适配
 */

#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief LED状态枚举
 */
typedef enum {
    LED_STATE_OFF = 0,      /**< LED关闭 */
    LED_STATE_ON = 1,       /**< LED开启 */
    LED_STATE_TOGGLE = 2    /**< LED切换状态 */
} LedState_t;

/**
 * @brief LED错误代码枚举
 */
typedef enum {
    LED_OK = 0,             /**< 操作成功 */
    LED_ERROR = -1,         /**< 一般错误 */
    LED_ERROR_INVALID_ID = -2, /**< 无效的LED ID */
    LED_ERROR_NOT_INIT = -3    /**< LED未初始化 */
} LedError_t;

/**
 * @brief LED配置结构体
 */
typedef struct {
    uint8_t led_id;         /**< LED ID */
    bool active_low;        /**< 是否低电平有效 */
    uint32_t blink_period;  /**< 闪烁周期（毫秒） */
} LedConfig_t;

/**
 * @brief LED控制接口结构体（硬件抽象层）
 */
typedef struct {
    /**< 硬件初始化函数指针 */
    LedError_t (*init)(uint8_t led_id, const LedConfig_t* config);
    
    /**< 设置LED状态函数指针 */
    LedError_t (*set_state)(uint8_t led_id, LedState_t state);
    
    /**< 获取LED状态函数指针 */
    LedState_t (*get_state)(uint8_t led_id);
    
    /**< 硬件去初始化函数指针 */
    LedError_t (*deinit)(uint8_t led_id);
} LedInterface_t;

/**
 * @brief LED控制器结构体
 */
typedef struct {
    const LedInterface_t* interface;  /**< 硬件接口 */
    LedConfig_t config;               /**< LED配置 */
    LedState_t current_state;         /**< 当前状态 */
    uint32_t last_toggle_time;        /**< 上次切换时间 */
    bool is_initialized;              /**< 初始化标志 */
} LedController_t;

/* ==================== 公共API函数声明 ==================== */

/**
 * @brief 注册LED硬件接口
 * @param interface LED硬件接口指针
 * @return 错误代码
 */
LedError_t Led_RegisterInterface(const LedInterface_t* interface);

/**
 * @brief 初始化LED控制器
 * @param led_id LED ID
 * @param config LED配置
 * @return 错误代码
 */
LedError_t Led_Init(uint8_t led_id, const LedConfig_t* config);

/**
 * @brief 设置LED状态
 * @param led_id LED ID
 * @param state LED状态
 * @return 错误代码
 */
LedError_t Led_SetState(uint8_t led_id, LedState_t state);

/**
 * @brief 获取LED状态
 * @param led_id LED ID
 * @return LED状态
 */
LedState_t Led_GetState(uint8_t led_id);

/**
 * @brief LED开启
 * @param led_id LED ID
 * @return 错误代码
 */
LedError_t Led_On(uint8_t led_id);

/**
 * @brief LED关闭
 * @param led_id LED ID
 * @return 错误代码
 */
LedError_t Led_Off(uint8_t led_id);

/**
 * @brief LED切换状态
 * @param led_id LED ID
 * @return 错误代码
 */
LedError_t Led_Toggle(uint8_t led_id);

/**
 * @brief LED闪烁控制（需要在定时器中调用）
 * @param led_id LED ID
 * @param current_time 当前时间（毫秒）
 * @return 错误代码
 */
LedError_t Led_BlinkUpdate(uint8_t led_id, uint32_t current_time);

/**
 * @brief 设置LED闪烁周期
 * @param led_id LED ID
 * @param period_ms 闪烁周期（毫秒）
 * @return 错误代码
 */
LedError_t Led_SetBlinkPeriod(uint8_t led_id, uint32_t period_ms);

/**
 * @brief 去初始化LED
 * @param led_id LED ID
 * @return 错误代码
 */
LedError_t Led_Deinit(uint8_t led_id);

/* ==================== 配置宏定义 ==================== */

/** 最大支持的LED数量 */
#define MAX_LED_COUNT   8

/** 预定义的LED ID */
#define LED_ID_1        0
#define LED_ID_2        1  
#define LED_ID_3        2
#define LED_ID_STATUS   LED_ID_1
#define LED_ID_ERROR    LED_ID_2
#define LED_ID_COMM     LED_ID_3

/** 预定义的闪烁周期 */
#define LED_BLINK_FAST    100   /**< 快速闪烁 100ms */
#define LED_BLINK_NORMAL  500   /**< 正常闪烁 500ms */
#define LED_BLINK_SLOW    1000  /**< 慢速闪烁 1000ms */

#ifdef __cplusplus
}
#endif

#endif /* __LED_H__ */
