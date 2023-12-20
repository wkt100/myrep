/*******************************************************************************
* Copyright (C): CTEC
* Filename: crc32.c
* Author: Li Meng
* Date:   2009/08/11  8:59
* Version: A001
* Description : 32 bit cyclic redundancy check function
*               if your system is not PowerPC 5200B, please define the marco---ONE_BYTE_CRC.
*               if not define macro "ONE_BYTE_CRC", please use these functions in big end.
* History:  <author>     <date>            <version>      <description>
*           Li Meng      2009/08/11        A001           create this file
*           Sun Guangyue 2010/06/07        A002           delete about little end

*******************************************************************************/
#include "typedef.h"
#include "crc32.h"

#define ONE_BYTE_CRC

/*******************************************************************************
* Macro Definition
*******************************************************************************/
#define CRC_TABLE_SIZE       (256U)         /* 32 bit CRC looup table size */
#define CRC_INDEX_MASK       (0xFFU)        /* CRC index mask */
#define CRC_SUM_MASK         (0xFFFFFFFFU)  /* CRC sum mask */

#define CRC_CONST_1          (1U)           /* const 1 */
#define CRC_CONST_2          (2U)           /* const 2 */
#define CRC_CONST_4          (4U)           /* const 4 */
#define CRC_CONST_8          (8U)           /* const 8 */
#define CRC_CONST_16         (16U)          /* const 16 */
#define CRC_CONST_24         (24U)          /* const 24 */

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
firm_bool_t crc32_calculate(byte_t a_u8_crc_cal_data[], u32_t u32_crc_cal_len, u32_t *p_u32_crc_cal_sum)
{
    /**************************************************************************
    * 32 bit CRC lookup table
    ***************************************************************************/
    static const u32_t crc32_table[CRC_TABLE_SIZE] =
    {
        0x00000000U, 0x8c8cd047U, 0xb5dec035U, 0x39521072U, 0xc77ae0d1U, 0x4bf63096U, 0x72a420e4U, 0xfe28f0a3U,
        0x2232a119U, 0xaebe715eU, 0x97ec612cU, 0x1b60b16bU, 0xe54841c8U, 0x69c4918fU, 0x509681fdU, 0xdc1a51baU,
        0x44654232U, 0xc8e99275U, 0xf1bb8207U, 0x7d375240U, 0x831fa2e3U, 0x0f9372a4U, 0x36c162d6U, 0xba4db291U,
        0x6657e32bU, 0xeadb336cU, 0xd389231eU, 0x5f05f359U, 0xa12d03faU, 0x2da1d3bdU, 0x14f3c3cfU, 0x987f1388U,
        0x88ca8464U, 0x04465423U, 0x3d144451U, 0xb1989416U, 0x4fb064b5U, 0xc33cb4f2U, 0xfa6ea480U, 0x76e274c7U,
        0xaaf8257dU, 0x2674f53aU, 0x1f26e548U, 0x93aa350fU, 0x6d82c5acU, 0xe10e15ebU, 0xd85c0599U, 0x54d0d5deU,
        0xccafc656U, 0x40231611U, 0x79710663U, 0xf5fdd624U, 0x0bd52687U, 0x8759f6c0U, 0xbe0be6b2U, 0x328736f5U,
        0xee9d674fU, 0x6211b708U, 0x5b43a77aU, 0xd7cf773dU, 0x29e7879eU, 0xa56b57d9U, 0x9c3947abU, 0x10b597ecU,
        0xbd526873U, 0x31deb834U, 0x088ca846U, 0x84007801U, 0x7a2888a2U, 0xf6a458e5U, 0xcff64897U, 0x437a98d0U,
        0x9f60c96aU, 0x13ec192dU, 0x2abe095fU, 0xa632d918U, 0x581a29bbU, 0xd496f9fcU, 0xedc4e98eU, 0x614839c9U,
        0xf9372a41U, 0x75bbfa06U, 0x4ce9ea74U, 0xc0653a33U, 0x3e4dca90U, 0xb2c11ad7U, 0x8b930aa5U, 0x071fdae2U,
        0xdb058b58U, 0x57895b1fU, 0x6edb4b6dU, 0xe2579b2aU, 0x1c7f6b89U, 0x90f3bbceU, 0xa9a1abbcU, 0x252d7bfbU,
        0x3598ec17U, 0xb9143c50U, 0x80462c22U, 0x0ccafc65U, 0xf2e20cc6U, 0x7e6edc81U, 0x473cccf3U, 0xcbb01cb4U,
        0x17aa4d0eU, 0x9b269d49U, 0xa2748d3bU, 0x2ef85d7cU, 0xd0d0addfU, 0x5c5c7d98U, 0x650e6deaU, 0xe982bdadU,
        0x71fdae25U, 0xfd717e62U, 0xc4236e10U, 0x48afbe57U, 0xb6874ef4U, 0x3a0b9eb3U, 0x03598ec1U, 0x8fd55e86U,
        0x53cf0f3cU, 0xdf43df7bU, 0xe611cf09U, 0x6a9d1f4eU, 0x94b5efedU, 0x18393faaU, 0x216b2fd8U, 0xade7ff9fU,
        0xd663b05dU, 0x5aef601aU, 0x63bd7068U, 0xef31a02fU, 0x1119508cU, 0x9d9580cbU, 0xa4c790b9U, 0x284b40feU,
        0xf4511144U, 0x78ddc103U, 0x418fd171U, 0xcd030136U, 0x332bf195U, 0xbfa721d2U, 0x86f531a0U, 0x0a79e1e7U,
        0x9206f26fU, 0x1e8a2228U, 0x27d8325aU, 0xab54e21dU, 0x557c12beU, 0xd9f0c2f9U, 0xe0a2d28bU, 0x6c2e02ccU,
        0xb0345376U, 0x3cb88331U, 0x05ea9343U, 0x89664304U, 0x774eb3a7U, 0xfbc263e0U, 0xc2907392U, 0x4e1ca3d5U,
        0x5ea93439U, 0xd225e47eU, 0xeb77f40cU, 0x67fb244bU, 0x99d3d4e8U, 0x155f04afU, 0x2c0d14ddU, 0xa081c49aU,
        0x7c9b9520U, 0xf0174567U, 0xc9455515U, 0x45c98552U, 0xbbe175f1U, 0x376da5b6U, 0x0e3fb5c4U, 0x82b36583U,
        0x1acc760bU, 0x9640a64cU, 0xaf12b63eU, 0x239e6679U, 0xddb696daU, 0x513a469dU, 0x686856efU, 0xe4e486a8U,
        0x38fed712U, 0xb4720755U, 0x8d201727U, 0x01acc760U, 0xff8437c3U, 0x7308e784U, 0x4a5af7f6U, 0xc6d627b1U,
        0x6b31d82eU, 0xe7bd0869U, 0xdeef181bU, 0x5263c85cU, 0xac4b38ffU, 0x20c7e8b8U, 0x1995f8caU, 0x9519288dU,
        0x49037937U, 0xc58fa970U, 0xfcddb902U, 0x70516945U, 0x8e7999e6U, 0x02f549a1U, 0x3ba759d3U, 0xb72b8994U,
        0x2f549a1cU, 0xa3d84a5bU, 0x9a8a5a29U, 0x16068a6eU, 0xe82e7acdU, 0x64a2aa8aU, 0x5df0baf8U, 0xd17c6abfU,
        0x0d663b05U, 0x81eaeb42U, 0xb8b8fb30U, 0x34342b77U, 0xca1cdbd4U, 0x46900b93U, 0x7fc21be1U, 0xf34ecba6U,
        0xe3fb5c4aU, 0x6f778c0dU, 0x56259c7fU, 0xdaa94c38U, 0x2481bc9bU, 0xa80d6cdcU, 0x915f7caeU, 0x1dd3ace9U,
        0xc1c9fd53U, 0x4d452d14U, 0x74173d66U, 0xf89bed21U, 0x06b31d82U, 0x8a3fcdc5U, 0xb36dddb7U, 0x3fe10df0U,
        0xa79e1e78U, 0x2b12ce3fU, 0x1240de4dU, 0x9ecc0e0aU, 0x60e4fea9U, 0xec682eeeU, 0xd53a3e9cU, 0x59b6eedbU,
        0x85acbf61U, 0x09206f26U, 0x30727f54U, 0xbcfeaf13U, 0x42d65fb0U, 0xce5a8ff7U, 0xf7089f85U, 0x7b844fc2U
    };

    firm_bool_t b_crc32_cal_ret = SUCCESS; /* crc32_calculate return value */
    u32_t u32_crc_sum = CRC_SUM_MASK;  /* CRC sum */
    u32_t u32_crc_cal_i = 0U;          /* byte count of CRC sum generate */
    u32_t u32_crc_index = 0U;          /* CRC table index */
    u32_t u32_crc_temp = 0U;          /* CRC u32_t table temp */
#ifndef ONE_BYTE_CRC
    u32_t u32_crc_left = 0U;          /* left count */
    u32_t u32_crc_count = 0U;          /* CRC 4 byte count */
    u32_t *p_u32_data = P_NULL;        /* pointer of CRC data */
    u8_t u8_crc_cal_temp = 0U;         /* CRC u8_t temp */
    u32_t u32_crc_cal_temp = 0U;         /* CRC u32_t temp */
#endif
    /* check argument */
#ifdef ONE_BYTE_CRC
    if ((a_u8_crc_cal_data == P_NULL) || (u32_crc_cal_len == 0U) || (p_u32_crc_cal_sum == P_NULL))   /* pointer or calculate length invalid */
    {
        b_crc32_cal_ret = FAILURE;
    }
#else
    if ((P_NULL == a_u8_crc_cal_data) || (P_NULL == p_u32_crc_cal_sum))
    {
        b_crc32_cal_ret = FAILURE;
    }

    u32_crc_cal_temp = (u32_t)a_u8_crc_cal_data;
    u32_crc_cal_temp = u32_crc_cal_temp & 0x3U;

    if (u32_crc_cal_temp != 0U)    /* pointer invalid */
    {
        b_crc32_cal_ret = FAILURE;
    }

    if (u32_crc_cal_len == 0U)                               /* calculate length invalid */
    {
        b_crc32_cal_ret = FAILURE;
    }
#endif

    /* calculate CRC sum */
    if (SUCCESS == b_crc32_cal_ret)
    {
#ifdef ONE_BYTE_CRC

        for (u32_crc_cal_i = 0U; u32_crc_cal_i < u32_crc_cal_len; u32_crc_cal_i++)
        {

            u32_crc_index = (u32_crc_sum ^ a_u8_crc_cal_data[u32_crc_cal_i]) & CRC_INDEX_MASK;   /* calculate table index */
            u32_crc_sum = crc32_table[u32_crc_index] ^ (u32_crc_sum >> CRC_CONST_8);          /* calculate CRC sum */
        }
#else

        p_u32_data = (u32_t *)a_u8_crc_cal_data;       /* get data address */
        if (u32_crc_cal_len > CRC_CONST_4)
        {
            u32_crc_count = u32_crc_cal_len >> CRC_CONST_2;

            for (u32_crc_cal_i = 0U; u32_crc_cal_i < u32_crc_count; u32_crc_cal_i++)
            {
                u32_crc_temp = p_u32_data[u32_crc_cal_i];                                        /* get 4 bytes data */

                u32_crc_index = (u32_crc_sum ^ (u32_crc_temp >> CRC_CONST_24)) & CRC_INDEX_MASK; /* calculate table index */
                if (u32_crc_index < CRC_TABLE_SIZE)
                {
                    u32_crc_sum = crc32_table[u32_crc_index] ^ (u32_crc_sum >> CRC_CONST_8);      /* calculate CRC sum */
                }

                u32_crc_index = (u32_crc_sum ^ (u32_crc_temp >> CRC_CONST_16)) & CRC_INDEX_MASK; /* calculate table index */
                if (u32_crc_index < CRC_TABLE_SIZE)
                {
                    u32_crc_sum = crc32_table[u32_crc_index] ^ (u32_crc_sum >> CRC_CONST_8);      /* calculate CRC sum */
                }

                u32_crc_index = (u32_crc_sum ^ (u32_crc_temp >> CRC_CONST_8)) & CRC_INDEX_MASK;  /* calculate table index */
                if (u32_crc_index < CRC_TABLE_SIZE)
                {
                    u32_crc_sum = crc32_table[u32_crc_index] ^ (u32_crc_sum >> CRC_CONST_8);      /* calculate CRC sum */
                }

                u32_crc_index = (u32_crc_sum ^ u32_crc_temp) & CRC_INDEX_MASK;                   /* calculate table index */
                if (u32_crc_index < CRC_TABLE_SIZE)
                {
                    u32_crc_sum = crc32_table[u32_crc_index] ^ (u32_crc_sum >> CRC_CONST_8);      /* calculate CRC sum */
                }
            }

            u32_crc_temp = u32_crc_cal_i;
        }

        u32_crc_left = u32_crc_temp << CRC_CONST_2;

        u32_crc_temp = u32_crc_left;
        while (1)
        {
            if (u32_crc_temp >= u32_crc_cal_len)
            {
                break;
            }

            u8_crc_cal_temp = a_u8_crc_cal_data[u32_crc_temp];
            u32_crc_index = (u32_t)(u32_crc_sum ^ (u32_t)u8_crc_cal_temp) & CRC_INDEX_MASK;   /* calculate table index */
            u32_crc_sum = crc32_table[u32_crc_index] ^ (u32_crc_sum >> CRC_CONST_8);          /* calculate CRC sum */

            u32_crc_temp++;
        }
#endif
        /* set CRC sum */
        u32_crc_sum ^= CRC_SUM_MASK;        /* reverse CRC sum */
        *p_u32_crc_cal_sum = u32_crc_sum;   /* set CRC sum */
    }
    /* return */
    return b_crc32_cal_ret;
}

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
firm_bool_t crc32_check(byte_t a_u8_chk_data[], u32_t u32_chk_len, u32_t u32_chk_sum)
{
    firm_bool_t b_crc32_check_ret = SUCCESS;        /* crc32_check return value */
    firm_bool_t b_crc32_check_call_ret = SUCCESS;   /* crc32_check call return value */
    u32_t u32_gen_sum = 0U;                      /* CRC generate sum */
    u32_t u32_recover_sum = 0U;                  /* CRC recover sum */
#ifndef ONE_BYTE_CRC
    u32_t u32_crc32_check_temp = 0U;  /*temp value*/

    /* argument check */
    if ((a_u8_chk_data == P_NULL) || (u32_chk_len == 0U))/* pointer or check length invalid */
    {
        b_crc32_check_call_ret = FAILURE;
    }
    u32_crc32_check_temp = (u32_t)a_u8_chk_data & 0x3U;

    if (0U != u32_crc32_check_temp)
    {
        b_crc32_check_call_ret = FAILURE;
    }
#else
    /* argument check */
    if ((a_u8_chk_data == P_NULL) || (u32_chk_len == 0U))   /* pointer or check length invalid */
    {
        b_crc32_check_call_ret = FAILURE;
    }
#endif
    if (b_crc32_check_call_ret == SUCCESS)
    {
        /* calculate CRC sum */
        b_crc32_check_call_ret = crc32_calculate(a_u8_chk_data, u32_chk_len,
        &u32_gen_sum);   /* generate CRC sum */
    }

    /* recover CRC sum */
    if (b_crc32_check_call_ret == SUCCESS)
    {

        u32_recover_sum = u32_chk_sum;

        if (u32_gen_sum == u32_recover_sum)              /* CRC sum equal */
        {
            b_crc32_check_ret = SUCCESS;                         /* set return value success */
        }
        else
        {
            b_crc32_check_ret = FAILURE;                         /* set return value failure */
        }
    }
    else
    {
        b_crc32_check_ret = FAILURE;                         /* set return value failure */
    }


    /* return */
    return b_crc32_check_ret;
}


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
firm_bool_t crc32_append(byte_t a_u8_apd_data[], u32_t u32_apd_len)
{
    firm_bool_t b_crc32_append_ret = SUCCESS;        /* crc32_append return value */
    u32_t u32_apd_crc_sum = 0U;                   /* CRC sum */
    u32_t u32_apd_i = 0U;                         /* loop variable */
    
#ifndef ONE_BYTE_CRC    
    /*temp value to store the last 2 bits of parameter u32_apd_len*/
    u32_t u32_crc32_apd_temp = 0U;  

    /* check argument */
    if ((a_u8_apd_data == P_NULL) || (u32_apd_len <= CRC_CONST_4)) /* pointer or append length invalid */
    {
        b_crc32_append_ret = FAILURE;
    }

    u32_crc32_apd_temp = (u32_t)a_u8_apd_data & 0x3U;
    if (0U != u32_crc32_apd_temp)
    {
        b_crc32_append_ret = FAILURE;
    }

#else
    /* check argument */
    if ((a_u8_apd_data == P_NULL) || (u32_apd_len <= CRC_CONST_4))   /* pointer or append length invalid */
    {
        b_crc32_append_ret = FAILURE;
    }
#endif
    
    /*check crc32*/
    if (b_crc32_append_ret == SUCCESS)
    {
        /* calculate CRC sum */
        u32_apd_len -= CRC_CONST_4;           /* calculate real data length */
        b_crc32_append_ret = crc32_calculate(a_u8_apd_data, u32_apd_len,
        &u32_apd_crc_sum);  /* generate CRC sum */
    }

    if (b_crc32_append_ret == SUCCESS)
    {
        /* append CRC sum to tail, big-endian */
        u32_apd_len += CRC_CONST_4;               /* add CRC sum data length */
        for (u32_apd_i = 0U; u32_apd_i < CRC_CONST_4; u32_apd_i++)
        {
            a_u8_apd_data[u32_apd_len - CRC_CONST_1 - u32_apd_i]
            = (u8_t)(u32_apd_crc_sum & CRC_INDEX_MASK); /* low byte in high address */

            u32_apd_crc_sum >>= CRC_CONST_8;          /* data shift right */
        }
    }

    /* return */
    return b_crc32_append_ret;
}


