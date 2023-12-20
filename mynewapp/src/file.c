#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file.h"
#include "error.h"
#include "crc32.h"

static u32_t parse_trend_var(u32_t u32_trend_var_size, trend_var_info_t *ptrend_var_info, u32_t *pu32_file_data);
static u32_t parse_trend_group(trend_group_info_t *ptrend_group_info, u32_t *pu32_file_data);
static u32_t parse_vars_info(vars_info_t *pvars_info, u32_t *pu32_file_data);
static u32_t set_trend_info(trend_group_info_t *ptrend_group_info, trend_var_info_t *ptrend_var_info, vars_info_t *pvars_info);
static char_t init_err_message[1024] = {0};
static u32_t check_comma_size(u32_t u32_check_size, char_t *pch_buf);
static u32_t replace_special_character(char_t *pch_str);
static u32_t replace_special_symbol(char_t *p_ch_buf);
static u32_t replace_greater_symbol(char_t *p_ch_buf);
static u32_t replace_less_symbol(char_t *p_ch_buf);
static u32_t str_right_shift(char_t *pch_str, u32_t u32_offset);

extern firm_bool_t file_read(u32_t u32_read_buf, u32_t u32_size, char_t *p_ch_file_name)
{
    firm_bool_t b_ret = SUCCESS; /* return value */
    u32_t u32_read_bytes = 0U;   /* read bytes */
    FILE *fp = NULL;             /* file pointer */

    i32_t i32_ret = 0;

    /* check input parameters */
    if ((0 == u32_read_buf) || (0U == u32_size) || (P_NULL == p_ch_file_name))
    {
        b_ret = FAILURE;
    }

    /* read file */
    if (SUCCESS == b_ret)
    {
        /* open file */
        //fp = fopen(p_ch_file_name, 0x11U);
    	fp = fopen(p_ch_file_name, "r");
    	fseek(fp, 0, SEEK_SET);
        /* check file pointer */
        if (NULL != fp)
        {
            /* read data */
        	//printf("success!!!");
            u32_read_bytes = fread((u8_t *)u32_read_buf,
                                   u32_size,	//1048576
                                   1U,
                                   fp);

            if (u32_read_bytes != 1U)	// read failed or end, ken
            {
                i32_ret = fseek(fp, 0, SEEK_SET);

                if (i32_ret != ERROR)
                {
                    /* read data */
                    u32_read_bytes = fread((u8_t *)u32_read_buf,
                                           1U,
                                           u32_size,
                                           fp);

                    if (u32_read_bytes != u32_size)	// 20263
                    {
                        b_ret = FAILURE;
                    }
                }
                else
                {
                    b_ret = FAILURE;
                }
            }

            fclose(fp);
        }
        else
        {
        	// printf("Fail to load!!!");
            b_ret = FAILURE;
        }
    }

    return b_ret;
}

extern u32_t parse_download_var_csv(trend_var_file_head_t *ptrend_var_file_head, trend_info_t *ptrend_info, vars_info_t *pvars_info, char_t *pch_recv_array)
{
    u32_t u32_err = OK;          /* function return value */
    firm_bool_t b_ret = FAILURE; /* function return value */
    u32_t u32_index = 0;         /* index */
    u32_t u32_app_data_addr = 0; /* app data start pointer */
    u32_t u32_file_data = 0;     /* download trend var addr */

    if ((P_NULL == ptrend_var_file_head) || (P_NULL == ptrend_info) || (P_NULL == pvars_info) || (P_NULL == pch_recv_array))
    {
        /* invalid inputs */
        u32_err = ERR_FAILED;
    }

    if (OK == u32_err)
    {
        /* qinshiling 20191011 start: add init var */
        memset(ptrend_info, 0, sizeof(trend_info_t));
        memset(pvars_info, 0, sizeof(vars_info_t));
        /* qinshiling 20191011 end: add init var */
        /* get app start pointer( pch_recv_array + 20 + ",\n") */
        u32_app_data_addr = (u32_t)pch_recv_array + sizeof(trend_var_file_head_t) + 2;

        /* get download_trend_var.csv file head  */
        memcpy(ptrend_var_file_head, pch_recv_array, sizeof(trend_var_file_head_t));
#if 0
        printf("version: %u\n", ptrend_var_file_head->u32_version_id);
        printf("trend_var_size: %u\n", ptrend_var_file_head->u32_trend_var_size);
        printf("data_len: %u\n", ptrend_var_file_head->u32_data_len);
        printf("data_crc: %u\n", ptrend_var_file_head->u32_data_crc);
        printf("head_crc: %u\n", ptrend_var_file_head->u32_head_crc);
#endif
        /* check file length */
        if (ptrend_var_file_head->u32_data_len > 10U * 1024U * 1024U)
        {
            /* file length > 10MB */
            u32_err = ERR_FAILED;
        }
    }

    if (OK == u32_err)
    {
        /* check data crc */
        b_ret = crc32_check((byte_t *)u32_app_data_addr, ptrend_var_file_head->u32_data_len, ptrend_var_file_head->u32_data_crc);
        if (FAILURE == b_ret)
        {
            /* data crc check failure */
            u32_err = ERR_FAILED;
        }
    }

    if (OK == u32_err)
    {
        /* check head crc */
        b_ret = crc32_check((byte_t *)pch_recv_array, sizeof(trend_var_file_head_t) - 4, ptrend_var_file_head->u32_head_crc);
        if (FAILURE == b_ret)
        {
            /* head crc check failure */
            u32_err = ERR_FAILED;
        }
    }

    if (OK == u32_err)
    {
        /* read trend var information */
        /* qinshiling 20181130 start: 为支持中文，将输入文件编码格式改为UTF-8，文件读取地址需增加2字节 */
        char_t bom_temp[3] = {0xEF, 0xBB, 0xBF};	// UTF-8 BOM
        if (0 == strncmp(bom_temp, (char_t *)u32_app_data_addr, 3))
        {
            u32_file_data = u32_app_data_addr + 3;
        }
        else
        {
            u32_file_data = u32_app_data_addr;
        }
        /* qinshiling 20181130 start: 为支持中文，将输入文件编码格式改为UTF-8，文件读取地址需增加2字节 */
        u32_err = parse_trend_var(ptrend_var_file_head->u32_trend_var_size, &ptrend_info->trend_var_info, &u32_file_data); /* trend var make parse */
    }

    if (OK == u32_err && 0 != ptrend_var_file_head->u32_trend_var_size)
    {
        /* trend var size if not 0 */
        u32_err = parse_trend_group(&ptrend_info->trend_group_info, &u32_file_data); /* trend group make parse */
    }

    if (OK == u32_err)
    {
        u32_err = parse_vars_info(pvars_info, &u32_file_data); /* vars information make parse */
    }

    if (OK == u32_err && 0 != ptrend_var_file_head->u32_trend_var_size)
    {
        u32_err = set_trend_info(&ptrend_info->trend_group_info, &ptrend_info->trend_var_info, pvars_info); /* set trend var information */
    }

    if (OK == u32_err && 0 != ptrend_var_file_head->u32_trend_var_size)
    {
        ptrend_info->trend_var_info.u32_trend_data_len = 0;
        for (u32_index = 0; u32_index < ptrend_info->trend_var_info.u32_trend_var_size; u32_index++)
        {
            switch (ptrend_info->trend_var_info.trend_var_buf[u32_index].u32_var_type)
            {
				case VAR_TYPE_REAL_SIGNAL:
					ptrend_info->trend_var_info.u32_trend_data_len += sizeof(real_signal_t);
					break;
				case VAR_TYPE_INT_SIGNAL:
					ptrend_info->trend_var_info.u32_trend_data_len += sizeof(int_signal_t);
					break;
				case VAR_TYPE_REAL:
					ptrend_info->trend_var_info.u32_trend_data_len += sizeof(real_net_t);
					break;
				case VAR_TYPE_BOOL_SIGNAL:
					ptrend_info->trend_var_info.u32_trend_data_len += sizeof(bool_signal_t);
					break;
				case VAR_TYPE_INT:
					ptrend_info->trend_var_info.u32_trend_data_len += sizeof(int_net_t);
					break;
				case VAR_TYPE_BOOL:
					ptrend_info->trend_var_info.u32_trend_data_len += sizeof(bool_net_t);
					break;
				case VAR_TYPE_DEVICE_SIGNAL:
					ptrend_info->trend_var_info.u32_trend_data_len += sizeof(dev_signal_t);
					break;
				default:
					u32_err = ERR_FAILED;
					break;
            }
        }
    }

    if (OK == u32_err && 0 != ptrend_var_file_head->u32_trend_var_size)
    {
        ptrend_info->trend_group_info.u32_trend_data_len = ptrend_info->trend_var_info.u32_trend_data_len;
    }

    return u32_err;
}

