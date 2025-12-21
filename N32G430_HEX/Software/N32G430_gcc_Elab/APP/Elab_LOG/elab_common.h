/**
 * @file elab_common.h
 * @brief 此文件与Linux Poxis 和 win32相关
 * @author BrookDawn
 * @date 2025-12-19
 * @version 0.1
 * @note 模仿狗哥的elab框架，恩师：极天隙流光
 */

#ifndef __ELAB_COMMON_H__
#define __ELAB_COMMON_H__

/*=========================== [Includes] ================================*/

#include "elab_std.h"   
#include "elab_def.h"

#ifdef __cplusplus

extern "C" {   
#endif


/*=========================== [Defines] =================================*/


/*=========================== [Macros] ==================================*/


/*=========================== [Typedefs] ================================*/ 


/*=========================== [Static Functions] ========================*/


/*=========================== [External Functions] ======================*/

_ELAB_WEAK uint32_t elab_time_ms(void); /**< 获取系统时间，单位：毫秒 */



#ifdef __cplusplus
}
#endif

#endif /* __ELAB_COMMON_H__ */