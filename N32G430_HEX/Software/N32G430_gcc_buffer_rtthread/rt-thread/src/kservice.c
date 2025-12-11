/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RT-Thread Console / kprintf
 */

#include <rtthread.h>
#include <rthw.h>
#include <stdarg.h>

#ifdef RT_USING_CONSOLE

/* Forward declarations */
static char *_itoa(int value, char *string, int radix);
int rt_vsnprintf(char *buf, rt_size_t size, const char *fmt, va_list args);

static char rt_log_buf[RT_CONSOLEBUF_SIZE];

/**
 * Console output character (weak, can be overwritten)
 */
RT_WEAK void rt_hw_console_output(const char *str)
{
    /* Empty, need to be implemented in board.c */
    (void)str;
}

/**
 * Kernel printf
 */
void rt_kprintf(const char *fmt, ...)
{
    va_list args;
    rt_size_t length;
    static char *ptr;

    va_start(args, fmt);

    /* Format string */
    length = rt_vsnprintf(rt_log_buf, sizeof(rt_log_buf) - 1, fmt, args);
    if (length > RT_CONSOLEBUF_SIZE - 1)
        length = RT_CONSOLEBUF_SIZE - 1;

    /* Output to console */
    rt_hw_console_output(rt_log_buf);

    va_end(args);
    (void)ptr;
}

/**
 * Simple integer to string
 */
static char *_itoa(int value, char *string, int radix)
{
    char tmp[33];
    char *tp = tmp;
    int i;
    unsigned v;
    int sign;
    char *sp;

    if (radix > 36 || radix <= 1)
    {
        return string;
    }

    sign = (radix == 10 && value < 0);
    if (sign)
    {
        v = -value;
    }
    else
    {
        v = (unsigned)value;
    }

    while (v || tp == tmp)
    {
        i = v % radix;
        v = v / radix;
        if (i < 10)
            *tp++ = i + '0';
        else
            *tp++ = i + 'a' - 10;
    }

    sp = string;

    if (sign)
        *sp++ = '-';
    while (tp > tmp)
        *sp++ = *--tp;
    *sp = '\0';

    return string;
}

/**
 * Simple vsnprintf implementation
 */
int rt_vsnprintf(char *buf, rt_size_t size, const char *fmt, va_list args)
{
    char *str, *end;
    int ch;
    char tmp[12];

    str = buf;
    end = buf + size - 1;

    while ((ch = *fmt++) != '\0' && str < end)
    {
        if (ch != '%')
        {
            *str++ = ch;
            continue;
        }

        ch = *fmt++;
        switch (ch)
        {
        case 'c':
            *str++ = (char)va_arg(args, int);
            break;

        case 's':
        {
            char *s = va_arg(args, char *);
            if (s == RT_NULL)
                s = "(null)";
            while (*s && str < end)
                *str++ = *s++;
        }
        break;

        case 'd':
        case 'i':
        {
            int val = va_arg(args, int);
            char *p = _itoa(val, tmp, 10);
            while (*p && str < end)
                *str++ = *p++;
        }
        break;

        case 'u':
        {
            unsigned int val = va_arg(args, unsigned int);
            char *p = _itoa(val, tmp, 10);
            while (*p && str < end)
                *str++ = *p++;
        }
        break;

        case 'x':
        case 'X':
        {
            unsigned int val = va_arg(args, unsigned int);
            char *p = _itoa(val, tmp, 16);
            while (*p && str < end)
                *str++ = *p++;
        }
        break;

        case 'p':
        {
            unsigned long val = (unsigned long)va_arg(args, void *);
            char *p = _itoa(val, tmp, 16);
            if (str + 2 < end)
            {
                *str++ = '0';
                *str++ = 'x';
            }
            while (*p && str < end)
                *str++ = *p++;
        }
        break;

        case '%':
            *str++ = '%';
            break;

        default:
            *str++ = '%';
            if (str < end)
                *str++ = ch;
            break;
        }
    }

    *str = '\0';
    return str - buf;
}

/**
 * snprintf
 */
int rt_snprintf(char *buf, rt_size_t size, const char *fmt, ...)
{
    int n;
    va_list args;

    va_start(args, fmt);
    n = rt_vsnprintf(buf, size, fmt, args);
    va_end(args);

    return n;
}

#endif /* RT_USING_CONSOLE */