extern u32_t force_var(var_hash_t *s_p_st_var_hash, char_t *p_ch_var_name, char_t *p_ch_var_val, char_t *p_ch_var_sig)
{
	u32_t u32_err = OK;          /* function return value */

#ifndef TEST
	var_hash_t *p_st_var = P_NULL; // 查找到的变量

	if ((P_NULL == p_ch_var_name) || (P_NULL == p_ch_var_val) || (P_NULL == p_ch_var_sig))
	{
		// 无变量名称或强制值
		u32_err = 1U;
	}
	else
	{
		// 根据变量名称从变量列表中查找变量描述
		HASH_FIND_STR(s_p_st_var_hash, p_ch_var_name, p_st_var);
		if (P_NULL == p_st_var)
		{
			// 变量名称无效
			u32_err = 2U;
		}
		else
		{
			// 设置变量强制值
			u32_t u32_var_addr;
			get_area_address(p_st_var->u32_var_id, &u32_var_addr);
			u32_var_addr += p_st_var->u32_var_offset;
			switch (p_st_var->u32_var_type)
			{
			case VAR_TYPE_REAL_SIGNAL:
			{
				real_signal_t *p_st_rs_val = (real_signal_t*)(u32_var_addr);
				p_st_rs_val->f32_real_signal_data = atof(p_ch_var_val);
				p_st_rs_val->u8_real_signal_stat = atoi(p_ch_var_sig);
				break;
			}
			case VAR_TYPE_INT_SIGNAL:
			{
				int_signal_t *p_st_is_val = (int_signal_t *)u32_var_addr;
				p_st_is_val->u16_int_signal_data = atoi(p_ch_var_val);
				p_st_is_val->u8_int_signal_stat = atoi(p_ch_var_sig);
				break;
			}
			case VAR_TYPE_REAL:
			{
				real_net_t *p_st_rn_val = (real_net_t *)u32_var_addr;
				p_st_rn_val->f32_real_net_data = atof(p_ch_var_val);
				break;
			}
			case VAR_TYPE_BOOL_SIGNAL:
			{
				bool_signal_t *p_st_bs_val = (bool_signal_t *)u32_var_addr;
				p_st_bs_val->u8_bool_signal_data = atoi(p_ch_var_val);
				p_st_bs_val->u8_bool_signal_stat = atoi(p_ch_var_sig);
				break;
			}
			case VAR_TYPE_INT:
			{
				int_net_t *p_st_in_val = (int_net_t *)u32_var_addr;
				p_st_in_val->s16_int_net_data = atoi(p_ch_var_val);
				break;
			}
			case VAR_TYPE_BOOL:
			{
				bool_net_t *p_st_bn_val = (bool_net_t *)u32_var_addr;
				p_st_bn_val->u8_bool_net_data = atoi(p_ch_var_val);
				break;
			}
			case VAR_TYPE_DEVICE_SIGNAL:
			{
				dev_signal_t *p_st_ds_val = (dev_signal_t *)u32_var_addr;
				p_st_ds_val->u32_dev_signal_stat = atoi(p_ch_var_val);
				break;
			}
			default:
				u32_err = 3U;
				break;
			}
		}
	}
#endif

}

