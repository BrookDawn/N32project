/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RT-Thread Object Management
 */

#include <rtthread.h>
#include <rthw.h>

/**
 * Initialize kernel object
 */
void rt_object_init(struct rt_object *object,
                    enum rt_object_class_type type,
                    const char *name)
{
    register rt_base_t temp;
    int i;

    object->type = type | RT_Object_Class_Static;
    object->flag = 0;
    object->parent = RT_NULL;

    /* Copy name */
    for (i = 0; i < RT_NAME_MAX - 1; i++)
    {
        if (name[i] == '\0')
            break;
        object->name[i] = name[i];
    }
    object->name[i] = '\0';

    (void)temp;
}

/**
 * Detach kernel object
 */
void rt_object_detach(rt_object_t object)
{
    /* Not implemented for nano version */
    (void)object;
}

/**
 * String compare
 */
rt_int32_t rt_strncmp(const char *cs, const char *ct, rt_ubase_t count)
{
    register signed char __res = 0;

    while (count)
    {
        if ((__res = *cs - *ct++) != 0 || !*cs++)
            break;
        count--;
    }

    return __res;
}

/**
 * String copy
 */
char *rt_strncpy(char *dst, const char *src, rt_ubase_t n)
{
    if (n != 0)
    {
        char *d = dst;
        const char *s = src;

        do
        {
            if ((*d++ = *s++) == 0)
            {
                while (--n != 0)
                    *d++ = 0;
                break;
            }
        } while (--n != 0);
    }

    return dst;
}

/**
 * String length
 */
rt_size_t rt_strlen(const char *s)
{
    const char *sc;

    for (sc = s; *sc != '\0'; ++sc) /* nothing */;

    return sc - s;
}

/**
 * Memory set
 */
void *rt_memset(void *s, int c, rt_ubase_t n)
{
    char *xs = (char *)s;

    while (n--)
        *xs++ = (char)c;

    return s;
}

/**
 * Memory copy
 */
void *rt_memcpy(void *dst, const void *src, rt_ubase_t n)
{
    char *d = (char *)dst;
    const char *s = (const char *)src;

    while (n--)
        *d++ = *s++;

    return dst;
}

/**
 * Memory compare
 */
rt_int32_t rt_memcmp(const void *cs, const void *ct, rt_ubase_t count)
{
    const unsigned char *su1, *su2;
    int res = 0;

    for (su1 = (const unsigned char *)cs, su2 = (const unsigned char *)ct; 0 < count; ++su1, ++su2, count--)
    {
        if ((res = *su1 - *su2) != 0)
            break;
    }

    return res;
}
