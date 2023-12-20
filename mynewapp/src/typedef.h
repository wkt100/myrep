/******************************************************************************
 * Copyright (C): CTEC
 * Filename: typedef.h
 * Author: Li Meng
 * Date: 2009/08/10  10:57
 * Version: A001
 * Description : basic data type definition
 * History:  <author>     <date>            <version>      <description>
 *           Li Meng      2009/08/10        A001            create this file
 *
 *****************************************************************************/
#ifndef TYPEDEF_H
#define TYPEDEF_H

/******************************************************************************
 * Basic data type definition
 ******************************************************************************/
/* integer type */
typedef unsigned char u8_t;   /* 8 bits unsigned integer */
typedef unsigned short u16_t; /* 16 bits unsigned integer */
typedef unsigned int u32_t;   /* 32 bits unsigned integer */
typedef unsigned long u64_t;
typedef signed char i8_t;   /* 8 bits signed integer */
typedef signed short i16_t; /* 16 bits signed integer */
typedef signed int i32_t;   /* 32 bits signed integer */

/* floating point type */
typedef float f32_t;  /* 32 bits floating point */
typedef double f64_t; /* 64 bits floating point */

/* character type */
typedef char char_t; /* character */

/* boolean type */
typedef u32_t firm_bool_t; /* boolean */

/* memory type */
typedef u8_t byte_t; /* byte */

/******************************************************************************
 * Net data type definition
 ******************************************************************************/
/* real signal type */
typedef struct real_signal
{
    f32_t f32_real_signal_data; /* real_signal_t data */
    u8_t u8_real_signal_stat;   /* real_signal_t status */
    u8_t u8_real_signal_pad[3]; /* real_signal_t pad */
} real_signal_t;

/* int signal type */
typedef struct int_signal
{
    i16_t u16_int_signal_data; /* int_signal_t data */
                               /* qinshiling 20190610: change u16 tp i16 */
    u8_t u8_int_signal_stat;   /* int_signal_t status */
    u8_t u8_int_signal_pad;    /* int_signal_t pad */
} int_signal_t;

/* real net type */
typedef struct real_net
{
    f32_t f32_real_net_data; /* real_net_t data */
} real_net_t;

/* bool signal type */
typedef struct bool_signal
{
    u8_t u8_bool_signal_data; /* bool_signal_t data */
    u8_t u8_bool_signal_stat; /* bool_signal_t status */
} bool_signal_t;

/* signed int net type */
typedef struct int_net
{
    i16_t s16_int_net_data; /* int_net_t data */
} int_net_t;

/* bool net type */
typedef struct bool_net
{
    u8_t u8_bool_net_data; /* bool_net_t data */
} bool_net_t;

/* device signal type */
typedef struct dev_signal
{
    u32_t u32_dev_signal_stat; /* dev_signal_t status */
} dev_signal_t;

/******************************************************************************
 * Extended data type definition
 ******************************************************************************/
/* register type */
typedef volatile unsigned char reg8_t;   /* 8 bits register */
typedef volatile unsigned short reg16_t; /* 16 bits register */
typedef volatile unsigned int reg32_t;   /* 32 bits register */

/*******************************************************************************
 * Macro Definition
 *******************************************************************************/
#define FALSE ((firm_bool_t)0U) /* boolean false */
#define TRUE ((firm_bool_t)1U)  /* boolean true */
#define FAILURE (FALSE)         /* boolean false */
#define SUCCESS (TRUE)          /* boolean true */
#define P_NULL ((void *)0)      /* null pointer */

#endif /* TYPEDEF_H */