static u32_t parse_trend_var(u32_t u32_trend_var_size, trend_var_info_t *ptrend_var_info, u32_t *pu32_file_data)
{
	i32_t i32_ret = 1; /* function return value */
	u32_t u32_err = OK;		/* function return value */
	u32_t u32_line = UINT_0;
	u32_t u32_str = UINT_0;
	u32_t u32_comma = UINT_0;
	u32_t u32_var = UINT_0;
	char_t ch_max_data[VAR_MAX_SIZE_MAX] = {0};	  /* string to save var max data */
	char_t ch_mini_data[VAR_MINI_SIZE_MAX] = {0}; /* string to save var mini data */
	char_t *pch_download_var = P_NULL;
	char_t *pch_trend_var = P_NULL;
	char_t a_ch_format[4] = {0};

	if ((P_NULL == ptrend_var_info) || (P_NULL == pu32_file_data))
	{
		/* invalid inputs */
		u32_err = ERR_FAILED;
	}
	// qinshiling_20180723:
	if (OK == u32_err)
	{
		/* read trend var headline */
		pch_trend_var = (char_t *)(*pu32_file_data);
		//printf("pch treand var headline:%s\n", pch_trend_var);
		if (UINT_0 == u32_trend_var_size)
		{
			/* no trend_var.csv/trend var size is 0 */
			pch_download_var = strtok(pch_trend_var, "\n"); /* get the first line */
			i32_ret = strncmp(pch_download_var,
							  VAR_INFO_TITLE, strlen(VAR_INFO_TITLE)); /* check if the first line is var info title */
			if (0 != i32_ret)
			{
				/* first line is not VAR_INFO_TITLE */
				while ((0 != i32_ret) &&
					   (P_NULL != (pch_download_var = strtok(P_NULL, "\n"))))
				{
					/* find VAR_INFO_TITLE */
					i32_ret = strncmp(pch_download_var,
									  VAR_INFO_TITLE, strlen(VAR_INFO_TITLE)); /* check if the first line is var info title */
				}
			}

			if (INT_0 != i32_ret)
			{
				/* no VAR_INFO_TITLE */
				u32_err = ERR_CHECK_TREND_VAR;
			}
			else
			{
				(*pu32_file_data) = (u32_t)pch_download_var; /* set the var info addr */
			}
			return u32_err;
		}
	}

	if (OK == u32_err)
	{
		/* trend var size is not 0*/
		ptrend_var_info->u32_trend_var_size = u32_trend_var_size; /* set trend var size */
		pch_download_var = strtok(pch_trend_var, "\n");			  /* skip the trend var title line */
		i32_ret = strncmp(pch_download_var,
						  TREND_VAR_TITLE, strlen(TREND_VAR_TITLE)); /* check if the first line is trend var title */
		if (INT_0 != i32_ret)
		{
			/* first line is not TREND_VAR_TITLE */
			while ((INT_0 != i32_ret) &&
				   (P_NULL != (pch_download_var = strtok(P_NULL, "\n"))))
			{
				/* find TREND_VAR_TITLE */
				i32_ret = strncmp(pch_download_var,
								  TREND_VAR_TITLE, strlen(TREND_VAR_TITLE)); /* check if the first line is trend var title */
			}
		}

		if (INT_0 != i32_ret)
		{
			/* no TREND_VAR_TITLE */
			u32_err = ERR_CHECK_TREND_VAR;
		}
	}
	// qinshiling_20180723:end
	if (OK == u32_err)
	{
		/* check trend var information */
		for (u32_line = UINT_0; u32_line < ptrend_var_info->u32_trend_var_size; u32_line++)
		{
			/* read trend var information */
			if (OK != u32_err)
			{
				/* read error */
				break;
			}
			else
			{
				/* get current postion */
				pch_download_var = strtok(P_NULL, "\n");
				if (P_NULL == pch_download_var)
				{
					/* get line error */
					u32_err = ERR_CHECK_TREND_VAR;
					//sprintf(init_err_message, "获取第%d行趋势变量数据失败", u32_line);
				}
			}
			if (OK == u32_err)
			{
				/* check comma size */
				u32_err = check_comma_size(UINT_12, pch_download_var);	// tpl
				if (OK != u32_err)
				{
					u32_err = check_comma_size(UINT_13, pch_download_var);	// hz
					//sprintf(init_err_message, "第%d行趋势变量数据非法", u32_line);
				}
			}
			/* get var name */
			if (OK == u32_err)
			{
				for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_1; u32_str++)
				{
					if (u32_var > VAR_NAME_SIZE_MAX - 1)
					{
						u32_err = ERR_CHECK_TREND_VAR;
						//sprintf(init_err_message, "第%d行趋势Name超限: %s", u32_line, ptrend_var_info->trend_var_buf[u32_line].ch_var_name);
						break;
					}

					ptrend_var_info->trend_var_buf[u32_line].ch_var_name[u32_var] = pch_download_var[u32_str];
					if (',' == ptrend_var_info->trend_var_buf[u32_line].ch_var_name[u32_var])
					{
						ptrend_var_info->trend_var_buf[u32_line].ch_var_name[u32_var] = '\0'; // make string end when meeting ','
					}
					u32_var++;

					if (',' == pch_download_var[u32_str])
						u32_comma++;
				}
			}
			if (OK == u32_err)
			{
				if ('\0' != ptrend_var_info->trend_var_buf[u32_line].ch_var_name[VAR_NAME_SIZE_MAX - 1])
				{
					u32_err = ERR_CHECK_TREND_VAR;
					//sprintf(init_err_message, "第%d行趋势Name超限: %s", u32_line, ptrend_var_info->trend_var_buf[u32_line].ch_var_name);
				}
			}

			/* get var max data */
			if (OK == u32_err)
			{
				for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_2; u32_str++)
				{
					if (u32_var > VAR_MAX_SIZE_MAX - 1)
					{
						u32_err = ERR_CHECK_TREND_VAR;
						//sprintf(init_err_message, "第%d行趋势MaxData长度超限", u32_line);
						break;
					}
					if (UINT_1 == u32_comma)
					{
						ch_max_data[u32_var] = pch_download_var[u32_str];
						if (',' == ch_max_data[u32_var])
						{
							ch_max_data[u32_var] = '\0'; // make string end when meeting ','
						}
						u32_var++;
					}
					if (',' == pch_download_var[u32_str])
						u32_comma++;
				}
			}
			if (OK == u32_err)
			{
				if ('\0' != ch_max_data[VAR_MAX_SIZE_MAX - 1])
				{
					u32_err = ERR_CHECK_TREND_VAR;
					//sprintf(init_err_message, "第%d行趋势MaxData长度超限", u32_line);
				}
			}
			if (OK == u32_err)
			{
				/* DIO */
				if (UINT_0 == strlen(ch_max_data))
				{
					ptrend_var_info->trend_var_buf[u32_line].f32_max_data = UINT_1;
				}
				/* AIO */
				else
				{
					ptrend_var_info->trend_var_buf[u32_line].f32_max_data = atof(ch_max_data);
				}
			}

			/* get mini data */
			if (OK == u32_err)
			{
				for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_3; u32_str++)
				{
					if (u32_var > VAR_MINI_SIZE_MAX - 1)
					{
						u32_err = ERR_CHECK_TREND_VAR;
						//sprintf(init_err_message, "第%d行趋势MinData长度超限", u32_line);
						break;
					}
					if (UINT_2 == u32_comma)
					{
						ch_mini_data[u32_var] = pch_download_var[u32_str];
						if (',' == ch_mini_data[u32_var])
						{
							ch_mini_data[u32_var] = '\0'; // make strinf end when meeting ','
						}
						u32_var++;
					}
					if (',' == pch_download_var[u32_str])
						u32_comma++;
				}
			}
			if (OK == u32_err)
			{
				if ('\0' != ch_mini_data[VAR_MINI_SIZE_MAX - 1])
				{
					u32_err = ERR_CHECK_TREND_VAR;
					//(init_err_message, "第%d行趋势MinData长度超限", u32_line);
				}
			}
			if (OK == u32_err)
			{
				/* DIO */
				if (UINT_0 == strlen(ch_mini_data))
				{
					ptrend_var_info->trend_var_buf[u32_line].f32_mini_data = UINT_0;
				}
				/* AIO */
				else
				{
					ptrend_var_info->trend_var_buf[u32_line].f32_mini_data = atof(ch_mini_data);
				}
			}

