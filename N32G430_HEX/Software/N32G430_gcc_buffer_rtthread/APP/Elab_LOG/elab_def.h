/**
 * @file elab_def.h
 * @brief 编译器相关定义
 * @author BrookDawn
 * @date 2025-12-19
 * @version 0.1
 * @note 模仿狗哥的elab框架，恩师：极天隙流光
 */

#ifndef __ELAB_DEF_H__
#define __ELAB_DEF_H__



/*=========================== [Includes] ================================*/
#include "elab_std.h"


#ifdef __cplusplus
extern "C" {    
#endif

/*=========================== [Defines] =================================*/

/*Compiler*/

#if defined ( __CC_ARM )  || defined (__CLANG_ARM)      /* ARM Compiler */
    #include "stdarg.h"
    #define ELAB_WEAK __attribute__((weak))
    #define _ELAB_WEAK __attribute__((weak))
#elif  defined ( __IAR_SYSTEMS_ICC__ )                  /* IAR Compiler */
    #include "stdarg.h"
    #define ELAB_WEAK __weak
    #define _ELAB_WEAK __weak
#elif  defined ( __GNUC__ )                             /* GNU Compiler */
    #include "stdarg.h"
    #define ELAB_WEAK __attribute__((weak))
    #define _ELAB_WEAK __attribute__((weak))
#elif defined ( __TASKING__ )                           /* TASKING Compiler */
    #include "stdarg.h"
    #define ELAB_WEAK __attribute__((weak))
    #define _ELAB_WEAK __attribute__((weak))
#else
    #error "Compiler not supported!"
#endif

/*=========================== [Macros] ==================================*/

/*=========================== [Typedefs] ================================*/

/*=========================== [Static Functions] ========================*/

/*=========================== [External Functions] ======================*/



#ifdef __cplusplus
}
#endif


#endif /* __ELAB_DEF_H__ */