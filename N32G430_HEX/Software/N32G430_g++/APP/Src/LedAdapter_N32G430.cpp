/**
 * @file LedAdapter_N32G430.cpp
 * @brief N32G430平台LED硬件适配层实现
 * @author AI Assistant
 * @version v1.0.0
 * @date 2025-09-18
 */

#include "Led.h"

// 包含N32G430的BSP头文件
extern "C" {
#include "bsp_led.h"
#include "n32g430_gpio.h"
#include "n32g430_rcc.h"
}

/* ==================== 平台相关配置 ==================== */

/**
 * @brief N32G430 LED硬件配置结构体
 */
typedef struct {
    GPIO_Module* gpio_port;     /**< GPIO端口 */
    uint16_t gpio_pin;          /**< GPIO引脚 */
    uint32_t gpio_clk;          /**< GPIO时钟 */
    bool is_configured;         /**< 配置标志 */
} N32G430_LedHwConfig_t;

/**
 * @brief N32G430平台LED硬件配置表
 */
static N32G430_LedHwConfig_t led_hw_configs[MAX_LED_COUNT] = {
    // LED_ID_1 (PA1)
    {
        .gpio_port      = LED1_GPIO_PORT,
        .gpio_pin       = LED1_GPIO_PIN,
        .gpio_clk       = LED1_GPIO_CLK,
        .is_configured  = false
    },
    // LED_ID_2 (PA2)  
    {
        .gpio_port      = LED2_GPIO_PORT,
        .gpio_pin       = LED2_GPIO_PIN,
        .gpio_clk       = LED2_GPIO_CLK,
        .is_configured  = false
    },
    // LED_ID_3 (PA3) 
    {
        .gpio_port      = LED3_GPIO_PORT,
        .gpio_pin       = LED3_GPIO_PIN,
        .gpio_clk       = LED3_GPIO_CLK,
        .is_configured  = false
    },
    // 其他LED配置可以在这里添加
    {0}, {0}, {0}, {0}, {0}
};

/* ==================== 私有函数声明 ==================== */

static LedError_t n32g430_led_init(uint8_t led_id, const LedConfig_t* config);
static LedError_t n32g430_led_set_state(uint8_t led_id, LedState_t state);
static LedState_t n32g430_led_get_state(uint8_t led_id);
static LedError_t n32g430_led_deinit(uint8_t led_id);
static bool is_valid_hw_led_id(uint8_t led_id);
static void configure_gpio_for_led(const N32G430_LedHwConfig_t* hw_config, bool active_low);

/* ==================== 硬件接口定义 ==================== */

/**
 * @brief N32G430 LED硬件接口
 */
static const LedInterface_t n32g430_led_interface = {
    .init       = n32g430_led_init,
    .set_state  = n32g430_led_set_state,
    .get_state  = n32g430_led_get_state,
    .deinit     = n32g430_led_deinit
};

/* ==================== 公共API函数 ==================== */

extern "C" {

/**
 * @brief 初始化N32G430 LED适配器
 * @return 错误代码
 */
LedError_t LedAdapter_N32G430_Init(void)
{
    return Led_RegisterInterface(&n32g430_led_interface);
}

/**
 * @brief 快速初始化所有可用的LED
 * @return 错误代码
 */
LedError_t LedAdapter_N32G430_InitAllLeds(void)
{
    // 首先初始化适配器
    LedError_t result = LedAdapter_N32G430_Init();
    if (result != LED_OK) 
    {
        return result;
    }
    
    // 初始化所有有效的LED
    for (uint8_t i = 0; i < MAX_LED_COUNT; i++) 
    {
        if (led_hw_configs[i].gpio_port != nullptr) 
        {
            LedConfig_t config = 
            {
                .led_id         = i,
                .active_low     = false,
                .blink_period   = 0  // 默认不闪烁
            };
            //函数初始化
            Led_Init(i, &config);
        }
    }
    
    return LED_OK;
}

} // extern "C"