/* get var unit */
#if 1
			if (OK == u32_err)
			{
				char_t *pch_unit_addr = P_NULL;
				firm_bool_t b_flag = FALSE;
				for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_4; u32_str++)
				{
					if (u32_var > VAR_UNIT_SIZE_MAX - 1)
					{
						u32_err = ERR_CHECK_TREND_VAR;
						sprintf(init_err_message, "第%d行趋势Unit长度超限", u32_line);
						break;
					}
					if (UINT_3 == u32_comma)
					{
						if (FALSE == b_flag)
						{
							pch_unit_addr = &pch_download_var[u32_str];
							b_flag = TRUE;
						}
						u32_var++;
					}
					if (',' == pch_download_var[u32_str])
						u32_comma++;
				}
				memcpy(ptrend_var_info->trend_var_buf[u32_line].ch_unit, pch_unit_addr, u32_var - UINT_1); // the last charcter is ','
			}
			if (OK == u32_err)
			{
				if ('\0' != ptrend_var_info->trend_var_buf[u32_line].ch_unit[VAR_UNIT_SIZE_MAX - 1])
				{
					u32_err = ERR_CHECK_TREND_VAR;
					sprintf(init_err_message, "第%d行趋势Unit长度超限", u32_line);
				}
				else
				{
					u32_err = replace_special_character(ptrend_var_info->trend_var_buf[u32_line].ch_unit);
				}
			}
#endif

			/* get trend type */
			if (OK == u32_err)
			{
				for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_5; u32_str++)
				{
					if (u32_var > TREND_TYPE_SIZE_MAX - 1)
					{
						u32_err = ERR_CHECK_TREND_VAR;
						sprintf(init_err_message, "第%d行趋势TrendType长度超限", u32_line);
						break;
					}
					if (UINT_4 == u32_comma)
					{
						ptrend_var_info->trend_var_buf[u32_line].ch_trend_type[u32_var] = pch_download_var[u32_str];
						if (',' == ptrend_var_info->trend_var_buf[u32_line].ch_trend_type[u32_var])
						{
							ptrend_var_info->trend_var_buf[u32_line].ch_trend_type[u32_var] = '\0'; // make strinf end when meeting ','
						}
						u32_var++;
					}
					if (',' == pch_download_var[u32_str])
						u32_comma++;
				}
			}
			if (OK == u32_err)
			{
				if ('\0' != ptrend_var_info->trend_var_buf[u32_line].ch_trend_type[TREND_TYPE_SIZE_MAX - 1])
				{
					u32_err = ERR_CHECK_TREND_VAR;
					sprintf(init_err_message, "第%d行趋势TrendType长度超限", u32_line);
				}
			}

			/* get var display format */
			if (OK == u32_err)
			{
				/* get var display format */
				for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_6; u32_str++)
				{
					/* read csv line */
					if (u32_var >= 4)
					{
						/* read format error */
						u32_err = ERR_FAILED;
						//sprintf(init_err_message, "第%d行趋势Format长度超限", u32_line);
						break;
					}
					if (UINT_5 == u32_comma)
					{
						/* copy letter */
						a_ch_format[u32_var] = pch_download_var[u32_str];
						if (',' == a_ch_format[u32_var])
						{
							/* make strinf end when meeting ',' */
							a_ch_format[u32_var] = '\0';
						}
						u32_var++;
					}
					if (',' == pch_download_var[u32_str])
					{
						/* meeting ',' */
						u32_comma++;
					}
				}
			}

			if (OK == u32_err)
			{
				/* check the string */
				if ('\0' != a_ch_format[3])
				{
					/* string check error */
					u32_err = ERR_FAILED;
					sprintf(init_err_message, "第%d行趋势Format长度超限", u32_line);
				}
			}

			if (OK == u32_err)
			{
				/* convert string to float */
				ptrend_var_info->trend_var_buf[u32_line].u32_display_format =
					atoi(a_ch_format);
			}



			/* qinshiling 20191011 start: add tagname */
			if (OK == u32_err)
			{
				char_t *pch_tag_name = P_NULL;
				firm_bool_t b_flag = FALSE;
				for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_7; u32_str++)
				{
					if (u32_var > VAR_NAME_SIZE_MAX - 1)
					{
						u32_err = ERR_CHECK_TREND_VAR;
						sprintf(init_err_message, "第%d行趋势TagName长度超限", u32_line);
						break;
					}
					if (UINT_6 == u32_comma)
					{
						if (FALSE == b_flag)
						{
							pch_tag_name = &pch_download_var[u32_str];
							b_flag = TRUE;
						}
						u32_var++;
					}
					if (',' == pch_download_var[u32_str])
						u32_comma++;
				}
				memcpy(ptrend_var_info->trend_var_buf[u32_line].ch_tag_name, pch_tag_name, u32_var - UINT_1); // the last charcter is ','
			}
			if (OK == u32_err)
			{
				if ('\0' != ptrend_var_info->trend_var_buf[u32_line].ch_tag_name[VAR_NAME_SIZE_MAX - 1])
				{
					u32_err = ERR_CHECK_TREND_VAR;
					sprintf(init_err_message, "第%d行趋势TagName长度超限", u32_line);
				}
			}
			/* qinshiling 20191011 end: add tagname */
		}

		if (OK == u32_err)
		{
			(*pu32_file_data) = (u32_t)pch_download_var;
		}
	}
#if 0	// ken
	printf("Var Name:\t%s\n", ptrend_var_info->trend_var_buf[1].ch_var_name);
	printf("Max Data:\t%f\n", ptrend_var_info->trend_var_buf[1].f32_max_data);
	printf("Min Data:\t%f\n", ptrend_var_info->trend_var_buf[1].f32_mini_data);
	printf("Unit:\t%s\n", ptrend_var_info->trend_var_buf[1].ch_unit);
	printf("Trend Type:\t%s\n", ptrend_var_info->trend_var_buf[1].ch_trend_type);
	printf("Format:\t%u\n", ptrend_var_info->trend_var_buf[1].u32_display_format);
	printf("Tag Name:\t%s\n", ptrend_var_info->trend_var_buf[1].ch_tag_name);
#endif
	return u32_err;
}

