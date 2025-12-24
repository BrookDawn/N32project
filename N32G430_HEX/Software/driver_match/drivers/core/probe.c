/**
* @file probe.c
* @brief Device probing implementation
* @author BrookDawn
* @date 2025-12-24
* @version 0.1
*/


#include "device.h"
#include "compatible.h"

/** 
 * 统一probe接口实现
  1. 根据设备类型和兼容性字符串匹配设备
  2. 返回设备结构体指针
 */
int device_probe(const char* type, const char* compatible)
{
    const struct device_t* dev;
    dev = compatible_match(type, compatible);

    /*1. 匹配compatible字符串 */
    if (!dev)
        return -1; /* 设备未找到 */
    /*2. 检查设备操作函数指针 */
    if(!dev->ops)
        return -2; 
    /*3. 设备初始化 ops的第一个函数必须是 init*/
    int (*init_func)(const struct device_t*);
    init_func = *(int (**)(const void *))dev->ops;

    return init_func(dev->desc);
}