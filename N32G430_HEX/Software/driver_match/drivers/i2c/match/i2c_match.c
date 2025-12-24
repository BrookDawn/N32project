/**
 * @file i2c_match.c
 * @brief I2C device matching implementation
 * @author BrookDawn
 * @date 2025-12-24
 * @version 0.1
 * @note compat -> ops映射表
 */



#include "drivers/core/device.h"
#include "i2c_ops.h"


static const struct device_t i2c_devices[] = {

    /* N32 设备描述信息 */
    {
        .type = "i2c",
        .name = "i2c1",
        .compatible = "nations,n32-i2c",
        .ops = &n32_i2c_ops,
        .desc = &(struct {
            uint32_t base;
            uint32_t irq;
        }) 
        {
            .base = 0x40005400,
            .irq = 31
        },                   
    },

    /* STM32 设备描述信息 */
    {
        .type = "i2c",
        .name = "i2c1",
        .compatible = "st,stm32-i2c",
        .ops = &stm32_i2c_ops,
        .desc = &(struct {
            uint32_t base;
            uint32_t irq;
        }) 
        {
            .base = 0x40005400,
            .irq = 31
        },                   
    },

    /* AT32 设备描述信息 */
    {
        .type = "i2c",
        .name = "i2c1",
        .compatible = "atmel,at32-i2c",
        .ops = &at32_i2c_ops,
        .desc = &(struct {
            uint32_t base;
            uint32_t irq;
        }) 
        {
            .base = 0x40005400,
            .irq = 31
        },                   
    }
}


REGISTER_COMPATIBLE_DEVICES("i2c", i2c_devices);