static u32_t parse_trend_group(trend_group_info_t *ptrend_group_info, u32_t *pu32_file_data)
{
	u32_t u32_err = OK;				  /* function return value */
	u32_t u32_line = UINT_0;		  /* download var size */
	u32_t u32_comma = UINT_0;		  /* comma index */
	u32_t u32_str = UINT_0;			  /* string index */
	u32_t u32_var = UINT_0;			  /* var index */
	u32_t u32_index = UINT_0;		  /* download var line pointer */
	char_t ch_var_size[UINT_3] = {0}; /* var size(1~5) */
	char_t *pch_download_var = P_NULL;
	char_t *p_ret = P_NULL; /* function return value */
	i32_t i32_ret = 1; /* function return value */

	u32_t u32_flag = UINT_0;	// 0 is tpl, 1 is hz

	if (P_NULL == ptrend_group_info || P_NULL == pu32_file_data)
	{
		u32_err = ERR_FAILED;
	}

	if (OK == u32_err)
	{
		pch_download_var = (char_t *)(*pu32_file_data); /* qinshiling 20181012 先判断指针是否空，再引用*/
		while (0 != i32_ret && P_NULL != (pch_download_var = strtok(P_NULL, "\n")))
		{
			i32_ret = strncmp(pch_download_var, TREND_GROUP_TITLE, strlen(TREND_GROUP_TITLE)); /* check if the first line is trend var title */
			if (0 != i32_ret)
			{
				i32_ret = strncmp(pch_download_var, TREND_GROUP_TITLE_HZ, strlen(TREND_GROUP_TITLE));
				if (0 == i32_ret) u32_flag = UINT_1;	// is hz, have GroupType
			}
		}
		if (0 != i32_ret)
		{
			u32_err = ERR_CHECK_TREND_VAR;

		}

	}

	if (OK == u32_err)
	{
		/* get trend group data */
		for (u32_line = UINT_0; u32_line < TREND_GROUP_SIZE_MAX; u32_line++)
		{
			if (OK != u32_err)
				break;
			else
			{
				pch_download_var = strtok(P_NULL, "\n"); /* get cur line data */
				if (P_NULL == pch_download_var)
				{
					u32_err = ERR_CHECK_TREND_VAR;
				}
			}
			if (OK == u32_err)
			{
				int i32_ret1 = -1;
				i32_ret1 = strncmp(pch_download_var, VAR_INFO_TITLE, strlen(VAR_INFO_TITLE));
				if (0 == i32_ret1)
				{
					ptrend_group_info->u32_group_size = u32_line; /* get group size */
					break;
				}
			}
			if (OK == u32_err)
			{
				/* check comma size */
				u32_err = check_comma_size(UINT_12 + u32_flag, pch_download_var); /* qinshiling 20200324: add GroupType */
				if (OK != u32_err)
				{
					u32_err = ERR_CHECK_TREND_VAR;
				}
			}

			/* get group name */
			if (OK == u32_err)
			{
				for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_1; u32_str++)
				{
					if (u32_var > TREND_GROUP_NAME_SIZE_MAX - 1)
					{
						u32_err = ERR_CHECK_TREND_VAR;
						break;
					}
					//					if( UINT_0 == u32_comma)
					{
						ptrend_group_info->trend_group_val_buf[u32_line].ch_group_name[u32_var] = pch_download_var[u32_str];
						if (',' == ptrend_group_info->trend_group_val_buf[u32_line].ch_group_name[u32_var])
						{
							ptrend_group_info->trend_group_val_buf[u32_line].ch_group_name[u32_var] = '\0'; // make string end when meeting ','
						}
						u32_var++;
					}
					if (',' == pch_download_var[u32_str])
						u32_comma++;
				}
			}
			if (OK == u32_err)
			{
				if ('\0' != ptrend_group_info->trend_group_val_buf[u32_line].ch_group_name[TREND_GROUP_NAME_SIZE_MAX - 1])
				{
					u32_err = ERR_CHECK_TREND_VAR;
					//sprintf(init_err_message, "第%d行趋势组Group长度超限", u32_line);
				}
			}

			/* get group var size */
			if (OK == u32_err)
			{
				for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_2 + u32_flag; u32_str++)
				{
					if (u32_var > UINT_2)
					{
						u32_err = ERR_CHECK_TREND_VAR;
						//sprintf(init_err_message, "第%d行趋势组VarSize长度超限", u32_line);
						break;
					}
					if (UINT_1 + u32_flag == u32_comma)
					{
						ch_var_size[u32_var] = pch_download_var[u32_str];
						if (',' == ch_var_size[u32_var])
						{
							ch_var_size[u32_var] = '\0'; // make string end when meeting ','
						}
						u32_var++;
					}
					if (',' == pch_download_var[u32_str])
						u32_comma++;
				}
				ptrend_group_info->trend_group_val_buf[u32_line].u32_var_size = (u32_t)strtol(ch_var_size, &p_ret, UINT_10);
				if ('\0' != p_ret[UINT_0])
				{
					u32_err = ERR_CHECK_TREND_VAR;
					//sprintf(init_err_message, "第%d行趋势组VarSize长度超限", u32_line);
				}
				if (GROUP_VAR_SIZE_MAX < ptrend_group_info->trend_group_val_buf[u32_line].u32_var_size)
					ptrend_group_info->trend_group_val_buf[u32_line].u32_var_size = GROUP_VAR_SIZE_MAX; /* trend var max size */
			}

			/* get var name */
			if (OK == u32_err)
			{
				for (u32_index = UINT_2; u32_index < UINT_11; u32_index++)
				{
					if (OK != u32_err)
						break;
					else
					{
						for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < (u32_index + UINT_1 + u32_flag); u32_str++)
						{
							if (u32_var > VAR_NAME_SIZE_MAX - 1)
							{
								u32_err = ERR_CHECK_TREND_VAR;
								sprintf(init_err_message, "第%d行趋势组Var%d长度超限", u32_line, u32_index - 1);
								break;
							}
							if (u32_index + u32_flag == u32_comma)
							{
								ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_index - UINT_2].ch_var_name[u32_var] = pch_download_var[u32_str];
								if (',' == ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_index - UINT_2].ch_var_name[u32_var])
								{
									ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_index - UINT_2].ch_var_name[u32_var] = '\0'; // make string end when meeting ','
								}
								u32_var++;
							}
							if (',' == pch_download_var[u32_str])
								u32_comma++;
						}
					}
					if (OK == u32_err)
					{
						if ('\0' != ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_index - UINT_2].ch_var_name[VAR_NAME_SIZE_MAX - 1])
						{
							u32_err = ERR_CHECK_TREND_VAR;
							sprintf(init_err_message, "第%d行趋势组Var%d长度超限", u32_line, u32_index - 1);
						}
					}
				}
				/* check if trend group var name is '\0'
				 * TRUE: ERR_FAILED
				 * FALSE: CONTINUE */
#if 1
				for (u32_index = UINT_0; u32_index < ptrend_group_info->trend_group_val_buf[u32_line].u32_var_size; u32_index++)
				{
					i32_ret = strcmp("", ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_index].ch_var_name);
					if (INT_0 == i32_ret)
					{
						u32_err = ERR_CHECK_TREND_VAR;
						sprintf(init_err_message, "第%d行趋势组Var%d为空", u32_line, u32_index + 1);
					}
				}
