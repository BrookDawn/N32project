/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RT-Thread Service
 */

#ifndef __RT_SERVICE_H__
#define __RT_SERVICE_H__

#include <rtdef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Kernel object management */
void rt_object_init(struct rt_object *object,
                    enum rt_object_class_type type,
                    const char *name);
void rt_object_detach(rt_object_t object);

/* String functions */
rt_int32_t rt_strncmp(const char *cs, const char *ct, rt_ubase_t count);
char *rt_strncpy(char *dst, const char *src, rt_ubase_t n);
rt_size_t rt_strlen(const char *s);
void *rt_memset(void *s, int c, rt_ubase_t n);
void *rt_memcpy(void *dst, const void *src, rt_ubase_t n);
rt_int32_t rt_memcmp(const void *cs, const void *ct, rt_ubase_t count);

#ifdef __cplusplus
}
#endif

#endif /* __RT_SERVICE_H__ */
