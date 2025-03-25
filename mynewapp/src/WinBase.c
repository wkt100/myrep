/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.03  */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"
#include "WinBase.h"
#include "mem.h"
#include "mq.h"

static char_t s_a_ch_file_trend_var[1U * 1024U * 1024U] = {0}; // download_trend_var.csv
static vars_info_t s_st_var_info = {0U};					   // 变量组态信息
static trend_info_t s_st_trend_info = {};					   // 趋势组态信息
static var_hash_t *s_p_st_var_hash = P_NULL;				   // 变量hash（key：变量名称；value：变量区号、偏移和类型）
static PtWidget_t *s_a_p_st_text_var_name[20] = {P_NULL};	   // 强制变量名称输入框
static PtWidget_t *s_a_p_st_text_var_val[20] = {P_NULL};	   // 强制变量值输入框
static PtWidget_t *s_a_p_st_text_var_sig[20] = {P_NULL};	   // 强制变量质量位

i32_t WinBaseSetup(PtWidget_t *p_st_link_instance, ApInfo_t *p_st_apinfo, PtCallbackInfo_t *p_st_cbinfo)
{
	//printf("hello~");
	u32_t u32_i = 0U; // 循环计数
	trend_var_file_head_t st_trend_var_head = {0};

	//* 1.初始化全局变量
	memset(s_a_ch_file_trend_var, 0, 1U * 1024U * 1024U);
	memset(&s_st_var_info, 0, sizeof(vars_info_t));
	memset(&s_st_trend_info, 0, sizeof(trend_info_t));
	s_a_p_st_text_var_name[0] = ABW_TextVarName1;
	s_a_p_st_text_var_name[1] = ABW_TextVarName2;
	s_a_p_st_text_var_name[2] = ABW_TextVarName3;
	s_a_p_st_text_var_name[3] = ABW_TextVarName4;
	s_a_p_st_text_var_name[4] = ABW_TextVarName5;
	s_a_p_st_text_var_name[5] = ABW_TextVarName6;
	s_a_p_st_text_var_name[6] = ABW_TextVarName7;
	s_a_p_st_text_var_name[7] = ABW_TextVarName8;
	s_a_p_st_text_var_name[8] = ABW_TextVarName9;
	s_a_p_st_text_var_name[9] = ABW_TextVarName10;
	s_a_p_st_text_var_name[10] = ABW_TextVarName11;
	s_a_p_st_text_var_name[11] = ABW_TextVarName12;
	s_a_p_st_text_var_name[12] = ABW_TextVarName13;
	s_a_p_st_text_var_name[13] = ABW_TextVarName14;
	s_a_p_st_text_var_name[14] = ABW_TextVarName15;
	s_a_p_st_text_var_name[15] = ABW_TextVarName16;
	s_a_p_st_text_var_name[16] = ABW_TextVarName17;
	s_a_p_st_text_var_name[17] = ABW_TextVarName18;
	s_a_p_st_text_var_name[18] = ABW_TextVarName19;
	s_a_p_st_text_var_name[19] = ABW_TextVarName20;
	s_a_p_st_text_var_val[0] = ABW_TextVarVal1;
	s_a_p_st_text_var_val[1] = ABW_TextVarVal2;
	s_a_p_st_text_var_val[2] = ABW_TextVarVal3;
	s_a_p_st_text_var_val[3] = ABW_TextVarVal4;
	s_a_p_st_text_var_val[4] = ABW_TextVarVal5;
	s_a_p_st_text_var_val[5] = ABW_TextVarVal6;
	s_a_p_st_text_var_val[6] = ABW_TextVarVal7;
	s_a_p_st_text_var_val[7] = ABW_TextVarVal8;
	s_a_p_st_text_var_val[8] = ABW_TextVarVal9;
	s_a_p_st_text_var_val[9] = ABW_TextVarVal10;
	s_a_p_st_text_var_val[10] = ABW_TextVarVal1;
	s_a_p_st_text_var_val[11] = ABW_TextVarVal2;
	s_a_p_st_text_var_val[12] = ABW_TextVarVal3;
	s_a_p_st_text_var_val[13] = ABW_TextVarVal4;
	s_a_p_st_text_var_val[14] = ABW_TextVarVal5;
	s_a_p_st_text_var_val[15] = ABW_TextVarVal6;
	s_a_p_st_text_var_val[16] = ABW_TextVarVal7;
	s_a_p_st_text_var_val[17] = ABW_TextVarVal8;
	s_a_p_st_text_var_val[18] = ABW_TextVarVal9;
	s_a_p_st_text_var_val[19] = ABW_TextVarVal20;
	s_a_p_st_text_var_sig[0] = ABW_TextVarSig1;
	s_a_p_st_text_var_sig[1] = ABW_TextVarSig2;
	s_a_p_st_text_var_sig[2] = ABW_TextVarSig3;
	s_a_p_st_text_var_sig[3] = ABW_TextVarSig4;
	s_a_p_st_text_var_sig[4] = ABW_TextVarSig5;
	s_a_p_st_text_var_sig[5] = ABW_TextVarSig6;
	s_a_p_st_text_var_sig[6] = ABW_TextVarSig7;
	s_a_p_st_text_var_sig[7] = ABW_TextVarSig8;
	s_a_p_st_text_var_sig[8] = ABW_TextVarSig9;
	s_a_p_st_text_var_sig[9] = ABW_TextVarSig10;
	s_a_p_st_text_var_sig[10] = ABW_TextVarSig11;
	s_a_p_st_text_var_sig[11] = ABW_TextVarSig12;
	s_a_p_st_text_var_sig[12] = ABW_TextVarSig13;
	s_a_p_st_text_var_sig[13] = ABW_TextVarSig14;
	s_a_p_st_text_var_sig[14] = ABW_TextVarSig15;
	s_a_p_st_text_var_sig[15] = ABW_TextVarSig16;
	s_a_p_st_text_var_sig[16] = ABW_TextVarSig17;
	s_a_p_st_text_var_sig[17] = ABW_TextVarSig18;
	s_a_p_st_text_var_sig[18] = ABW_TextVarSig19;
	s_a_p_st_text_var_sig[19] = ABW_TextVarSig20;

	//* 2.初始化消息队列
	mqueue_init();

	//* 3.读取download_trend_var.csv
	file_read((u32_t)s_a_ch_file_trend_var, 1U * 1024U * 1024U, FILE_NAME_TREND_VAR);

	//* 4.解析变量信息
	parse_download_var_csv(&st_trend_var_head, &s_st_trend_info, &s_st_var_info, s_a_ch_file_trend_var);
	 char str[20];
	//* 5.创建变量hash
	for (u32_i = 0U; u32_i < s_st_var_info.u32_vars_parse_lines; u32_i++)
	{
		var_hash_t *p_st_var_hash_node = malloc(sizeof(var_hash_t));
		vars_parse_des_t *p_st_var = &s_st_var_info.vars_parse_buf[u32_i];
		strcpy(p_st_var_hash_node->a_ch_var_name, p_st_var->ch_name);
		p_st_var_hash_node->u32_var_id = p_st_var->u32_data_id;
		p_st_var_hash_node->u32_var_offset = p_st_var->u32_offset;
		p_st_var_hash_node->u32_var_type = p_st_var->u32_data_type;
		HASH_ADD_STR(s_p_st_var_hash, a_ch_var_name, p_st_var_hash_node);
//		printf("current var name is : %s\n", p_st_var->ch_name);
		sprintf(str, "%u", u32_i);
		if(u32_i==968 || u32_i==969)
			;
		else force_var(s_p_st_var_hash, p_st_var->ch_name, "1", "0");	/* 强制变量，值为600，质量位为2（bad） */
	}
	printf("hello~");
	//* 6.批量强制变量
	const char_t *filename = "/SCID300/Config/test.csv";
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file\n");
		return (Pt_CONTINUE);
	}
	// 逐行读取CSV文件
	char line[256];  // 假设一行最多有256个字符
	while (fgets(line, sizeof(line), file) != NULL) {
		// 创建一个结构体实例来存储CSV行的数据
		struct CSVRow row;

		// 使用strtok函数分割CSV行中的数据
		// name
		char *token = strtok(line, ",");
		if(token == NULL)
		{
			continue;
		}
		strcpy(row.name, token);

		// val
		token = strtok(NULL, ",");
		if(token == NULL)
		{
			continue;
		}
		strcpy(row.val, token);

		// sig
		token = strtok(NULL, ",");
		if(token == NULL)
		{
			continue;
		}
		strcpy(row.sig, token);

		//printf("name: %s, val: %s, sig:%s\n", row.name, row.val, row.sig);
		force_var(s_p_st_var_hash, row.name, row.val, row.sig);
	}

	// 关闭文件
	fclose(file);


	return (Pt_CONTINUE);
}