#endif
			}

			/* get group description */
			if (OK == u32_err)
			{
				for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_13 + u32_flag && pch_download_var[u32_str] != '\r'; u32_str++)
				{
					if (u32_var > TREND_GROUP_DES_SIZE_MAX - 1)
					{
						u32_err = ERR_CHECK_TREND_VAR;
						sprintf(init_err_message, "第%d行趋势组GroupDes长度超限", u32_line);
						break;
					}
					if (UINT_12 + u32_flag == u32_comma)
					{
						ptrend_group_info->trend_group_val_buf[u32_line].ch_group_des[u32_var] = pch_download_var[u32_str];
						if ('\0' == ptrend_group_info->trend_group_val_buf[u32_line].ch_group_des[u32_var])
						{
							ptrend_group_info->trend_group_val_buf[u32_line].ch_group_des[u32_var] = '\0'; // make strinf end when meeting ','
							break;
						}
						u32_var++;
					}
					if (',' == pch_download_var[u32_str])
						u32_comma++;
				}
			}
			if (OK == u32_err)
			{
				if ('\0' != ptrend_group_info->trend_group_val_buf[u32_line].ch_group_des[TREND_GROUP_DES_SIZE_MAX - 1])
				{
					u32_err = ERR_CHECK_TREND_VAR;
					sprintf(init_err_message, "第%d行趋势组GroupDes长度超限", u32_line);
				}
			}
		}

		if (OK == u32_err)
		{
			if (TREND_GROUP_SIZE_MAX == u32_line)
			{
				ptrend_group_info->u32_group_size = TREND_GROUP_SIZE_MAX;
				while (INT_0 != strncmp(pch_download_var, VAR_INFO_TITLE, strlen(VAR_INFO_TITLE)) && OK == u32_err)
				{
					pch_download_var = strtok(P_NULL, "\n"); /* skip to var info line */
					if (P_NULL == pch_download_var)
					{
						u32_err = ERR_CHECK_TREND_VAR;
						sprintf(init_err_message, "变量列表 表头不匹配");
					}
				}
			}
		}

		if (OK == u32_err)
		{
			(*pu32_file_data) = (u32_t)pch_download_var;
		}
	}

#if 0
	printf("%u\n", ptrend_group_info->trend_group_val_buf[0].u32_var_size);
	printf("%s\n", ptrend_group_info->trend_group_val_buf[0].ch_group_name);
	printf("%s\n", ptrend_group_info->trend_group_val_buf[0].trend_group_var_buf[0].ch_var_name);
	printf("%s\n", ptrend_group_info->trend_group_val_buf[0].ch_group_des);

#endif
	return u32_err;
}

static u32_t parse_vars_info(vars_info_t *pvars_info, u32_t *pu32_file_data)
{
	u32_t u32_err = OK;
	u32_t u32_line = UINT_0;									 /* download var size */
	u32_t u32_comma = UINT_0;									 /* comma index */
	u32_t u32_str = UINT_0;										 /* string index */
	u32_t u32_var = UINT_0;										 /* var index */
	char_t ch_data_id[UINT_4] = {0};							 /* string to save DataID */
	char_t ch_offset[UINT_16] = {0}; /* string to save offset */ /* qinshiling 20181012 修改内存访问超限 */
	char_t ch_data_type[UINT_4] = {0};							 /* string to save data type */
	char_t *pch_download_var = P_NULL;
	firm_bool_t b_flag = FALSE;
	//	char_t *pch_unit_addr = P_NULL;
	char_t *pch_note_addr = P_NULL;

	if (P_NULL == pvars_info || P_NULL == pu32_file_data)
	{
		u32_err = ERR_FAILED;
	}

	if (OK == u32_err)
	{
		pch_download_var = (char_t *)(*pu32_file_data); /* qinshiling 20181012 先判断指针是否为空再引用 */
		while (P_NULL != (pch_download_var = strtok(P_NULL, "\n")) && OK == u32_err)
		{
			/* get var name */
			for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_1; u32_str++)
			{
				//				if( UINT_0 == u32_comma)
				{
					pvars_info->vars_parse_buf[u32_line].ch_name[u32_var] = pch_download_var[u32_str];
					if (',' == pvars_info->vars_parse_buf[u32_line].ch_name[u32_var])
					{
						pvars_info->vars_parse_buf[u32_line].ch_name[u32_var] = '\0'; // make string end when meeting ','
					}
					u32_var++;
				}
				if (',' == pch_download_var[u32_str])
					u32_comma++;
			}

			/* get var data id */
			for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_2; u32_str++)
			{
				if (UINT_1 == u32_comma)
				{
					ch_data_id[u32_var] = pch_download_var[u32_str];
					if (',' == ch_data_id[u32_var])
					{
						ch_data_id[u32_var] = '\0'; // make strinf end when meeting ','
					}
					u32_var++;
				}
				if (',' == pch_download_var[u32_str])
					u32_comma++;
			}
			pvars_info->vars_parse_buf[u32_line].u32_data_id = atoi(ch_data_id);

			/* get var offset */
			for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_3; u32_str++)
			{
				if (UINT_2 == u32_comma)
				{
					ch_offset[u32_var] = pch_download_var[u32_str];
					if (',' == ch_offset[u32_var])
					{
						ch_offset[u32_var] = '\0'; // make strinf end when meeting ','
					}
					u32_var++;
				}
				if (',' == pch_download_var[u32_str])
					u32_comma++;
			}
			pvars_info->vars_parse_buf[u32_line].u32_offset = atoi(ch_offset);

			/* get var data type */
			for (u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_4; u32_str++)
			{
				if (UINT_3 == u32_comma)
				{
					ch_data_type[u32_var] = pch_download_var[u32_str];
					if (',' == ch_offset[u32_var])
					{
						ch_data_type[u32_var] = '\0';
					}
					u32_var++;
				}
				if (',' == pch_download_var[u32_str])
					u32_comma++;
			}
			pvars_info->vars_parse_buf[u32_line].u32_data_type = atoi(ch_data_type);
// qinshiling_2080723:
#if 0
			/* get var unit */
			for( b_flag = FALSE, u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_5; u32_str++ )
			{
				if( VAR_UNIT_SIZE_MAX - UINT_1 < u32_var )
				{
					u32_err = ERR_CHECK_TREND_VAR;
					break;
				}
				if( UINT_4 == u32_comma)
				{
					if( FALSE == b_flag )
					{
						b_flag = TRUE;
						pch_unit_addr = &pch_download_var[u32_str];
					}
					u32_var++;
				}
				if( ',' == pch_download_var[u32_str])
					u32_comma++;
			}
			if( P_NULL == pch_unit_addr )
			{
				u32_err = ERR_FAILED;
				break;/* qinshiling 20181012 防止空指针 */
			}
			memcpy( pvars_info->vars_parse_buf[u32_line].ch_unit, pch_unit_addr, u32_var-UINT_1 );
			u32_err = replace_special_character(pvars_info->vars_parse_buf[u32_line].ch_unit);
