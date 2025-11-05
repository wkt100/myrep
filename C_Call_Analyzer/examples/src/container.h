/*
 * container.h
 *
 *  Created on: 2018-6-20
 *      Author: Administrator
 */

#ifndef CONTAINER_H_
#define CONTAINER_H_

#define CONTAINER_VAR_SIZE_MAX (30U)         /*单个容器可放置最大变量(模拟量or开关量)个数 */
#define CONTAINER_SIZE_MAX (256U)            /* 容器最大个数 */

#include "typedef.h"
#include "photon_module_parse.h"
#include "photon_data_com.h"

/* qinshiling 20181205 start: 添加记录位置功能 */
typedef struct container_des
{
	char_t ch_container_name[65];                /* 容器名称 */
	u32_t u32_var_pos;                           /* 变量位置 */
}container_des_t;

typedef struct container_list_tag
{
	u32_t u32_container_size;
	container_des_t container_des_buf[ CONTAINER_SIZE_MAX];
}container_list_t;
/* qinshiling 20181205 end: 添加记录位置功能 */

#endif /* CONTAINER_H_ */