i32_t BtnForceActivate(PtWidget_t *p_st_widget, ApInfo_t *p_st_apinfo, PtCallbackInfo_t *P_st_cbinfo)
{
	u32_t u32_i = 0U;				// 循环计数值
	char_t *p_ch_var_name = P_NULL; // 变量名称
	char_t *p_ch_var_val = P_NULL;	// 变量强制值
	char_t *p_ch_var_sig = P_NULL;	// 变量强制质量位

	for (u32_i = 0U; u32_i < 20U; u32_i++)
	{
		PtGetResource(s_a_p_st_text_var_name[u32_i], Pt_ARG_TEXT_STRING, &p_ch_var_name, 0);
		PtGetResource(s_a_p_st_text_var_val[u32_i], Pt_ARG_TEXT_STRING, &p_ch_var_val, 0);
		PtGetResource(s_a_p_st_text_var_sig[u32_i], Pt_ARG_TEXT_STRING, &p_ch_var_sig, 0);

		force_var(s_p_st_var_hash, p_ch_var_name, p_ch_var_val, p_ch_var_sig);
	}

	return (Pt_CONTINUE);
}

i32_t TimerRefreshActivate(PtWidget_t *p_st_widget, ApInfo_t *p_st_apinfo, PtCallbackInfo_t *p_st_cbinfo)
{

	ack_display_data();

	return (Pt_CONTINUE);
}
