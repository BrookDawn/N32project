/**
 * @file elab_log.c
 * @brief Elab日志模块头文件
 * @author BrookDawn
 * @date 2025-12-19
 * @version 0.1
 * @note 模仿狗哥的elab框架，恩师：极天隙流光
 */

/*=========================== [Includes] ================================*/
#include "elab_log.h"
#include <stdio.h>
#include <stdarg.h>



/*=========================== [Defines] =================================*/

//终端文字颜色
#define NONE                "\033[0;0m"
#define LIGHT_RED           "\033[1;31m"    
#define GREEN               "\033[0;32m"    
#define YELLOW              "\033[0;33m"
#define LIGHT_BLUE          "\033[1;34m"


#define ELAB_LOG_BUFFER_SIZE    256     /**< 日志打印缓冲区大小 */

/*=========================== [Macros] ==================================*/


/*=========================== [Typedefs] ================================*/ 


/*=========================== [Static Variables] ========================*/

//使能日志颜色输出
#if (ELAB_COLOR_ENABLE != 0)
static const char *const elog_color_table[ELAB_LOG_LEVEL_MAX] = 
{
    NONE,           //Debug
    LIGHT_BLUE,     //Info
    GREEN,          //Warn
    YELLOW,         //Error
    LIGHT_RED,      //Fatal 
};
#endif

static const char elog_level_table[ELAB_LOG_LEVEL_MAX] = 
{
    'D',    //Debug
    'I',    //Info
    'W',    //Warn
    'E',    //Error
    'F',    //Fatal 
};

static char _buff[ELAB_LOG_BUFFER_SIZE] = {0};

/*=========================== [Static Functions] ========================*/


/*=========================== [External Functions] ======================*/
void _elab_printf(  const char *name, 
                    uint32_t line, 
                    uint8_t level,
                    const char *format, ...)
{
    printf("%s[%c/%s L:%lu, T%lu] ", elog_color_table[level],
            elog_level_table[level],
            name,
            (unsigned long)line,
            (unsigned long)elab_time_ms());
    va_list args;
    va_start(args, format);
    int count = vsnprintf((char *)_buff, ELAB_LOG_BUFFER_SIZE - 1, format, args);
    va_end(args);
    _buff[count] = '\0';
    printf("%s%s\r\n", _buff, NONE);
    printf(NONE"\r\n");
}