#endif
			/* get var note */
			for (b_flag = FALSE, u32_str = UINT_0, u32_comma = UINT_0, u32_var = UINT_0; u32_comma < UINT_5; u32_str++)
			{
				if (VAR_NOTE_SIZE_MAX - UINT_1 < u32_var)
				{
					u32_err = ERR_CHECK_TREND_VAR;
					break;
				}
				if (UINT_4 == u32_comma)
				{
					if (FALSE == b_flag)
					{
						pch_note_addr = &pch_download_var[u32_str];
						b_flag = TRUE;
					}
					u32_var++;
				}
				if (',' == pch_download_var[u32_str])
					u32_comma++;
			}
			if (P_NULL == pch_note_addr)
			{
				u32_err = ERR_FAILED;
				//sprintf(init_err_message, "第%d行变量Note获取失败", u32_line);
				break; /* qinshiling 20181012 防止空指针 */
			}
			memcpy(pvars_info->vars_parse_buf[u32_line].ch_note, pch_note_addr, u32_var - UINT_1);
			/* qinshiling 20190516 add special symbol replace */
			replace_special_symbol(pvars_info->vars_parse_buf[u32_line].ch_note);
			u32_line++;
		}
		// qinshiling_20180723:end
		if (OK == u32_err)
		{
			pvars_info->u32_vars_parse_lines = u32_line;
			(*pu32_file_data) = (u32_t)pch_download_var;
		}
	}

	return u32_err;
}

static u32_t set_trend_info(trend_group_info_t *ptrend_group_info, trend_var_info_t *ptrend_var_info, vars_info_t *pvars_info)
{
	u32_t u32_err = OK;		  /* function return value */
	u32_t u32_line = UINT_0;  /* download var size */
	u32_t u32_var = UINT_0;	  /* var index */
	u32_t u32_index = UINT_0; /* index */

	if (P_NULL == ptrend_group_info || P_NULL == ptrend_var_info || P_NULL == pvars_info)
	{
		u32_err = ERR_FAILED;
	}

	if (OK == u32_err)
	{
		/* set trend var information( id&offset&type )*/
		for (u32_index = UINT_0; u32_index < ptrend_var_info->u32_trend_var_size; u32_index++)
		{
			if (OK != u32_err)
				break;
			/* plan to make index table !!! */
			for (u32_var = UINT_0; u32_var < pvars_info->u32_vars_parse_lines; u32_var++)
			{
				if (UINT_0 != ptrend_var_info->trend_var_buf[u32_index].u32_var_type) /* trend var found */
					break;

				else if (0 == strcmp(ptrend_var_info->trend_var_buf[u32_index].ch_var_name, /* search trend var name */
									 pvars_info->vars_parse_buf[u32_var].ch_name))
				{
					ptrend_var_info->trend_var_buf[u32_index].u32_var_id = /* set trend var id */
						pvars_info->vars_parse_buf[u32_var].u32_data_id;
					ptrend_var_info->trend_var_buf[u32_index].u32_var_offset = /* set trend var offset */
						pvars_info->vars_parse_buf[u32_var].u32_offset;
					ptrend_var_info->trend_var_buf[u32_index].u32_var_type = /* set trend var type */
						pvars_info->vars_parse_buf[u32_var].u32_data_type;
					// qinshiling 20181120 start: 添加趋势变量描述
					strcpy(ptrend_var_info->trend_var_buf[u32_index].ch_note, pvars_info->vars_parse_buf[u32_var].ch_note);
					// qinshiling 20181120 end
#if 0
					strncpy( ptrend_var_info->trend_var_buf[u32_index].ch_unit,
							pvars_info->vars_parse_buf[u32_var].ch_unit, VAR_UNIT_SIZE_MAX);//单位
#endif
					// qinshling 20181119 start
					if ((VAR_TYPE_BOOL == ptrend_var_info->trend_var_buf[u32_index].u32_var_type) ||
						(VAR_TYPE_BOOL_SIGNAL == ptrend_var_info->trend_var_buf[u32_index].u32_var_type))
					{
						memset(ptrend_var_info->trend_var_buf[u32_index].ch_unit, 0, sizeof(ptrend_var_info->trend_var_buf[u32_index].ch_unit));
						ptrend_var_info->trend_var_buf[u32_index].f32_max_data = 1;
						ptrend_var_info->trend_var_buf[u32_index].f32_mini_data = 0;
					}
					if (VAR_TYPE_DEVICE_SIGNAL == ptrend_var_info->trend_var_buf[u32_index].u32_var_type)
					{
						memset(ptrend_var_info->trend_var_buf[u32_index].ch_unit, 0, sizeof(ptrend_var_info->trend_var_buf[u32_index].ch_unit));
					}
					// qinshling 20181119 end
				}

				else
				{
					/* do nothing */
				}
			}
			if (u32_var == pvars_info->u32_vars_parse_lines &&
				UINT_0 == ptrend_var_info->trend_var_buf[u32_index].u32_var_type)
			{ /* trend var not found */
				u32_err = ERR_CHECK_TREND_VAR;
			}
		}
	}

	if (OK == u32_err)
	{
		/* set trend group var information */
		for (u32_line = UINT_0; u32_line < ptrend_group_info->u32_group_size; u32_line++)
		{
			if (OK != u32_err)
				break;
			for (u32_var = UINT_0; u32_var < ptrend_group_info->trend_group_val_buf[u32_line].u32_var_size; u32_var++)
			{
				if (OK != u32_err)
					break;
				for (u32_index = UINT_0; u32_index < ptrend_var_info->u32_trend_var_size; u32_index++)
				{
					if (0 == strcmp(ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_var].ch_var_name,
										ptrend_var_info->trend_var_buf[u32_index].ch_var_name)) /* search var name */
					{
						ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_var].u32_var_id =
							ptrend_var_info->trend_var_buf[u32_index].u32_var_id; /* set trend var id */
						ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_var].u32_var_offset =
							ptrend_var_info->trend_var_buf[u32_index].u32_var_offset; /* set trend var offset */
						ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_var].u32_var_type =
							ptrend_var_info->trend_var_buf[u32_index].u32_var_type; /* set trend var data type */
						ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_var].f32_max_data =
							ptrend_var_info->trend_var_buf[u32_index].f32_max_data; /* set trend var max data */
						ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_var].f32_mini_data =
							ptrend_var_info->trend_var_buf[u32_index].f32_mini_data; /* set trend var min data */
						strcpy(ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_var].ch_unit,
							   ptrend_var_info->trend_var_buf[u32_index].ch_unit); /* set trend var unit*/
						strcpy(ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_var].ch_trend_type,
							   ptrend_var_info->trend_var_buf[u32_index].ch_trend_type); /* set status var name */
						/* qinshiling 20181120 start: 添加趋势变量描述 */
						strcpy(ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_var].ch_note,
							   ptrend_var_info->trend_var_buf[u32_index].ch_note);
						/* qinshiling 20181120 end */
						ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_var].u32_display_format =
							ptrend_var_info->trend_var_buf[u32_index].u32_display_format;
						/* qinshiling 20191011 start: add tagname */
						strcpy(ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_var].ch_tag_name,
							   ptrend_var_info->trend_var_buf[u32_index].ch_tag_name);
						/* qinshiling 20191011 end: add tagname */
						break;
					}
				}
				if (u32_index == ptrend_var_info->u32_trend_var_size &&
					UINT_0 == ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_var].u32_var_id)
				{
					u32_err = ERR_CHECK_TREND_VAR; /* trend var not found */
					//sprintf(init_err_message, "趋势变量不合法: %s",
							//ptrend_group_info->trend_group_val_buf[u32_line].trend_group_var_buf[u32_var].ch_var_name);
				}
			}
		}
	}

	return u32_err;
}

