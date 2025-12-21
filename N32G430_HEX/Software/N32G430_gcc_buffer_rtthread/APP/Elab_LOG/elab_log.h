/**
 * @file elab_log.h
 * @brief Elab日志模块头文件
 * @author BrookDawn
 * @date 2025-12-19
 * @version 0.1
 * @note 模仿狗哥的elab框架，恩师：极天隙流光
 */


#ifndef __ELAB_LOG_H__
#define __ELAB_LOG_H__

/*=========================== [Includes] ================================*/

#include "elab_std.h"
#include "elab_def.h"
#include "elab_common.h"


/*=========================== [Defines] =================================*/
#define ELAB_COLOR_ENABLE      1       /**< 是否使能颜色输出，0：不使能，1：使能 */





#define ELOG_DEBUG(...)     _elab_printf(__FILE__, __LINE__, ELAB_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define ELOG_INFO(...)      _elab_printf(__FILE__, __LINE__, ELAB_LOG_LEVEL_INFO,  __VA_ARGS__)
#define ELOG_WARN(...)      _elab_printf(__FILE__, __LINE__, ELAB_LOG_LEVEL_WARN,  __VA_ARGS__)
#define ELOG_ERROR(...)     _elab_printf(__FILE__, __LINE__, ELAB_LOG_LEVEL_ERROR, __VA_ARGS__)
#define ELOG_FATAL(...)     _elab_printf(__FILE__, __LINE__, ELAB_LOG_LEVEL_FATAL, __VA_ARGS__)


#ifdef __cplusplus
extern "C" {   
#endif

/*=========================== [Macros] ==================================*/




/*=========================== [Typedefs] ================================*/

/**
 * @brief 日志等级枚举定义
 */
typedef enum
{
    ELAB_LOG_LEVEL_DEBUG = 0,           /**< 调试信息 */
    ELAB_LOG_LEVEL_INFO  = 1,           /**< 普通信息 */
    ELAB_LOG_LEVEL_WARN  = 2,           /**< 警告信息 */
    ELAB_LOG_LEVEL_ERROR = 3,           /**< 错误信息 */
    ELAB_LOG_LEVEL_FATAL = 4,           /**< 严重错误信息 */

    ELAB_LOG_LEVEL_MAX
} ElabLogLevel_t;


/*=========================== [Static Functions] ========================*/



/*=========================== [External Functions] ======================*/

void _elab_printf(const char *name, uint32_t line, uint8_t level, const char *format, ...);



#ifdef __cplusplus
}
#endif

#endif /* __ELAB_LOG_H__ */
