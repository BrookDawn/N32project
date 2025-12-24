/**
 * @file    compatible.h
 * @brief   Compatible structure definition
 * @author  BrookDawn
 * @date    2025-12-24
 * @version 0.1
 */


#ifndef __COMPATIBLE_H__
#define __COMPATIBLE_H__


#include "device.h"



/**
 * @brief  根据设备类型和兼容性字符串匹配设备
 */

const struct device_t *compatible_match(
                        const char* type,
                        const char* compatible);








#endif /* __COMPATIBLE_H__ */