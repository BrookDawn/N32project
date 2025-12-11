/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RT-Thread Nano for N32G430
 */

#ifndef __RT_DEF_H__
#define __RT_DEF_H__

#include <rtconfig.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RT-Thread version */
#define RT_VERSION                      4
#define RT_SUBVERSION                   1
#define RT_REVISION                     0

/* RT-Thread basic data type definitions */
typedef int                             rt_bool_t;
typedef signed long                     rt_base_t;
typedef unsigned long                   rt_ubase_t;
typedef rt_base_t                       rt_err_t;
typedef rt_ubase_t                      rt_time_t;
typedef rt_ubase_t                      rt_tick_t;
typedef rt_base_t                       rt_flag_t;
typedef rt_ubase_t                      rt_size_t;
typedef rt_ubase_t                      rt_dev_t;
typedef rt_base_t                       rt_off_t;

typedef signed char                     rt_int8_t;
typedef signed short                    rt_int16_t;
typedef signed int                      rt_int32_t;
typedef unsigned char                   rt_uint8_t;
typedef unsigned short                  rt_uint16_t;
typedef unsigned int                    rt_uint32_t;

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
typedef signed long long                rt_int64_t;
typedef unsigned long long              rt_uint64_t;
#elif defined(__GNUC__)
typedef signed long long                rt_int64_t;
typedef unsigned long long              rt_uint64_t;
#else
typedef signed long long                rt_int64_t;
typedef unsigned long long              rt_uint64_t;
#endif

/* boolean type definitions */
#define RT_TRUE                         1
#define RT_FALSE                        0

/* maximum value of base type */
#define RT_UINT8_MAX                    0xff
#define RT_UINT16_MAX                   0xffff
#define RT_UINT32_MAX                   0xffffffff
#define RT_TICK_MAX                     RT_UINT32_MAX

/* null pointer definition */
#define RT_NULL                         ((void *)0)

/* RT-Thread error code definitions */
#define RT_EOK                          0
#define RT_ERROR                        1
#define RT_ETIMEOUT                     2
#define RT_EFULL                        3
#define RT_EEMPTY                       4
#define RT_ENOMEM                       5
#define RT_ENOSYS                       6
#define RT_EBUSY                        7
#define RT_EIO                          8
#define RT_EINTR                        9
#define RT_EINVAL                       10

/* waiting forever */
#define RT_WAITING_FOREVER              ((rt_int32_t)0xFFFFFFFF)
#define RT_WAITING_NO                   ((rt_int32_t)0)

/* RT-Thread object types */
enum rt_object_class_type
{
    RT_Object_Class_Null          = 0x00,
    RT_Object_Class_Thread        = 0x01,
    RT_Object_Class_Semaphore     = 0x02,
    RT_Object_Class_Mutex         = 0x03,
    RT_Object_Class_Event         = 0x04,
    RT_Object_Class_MailBox       = 0x05,
    RT_Object_Class_MessageQueue  = 0x06,
    RT_Object_Class_MemHeap       = 0x07,
    RT_Object_Class_MemPool       = 0x08,
    RT_Object_Class_Device        = 0x09,
    RT_Object_Class_Timer         = 0x0a,
    RT_Object_Class_Module        = 0x0b,
    RT_Object_Class_Unknown       = 0x0c,
    RT_Object_Class_Static        = 0x80
};

/* RT-Thread object container */
struct rt_object
{
    char       name[RT_NAME_MAX];
    rt_uint8_t type;
    rt_uint8_t flag;
    void      *parent;
};
typedef struct rt_object *rt_object_t;

/* Double List structure */
struct rt_list_node
{
    struct rt_list_node *next;
    struct rt_list_node *prev;
};
typedef struct rt_list_node rt_list_t;

/* Timer structure */
struct rt_timer
{
    struct rt_object parent;
    rt_list_t        row[RT_TIMER_SKIP_LIST_LEVEL];
    void (*timeout_func)(void *parameter);
    void            *parameter;
    rt_tick_t        init_tick;
    rt_tick_t        timeout_tick;
};
typedef struct rt_timer *rt_timer_t;

/* Thread state definitions */
#define RT_THREAD_INIT                  0x00
#define RT_THREAD_READY                 0x01
#define RT_THREAD_SUSPEND               0x02
#define RT_THREAD_RUNNING               0x03
#define RT_THREAD_BLOCK                 RT_THREAD_SUSPEND
#define RT_THREAD_CLOSE                 0x04
#define RT_THREAD_STAT_MASK             0x0f

#define RT_THREAD_STAT_YIELD            0x08
#define RT_THREAD_STAT_YIELD_MASK       RT_THREAD_STAT_YIELD

#define RT_THREAD_STAT_SIGNAL           0x10
#define RT_THREAD_STAT_SIGNAL_READY     (RT_THREAD_STAT_SIGNAL | RT_THREAD_READY)
#define RT_THREAD_STAT_SIGNAL_WAIT      0x20
#define RT_THREAD_STAT_SIGNAL_PENDING   0x40
#define RT_THREAD_STAT_SIGNAL_MASK      0xf0

/* Thread control command definitions */
#define RT_THREAD_CTRL_STARTUP          0x00
#define RT_THREAD_CTRL_CLOSE            0x01
#define RT_THREAD_CTRL_CHANGE_PRIORITY  0x02
#define RT_THREAD_CTRL_INFO             0x03
#define RT_THREAD_CTRL_BIND_CPU         0x04

/* Thread structure */
struct rt_thread
{
    struct rt_object parent;

    rt_list_t   tlist;

    /* stack point and entry */
    void       *sp;
    void       *entry;
    void       *parameter;
    void       *stack_addr;
    rt_uint32_t stack_size;

    /* error code */
    rt_err_t    error;