/* ==================== 硬件接口实现 ==================== */

/**
 * @brief N32G430 LED硬件初始化
 */
static LedError_t n32g430_led_init(uint8_t led_id, const LedConfig_t* config)
{
    if (!is_valid_hw_led_id(led_id) || config == nullptr) 
    {
        return LED_ERROR_INVALID_ID;
    }
    
    N32G430_LedHwConfig_t* hw_config = &led_hw_configs[led_id];
    
    if (hw_config->gpio_port == nullptr) 
    {
        return LED_ERROR_INVALID_ID;
    }
    
    // 使用BSP函数初始化LED GPIO
    LED_Initialize(hw_config->gpio_port, hw_config->gpio_pin, hw_config->gpio_clk);
    
    // 根据配置设置初始状态
    configure_gpio_for_led(hw_config, config->active_low);
    
    hw_config->is_configured = true;
    
    return LED_OK;
}

/**
 * @brief N32G430 LED状态设置
 */
static LedError_t n32g430_led_set_state(uint8_t led_id, LedState_t state)
{
    if (!is_valid_hw_led_id(led_id)) 
    {
        return LED_ERROR_INVALID_ID;
    }
    
    N32G430_LedHwConfig_t* hw_config = &led_hw_configs[led_id];
    
    if (!hw_config->is_configured) 
    {
        return LED_ERROR_NOT_INIT;
    }
    
    switch (state) 
    {
        case LED_STATE_ON:
            LED_On(hw_config->gpio_port, hw_config->gpio_pin);
            break;
            
        case LED_STATE_OFF:
            LED_Off(hw_config->gpio_port, hw_config->gpio_pin);
            break;
            
        case LED_STATE_TOGGLE:
            LED_Toggle(hw_config->gpio_port, hw_config->gpio_pin);
            break;
            
        default:
            return LED_ERROR;
    }
    
    return LED_OK;
}

/**
 * @brief N32G430 LED状态获取
 */
static LedState_t n32g430_led_get_state(uint8_t led_id)
{
    if (!is_valid_hw_led_id(led_id)) 
    {
        return LED_STATE_OFF;
    }
    
    N32G430_LedHwConfig_t* hw_config = &led_hw_configs[led_id];
    
    if (!hw_config->is_configured) 
    {
        return LED_STATE_OFF;
    }
    
    // 读取GPIO输出状态
    uint8_t pin_state = GPIO_Output_Pin_Data_Get(hw_config->gpio_port, hw_config->gpio_pin);
    
    return (pin_state != 0) ? LED_STATE_ON : LED_STATE_OFF;
}

/**
 * @brief N32G430 LED去初始化
 */
static LedError_t n32g430_led_deinit(uint8_t led_id)
{
    if (!is_valid_hw_led_id(led_id)) 
    {
        return LED_ERROR_INVALID_ID;
    }
    
    N32G430_LedHwConfig_t* hw_config = &led_hw_configs[led_id];
    
    if (hw_config->is_configured) 
    {
        // 关闭LED
        LED_Off(hw_config->gpio_port, hw_config->gpio_pin);
        hw_config->is_configured = false;
    }
    
    return LED_OK;
}

/* ==================== 私有函数实现 ==================== */

/**
 * @brief 检查硬件LED ID是否有效
 */
static bool is_valid_hw_led_id(uint8_t led_id)
{
    return (led_id < MAX_LED_COUNT && led_hw_configs[led_id].gpio_port != nullptr);
}

/**
 * @brief 配置GPIO用于LED控制
 */
static void configure_gpio_for_led(const N32G430_LedHwConfig_t* hw_config, bool active_low)
{
    // 这里可以根据active_low参数配置GPIO的极性
    // 当前使用默认的BSP配置
    (void)active_low; // 暂时未使用，避免警告
}

/* ==================== 便利函数 ==================== */
