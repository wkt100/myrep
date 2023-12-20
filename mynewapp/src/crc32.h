/*******************************************************************************
* Copyright (C): CTEC
* Filename: crc32.h
* Author: Li Meng
* Date:   2009/08/11  8:59
* Version: A001
* Description : 32 bit cyclic redundancy check function
*               if your system is not PowerPC 5200B, please define the marco---ONE_BYTE_CRC.
*               if not define macro "ONE_BYTE_CRC", please use these functions in big end.
* History:  <author>     <date>            <version>      <description>
*           Li Meng      2009/08/11        A001           create this file
*           Sun Guangyue 2010/06/07        A002           delete about little end
*
*******************************************************************************/
#ifndef CRC32_H
#define CRC32_H
#include "typedef.h"

/*******************************************************************************
* Function: crc32_calculate
* Identifier: COD-1LIB-32-031 (Trace to: SPD-1LIB-32-031)
* Description: calculate the 32 bit CRC sum
* Input: a_u8_crc_cal_data---start address of CRC sum calculate data
*        u32_crc_cal_len---byte count of CRC sum calculate, the length not include the 4 bytes CRC sum
* Output: p_u32_cal_crc_sum---output address of CRC sum
* Return: SUCCESS---calculate CRC sum finish
*         FAILURE---calculate CRC sum failure
* Others: polynomial---x32+x31+x29+x28+x27+x25+x19+x18+x16+x15+x14+x10+x9+x6+x5+x3+x1+1 
*         analysis-----1 1011 1010 0000 1101 1100 0110 0110 1011
*         check code---0xBA0DC66B
*         if your system is not PowerPC 5200B, please define the marco---ONE_BYTE_CRC
* Log: 2009/08/11
*******************************************************************************/
extern firm_bool_t crc32_calculate(byte_t a_u8_crc_cal_data[], u32_t u32_crc_cal_len, u32_t *p_u32_crc_cal_sum);

/*******************************************************************************
* Function: crc32_check
* Identifier: COD-1LIB-32-032 (Trace to: SPD-1LIB-32-032)
* Description: check the CRC sum whether is correct
* Input: a_u8_chk_data---start address of CRC check
*        u32_chk_len---byte count of CRC check, the length not include the 4 bytes CRC sum
*        u32_chk_sum---input CRC sum from user
* Output: None
* Return: SUCCESS---CRC sum is correct
*         FAILURE---CRC sum is incorrect
* Others: None
* Log: 2010/06/07
*******************************************************************************/
extern firm_bool_t crc32_check(byte_t a_u8_chk_data[], u32_t u32_chk_len, u32_t u32_chk_sum);


/*******************************************************************************
* Function: crc32_append
* Identifier: COD-1LIB-32-033 (Trace to: SPD-1LIB-32-033)
* Description: append the CRC sum to tail use big endian data
* Input: a_u8_apd_data---start address of CRC sum append
*        u32_apd_len---byte count of CRC sum calculate, the length include the 4 bytes CRC sum
* Output: None
* Return: SUCCESS---CRC append finish
*         FAILURE---CRC append undo
* Others: None
* Log: 2009/08/11
*******************************************************************************/
extern firm_bool_t crc32_append(byte_t a_u8_apd_data[], u32_t u32_apd_len);

#endif /* CRC32_H */
