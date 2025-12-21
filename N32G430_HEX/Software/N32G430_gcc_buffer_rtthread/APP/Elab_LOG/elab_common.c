/**
 * @file elab_common.c
 * @brief 此文件与Linux Poxis 和 win32相关
 * @author BrookDawn
 * @date 2025-12-19
 * @version 0.1
 * @note 模仿狗哥的elab框架，恩师：极天隙流光
 */

/*=========================== [Includes] ================================*/
#include "elab_common.h"

/*=========================== [Defines] =================================*/


/*=========================== [Macros] ==================================*/


/*=========================== [Typedefs] ================================*/ 


/*=========================== [Static Functions] ========================*/


/*=========================== [External Functions] ======================*/

// 弱函数实现：获取系统时间，单位：毫秒
_ELAB_WEAK uint32_t elab_time_ms(void)
{
    //对liunx或win32系统可在此处实现具体时间获取逻辑
#if defined(__linux__) || defined(_WIN32)

    return 0;
    
#else
    return 0;

#endif
}