    rt_uint8_t  stat;
    rt_uint8_t  current_priority;
    rt_uint8_t  init_priority;
    rt_uint8_t  number_mask;

#if RT_THREAD_PRIORITY_MAX > 32
    rt_uint8_t  number;
    rt_uint8_t  high_mask;
#endif

    rt_ubase_t  init_tick;
    rt_ubase_t  remaining_tick;

    struct rt_timer thread_timer;

    void (*cleanup)(struct rt_thread *tid);
    rt_uint32_t user_data;
};
typedef struct rt_thread *rt_thread_t;

/* Timer skip list level */
#ifndef RT_TIMER_SKIP_LIST_LEVEL
#define RT_TIMER_SKIP_LIST_LEVEL        1
#endif

/* Timer flag definitions */
#define RT_TIMER_FLAG_DEACTIVATED       0x0
#define RT_TIMER_FLAG_ACTIVATED         0x1
#define RT_TIMER_FLAG_ONE_SHOT          0x0
#define RT_TIMER_FLAG_PERIODIC          0x2
#define RT_TIMER_FLAG_HARD_TIMER        0x0
#define RT_TIMER_FLAG_SOFT_TIMER        0x4

#define RT_TIMER_CTRL_SET_TIME          0x0
#define RT_TIMER_CTRL_GET_TIME          0x1
#define RT_TIMER_CTRL_SET_ONESHOT       0x2
#define RT_TIMER_CTRL_SET_PERIODIC      0x3
#define RT_TIMER_CTRL_GET_STATE         0x4

/* Semaphore structure */
#ifdef RT_USING_SEMAPHORE
struct rt_semaphore
{
    struct rt_object parent;
    rt_list_t        suspend_thread;
    rt_uint16_t      value;
    rt_uint16_t      reserved;
};
typedef struct rt_semaphore *rt_sem_t;
#endif

/* Mutex structure */
#ifdef RT_USING_MUTEX
struct rt_mutex
{
    struct rt_object parent;
    rt_list_t        suspend_thread;
    rt_uint16_t      value;
    rt_uint8_t       original_priority;
    rt_uint8_t       hold;
    struct rt_thread *owner;
};
typedef struct rt_mutex *rt_mutex_t;
#endif

/* IPC flag definitions */
#define RT_IPC_FLAG_FIFO                0x00
#define RT_IPC_FLAG_PRIO                0x01
#define RT_IPC_CMD_UNKNOWN              0x00
#define RT_IPC_CMD_RESET                0x01
#define RT_IPC_CMD_GET_STATE            0x02

/* Kernel service API */
#define RT_KERNEL_MALLOC(sz)            rt_malloc(sz)
#define RT_KERNEL_FREE(ptr)             rt_free(ptr)
#define RT_KERNEL_REALLOC(ptr, size)    rt_realloc(ptr, size)

/* Section definitions */
#if defined(__CC_ARM) || defined(__CLANG_ARM)
    #define SECTION(x)                  __attribute__((section(x)))
    #define RT_UNUSED                   __attribute__((unused))
    #define RT_USED                     __attribute__((used))
    #define ALIGN(n)                    __attribute__((aligned(n)))
    #define RT_WEAK                     __attribute__((weak))
#elif defined (__IAR_SYSTEMS_ICC__)
    #define SECTION(x)                  @ x
    #define RT_UNUSED
    #define RT_USED                     __root
    #define PRAGMA(x)                   _Pragma(#x)
    #define ALIGN(n)                    PRAGMA(data_alignment=n)
    #define RT_WEAK                     __weak
#elif defined (__GNUC__)
    #define SECTION(x)                  __attribute__((section(x)))
    #define RT_UNUSED                   __attribute__((unused))
    #define RT_USED                     __attribute__((used))
    #define ALIGN(n)                    __attribute__((aligned(n)))
    #define RT_WEAK                     __attribute__((weak))
#else
    #define SECTION(x)
    #define RT_UNUSED
    #define RT_USED
    #define ALIGN(n)
    #define RT_WEAK
#endif

/* Compiler Related Definitions */
#if defined(__CC_ARM) || defined(__CLANG_ARM)
    #define rt_inline                   static __inline
    #define INLINE                      __inline
#elif defined (__IAR_SYSTEMS_ICC__)
    #define rt_inline                   static inline
    #define INLINE                      inline
#elif defined (__GNUC__)
    #define rt_inline                   static __inline
    #define INLINE                      __inline__
#else
    #define rt_inline                   static __inline
    #define INLINE                      __inline
#endif

/* initialization export */
typedef int (*init_fn_t)(void);

#ifdef RT_USING_COMPONENTS_INIT
#define INIT_EXPORT(fn, level)  \
    RT_USED const init_fn_t __rt_init_##fn SECTION(".rti_fn." level) = fn
#else
#define INIT_EXPORT(fn, level)
#endif

#define INIT_BOARD_EXPORT(fn)       INIT_EXPORT(fn, "1")
#define INIT_PREV_EXPORT(fn)        INIT_EXPORT(fn, "2")
#define INIT_DEVICE_EXPORT(fn)      INIT_EXPORT(fn, "3")
#define INIT_COMPONENT_EXPORT(fn)   INIT_EXPORT(fn, "4")
#define INIT_ENV_EXPORT(fn)         INIT_EXPORT(fn, "5")
#define INIT_APP_EXPORT(fn)         INIT_EXPORT(fn, "6")

/* RT_ASSERT macro */
#ifdef RT_DEBUG
#define RT_ASSERT(EX)                                                         \
if (!(EX))                                                                    \
{                                                                             \
    rt_kprintf("assertion failed at %s:%d\n", __FILE__, __LINE__);            \
    while (1);                                                                \
}
#else
#define RT_ASSERT(EX)               ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RT_DEF_H__ */
