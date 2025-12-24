/**
 * @file    device.h
 * @brief   Device structure definition
 * @author  BrookDawn
 * @date    2025-12-24
 * @version 0.1
 */


#ifndef __DEVICE_H__
#define __DEVICE_H__


typedef struct device {
    const char* type;       /* 设备类型: "i2c" , "spi" , "gpio" */
    const char* name;       /* 设备标识: "i2c1", "spi1", "uart1" */
    const char* compatible; /* 硬件标识*/
   
    const void* ops;        /* 设备操作函数指针集合 */
    const void* desc;       /* 设备描述信息 */

} device_t;







#endif /* __DEVICE_H__ */