static u32_t check_comma_size(u32_t u32_check_size, char_t *pch_buf)
{
	u32_t u32_err = OK; /* function return value */
	u32_t u32_str_index = UINT_0;
	u32_t u32_comma_index = UINT_0;

	if (P_NULL == pch_buf)
	{
		u32_err = ERR_FAILED;
	}

	if (OK == u32_err)
	{
		for (u32_str_index = UINT_0; u32_str_index < strlen(pch_buf); u32_str_index++)
		{
			if (',' == pch_buf[u32_str_index])
				u32_comma_index++;
		}
		if (u32_check_size != u32_comma_index)
			u32_err = ERR_FAILED;
	}

	return u32_err;
}

static u32_t replace_special_character(char_t *pch_str)
{
	u32_t u32_err = OK;		  // 函数返回值
	char_t *pch_ret = P_NULL; // 函数返回值

	if (P_NULL == pch_str)
	{
		u32_err = ERR_FAILED; // 无效输入
	}

	if (OK == u32_err)
	{
		if (P_NULL != (pch_ret = strstr(pch_str, M2)))
		{
			str_right_shift(pch_ret, UINT_2);			 // 特殊字符占3格字节，需向后移动一位
			memcpy(pch_ret, M_SQUARE, strlen(M_SQUARE)); // 若找到m2,替换为m²
		}
		else if (P_NULL != (pch_ret = strstr(pch_str, M3)))
		{
			str_right_shift(pch_ret, UINT_2);		 // 特殊字符占3格字节，需向后移动一位
			memcpy(pch_ret, M_CUBE, strlen(M_CUBE)); // 若找到m3,替换为m³
		}
		else
		{
			// 待添加其他特殊字符
		}
	}

	return u32_err;
}

static u32_t replace_special_symbol(char_t *p_ch_buf)
{
	u32_t u32_err = OK; /* function return value */

	if (P_NULL == p_ch_buf)
	{
		/* invalid inputs */
		u32_err = ERR_FAILED;
	}

	if (OK == u32_err)
	{
		/* replace ">" */
		u32_err = replace_greater_symbol(p_ch_buf);
	}

	if (OK == u32_err)
	{
		/* replace "<" */
		u32_err = replace_less_symbol(p_ch_buf);
	}

	return u32_err;
}

static u32_t replace_greater_symbol(char_t *p_ch_buf)
{
	u32_t u32_err = OK;		   /* function return value */
	char_t *p_ch_ret = P_NULL; /* function return value */
	char_t *p_ch_temp = P_NULL;
	char_t a_ch_back[VAR_NOTE_SIZE_MAX] = {0};

	if (P_NULL == p_ch_buf)
	{
		/* invalid inputs */
		u32_err = ERR_FAILED;
	}

	if (OK == u32_err)
	{
		/* find greater html string */
		u32_t u32_old_len = strlen(p_ch_buf);
		p_ch_temp = p_ch_buf; /* init the pointer */
		while (P_NULL != (p_ch_ret = strstr(p_ch_temp, GREATER_HTML)))
		{
			/* replace GREATER_HTML with GREATER_SYMBOL */
			u32_t u32_left_len = (u32_t)(p_ch_ret - p_ch_temp);
			strncat(a_ch_back, p_ch_temp, u32_left_len);
			strcat(a_ch_back, GREATER_SYMBOL);
			p_ch_temp = p_ch_ret + strlen(GREATER_HTML);
		}
		strcat(a_ch_back, p_ch_temp);
		memset(p_ch_buf, 0, u32_old_len);
		strcpy(p_ch_buf, a_ch_back);
	}

	return u32_err;
}

static u32_t replace_less_symbol(char_t *p_ch_buf)
{
	u32_t u32_err = OK;		   /* function return value */
	char_t *p_ch_ret = P_NULL; /* function return value */
	char_t a_ch_back[VAR_NOTE_SIZE_MAX] = {0};
	char_t *p_ch_temp = P_NULL;

	if (P_NULL == p_ch_buf)
	{
		/* invalid inputs */
		u32_err = ERR_FAILED;
	}

	if (OK == u32_err)
	{
		/* find less html string */
		u32_t u32_old_len = strlen(p_ch_buf);
		p_ch_temp = p_ch_buf; /* init the pointer */
		while (P_NULL != (p_ch_ret = strstr(p_ch_temp, LESS_HTML)))
		{
			/* replace LESS_HTML with LESS_SYMBOL */
			u32_t u32_left_len = (u32_t)(p_ch_ret - p_ch_temp);
			strncat(a_ch_back, p_ch_temp, u32_left_len);
			strcat(a_ch_back, LESS_SYMBOL);
			p_ch_temp = p_ch_ret + strlen(LESS_HTML);
		}
		strcat(a_ch_back, p_ch_temp);
		memset(p_ch_buf, 0, u32_old_len);
		strcpy(p_ch_buf, a_ch_back);
	}

	return u32_err;
}

static u32_t str_right_shift(char_t *pch_str, u32_t u32_offset)
{
	u32_t u32_err = OK;		  // 函数返回值
	u32_t u32_index = UINT_0; // 计数值
	u32_t u32_len = UINT_0;	  // 字符串长度

	if (P_NULL == pch_str)
	{
		u32_err = ERR_FAILED; // 无效输入
	}

	if (OK == u32_err)
	{
		u32_len = strlen(pch_str); // 获取字符串长度
		for (u32_index = u32_len + UINT_1; u32_index > u32_offset; u32_index--)
		{
			pch_str[u32_index] = pch_str[u32_index - UINT_1];
		}
	}

	return u32_err;
}
