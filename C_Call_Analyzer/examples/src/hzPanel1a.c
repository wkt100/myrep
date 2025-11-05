/*******************************************************************************
* Copyright (C): CTEC
* Filename: hzPanel1a.c
* Author: Xie Xinxin
* Date:   2021/07/26 11:20
* Version: A001
* Description : HZPanel1a
* History:  <author>     <date>            <version>      <description>
*           Xie Xinxin  2021/07/26         A001           create this file
*******************************************************************************/
#include "hzPanel1a.h"
#include "LangSwitchForPanel.h"
#include "photon/HZ_Common.h"
#include "photon_module_parse.h"

static i32_t  trace_hzPanel1a_auto_manu_btn[16] = {-1};/*跟踪手自动按钮(需要二次确认)的状态（1:按下；其它：弹起）*/
static time_t time_hzPanel1a_start_time[16] = {0U};/*记录手自动按钮按下的时间*/
static time_t time_hzPanel1a_des_add_start_time = 0;/*记录增、减按钮的按压时间，用于增减按钮的计数器，400ms累加1次*/
static i32_t  trace_hzPanel1a_auto_manu_mode[16] = {0U};/*跟踪手自动的模式*/
static i32_t  trace_hzPanel1a_fast_slow_mode[16] = {0U};/*跟踪fast/slow的模式*/

static i32_t  trace_hzPanel1a_MVi_btn_green[16] = {0U};/*跟踪增按钮的背关反馈1为有*/
static i32_t  trace_hzPanel1a_MVd_btn_green[16] = {0U};/*跟踪减按钮的背关反馈1为有*/

static i32_t  trace_hzPanel1a_open_mode = FLOW_CHART_PANEL1A;/*记录面板的打开方式*/
static u32_t u32_recv_ddata_index[16] = {UINT_0};
static u32_t u32_hzPanel1a_btn_id[16] = {0U};/*记录需要二次确认按钮的按钮ID（发指令用）*/
//extern u32_t openAllPanelNum;
static var_recv_des_t ack_hzPanel1a_data[16*PANEL_VAR_SIZE_MAX];
//static u32_t traceBtnOpState3[16] = {0U};//on or off
static u32_t hzPanel1a_equip_id[16] = {0};				//打开面板的设备图符绑定的设备ID
static ana_panel_des_t p_st_ana_hzPanel1a;  /* the information of the hzPanel1a configuration */
static u32_t u32_max_pnames_num_hzPanel1a = 16;/*the max pnames num of hzPanel1a*/
static u32_t equipment_widget_index_hzP1a = 0;		   /*打开面板的设备图符在面板储存的设备图符数组中的位置,如果为100，则为群组连续控制器或者设定值动态图符;为200,则为调屏打开*/
static int save_equip_widgets_hzP1a[36] = {0};		   /*save all equipment widgets in the Panel1a*/
static PtWidget_t* now_equipmentWidget_hzP1a[16] = {P_NULL};   /*now Equipment Widget in the Panel1a*/
static mq_send_ldata_t mq_send_ldata_hzPanel1a;
static u32_t  devType =0;
static u32_t s_u32_panel1a_init_tick = 0;

extern ana_panel_des_with_widget g_ana_panel_des_array[UINT_16];

/* 处理变量反馈 */
static void change_widgetState_hzP1a( PtWidget_t *theWindow, u32_t indexCount, u32_t PNameNum )
{
	u32_t  signal_type_hzPanel1a_data[PANELPNAME_REQUEST_NUM]; /*存储变量反馈值数据位*/
	u8_t  signal_type_hzPanel1a_status[PANELPNAME_REQUEST_NUM]; /*存储变量反馈值质量位 */
	u32_t equip_bad_flag = 0;
	u32_t j;
	u32_t u32_index = 0;/*u32_index<PNameNum,计数signal_type_hzPanel1a_data/signal_type_hzPanel1a_status数值索引*/
	char_t a_ch_val[VAR_VAL_SIZE_MAX] = {0};
	char_t automanubtn_text[TEXT_MAX_PANEL_LABEL*2 + 2] = {0};	/*wkt20240703:automanubtn_text => auto_text + '\n' + manu_text*/
	char_t fastslowbtn_text[TEXT_MAX_PANEL_LABEL*2 + 2] = {0};	/*wkt20240703:fastslowbtn_text => fast_text + '\0' + manu_text*/
	char_t *temp_text = P_NULL;
	//PtWidget_t *p_widget = P_NULL;
	u32_t u32_i = 0;
//	if(0 ==  equipment_widget_index_hzP1a)	//wkt20240905 del it because of error that open an error widget will destroy panel state before
//	{
//		return;
//	}
	memset( signal_type_hzPanel1a_data, UINT_0, sizeof(signal_type_hzPanel1a_data) );
	memset( signal_type_hzPanel1a_status, UINT_0, sizeof(signal_type_hzPanel1a_status) );
	for ( j = 0; j < PNameNum; j++ )
	{   //data + status
	    switch (ack_hzPanel1a_data[indexCount*PANELPNAME_REQUEST_NUM + j].u32_type)
	    {
			case VAR_TYPE_REAL_SIGNAL:
				memcpy(&signal_type_hzPanel1a_data[j],ack_hzPanel1a_data[indexCount*PANELPNAME_REQUEST_NUM + j].u8_val,sizeof(u8_t)*4);
				memcpy(&signal_type_hzPanel1a_status[j],&ack_hzPanel1a_data[indexCount*PANELPNAME_REQUEST_NUM + j].u8_val[4],sizeof(u8_t));
			continue;//real signal 1
			case VAR_TYPE_INT_SIGNAL:
				memcpy(&signal_type_hzPanel1a_data[j],ack_hzPanel1a_data[indexCount*PANELPNAME_REQUEST_NUM + j].u8_val,sizeof(u8_t)*2);
				memcpy(&signal_type_hzPanel1a_status[j],&ack_hzPanel1a_data[indexCount*PANELPNAME_REQUEST_NUM + j].u8_val[2],sizeof(u8_t));
			continue;/* int signal  2*/
			case VAR_TYPE_BOOL_SIGNAL:
				memcpy(&signal_type_hzPanel1a_data[j],ack_hzPanel1a_data[indexCount*PANELPNAME_REQUEST_NUM + j].u8_val,sizeof(u8_t));
				memcpy(&signal_type_hzPanel1a_status[j],&ack_hzPanel1a_data[indexCount*PANELPNAME_REQUEST_NUM + j].u8_val[1],sizeof(u8_t));
			continue;/* bool signal  4*/
			case VAR_TYPE_DEVICE_SIGNAL:
				memcpy(&signal_type_hzPanel1a_data[j],ack_hzPanel1a_data[indexCount*PANELPNAME_REQUEST_NUM + j].u8_val,sizeof(u8_t)*4);
			continue;/* device signal 7*/
			case VAR_TYPE_REAL:
				memcpy(&signal_type_hzPanel1a_data[j],ack_hzPanel1a_data[indexCount*PANELPNAME_REQUEST_NUM + j].u8_val,sizeof(u8_t)*4);
			continue; /* real_net 3*/
			case VAR_TYPE_INT:
				memcpy(&signal_type_hzPanel1a_data[j],ack_hzPanel1a_data[indexCount*PANELPNAME_REQUEST_NUM + j].u8_val,sizeof(u8_t)*2);
			continue;/* int_net 5 */
			case VAR_TYPE_BOOL:
				memcpy(&signal_type_hzPanel1a_data[j],ack_hzPanel1a_data[indexCount*PANELPNAME_REQUEST_NUM + j].u8_val,sizeof(u8_t));
			continue;/* bool_net 6 */
	    }
	}

	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM])
	{
#if 0
		/*AUTO/MANU按钮的自动弹起设置*/
		if ( trace_hzPanel1a_auto_manu_btn[indexCount] != DOWN1a )
		{
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAutoManuBtn ),OB_BACKCOLOR,PgRGB(191,191,191),0);
			if ( activateState[indexCount] == 1 )
			{
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAutoManuBtn ),OB_COLOR,PgRGB(0,0,0),0);
				panelBtnFloat(ApGetWidgetPtr( theWindow, ABN_hzP1aAutoManuBtn ));
			}
			else
			{
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAutoManuBtn ),OB_COLOR,PgRGB(166,166,166),0);
				panelBtnGUI(ApGetWidgetPtr( theWindow, ABN_hzP1aAutoManuBtn ));
			}
		}
		else
		{
			/*do nothing*/
		}
#endif
		ana_panel_des_t p_st_ana_hzPanel1a;
		for (u32_i = 0; u32_i < PANEL_SIZE_MAX; u32_i++)
		{
			if (theWindow == g_ana_panel_des_array[u32_i].parent_widget)
			{
				p_st_ana_hzPanel1a = g_ana_panel_des_array[u32_i].p_st_ana_hzPanel;
				break;
			}
		}
		/*AUTO/MANU标签状态设置(AUTO/MANU按钮的指令反馈信号)*/
		if (signal_type_hzPanel1a_data[u32_index] == 1)
		{
			/*AUTO/MANU在变量值为1时“AUTO”背景颜色为绿色（真），“MANU”则为面板默认的背景颜色（假）*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAuto ),Pt_ARG_TEXT_STRING,p_st_ana_hzPanel1a.st_auto_text.a_ch_text_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAuto ),IDD_COLOR,p_st_ana_hzPanel1a.st_auto_text.st_text_color_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAuto ),IDD_BACKCOLOR,p_st_ana_hzPanel1a.st_auto_text.st_back_color_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),Pt_ARG_TEXT_STRING,p_st_ana_hzPanel1a.st_manu_text.a_ch_text_2,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),IDD_COLOR,p_st_ana_hzPanel1a.st_manu_text.st_text_color_2,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),IDD_BACKCOLOR,p_st_ana_hzPanel1a.st_manu_text.st_back_color_2,0);

			trace_hzPanel1a_auto_manu_mode[indexCount] = AUTO1a;
		}
		else
		{
			/*AUTO/MANU在变量值为0时“AUTO”背景颜色为假的设置，“MANU”则为面板默认的背景颜色为真的设置*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAuto ),Pt_ARG_TEXT_STRING,p_st_ana_hzPanel1a.st_auto_text.a_ch_text_2,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAuto ),IDD_COLOR,p_st_ana_hzPanel1a.st_auto_text.st_text_color_2,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAuto ),IDD_BACKCOLOR,p_st_ana_hzPanel1a.st_auto_text.st_back_color_2,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),Pt_ARG_TEXT_STRING,p_st_ana_hzPanel1a.st_manu_text.a_ch_text_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),IDD_COLOR,p_st_ana_hzPanel1a.st_manu_text.st_text_color_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),IDD_BACKCOLOR,p_st_ana_hzPanel1a.st_manu_text.st_back_color_1,0);

			trace_hzPanel1a_auto_manu_mode[indexCount] = MANU1a;
		}

		if(signal_type_hzPanel1a_status[u32_index] != 0)
		{
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAuto ),ID_STATE,ID_BAD,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),ID_STATE,ID_BAD,0);
		}
		else
		{
			if (signal_type_hzPanel1a_data[u32_index] == 1)
			{
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAuto ),ID_STATE,ID_HIGH,0);
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),ID_STATE,ID_LOW,0);
			}
			else
			{
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAuto ),ID_STATE,ID_LOW,0);
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),ID_STATE,ID_HIGH,0);
			}
		}
		u32_index++;
	}
	else
	{
		/* 无手自动变量组态，默认为手动状态 */
		trace_hzPanel1a_auto_manu_mode[indexCount] = MANU1a;
	}

	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 1])
	{
		/*AUTO/MANU标签状态设置(AUTO/MANU强制切手动操作信号)*/
		if (signal_type_hzPanel1a_data[u32_index] == 1)
		{
			/*“MANU”则为强制的红色背景颜色*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),Pt_ARG_TEXT_STRING,p_st_ana_hzPanel1a.st_manu_text.a_ch_text_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),IDD_COLOR,p_st_ana_hzPanel1a.st_manu_text.st_text_color_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),IDD_BACKCOLOR,PgRGB(255,0,0),0);

			trace_hzPanel1a_auto_manu_mode[indexCount] = FORCEMANU1a;
			panelBtnGUI(ApGetWidgetPtr( theWindow, ABN_hzP1aAutoManuBtn ));/*AUTO/MANU按钮闭锁*/
		}
		else
		{
			/*无AUTO/MANU强制切手动操作信号*/
			if ( activateState[indexCount] == 1 && DOWN1a != trace_hzPanel1a_auto_manu_btn[indexCount])
			{
				//PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAutoManuBtn ),OB_COLOR,PgRGB(0,0,0),0);
				panelBtnFloat(ApGetWidgetPtr( theWindow, ABN_hzP1aAutoManuBtn ));/*AUTO/MANU按钮弹起*/
			}
		}

		if (signal_type_hzPanel1a_status[u32_index] == 0)
		{
			if ((1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM])&& (0 != signal_type_hzPanel1a_status[u32_index-1]))
			{
				/* 保持红框 */
			}
			else
			{
				if (signal_type_hzPanel1a_data[u32_index] == 1)
				{
					PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),ID_STATE,ID_HIGH,0);
				}
				else
				{
					PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),ID_STATE,ID_LOW,0);
				}
			}
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAuto ),ID_STATE,ID_BAD,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu ),ID_STATE,ID_BAD,0);
		}

		u32_index++;
	}

	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 2])
	{
		/*FAST/SLOW标签状态设置(FAST/SLOW按钮的指令反馈信号)*/
		if (signal_type_hzPanel1a_data[u32_index] == 1)
		{
			/*FAST/SLOW在变量值为1时“FAST”背景颜色为绿色（真），“SLOW”则为面板默认的背景颜色（假）*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFast ),Pt_ARG_TEXT_STRING,p_st_ana_hzPanel1a.st_fast_text.a_ch_text_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFast ),IDD_COLOR,p_st_ana_hzPanel1a.st_fast_text.st_text_color_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFast ),IDD_BACKCOLOR,p_st_ana_hzPanel1a.st_fast_text.st_back_color_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aSlow ),Pt_ARG_TEXT_STRING,p_st_ana_hzPanel1a.st_slow_text.a_ch_text_2,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aSlow ),IDD_COLOR,p_st_ana_hzPanel1a.st_slow_text.st_text_color_2,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aSlow ),IDD_BACKCOLOR,p_st_ana_hzPanel1a.st_slow_text.st_back_color_2,0);

			trace_hzPanel1a_fast_slow_mode[indexCount] = FAST1a;
		}
		else
		{
			/*FAST/SLOW在变量值为0时“FAST”背景颜色为假的设置，“SLOW”则为面板默认的背景颜色为真的设置*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFast ),Pt_ARG_TEXT_STRING,p_st_ana_hzPanel1a.st_fast_text.a_ch_text_2,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFast ),IDD_COLOR,p_st_ana_hzPanel1a.st_fast_text.st_text_color_2,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFast ),IDD_BACKCOLOR,p_st_ana_hzPanel1a.st_fast_text.st_back_color_2,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aSlow ),Pt_ARG_TEXT_STRING,p_st_ana_hzPanel1a.st_slow_text.a_ch_text_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aSlow ),IDD_COLOR,p_st_ana_hzPanel1a.st_slow_text.st_text_color_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aSlow ),IDD_BACKCOLOR,p_st_ana_hzPanel1a.st_slow_text.st_back_color_1,0);

			trace_hzPanel1a_fast_slow_mode[indexCount] = SLOW1a;
		}

		if(signal_type_hzPanel1a_status[u32_index] != 0)
		{
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFast ),ID_STATE,ID_BAD,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aSlow ),ID_STATE,ID_BAD,0);
		}
		else
		{
			if (signal_type_hzPanel1a_data[u32_index] == 1)
			{
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFast ),ID_STATE,ID_HIGH,0);
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aSlow ),ID_STATE,ID_LOW,0);
			}
			else
			{
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFast ),ID_STATE,ID_LOW,0);
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aSlow ),ID_STATE,ID_HIGH,0);
			}
		}

		u32_index++;
	}
	else
	{
		/* 快慢速组态，默认为慢速状态 */
		trace_hzPanel1a_fast_slow_mode[indexCount] = SLOW1a;
	}

	/*PV*/
	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 3])
	{
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayPV ), AD_VARTYPE, REAL, 0);
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayPV ), AD_STATE, p_st_ana_hzPanel1a.u32_pv_format, 0);

		memset(a_ch_val, 0, sizeof(a_ch_val));
		sprintf(a_ch_val, "%.40f", *(f32_t*)(&signal_type_hzPanel1a_data[u32_index]));
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayPV ), Pt_ARG_TEXT_STRING, a_ch_val, 0);
		if(signal_type_hzPanel1a_status[u32_index] == COM_FAILURE_STATUS )
		{
			/*通讯故障*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayPV ), AD_RUNSTATE, ADCOMFAILURE, 0);
		}
		else if ( signal_type_hzPanel1a_status[u32_index] != 0 )
		{
			/*质量位坏*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayPV ),AD_RUNSTATE,BADQUALITY,0);
		}
		else
		{
			/*正常*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayPV ),AD_RUNSTATE,NORMAL,0);
		}
		u32_index++;
	}

	/*SV*/
	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 4])
	{
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplaySV ), AD_VARTYPE, REAL, 0);
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplaySV ), AD_STATE, p_st_ana_hzPanel1a.u32_sv_format, 0);

		memset(a_ch_val, 0, sizeof(a_ch_val));
		sprintf(a_ch_val, "%.40f", *(f32_t*)(&signal_type_hzPanel1a_data[u32_index]));
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplaySV ), Pt_ARG_TEXT_STRING, a_ch_val, 0);
		if(signal_type_hzPanel1a_status[u32_index] == COM_FAILURE_STATUS )
		{
			/*通讯故障*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplaySV ), AD_RUNSTATE, ADCOMFAILURE,0);
		}
		else if ( signal_type_hzPanel1a_status[u32_index] != 0 )
		{
			/*质量位坏*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplaySV ),AD_RUNSTATE,BADQUALITY,0);
		}
		else
		{
			/*正常*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplaySV ),AD_RUNSTATE,NORMAL,0);
		}
		u32_index++;
	}

	/*DV*/
	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 5])
	{
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayDV ), AD_VARTYPE, REAL, 0);
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayDV ), AD_STATE, XXXX_X, 0);

		memset(a_ch_val, 0, sizeof(a_ch_val));
		sprintf(a_ch_val, "%.40f", *(f32_t*)(&signal_type_hzPanel1a_data[u32_index]));
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayDV ), Pt_ARG_TEXT_STRING, a_ch_val, 0);
		if(signal_type_hzPanel1a_status[u32_index] == COM_FAILURE_STATUS )
		{
			/*通讯故障*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayDV ), AD_RUNSTATE, ADCOMFAILURE,0);
		}
		else if ( signal_type_hzPanel1a_status[u32_index] != 0 )
		{
			/*质量位坏*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayDV ),AD_RUNSTATE,BADQUALITY,0);
		}
		else
		{
			/*正常*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayDV ),AD_RUNSTATE,NORMAL,0);
		}
		u32_index++;
	}

	/*LV*/
	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 6])
	{
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayLV ), AD_VARTYPE, REAL, 0);
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayLV ), AD_STATE, XXX_XX, 0);

		memset(a_ch_val, 0, sizeof(a_ch_val));
		sprintf(a_ch_val, "%.40f", *(f32_t*)(&signal_type_hzPanel1a_data[u32_index]));
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayLV ), Pt_ARG_TEXT_STRING, a_ch_val, 0);
		if(signal_type_hzPanel1a_status[u32_index] == COM_FAILURE_STATUS )
		{
			/*通讯故障*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayLV ), AD_RUNSTATE, ADCOMFAILURE,0);
		}
		else if ( signal_type_hzPanel1a_status[u32_index] != 0 )
		{
			/*质量位坏*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayLV ),AD_RUNSTATE,BADQUALITY,0);
		}
		else
		{
			/*正常*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayLV ),AD_RUNSTATE,NORMAL,0);
		}
		u32_index++;
	}

	/*MV*/
	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 7])
	{
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayMV ), AD_VARTYPE, REAL, 0);
		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayMV ), AD_STATE, XXX_XX, 0);

		memset(a_ch_val, 0, sizeof(a_ch_val));
		sprintf(a_ch_val, "%.40f", *(f32_t*)(&signal_type_hzPanel1a_data[u32_index]));

		PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayMV ), Pt_ARG_TEXT_STRING, a_ch_val, 0);
		if(signal_type_hzPanel1a_status[u32_index] == COM_FAILURE_STATUS )
		{
			/*通讯故障*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayMV ), AD_RUNSTATE, ADCOMFAILURE,0);
		}
		else if ( signal_type_hzPanel1a_status[u32_index] != 0 )
		{
			/*质量位坏*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayMV ),AD_RUNSTATE,BADQUALITY,0);
		}
		else
		{
			/*正常*/
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAnalogDisplayMV ),AD_RUNSTATE,NORMAL,0);
		}
		u32_index++;
	}
	/*UNAVA故障信息显示*/
	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 8])
	{
		if (signal_type_hzPanel1a_data[u32_index] == 1)
		{
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aUnava ), Pt_ARG_TEXT_STRING, p_st_ana_hzPanel1a.st_unava_text.a_ch_text_1, 0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aUnava ),IDD_COLOR,p_st_ana_hzPanel1a.st_unava_text.st_text_color_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aUnava ),IDD_BACKCOLOR,p_st_ana_hzPanel1a.st_unava_text.st_back_color_1,0);
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aUnava ), Pt_ARG_TEXT_STRING, p_st_ana_hzPanel1a.st_unava_text.a_ch_text_2, 0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aUnava ),IDD_COLOR,p_st_ana_hzPanel1a.st_unava_text.st_text_color_2,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aUnava ),IDD_BACKCOLOR,p_st_ana_hzPanel1a.st_unava_text.st_back_color_2,0);
		}

		if (0 == signal_type_hzPanel1a_status[u32_index])
		{
			if (signal_type_hzPanel1a_data[u32_index] == 1)
			{
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aUnava ),ID_STATE,ID_HIGH,0);
			}
			else
			{
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aUnava ),ID_STATE,ID_LOW,0);
			}
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aUnava ),ID_STATE,ID_BAD,0);
		}

		u32_index++;
	}
	/*FB ER故障信息显示*/
	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 9])
	{
		if (signal_type_hzPanel1a_data[u32_index] == 1)
		{
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFber ), Pt_ARG_TEXT_STRING, p_st_ana_hzPanel1a.st_fber_text.a_ch_text_1, 0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFber ),IDD_COLOR,p_st_ana_hzPanel1a.st_fber_text.st_text_color_1,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFber ),IDD_BACKCOLOR,p_st_ana_hzPanel1a.st_fber_text.st_back_color_1,0);
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFber ), Pt_ARG_TEXT_STRING, p_st_ana_hzPanel1a.st_fber_text.a_ch_text_2, 0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFber ),IDD_COLOR,p_st_ana_hzPanel1a.st_fber_text.st_text_color_2,0);
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFber ),IDD_BACKCOLOR,p_st_ana_hzPanel1a.st_fber_text.st_back_color_2,0);
		}

		if (0 == signal_type_hzPanel1a_status[u32_index])
		{
			if (signal_type_hzPanel1a_data[u32_index] == 1)
			{
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFber ),ID_STATE,ID_HIGH,0);
			}
			else
			{
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFber ),ID_STATE,ID_LOW,0);
			}
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFber ),ID_STATE,ID_BAD,0);
		}

		u32_index++;
	}
	/*设备图符*/
	if((1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 10]) && (1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 11])) /*全开、全关组态变量点*/
	{
		/*全开全关变量逻辑处理*/
		/*全开状态*/
		if ( (signal_type_hzPanel1a_data[u32_index] == 1) && (signal_type_hzPanel1a_data[u32_index + 1] == 0) )
		{
			if(P_NULL != now_equipmentWidget_hzP1a[indexCount])
			{
				set_equip_status_hzPanel1a(theWindow, now_equipmentWidget_hzP1a[indexCount], START1a, indexCount);
			}
			if ( (signal_type_hzPanel1a_status[u32_index] == 0) && (signal_type_hzPanel1a_status[u32_index + 1] == 0) &&
				 (P_NULL != now_equipmentWidget_hzP1a[indexCount]) )/*good*/
			{
				trace_hzPanels_ms_state[indexCount] = 0;
				equip_bad_flag = 0;
				set_hzPanel1a_Q_State(theWindow, now_equipmentWidget_hzP1a[indexCount], QRect1a, indexCount);
			}
			else if( P_NULL != now_equipmentWidget_hzP1a[indexCount] )/*bad*/
			{
				trace_hzPanels_ms_state[indexCount] = 1;
				equip_bad_flag = 1;
				set_hzPanel1a_Q_State(theWindow, now_equipmentWidget_hzP1a[indexCount], Q1a, indexCount);
			}
			else
			{
				/*do nothing*/
			}
		}
		/*全关状态*/
		else if ( (signal_type_hzPanel1a_data[u32_index] == 0) && (signal_type_hzPanel1a_data[u32_index + 1] == 1)
				&& (P_NULL != now_equipmentWidget_hzP1a[indexCount]) )
		{
			set_equip_status_hzPanel1a(theWindow, now_equipmentWidget_hzP1a[indexCount], STOP1a, indexCount);
			if ( (signal_type_hzPanel1a_status[u32_index] == 0) && (signal_type_hzPanel1a_status[u32_index + 1] == 0) )/*good*/
			{
				trace_hzPanels_ms_state[indexCount] = 0;
				equip_bad_flag = 0;
				set_hzPanel1a_Q_State(theWindow, now_equipmentWidget_hzP1a[indexCount], QRect1a, indexCount);
			}
			else/*bad*/
			{
				trace_hzPanels_ms_state[indexCount] = 1;
				equip_bad_flag = 1;
				set_hzPanel1a_Q_State(theWindow, now_equipmentWidget_hzP1a[indexCount], Q1a, indexCount);
			}
		}
		/*中间态*/
		else if ( (signal_type_hzPanel1a_data[u32_index] == 0) && (signal_type_hzPanel1a_data[u32_index + 1] == 0)
				&& (P_NULL != now_equipmentWidget_hzP1a[indexCount]) )
		{
			if ( (signal_type_hzPanel1a_status[u32_index] == 0) && (signal_type_hzPanel1a_status[u32_index + 1] == 0) )/*good*/
			{
				trace_hzPanels_ms_state[indexCount] = 0;
				equip_bad_flag = 0;
				set_hzPanel1a_Q_State(theWindow, now_equipmentWidget_hzP1a[indexCount], QRect1a, indexCount);
			}
			else/*bad*/
			{
				trace_hzPanels_ms_state[indexCount] = 1;
				equip_bad_flag = 1;
				set_hzPanel1a_Q_State(theWindow, now_equipmentWidget_hzP1a[indexCount], Q1a, indexCount);
			}
			set_equip_status_hzPanel1a(theWindow, now_equipmentWidget_hzP1a[indexCount], MIDDLE1a, indexCount);
		}
		/*不确定状态*/
		else if( P_NULL != now_equipmentWidget_hzP1a[indexCount] )
		{
			set_equip_status_hzPanel1a(theWindow, now_equipmentWidget_hzP1a[indexCount], UNDEFINED1a, indexCount);
			//if ( (signal_type_hzPanel1a_status[u32_index] == 0) && (signal_type_hzPanel1a_status[u32_index + 1] == 0) )/*good*/
			//{
			//	trace_hzPanels_ms_state[indexCount] = 0;
			//	equip_bad_flag = 0;
			//	set_hzPanel1a_Q_State(theWindow, now_equipmentWidget_hzP1a[indexCount], QRect1a, indexCount);
			//}
			//else/*bad*/
			//{
				trace_hzPanels_ms_state[indexCount] = 1;
				equip_bad_flag = 1;
				set_hzPanel1a_Q_State(theWindow, now_equipmentWidget_hzP1a[indexCount], Q1a, indexCount);
			//}
		}
		else
		{
			/*do nothing*/
		}
		u32_index++;
		u32_index++;
	}
	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 12])/*偏差信号*/
	{
		if ( ((signal_type_hzPanel1a_data[u32_index] == 1) || (signal_type_hzPanel1a_status[u32_index] != 0))
			&& (P_NULL != now_equipmentWidget_hzP1a[indexCount]) )
		{
			/* 不确定状态 */
			//set_equip_status_hzPanel1a(theWindow, now_equipmentWidget_hzP1a[indexCount], UNDEFINED1a, indexCount);
			set_hzPanel1a_Q_State(theWindow, now_equipmentWidget_hzP1a[indexCount], Q1a, indexCount);
		}
		u32_index++;
	}
	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 13]) /*(标签信号)RS、TS组态变量点*/
	{
		u16_t u16_var3_val = 0;
		u16_var3_val = (u16_t)(signal_type_hzPanel1a_data[u32_index] >> 16);
		/*响应挂牌(隔离)和试验状态*/
		if(P_NULL != now_equipmentWidget_hzP1a[indexCount])
		{
			if (SYS_INFO.u32_monitor_status != SCID_MONITOR_STATUS_AVA)
			{
				set_hzPanel1a_ts_rs_state(theWindow, now_equipmentWidget_hzP1a[indexCount], TSRSNone, indexCount);
			}
			else {
				switch (u16_var3_val)
				{
				case 0:
//					if(equip_bad_flag == 0)
					{
						set_hzPanel1a_ts_rs_state(theWindow, now_equipmentWidget_hzP1a[indexCount], TSRSNone, indexCount);
					} /* 未挂牌、未试验 */
					break;
				case 1 :
					set_hzPanel1a_ts_rs_state(theWindow, now_equipmentWidget_hzP1a[indexCount], RS1a, indexCount); /* 挂牌、未试验 */
					break;
				case 2:
					set_hzPanel1a_ts_rs_state(theWindow, now_equipmentWidget_hzP1a[indexCount], TS1a, indexCount); /* 未挂牌、试验 */
					break;
				case 3:
					set_hzPanel1a_ts_rs_state(theWindow, now_equipmentWidget_hzP1a[indexCount], TSandRS1a, indexCount); /* 挂牌、试验 */
					break;
				default:
//					if(equip_bad_flag == 0)
					{
						set_hzPanel1a_ts_rs_state(theWindow, now_equipmentWidget_hzP1a[indexCount], TSRSNone, indexCount);
					}/* 未挂牌、未试验 */
					break;
				}
			}
		}
		u32_index++;
	}
#if 0
	/*MV/LV增、减按钮*/
	if((1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 14])||
	   (1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 15]))
	{
		/*依据MV/LV增、减按钮状态，设置设定值图符的状态*/
		if((equipment_widget_index_hzP1a == 100) && (PtWidgetIsClass(widgetEquipPanel[indexCount], HZSetValue) != 0))/*设定值图符*/
		{
			PtSetResource(widgetEquipPanel[indexCount],HZ_SV_STATE,HZ_SVEXTERNAL,0);/*则为内给定状态*/
			PtSetResource(widgetEquipPanel[indexCount],HZ_SV_INORDE,HZ_SVNONE,0);/*无增、减状态*/
		}
	}
#endif
	/*MV/LV增按钮*/
	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 14])
	{
		//此处与set_btnState_by_mode_hzP1a设置重复，且逻辑冲突，不再使用
		if (signal_type_hzPanel1a_data[u32_index] == 1)/*背光反馈*/
		{
			trace_hzPanel1a_MVi_btn_green[indexCount] = 1;
		}
		else
		{
			trace_hzPanel1a_MVi_btn_green[indexCount] = 0;
		}
		u32_index++;
	}
	/*MV/LV减按钮*/
	if(1 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 15])
	{
		if (signal_type_hzPanel1a_data[u32_index] == 1)/*背光反馈*/
		{
			trace_hzPanel1a_MVd_btn_green[indexCount] = 1;
		}
		else
		{
			trace_hzPanel1a_MVd_btn_green[indexCount] = 0;
		}
		u32_index++;
	}

	PtGetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aAuto), Pt_ARG_TEXT_STRING, &temp_text, 0 );
	strncat(automanubtn_text, temp_text, TEXT_MAX_PANEL_LABEL);
	strncat(automanubtn_text, "\n", 1);
	PtGetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aManu), Pt_ARG_TEXT_STRING, &temp_text, 0 );
	strncat(automanubtn_text, temp_text, TEXT_MAX_PANEL_LABEL);
	strncat(automanubtn_text, "\0", 1);
	PtSetResource(ApGetWidgetPtr(theWindow, ABN_hzP1aAutoManuBtn), Pt_ARG_TEXT_STRING, automanubtn_text, 0);

	PtGetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aFast), Pt_ARG_TEXT_STRING, &temp_text, 0 );
	strncat(fastslowbtn_text, temp_text, TEXT_MAX_PANEL_LABEL);
	strncat(fastslowbtn_text, "\n", 1);
	PtGetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aSlow), Pt_ARG_TEXT_STRING, &temp_text, 0 );
	strncat(fastslowbtn_text, temp_text, TEXT_MAX_PANEL_LABEL);
	strncat(fastslowbtn_text, "\0", 1);
	PtSetResource(ApGetWidgetPtr(theWindow, ABN_hzP1aFastSlowBtn), Pt_ARG_TEXT_STRING, fastslowbtn_text, 0);
}

/*******************************************************************************
* Function: trace_hzP1a_orderBtn_is_auto_up
* Identifier:
* Description: set panel7 button auto up
* Input: indexPName——current panel index in panel arrays
* 		widget1——button pttimer widget pointer
* 		widget2——button widget pointer
* 		widget3——P7ExecuteBtn pointer
* 		trace——trace flag，1 is to trace，0 is not to do
* 		PeriodTime——periodtime of panel periodrefresh function
* Output: trace——updated trace
* Return: trace
* Call: None
* Others: None
* Log: 2024/09/20  wkt add Code Comments
*******************************************************************************/
/*设置需要二次确认按钮的自动弹起*/
static i32_t trace_hzP1a_orderBtn_is_auto_up(u32_t indexPName, PtWidget_t *widget1, PtWidget_t *widget2, PtWidget_t *widget3, i32_t trace, time_t periodTime)
{
	time_t end_time = 0U;
	time_t *timer_time = P_NULL;
	u32_t  u32_ret = OK;
	if ( trace == DOWN1a )
	{
		PtGetResource(widget1,Pt_ARG_TIMER_REPEAT,&timer_time,0);
		if ( *timer_time == 0 )
		{
			trace = UP1a;
		}
		else if ( (*timer_time > 0) && (*timer_time < 999000) )
		{
			time_t t = *timer_time;//ms
			i32_t ret = UINT_0;
			struct timeb timer_endTime;  // millisecond time
			memset(&timer_endTime, UINT_0, sizeof(struct timeb));
			ret = ftime(&timer_endTime);
			end_time = timer_endTime.time*1000 + timer_endTime.millitm;//ms
			time_t time = end_time - time_hzPanel1a_start_time[indexPName];
			if((time >= t) )//&& (time <= t + periodTime))//ms防止运行超时导致误判
			{
				panelBtnFloat( widget2 );
				trace = UP1a;
				panelBtnGUI( widget3 );
			}
		}
		else
		{
			u32_ret = photon_show_msg(MSG_BUTTON_AUTO_UP_TIME_ILLEGAL);
			trace = UP1a;
		}
	}
	return trace;
}


#if 0
static u32_t check_app_mode()
{
	u32_t u32_ret = UINT_0;//鍑芥暟杩斿洖鍊�
	char_t ch_app_mode[UINT_3] = {0};//搴旂敤妯″紡
	strncpy( ch_app_mode,  mq_cyc_recv_data.mq_recv_ddata.var_recv_buf[INFO_INDEX_APP_MODE].ch_var_val, UINT_3);//鑾峰彇褰撳墠搴旂敤妯″紡
	u32_ret = atoi( ch_app_mode);

	return u32_ret;
}
#endif

/* 根据运行模式设置面板的闭锁状态 */
static u32_t set_hzPanel1a_block_state( PtWidget_t *pwidget, u32_t u32_app_mode, u32_t u32_pname_index )
{
	u32_t u32_err = OK;

	if( P_NULL == pwidget )
	{
		u32_err = ERR_FAILED;
	}

	if( OK == u32_err )
	{
		switch (SYS_INFO.u32_control_status)
		{
		case SCID_CONTROL_STATUS_AVA :
			PtSetResource(ApGetWidgetPtr( pwidget, ABN_hzP1aActivateButton ), Pt_ARG_FLAGS, 0, Pt_BLOCKED );
			PtSetResource(ApGetWidgetPtr( pwidget, ABN_hzP1aActivateButton ), Pt_ARG_FLAGS, 0x2000480, 0x2000480 );
			if (1 == activateState[u32_pname_index])
			{
				/* 激活状态 */
				PtSetResource(ApGetWidgetPtr( pwidget, ABN_hzP1aActivateButton ),AB_STATE,AB_DOWN,0);
				PtSetResource(ApGetWidgetPtr( pwidget, ABN_hzP1aActivateButton),AB_COLOR,PgRGB(255,255,255),0);
			}
			else
			{
				/* 非激活状态 */
				PtSetResource(ApGetWidgetPtr( pwidget, ABN_hzP1aActivateButton ), AB_STATE, AB_UP, 0);
				PtSetResource(ApGetWidgetPtr( pwidget, ABN_hzP1aActivateButton ), AB_COLOR, PgRGB(0,0,0), 0);
			}
			break;
		case SCID_CONTROL_STATUS_UNAVA :
			activateState[u32_pname_index] = 0;/*未激活状态*/
			PtSetResource(ApGetWidgetPtr( pwidget, ABN_hzP1aActivateButton ), Pt_ARG_FLAGS, Pt_BLOCKED, Pt_BLOCKED);
			PtSetResource(ApGetWidgetPtr( pwidget, ABN_hzP1aActivateButton ), AB_STATE, AB_UP, 0);
			PtSetResource(ApGetWidgetPtr( pwidget, ABN_hzP1aActivateButton ), AB_COLOR, PgRGB(0,0,0), 0);	/*wkt20240711*/

			panelBtnGUI( ApGetWidgetPtr( pwidget, ABN_hzP1aAutoManuBtn ) );
			panelBtnGUI( ApGetWidgetPtr( pwidget, ABN_hzP1aFastSlowBtn ) );
			panelBtnGUINoClr( ApGetWidgetPtr( pwidget, ABN_hzP1aMViBtn ) );
			panelBtnGUINoClr( ApGetWidgetPtr( pwidget, ABN_hzP1aMVdBtn ) );
			panelBtnGUI( ApGetWidgetPtr( pwidget, ABN_hzP1aExecuteBtn ) );

			trace_hzPanel1a_auto_manu_btn[u32_pname_index] = -1;

			break;
		default:
			u32_err = ERR_FAILED;
			break;
		}
	}
	return u32_err;
}

int
end_dragging_hzP1a_win( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PhDragEvent_t *dragData;
	PhPoint_t new_pos;

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	/* Ignore all events until dragging is done. */

	if (cbinfo->event->subtype != Ph_EV_DRAG_MOVE)//Ph_EV_DRAG_COMPLETE//Ph_EV_DRAG_MOVE//The user has released the mouse button
	{
		return (Pt_CONTINUE);
	}

	/* Get the data associated with the event. */
	dragData = PhGetData (cbinfo->event);

	/* The rectangle in this data is the absolute
	   coordinates of the dragging rectangle. We want to
	   calculate the new position of the widget, relative to
	   the dragging region. */

	new_pos.x = dragData->rect.ul.x
				+ cbinfo->event->translation.x;
	new_pos.y = dragData->rect.ul.y
				+ cbinfo->event->translation.y;

	//printf ("New position: (%d, %d)\n", new_pos.x, new_pos.y);

	/* Move the widget. */
	PtSetResource (dragged_panel, Pt_ARG_POS, &new_pos, 0);

	return( Pt_CONTINUE );
}


/*modifty: get the PNames and texts of the hzPanel1a configuration*/
static u32_t get_PNames_hzPanel1a_configuration(PtWidget_t *window, u32_t indexPNames)
{
	u32_t u32_dev_id = 0;                           /* 设备ID */
	u32_t u32_return_get_info = OK;
	char_t *result = P_NULL;

	/*modifty: 还是流程图设备图符打开面板*/
	if(	trace_hzPanel1a_open_mode == NC_SCREEN_PANEL1A )
	{
		u32_dev_id = photon_get_instruction_id();   /* get the ID of the device */
		//u32_dev_id = 3167;/*temp:只是为了调试用，之后去掉*/
		char num_to_str[VAR_NAME_SIZE_MAX];
		//PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aEquipmentID), Pt_ARG_TEXT_STRING, itoa(u32_dev_id, num_to_str, 10), 0);
		PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aEquipmentID), Pt_ARG_TEXT_STRING, p_st_ana_hzPanel1a.a_ch_panel_id, 0);
	}
	else
	{
		u32_dev_id = hzPanel1a_equip_id[indexPNames];
	}

	memset(&p_st_ana_hzPanel1a, UINT_0, sizeof(ana_panel_des_t));
	u32_return_get_info = photon_get_ana_panel_info(&p_st_ana_hzPanel1a, u32_dev_id);  /* get the information of the hzPanel2 configuration */
	if(OK != u32_return_get_info)
	{
		//PanelPName_NO[indexPNames] = 1;
		return u32_return_get_info;
	}
	else
	{
		/* do the following codes */
	}

	memcpy(&g_ana_panel_des_array[indexPNames].p_st_ana_hzPanel, &p_st_ana_hzPanel1a, sizeof(ana_panel_des_t));
	g_ana_panel_des_array[indexPNames].parent_widget = window;

	//获取AUTO/MANU指令反馈信号
	result = p_st_ana_hzPanel1a.a_ch_am_var;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM], result, VAR_NAME_SIZE_MAX);
	}

	//获取AUTO/MANU强制切手动操作信号
	result = p_st_ana_hzPanel1a.a_ch_fm_var;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 1] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 1] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 1], result, VAR_NAME_SIZE_MAX);
	}

	//获取FAST/SLOW指令反馈信号
	result = p_st_ana_hzPanel1a.a_ch_fs_var;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 2] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 2] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 2], result, VAR_NAME_SIZE_MAX);
	}

	//获取模拟量PV输入变量
	result = p_st_ana_hzPanel1a.a_ch_pv_var;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 3] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 3] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 3], result, VAR_NAME_SIZE_MAX);
	}

	//获取模拟量SV输入变量
	result = p_st_ana_hzPanel1a.a_ch_sv_var;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 4] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 4] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 4], result, VAR_NAME_SIZE_MAX);
	}

	//获取模拟量DV输入变量
	result = p_st_ana_hzPanel1a.a_ch_dv_var;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 5] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 5] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 5], result, VAR_NAME_SIZE_MAX);
	}

	//获取模拟量LV输入变量
	result = p_st_ana_hzPanel1a.a_ch_lv_var;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 6] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 6] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 6], result, VAR_NAME_SIZE_MAX);
	}

	//获取模拟量MV输入变量
	result = p_st_ana_hzPanel1a.a_ch_mv_var;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 7] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 7] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 7], result, VAR_NAME_SIZE_MAX);
	}

	//获取UNAVA故障信号
	/* get the var of UNAVA widget*/
	result = p_st_ana_hzPanel1a.a_ch_unava_var;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 8] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 8] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 8], result, VAR_NAME_SIZE_MAX);
	}
	//获取FBER故障信号
	/* get the var of FBER widget*/
	result = p_st_ana_hzPanel1a.a_ch_fber_var;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 9] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 9] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 9], result, VAR_NAME_SIZE_MAX);
	}

	//获取设备图符绑定的全开变量
	/* get the var of all open state of dev widget*/
	result = p_st_ana_hzPanel1a.a_ch_g1_var1;/*a_ch_g1_var1*/
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 10] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 10] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 10], result, VAR_NAME_SIZE_MAX);
	}

	//获取设备图符绑定的全关变量
	/* get the var of all close state of dev widget*/
	result = p_st_ana_hzPanel1a.a_ch_g1_var2;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 11] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 11] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 11], result, VAR_NAME_SIZE_MAX);
	}

	//获取设备图符绑定的偏差信号
	/* get the var of padlock state of dev widget*/
	result = p_st_ana_hzPanel1a.a_ch_fb_var;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 12] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 12] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 12], result, VAR_NAME_SIZE_MAX);
	}

	//获取设备图符绑定的试验/隔离（标签信号）变量
	/* get the var of padlock state of dev widget*/
	result = p_st_ana_hzPanel1a.a_ch_g1_var3;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 13] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 13] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 13], result, VAR_NAME_SIZE_MAX);
	}

	//获取MV/LV增按钮指令反馈信号
	result = p_st_ana_hzPanel1a.a_ch_mvi_var;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 14] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 14] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 14], result, VAR_NAME_SIZE_MAX);
	}

	//获取MV/LV减按钮指令反馈信号
	result = p_st_ana_hzPanel1a.a_ch_mvd_var;
	if(result == P_NULL || strlen(result) == 0)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 15] = 0;/*未组态变量点*/
	}
	else if(result != P_NULL)
	{
		PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + 15] = 1;/*组态变量点*/
		strncpy(PName_Request[indexPNames * PANELPNAME_REQUEST_NUM + 15], result, VAR_NAME_SIZE_MAX);
	}

	return OK;
}

/*设置当前打开的设备图符（流程图）及获得设备ID，写面板的设备ID*/
static void set_hzPanel1a_equipmentWidget(PtWidget_t *flowChart, PtWidget_t* window, u32_t indexCount, PtWidget_t* widget)
{
	char_t *p_result = P_NULL;
	char_t *p_endstr = P_NULL;
	PgColor_t* u32_wgt_color = P_NULL;

	save_equip_widgets_hzP1a[0] = ABN_hzP1aAirOpDamper;
	save_equip_widgets_hzP1a[1] = ABN_hzP1aMtDrvDamper;
	save_equip_widgets_hzP1a[2] = ABN_hzP1aMtFireDamper;

	save_equip_widgets_hzP1a[3] = ABN_hzP1aHZ_AiroperatedControlValve;
	save_equip_widgets_hzP1a[4] = ABN_hzP1aHZ_AirOperatedValve;
	save_equip_widgets_hzP1a[5] = ABN_hzP1aHZ_MotorDriveValve;
	save_equip_widgets_hzP1a[6] = ABN_hzP1aHZ_PilotoperateOne;
	save_equip_widgets_hzP1a[7] = ABN_hzP1aHZ_PilotoperateTwo;
	save_equip_widgets_hzP1a[8] = ABN_hzP1aHZ_PistonDrivenValve;
	save_equip_widgets_hzP1a[9] = ABN_hzP1aHZ_SafetyValveOne;
	save_equip_widgets_hzP1a[10] = ABN_hzP1aHZ_SolenoidControlValve;
	save_equip_widgets_hzP1a[11] = ABN_hzP1aHZ_SolenoidDriveValve;

	save_equip_widgets_hzP1a[12] = ABN_hzP1aHZ_MotorDriveTreeWayValve;
	save_equip_widgets_hzP1a[13] = ABN_hzP1aHZ_VacuumBreakingValve;
	save_equip_widgets_hzP1a[14] = ABN_hzP1aHZ_MotorControlValve;
	save_equip_widgets_hzP1a[15] = ABN_hzP1aHZ_MotorDriveTreeWayControlValve;
	save_equip_widgets_hzP1a[16] = ABN_hzP1aHZManualValve;
	save_equip_widgets_hzP1a[17] = ABN_hzP1aHZ_Solenoid_Three_Valve;
	save_equip_widgets_hzP1a[18] = ABN_hzP1aHZPistonControlValve;

	save_equip_widgets_hzP1a[19] = ABN_hzP1aHZ_MotorThreeWayOne;
	save_equip_widgets_hzP1a[20] = ABN_hzP1aHZ_MotorThreeWayTwo;
	save_equip_widgets_hzP1a[21] = ABN_hzP1aHZ_MotorThreeWayThree;

	equipment_widget_index_hzP1a = 0;

	ana_panel_des_t st_ana_hzPanel_temp;
	u32_t u32_err = OK;

	//判断打开面板的设备图符是哪个设备图符
	if(PtWidgetIsClass(widget, HZAirOpDamper) != 0)
	{
		PtGetResource(widget, HZ_AOD_COLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[0]), HZ_AOD_COLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_AOD_EQUIPMENTID, &p_result, 0);
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_AIR_OPERATED_DAMPER){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}

		equipment_widget_index_hzP1a = 1;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aAirOpDamper);

		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AOD_DevOrDyn,HZ_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AOD_Q,HZ_AODOPEN,0);/*设置为打开状态*/
		return;
	}

	if(PtWidgetIsClass(widget, HZMtDrvDamper) != 0)
	{
		PtGetResource(widget, HZ_MDD_COLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[1]), HZ_MDD_COLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_MDD_EQUIPMENTID, &p_result, 0);
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_MOTOR_DRIVE_DAMPER){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}

		equipment_widget_index_hzP1a = 2;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aMtDrvDamper);

		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MDD_DevOrDyn,HZ_MDDDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MDD_Q,HZ_MDDOPEN,0);/*设置为打开状态*/
		return;
	}

	if(PtWidgetIsClass(widget, HZMtFireDamper) != 0)
	{
		PtGetResource(widget, HZ_MFD_COLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[2]), HZ_MFD_COLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_MFD_EQUIPMENTID, &p_result, 0);
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_MOTOR_FIRE_DAMPER){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}

		equipment_widget_index_hzP1a = 3;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aMtFireDamper);//hzP2AirOpDamper

		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MFD_DevOrDyn,HZ_MFDDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MFD_Q,HZ_MFDOPEN,0);/*设置为打开状态*/
		return;
	}

	if(PtWidgetIsClass(widget, HZ_AiroperatedControlValve) != 0)
	{
		PtGetResource(widget, HZ_AIRTwo_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[3]), HZ_AIRTwo_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_AIRTwo_SAV_EQUIPMENTID, &p_result, 0);
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_AIR_OPERATED_CONTROL_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}

		equipment_widget_index_hzP1a = 4;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_AiroperatedControlValve);

		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AIRTwo_SAV_DevOrDyn,HZ_AIRTwo_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AIRTwo_SAV_Q,HZ_AIRTwo_AOOPEN,0);/*设置为打开状态*/

		return;
	}

	if(PtWidgetIsClass(widget, HZ_AirOperatedValve) != 0)
	{
		PtGetResource(widget, HZ_AIROne_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[4]), HZ_AIROne_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_AIROne_SAV_EQUIPMENTID, &p_result, 0);
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_AIR_OPERATED_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}

		equipment_widget_index_hzP1a = 5;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_AirOperatedValve);

		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AIROne_SAV_DevOrDyn,HZ_AIROne_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AIROne_SAV_Q,HZ_AIROne_AOOPEN,0);/*设置为打开状态*/

		return;
	}
#if 0
	if(PtWidgetIsClass(widget, HZGroupController) != 0)/*群组连续控制器图符*/
	{
		PtGetResource(widget, HZ_GC_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		if((0 != hzPanel1a_equip_id[indexCount]) && (0 == strlen(p_endstr)))
		{
			PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aEquipmentID), Pt_ARG_TEXT_STRING, p_result, 0);
		}
		else if(0 != hzPanel1a_equip_id[indexCount])
		{
			char num_to_str[VAR_NAME_SIZE_MAX];
			PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aEquipmentID), Pt_ARG_TEXT_STRING, itoa(hzPanel1a_equip_id[indexCount], num_to_str, 10), 0);
		}
		equipment_widget_index_hzP1a = 100;
		//now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP2HZ_PilotoperateOne);
		/*设置流程图上打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILOne_SAV_DevOrDyn,HZ_PILOne_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_GC_Q,HZ_GCOPEN,0);/*设置为打开状态*/
		return;
	}
#endif

	if(PtWidgetIsClass(widget, HZ_MotorDriveValve) != 0)
	{
		PtGetResource(widget, HZ_Motor_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[5]), HZ_Motor_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_Motor_SAV_EQUIPMENTID, &p_result, 0);
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_MOTOR_DRIVEN_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}

		equipment_widget_index_hzP1a = 6;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_MotorDriveValve);

		/*设置流程图上打开面板的设备图符状态*/
		int a = 0;
		a = PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Motor_SAV_DevOrDyn,HZ_Motor_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		int b = 0;
		b = PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Motor_SAV_Q,HZ_Motor_AOOPEN,0);/*设置为打开状态*/
		return;
	}

	if(PtWidgetIsClass(widget, HZ_PilotoperateOne) != 0)
	{
		PtGetResource(widget, HZ_PILOne_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[6]), HZ_PILOne_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_PILOne_SAV_EQUIPMENTID, &p_result, 0);
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_PILOT_OPERATED_SAFETY_VALVE1){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}

		equipment_widget_index_hzP1a = 7;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_PilotoperateOne);

		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILOne_SAV_DevOrDyn,HZ_PILOne_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILOne_SAV_Q,HZ_PILOne_AOOPEN,0);/*设置为打开状态*/

		return;
	}

	if(PtWidgetIsClass(widget, HZ_PilotoperateTwo) != 0)
	{
		PtGetResource(widget, HZ_PILTwo_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[7]), HZ_PILTwo_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_PILTwo_SAV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_PILOT_OPERATED_SAFETY_VALVE2){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}
		equipment_widget_index_hzP1a = 8;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_PilotoperateTwo);
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILTwo_SAV_DevOrDyn,HZ_PILTwo_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILTwo_SAV_Q,HZ_PILTwo_AOOPEN,0);/*设置为打开状态*/
		return;
	}

	if(PtWidgetIsClass(widget, HZ_PistonDrivenValve) != 0)
	{
		PtGetResource(widget, HZ_Piston_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[8]), HZ_Piston_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_Piston_SAV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_PISTON_DRIVEN_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}
		equipment_widget_index_hzP1a = 9;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_PistonDrivenValve);
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Piston_SAV_DevOrDyn,HZ_Piston_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Piston_SAV_Q,HZ_Piston_AOOPEN,0);/*设置为打开状态*/
		return;
	}

	if(PtWidgetIsClass(widget, HZ_SafetyValveOne) != 0)
	{
		PtGetResource(widget, HZ_SAFE_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[9]), HZ_SAFE_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_SAFE_SAV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_SAFETY_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}
		equipment_widget_index_hzP1a = 10;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_SafetyValveOne);
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SAFE_SAV_DevOrDyn,HZ_SAFE_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SAFE_SAV_Q,HZ_SAFE_AOOPEN,0);/*设置为打开状态*/
		return;
	}

	if(PtWidgetIsClass(widget, HZ_SolenoidControlValve) != 0)
	{
		PtGetResource(widget, HZ_SOLTwo_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[10]), HZ_SOLTwo_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_SOLTwo_SAV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_SOLENOID_CONTROL_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}
		equipment_widget_index_hzP1a = 11;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_SolenoidControlValve);
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SOLTwo_SAV_DevOrDyn,HZ_SOLTwo_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SOLTwo_SAV_Q,HZ_SOLTwo_AOOPEN,0);/*设置为打开状态*/
		return;
	}

	if(PtWidgetIsClass(widget, HZ_SolenoidDriveValve) != 0)
	{
		PtGetResource(widget, HZ_SOLOne_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[11]), HZ_SOLOne_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_SOLOne_SAV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_SOLENOID_DRIVE_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}
		equipment_widget_index_hzP1a = 12;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_SolenoidDriveValve);
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SOLOne_SAV_DevOrDyn,HZ_SOLOne_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SOLOne_SAV_Q,HZ_SOLOne_AOOPEN,0);/*设置为打开状态*/
		return;
	}

	if(PtWidgetIsClass(widget, HZ_MotorDriveTreeWayValve) != 0)
	{
		PtGetResource(widget, HZ_MotorOne_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[12]), HZ_MotorOne_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_MotorOne_SAV_EQUIPMENTID, &p_result, 0);
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_MOTOR_DEIVE_THREE_WAY_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}
		equipment_widget_index_hzP1a = 13;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_MotorDriveTreeWayValve);
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorOne_SAV_DevOrDyn,HZ_MotorOne_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorOne_SAV_Q,HZ_MotorOne_AOOPEN,0);/*设置为打开状态*/
		return;
	}

	if(PtWidgetIsClass(widget, HZ_VacuumBreakingValve) != 0)
	{
		PtGetResource(widget, HZ_VAC_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[13]), HZ_VAC_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_VAC_SAV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_VACUUM_BRAKING_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}
		equipment_widget_index_hzP1a = 14;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_VacuumBreakingValve);
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_VAC_SAV_DevOrDyn,HZ_VAC_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_VAC_SAV_Q,HZ_VAC_AOOPEN,0);/*设置为打开状态*/

		return;
	}

	if(PtWidgetIsClass(widget, HZ_MotorControlValve) != 0)
	{
		PtGetResource(widget, HZ_MotorTwo_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[14]), HZ_MotorTwo_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_MotorTwo_SAV_EQUIPMENTID, &p_result, 0);
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_MOTOR_CONTROL_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}
		equipment_widget_index_hzP1a = 15;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_MotorControlValve);
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorTwo_SAV_DevOrDyn,HZ_MotorTwo_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorTwo_SAV_Q,HZ_MotorTwo_AOOPEN,0);/*设置为打开状态*/

		return;
	}

	if(PtWidgetIsClass(widget, HZ_MotorDriveTreeWayControlValve) != 0)
	{
		PtGetResource(widget, HZ_MotorThree_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[15]), HZ_MotorThree_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_MotorThree_SAV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_MOTOR_DRIVE_THREE_WAY_CONTROL_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}
		equipment_widget_index_hzP1a = 16;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_MotorDriveTreeWayControlValve);
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThree_SAV_DevOrDyn,HZ_MotorThree_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThree_SAV_Q,HZ_MotorThree_AOOPEN,0);/*设置为打开状态*/

		return;
	}

	if(PtWidgetIsClass(widget, HZManualValve) != 0)
	{
		PtGetResource(widget, HZManualValve_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[16]), HZManualValve_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZManualValve_SAV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_MANUAL_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}
		equipment_widget_index_hzP1a = 17;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZManualValve);
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZManualValve_SAV_DevOrDyn, HZManualValve_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZManualValve_SAV_Q, HZManualValve_AOOPEN,0);/*设置为打开状态*/

		return;
	}

	if(PtWidgetIsClass(widget, HZ_Solenoid_Three_Valve) != 0)
	{
		PtGetResource(widget, HZ_SolenoidThree_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[17]), HZ_SolenoidThree_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZ_SolenoidThree_SAV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_SOLENOID_THREE_WAY_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}
		equipment_widget_index_hzP1a = 18;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_Solenoid_Three_Valve);
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SolenoidThree_SAV_DevOrDyn, HZ_SolenoidThree_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SolenoidThree_SAV_Q, HZ_SolenoidThree_AOOPEN,0);/*设置为打开状态*/

		return;
	}

	if(PtWidgetIsClass(widget, HZPistonControlValve) != 0)
	{
		PtGetResource(widget, HZPistonControlValve_SAV_FILLCOLOR, &u32_wgt_color, 0);
		if (P_NULL != u32_wgt_color){
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[18]), HZPistonControlValve_SAV_FILLCOLOR, *u32_wgt_color, 0);
		}

		PtGetResource(widget, HZPistonControlValve_SAV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
		if(OK != u32_err)
		{
			photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
			return;
		}
		if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL && st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_PISTON_CONTROL_VALVE){
			photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
			return;
		}
		equipment_widget_index_hzP1a = 19;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZPistonControlValve);
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZPistonControlValve_SAV_DevOrDyn, HZPistonControlValve_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZPistonControlValve_SAV_Q, HZPistonControlValve_AOOPEN,0);/*设置为打开状态*/

		return;
	}

	if(PtWidgetIsClass(widget, HZGroupController) != 0)/*群组连续控制器图符*/
	{
		PtGetResource(widget, HZ_GC_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		equipment_widget_index_hzP1a = 100;
		now_equipmentWidget_hzP1a[indexCount] = P_NULL;
		/*设置流程图上打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILOne_SAV_DevOrDyn,HZ_PILOne_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_GC_Q,HZ_GCOPEN,0);/*设置为打开状态*/
		return;
	}

	if(PtWidgetIsClass(widget, HZSetValue) != 0)/*设定值图符*/
	{
		PtGetResource(widget, HZ_SV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
		if(P_NULL == p_result)
		{
			hzPanel1a_equip_id[indexCount] = 0;
			equipment_widget_index_hzP1a = 0;
			photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
			return;
		}
		else
		{
			hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
		}

		equipment_widget_index_hzP1a = 100;
		now_equipmentWidget_hzP1a[indexCount] = P_NULL;
		/*设置流程图上打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILOne_SAV_DevOrDyn,HZ_PILOne_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SV_Q, HZ_SVOPEN, 0);/*设置为打开状态*/
		return;
	}

	// ljx
		if(PtWidgetIsClass(widget, HZ_MotorThreeWayValve) != 0)
		{
			// DEV_TYPE_MOTOR_THREE_WAY_VALVE

			PtGetResource(widget, HZ_MotorThreeWayOne_SAV_FILLCOLOR, &u32_wgt_color, 0);
			if (P_NULL != u32_wgt_color){
				PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[19]), HZ_MotorThreeWayOne_SAV_FILLCOLOR, *u32_wgt_color, 0);
			}

			PtGetResource(widget, HZ_MotorThreeWayOne_SAV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
			if(P_NULL == p_result)
			{
				hzPanel1a_equip_id[indexCount] = 0;
				equipment_widget_index_hzP1a = 0;
				photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
				return;
			}
			else
			{
				hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
			}

			u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel1a configuration */
			if(OK != u32_err)
			{
				photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
				return;
			}
			if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL &&
				(st_ana_hzPanel_temp.u32_dev_type < DEV_TYPE_MOTOR_THREE_WAY_VALVE_LR || st_ana_hzPanel_temp.u32_dev_type > DEV_TYPE_MOTOR_THREE_WAY_VALVE_UD)) {
				photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
				return;
			}
			equipment_widget_index_hzP1a = 20;
			now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window,  ABN_hzP1aHZ_MotorThreeWayOne);
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ), HZ_MotorThreeWayOne_SAV_DevOrDyn, HZ_MotorThreeWayOne_AODDyn,0);/*设置为动态状态*//*hzP2_hzP2_widgetEquipPanel[windowEquipPanel]*/
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ), HZ_MotorThreeWayOne_SAV_Q, HZ_MotorThreeWayOne_AOOPEN,0);/*设置为打开状态*/
		    return;

		}
		if(PtWidgetIsClass(widget, HZ_MotorThreeWayControlValve) != 0)
		{
			//DEV_TYPE_MOTOR_THREE_WAY_CONTROL_VALVE
			PtGetResource(widget, HZ_MotorThreeWayTwo_SAV_FILLCOLOR, &u32_wgt_color, 0);
			if (P_NULL != u32_wgt_color){
				PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[20]), HZ_MotorThreeWayTwo_SAV_FILLCOLOR, *u32_wgt_color, 0);
			}

			PtGetResource(widget, HZ_MotorThreeWayTwo_SAV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
			if(P_NULL == p_result)
			{
				hzPanel1a_equip_id[indexCount] = 0;
				equipment_widget_index_hzP1a = 0;
				photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
				return;
			}
			else
			{
				hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
			}

			u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel7 configuration */
			if(OK != u32_err)
			{
				photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
				return;
			}
			if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL &&
				(st_ana_hzPanel_temp.u32_dev_type < DEV_TYPE_MOTOR_THREE_WAY_CONTROL_VALVE_LR || st_ana_hzPanel_temp.u32_dev_type > DEV_TYPE_MOTOR_THREE_WAY_CONTROL_VALVE_UD)) {
				photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
				return;
			}
			equipment_widget_index_hzP1a = 21;
			now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window,  ABN_hzP1aHZ_MotorThreeWayTwo);
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ), HZ_MotorThreeWayTwo_SAV_DevOrDyn, HZ_MotorThreeWayTwo_AODDyn,0);/*设置为动态状态*//*hzP2_hzP2_widgetEquipPanel[windowEquipPanel]*/
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ), HZ_MotorThreeWayTwo_SAV_Q, HZ_MotorThreeWayTwo_AOOPEN,0);/*设置为打开状态*/
		    return;

		}
		if(PtWidgetIsClass(widget, HZ_SolenoidThreeWayValve) != 0)
		{
			//DEV_TYPE_MOTOR_SOLENOID_THREE_WAY_CONTROL_VALVE
			PtGetResource(widget, HZ_MotorThreeWayThree_SAV_FILLCOLOR, &u32_wgt_color, 0);
			if (P_NULL != u32_wgt_color){
				PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[21]), HZ_MotorThreeWayThree_SAV_FILLCOLOR, *u32_wgt_color, 0);
			}

			PtGetResource(widget, HZ_MotorThreeWayThree_SAV_EQUIPMENTID, &p_result, 0);/*获得设备ID属性值*/
			if(P_NULL == p_result)
			{
				hzPanel1a_equip_id[indexCount] = 0;
				equipment_widget_index_hzP1a = 0;
				photon_show_msg(MSG_WIDGET_WITH_NO_EQUIPID);
				return;
			}
			else
			{
				hzPanel1a_equip_id[indexCount] = strtol(p_result, &p_endstr, 10);
			}

			u32_err = photon_get_ana_panel_info(&st_ana_hzPanel_temp, hzPanel1a_equip_id[indexCount]);  /* get the information of the hzPanel7 configuration */
			if(OK != u32_err)
			{
				photon_show_msg(MSG_EQUIPID_NOT_IN_SETTING_FILES);
				return;
			}
			if (st_ana_hzPanel_temp.u32_dev_type != DEV_TYPE_NULL &&
				(st_ana_hzPanel_temp.u32_dev_type < DEV_TYPE_MOTOR_SOLENOID_THREE_WAY_CONTROL_VALVE_LR || st_ana_hzPanel_temp.u32_dev_type > DEV_TYPE_MOTOR_SOLENOID_THREE_WAY_CONTROL_VALVE_UD)) {
				photon_show_msg(MSG_WIDGET_DIFFERENT_WITH_SYMBOL_IN_SETTING_FILE);
				return;
			}
			equipment_widget_index_hzP1a = 22;
			now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window,  ABN_hzP1aHZ_MotorThreeWayThree);
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ), HZ_MotorThreeWayThree_SAV_DevOrDyn, HZ_MotorThreeWayThree_AODDyn,0);/*设置为动态状态*//*hzP2_hzP2_widgetEquipPanel[windowEquipPanel]*/
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ), HZ_MotorThreeWayThree_SAV_Q, HZ_MotorThreeWayThree_AOOPEN,0);/*设置为打开状态*/
		    return;

		}

}

static void set_hzPanel1a_ts_rs_state(PtWidget_t * window, PtWidget_t * widget, u32_t state, u32_t indexCount)
{
	if(PtWidgetIsClass(widget, HZAirOpDamper) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[0]), HZ_AOD_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_AOD_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZMtDrvDamper) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[1]), HZ_MDD_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MDD_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZMtFireDamper) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[2]), HZ_MFD_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MFD_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_AiroperatedControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[3]), HZ_AIRTwo_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_AIRTwo_SAV_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_AirOperatedValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[4]), HZ_AIROne_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_AIROne_SAV_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorDriveValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[5]), HZ_Motor_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_Motor_SAV_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_PilotoperateOne) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[6]), HZ_PILOne_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_PILOne_SAV_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_PilotoperateTwo) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[7]), HZ_PILTwo_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_PILTwo_SAV_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_PistonDrivenValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[8]), HZ_Piston_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_Piston_SAV_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SafetyValveOne) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[9]), HZ_SAFE_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_SAFE_SAV_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SolenoidControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[10]), HZ_SOLTwo_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_SOLTwo_SAV_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SolenoidDriveValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[11]), HZ_SOLOne_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_SOLOne_SAV_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorDriveTreeWayValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[12]), HZ_MotorOne_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorOne_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_VacuumBreakingValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[13]), HZ_VAC_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_VAC_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[14]), HZ_MotorTwo_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorTwo_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorDriveTreeWayControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[15]), HZ_MotorThree_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorThree_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZManualValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[16]), HZManualValve_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorThree_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_Solenoid_Three_Valve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[17]), HZ_SolenoidThree_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorThree_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZPistonControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[18]), HZPistonControlValve_SAV_TSRS, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorThree_SAV_RUNSTATE, state, 0);
		return;
	}
	/*20240801wkt:tsrs show*/
	if(PtWidgetIsClass(widget, HZ_MotorThreeWayValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[19]), HZ_MotorThreeWayOne_SAV_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorThreeWayControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[20]), HZ_MotorThreeWayTwo_SAV_TSRS, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SolenoidThreeWayValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[21]), HZ_MotorThreeWayThree_SAV_TSRS, state, 0);
		return;
	}
#if 0
#endif

}

/*New:设置质量位*/
static void set_hzPanel1a_Q_State(PtWidget_t * window, PtWidget_t * widget, u32_t state, u32_t indexCount)
{
	if(PtWidgetIsClass(widget, HZAirOpDamper) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[0]), HZ_AOD_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_AOD_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZMtDrvDamper) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[1]), HZ_MDD_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MDD_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZMtFireDamper) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[2]), HZ_MFD_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MFD_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_AiroperatedControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[3]), HZ_AIRTwo_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_AIRTwo_SAV_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_AirOperatedValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[4]), HZ_AIROne_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_AIROne_SAV_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorDriveValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[5]), HZ_Motor_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_Motor_SAV_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_PilotoperateOne) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[6]), HZ_PILOne_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_PILOne_SAV_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_PilotoperateTwo) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[7]), HZ_PILTwo_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_PILTwo_SAV_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_PistonDrivenValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[8]), HZ_Piston_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_Piston_SAV_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SafetyValveOne) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[9]), HZ_SAFE_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_SAFE_SAV_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SolenoidControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[10]), HZ_SOLTwo_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_SOLTwo_SAV_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SolenoidDriveValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[11]), HZ_SOLOne_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_SOLOne_SAV_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorDriveTreeWayValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[12]), HZ_MotorOne_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorOne_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_VacuumBreakingValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[13]), HZ_VAC_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_VAC_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[14]), HZ_MotorTwo_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorTwo_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorDriveTreeWayControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[15]), HZ_MotorThree_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorThree_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZManualValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[16]), HZManualValve_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorThree_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_Solenoid_Three_Valve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[17]), HZ_SolenoidThree_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorThree_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZPistonControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[18]), HZPistonControlValve_SAV_Q, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorThree_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorThreeWayValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[19]), HZ_MotorThreeWayOne_SAV_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorThreeWayControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[20]), HZ_MotorThreeWayTwo_SAV_Q, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SolenoidThreeWayValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[21]), HZ_MotorThreeWayThree_SAV_Q, state, 0);
		return;
	}

#if 0
#endif

}


/*modifty:改为新的设备图符*/
static void set_equip_status_hzPanel1a(PtWidget_t *window, PtWidget_t *widget, u32_t state, u32_t indexCount)
{
	/*挡板类动态图符*/
	if(PtWidgetIsClass(widget, HZAirOpDamper) != 0)
	{
		if(state == MIDDLE1a)/*中间态*/
		{
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[0]), HZ_AOD_RUNSTATE, UNDEFINED1a, 0);
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[0]), HZ_AOD_Q, Q1a, 0);
			/*设置流程图打开面板的设备图符状态*/
			//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_AOD_RUNSTATE, UNDEFINED1a, 0);
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[0]), HZ_AOD_RUNSTATE, state, 0);
			/*设置流程图打开面板的设备图符状态*/
			//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_AOD_RUNSTATE, state, 0);
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZMtDrvDamper) != 0)
	{
		if(state == MIDDLE1a)/*中间态*/
		{
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[1]), HZ_MDD_RUNSTATE, UNDEFINED1a, 0);
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[1]), HZ_MDD_Q, Q1a, 0);
			/*设置流程图打开面板的设备图符状态*/
			//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MDD_RUNSTATE, UNDEFINED1a, 0);
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[1]), HZ_MDD_RUNSTATE, state, 0);
			/*设置流程图打开面板的设备图符状态*/
			//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MDD_RUNSTATE, state, 0);
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZMtFireDamper) != 0)
	{
		if(state == MIDDLE1a)/*中间态*/
		{
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[2]), HZ_MFD_RUNSTATE, UNDEFINED1a, 0);
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[2]), HZ_MFD_Q, Q1a, 0);
			/*设置流程图打开面板的设备图符状态*/
			//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MFD_RUNSTATE, UNDEFINED1a, 0);
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[2]), HZ_MFD_RUNSTATE, state, 0);
			/*设置流程图打开面板的设备图符状态*/
			//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MFD_RUNSTATE, state, 0);
		}
		return;
	}
	/*阀门类动态图符*/
	if(PtWidgetIsClass(widget, HZ_AiroperatedControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[3]), HZ_AIRTwo_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_AIRTwo_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_AirOperatedValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[4]), HZ_AIROne_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_AIROne_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorDriveValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[5]), HZ_Motor_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_Motor_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_PilotoperateOne) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[6]), HZ_PILOne_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_PILOne_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_PilotoperateTwo) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[7]), HZ_PILTwo_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_PILTwo_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_PistonDrivenValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[8]), HZ_Piston_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_Piston_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SafetyValveOne) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[9]), HZ_SAFE_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_SAFE_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SolenoidControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[10]), HZ_SOLTwo_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_SOLTwo_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SolenoidDriveValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[11]), HZ_SOLOne_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_SOLOne_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorDriveTreeWayValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[12]), HZ_MotorOne_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorOne_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_VacuumBreakingValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[13]), HZ_VAC_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_VAC_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[14]), HZ_MotorTwo_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorTwo_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorDriveTreeWayControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[15]), HZ_MotorThree_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorThree_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZManualValve) != 0)
	{
		if(state == MIDDLE1a)/*中间态*/
		{
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[16]), HZManualValve_SAV_RUNSTATE, UNDEFINED1a, 0);
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[16]), HZManualValve_SAV_Q, Q1a, 0);
			/*设置流程图打开面板的设备图符状态*/
			//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MFD_RUNSTATE, UNDEFINED1a, 0);
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[16]), HZManualValve_SAV_RUNSTATE, state, 0);
			/*设置流程图打开面板的设备图符状态*/
			//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MFD_RUNSTATE, state, 0);
		}
		//PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[16]), HZManualValve_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorThree_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_Solenoid_Three_Valve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[17]), HZ_SolenoidThree_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorThree_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZPistonControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[18]), HZPistonControlValve_SAV_RUNSTATE, state, 0);
		/*设置流程图打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( thisFlowChart, ApName(widgetEquipPanel[indexCount])), HZ_MotorThree_SAV_RUNSTATE, state, 0);
		return;
	}


	// ljx
	if(PtWidgetIsClass(widget, HZ_MotorThreeWayValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[19]), HZ_MotorThreeWayOne_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorThreeWayControlValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[20]), HZ_MotorThreeWayTwo_SAV_RUNSTATE, state, 0);
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SolenoidThreeWayValve) != 0)
	{
		PtSetResource(ApGetWidgetPtr( window, save_equip_widgets_hzP1a[21]), HZ_MotorThreeWayThree_SAV_RUNSTATE, state, 0);
		return;
	}
}

/*get the texts of the hzPanel1a configuration:设置设备名称(激活按钮的文本)、设备ID、模拟量单位、AUTO/MANU按钮文本、FAST/SLOW按钮文本*/
static void set_hzPanel1a_equipmentName( PtWidget_t* window/*, PtWidget_t *widget*/)
{
	char_t *ch_panel_name_text = P_NULL;
	char_t *p_PV_unit_text = P_NULL; /*the unit of PV*/
	char_t *p_SV_uint_text = P_NULL; /*the unit of SV*/
	char_t *p_LV_unit_text = P_NULL; /*the unit of LV*/
	char_t *p_increase_btn_text = P_NULL; /*the text of increase button*/
	char_t *p_decrease_btn_text = P_NULL; /*the text of decrease button*/
	char_t *p_execute_text = P_NULL;	/*wkt20240822*/
	char_t *p_exit_text = P_NULL;
	u32_t u32_err = OK;

	/*对入参判断处理*/
	if(P_NULL == window)
	{
		return/*( Pt_END )*/;
	}

	/*the text of ActivateButton widget*/
	ch_panel_name_text = (char *) malloc(3*TEXT_MAX_PANEL_NAME);
	if(P_NULL != ch_panel_name_text)
	{
		memset(ch_panel_name_text, UINT_0, 3*TEXT_MAX_PANEL_NAME);
		if(strlen(p_st_ana_hzPanel1a.a_ch_panel_name1) > 0)
		{
			strcpy(ch_panel_name_text, p_st_ana_hzPanel1a.a_ch_panel_name1);
		}
		if(strlen(p_st_ana_hzPanel1a.a_ch_panel_name2) > 0)
		{
			strcat(ch_panel_name_text, "\n");
			strcat(ch_panel_name_text, p_st_ana_hzPanel1a.a_ch_panel_name2);
		}
		if(strlen( p_st_ana_hzPanel1a.a_ch_panel_name3) > 0)
		{
			strcat(ch_panel_name_text, "\n");
			strcat(ch_panel_name_text, p_st_ana_hzPanel1a.a_ch_panel_name3);
		}
		PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aActivateButton), Pt_ARG_TEXT_STRING, ch_panel_name_text, 0);
	}
	else
	{
		/*do nothing*/
	}

	/*the text of EquipmentID widget*/
#if 0
	if(0 == p_st_ana_hzPanel1a.u32_dev_id)/*设备ID未组态???*/
	{
		PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aEquipmentID), Pt_ARG_TEXT_STRING, p_result, 0);
	}
	else
#endif
	{
		char num_to_str[VAR_NAME_SIZE_MAX];
		//PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aEquipmentID), Pt_ARG_TEXT_STRING, itoa(p_st_ana_hzPanel1a.u32_dev_id, num_to_str, 10), 0);
		PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aEquipmentID), Pt_ARG_TEXT_STRING, p_st_ana_hzPanel1a.a_ch_panel_id, 0);
	}

	/*the text of the PV unit*/
	u32_err = replace_special_character_hzp1a(p_st_ana_hzPanel1a.a_ch_pv_unit);
	p_PV_unit_text = p_st_ana_hzPanel1a.a_ch_pv_unit;
	/*if( P_NULL != (pch_ret = strstr( p_st_ana_hzPanel1a.a_ch_pv_unit, M3)))
	{
		str_right_shift( pch_ret, UINT_2);//特殊字符占3格字节，需向后移动一位
		memcpy( pch_ret, M_CUBE, strlen(M_CUBE));//若找到m3,替换为m³
	}*/

	/*the text of the SV unit*/
	u32_err = replace_special_character_hzp1a(p_st_ana_hzPanel1a.a_ch_sv_unit);
	p_SV_uint_text = p_st_ana_hzPanel1a.a_ch_sv_unit;

	/*the text of the LV unit*/
	u32_err = replace_special_character_hzp1a(p_st_ana_hzPanel1a.a_ch_lv_unit);
	p_LV_unit_text = p_st_ana_hzPanel1a.a_ch_lv_unit;
	p_execute_text = p_st_ana_hzPanel1a.a_btnExecute_text;		/*wkt20240821*/
	p_exit_text = p_st_ana_hzPanel1a.a_btnExit_text;
	/*the text of AUTO/MANU button*/
	/*if(P_NULL != p_st_ana_hzPanel1a.a_ch_am_text)
	{
		p_auto_manu_btn_text = p_st_ana_hzPanel1a.a_ch_am_text;
	}*/
	/*the text of FAST/SLOW button*/
	/*if(P_NULL != p_st_ana_hzPanel1a.a_ch_fs_text)
	{
		p_fast_slow_btn_text = p_st_ana_hzPanel1a.a_ch_fs_text;
	}*/
	PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aPtLabelPVUnit), Pt_ARG_TEXT_STRING, p_PV_unit_text, 0);
	PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aPtLabelSVUnit), Pt_ARG_TEXT_STRING, p_SV_uint_text, 0);
	PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aPtLabelDVUnit), Pt_ARG_TEXT_STRING, p_LV_unit_text, 0);
	PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aPtLabelLVUnit), Pt_ARG_TEXT_STRING, p_LV_unit_text, 0);
	PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aPtLabelMVUnit), Pt_ARG_TEXT_STRING, p_LV_unit_text, 0);
	/*
	PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aFastSlowBtn), Pt_ARG_TEXT_STRING, p_fast_slow_btn_text, 0);
	PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aAutoManuBtn), Pt_ARG_TEXT_STRING, p_auto_manu_btn_text, 0);
	*/

	/*增减按钮文本*/
	p_increase_btn_text = UPPER_TRIANGULAR;
	p_decrease_btn_text = LOWER_TRIANGULAR;
	PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aMViBtn), Pt_ARG_TEXT_STRING, p_increase_btn_text, 0);
	PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aMVdBtn), Pt_ARG_TEXT_STRING, p_decrease_btn_text, 0);

	PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aExecuteBtn), Pt_ARG_TEXT_STRING, p_execute_text, 0);		/*wkt20240821*/
	PtSetResource(ApGetWidgetPtr(window, ABN_hzP1aExitBtn), Pt_ARG_TEXT_STRING, p_exit_text, 0);
	if(P_NULL != ch_panel_name_text)
	{
		free(ch_panel_name_text);/*释放内存*/
	}

	//return(Pt_CONTINUE);
}


/*设置面板控件的显示和隐藏（包括设备图符）*/
static void set_widget_show_hide_hzPanel1a(PtWidget_t * link_instance/*, PtWidget_t *widget*/, u32_t indexCount)
{
	PhPoint_t offset_hide = {2000, 2000};
	//u8_t *show_flag = P_NULL;

	/*设备ID的显示状态（不隐藏，一直显示）*/ /*here:is a problem
	PtGetResource(widget, PO_EQUIPMENTID_SHOW, &show_flag, sizeof(uchar_t));
	if(HIDE2 == *show_flag)
	{
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP2EquipmentID), Pt_ARG_POS, &offset_hide, 0);
	}
	*/
	if(0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM])
	{
		/*AUTO/MANU按钮以及AUTO/MANU标签的hide状态*/
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aAutoManuBtn), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aAuto), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aManu), Pt_ARG_POS, &offset_hide, 0);
#if 0
		if((equipment_widget_index_hzP1a == 100) && (PtWidgetIsClass(widgetEquipPanel[indexCount], HZGroupController) != 0))/*群组连续控制器图符*/
		{
			PtSetResource(widgetEquipPanel[indexCount],HZ_GC_STATE,HZ_GCUNDEFINED,0);/*AUTO/MANU未绑点，则为未定义状态*/
		}
#endif
	}
	if(0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 2])
	{
		/*FAST/SLOW按钮以及FAST/SLOW标签的hide状态*/
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aFastSlowBtn), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aFast), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aSlow), Pt_ARG_POS, &offset_hide, 0);
	}
	if(0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 3])
	{
		/*PV的hide状态*/
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aAnalogDisplayPV), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aPtLabelPV), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aPtLabelPVUnit), Pt_ARG_POS, &offset_hide, 0);
	}
	if(0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 4])
	{
		/*SV的hide状态*/
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aAnalogDisplaySV), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aPtLabelSV), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aPtLabelSVUnit), Pt_ARG_POS, &offset_hide, 0);
	}
	if(0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 5])
	{
		/*DV的hide状态*/
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aAnalogDisplayDV), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aPtLabelDV), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aPtLabelDVUnit), Pt_ARG_POS, &offset_hide, 0);
	}
	if(0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 6])
	{
		/*LV的hide状态*/
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aAnalogDisplayLV), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aPtLabelLV), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aPtLabelLVUnit), Pt_ARG_POS, &offset_hide, 0);
	}
	if(0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 7])
	{
		/*MV的hide状态*/
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aAnalogDisplayMV), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aPtLabelMV), Pt_ARG_POS, &offset_hide, 0);
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aPtLabelMVUnit), Pt_ARG_POS, &offset_hide, 0);
	}
	if(0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 8])
	{
		/*故障信息UNAVA的hide状态*/
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aUnava), Pt_ARG_POS, &offset_hide, 0);
	}
	if(0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 9])
	{
		/*故障信息FBER的hide状态*/
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aFber), Pt_ARG_POS, &offset_hide, 0);
	}
	if(0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 10] || 0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 11])
	{
		/*设备图符的hide状态*/
		if(equipment_widget_index_hzP1a >= 1 && equipment_widget_index_hzP1a <= 16)/*面板上的设备图符*/
		{
			PtSetResource(ApGetWidgetPtr(link_instance, save_equip_widgets_hzP1a[equipment_widget_index_hzP1a - 1]), Pt_ARG_POS, &offset_hide, 0);
		}
	}
	if(0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 14])
	{
		/*MV/LV增按钮的hide状态*/
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aMViBtn), Pt_ARG_POS, &offset_hide, 0);
	}
	if(0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 15])
	{
		/*MV/LV减按钮的hide状态*/
		PtSetResource(ApGetWidgetPtr(link_instance, ABN_hzP1aMVdBtn), Pt_ARG_POS, &offset_hide, 0);
	}
#if 0
	if((0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 14]) &&
	   (0 == PanelPName_NO[indexCount*PANELPNAME_REQUEST_NUM + 15]))
	{
		/*MV/LV增、减按钮的hide状态*/
		if((equipment_widget_index_hzP1a == 100) && (PtWidgetIsClass(widgetEquipPanel[indexCount], HZSetValue) != 0))/*设定值图符*/
		{
			PtSetResource(widgetEquipPanel[indexCount],HZ_SV_STATE,HZ_SVUNDEFINED,0);/*MV/LV增、减按钮未绑点，则为未定义状态*/
		}
	}
#endif
}
/*设置设备图符的显示隐藏*/
static void set_hzPanel1a_quipment_hide(PtWidget_t * link_instance/*, PtWidget_t *widget*/)
{
	int i;
	PhPoint_t offset_hide;
	offset_hide.x = 2000;
	offset_hide.y = 2000;
	for(i = 0; i < 30; i++)
	{
		if(i != (equipment_widget_index_hzP1a - 1) && save_equip_widgets_hzP1a[i] != 0)
		{
			int widgetID = save_equip_widgets_hzP1a[i];
			if(widgetID != 0)
			{
				PtWidget_t *equipmentWidget = ApGetWidgetPtr(link_instance, widgetID);
				if(P_NULL != equipmentWidget)
				{
					PtSetResource(equipmentWidget, Pt_ARG_POS, &offset_hide, 0);
				}
				else
				{
                 /*P_NULL*/
				}
			}
			else
			{

			}
		}
	}
}
/*
static int count_open_hzPanel2_times()
{
	if( 0 == open_times )
	{
		int i;
		for( i = 0; i < 4; i++ )
		{
			memset(high_text2[i], 0, 256);
			memset(low_text2[i], 0, 256);
		}

	}
	return open_times;
}
*/

int
hzPanel1a_init( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	u32_t  indexPNames = 0;
	PtWidget_t *thisWindow;
	thisWindow = ApGetInstance( widget );/* 获取面板窗口实例 */
	u32_t  i;

	if(	trace_hzPanel1a_open_mode == NC_SCREEN_PANEL1A )
	{
		indexPNames = SET_UP_NC_SCREEN;
	}
	else
	{/*FLOW_CHART_1a*/
		for ( i = 0; i < max_panel_num; i++ )
		{
			if ( windowEquipPanel[i] == thisWindow )
			{
				indexPNames = i;
				break;
			}
		}
	}

	/* 初始化按钮追踪状态 */
	trace_hzPanel1a_auto_manu_btn[indexPNames] = -1;

	trace_hzPanel1a_fast_slow_mode[indexPNames] = NOFASTSLOW1a;
	trace_hzPanel1a_auto_manu_mode[indexPNames] = NOAUTOMANU1a;
	time_hzPanel1a_des_add_start_time = 0;
	trace_hzPanels_ms_state[indexPNames] = 0;

	/* 设置激活按钮初始显示状态 */
	PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aActivateButton ),AB_STATE,AB_UP,0);
	PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aActivateButton ),AB_COLOR,PgRGB(0,0,0),0);
	/* 初始化操作按钮 */
	panelBtnGUI( ApGetWidgetPtr( thisWindow, ABN_hzP1aAutoManuBtn ) );
	panelBtnGUI( ApGetWidgetPtr( thisWindow, ABN_hzP1aFastSlowBtn ) );
	panelBtnGUI( ApGetWidgetPtr( thisWindow, ABN_hzP1aMViBtn ) );
	panelBtnGUI( ApGetWidgetPtr( thisWindow, ABN_hzP1aMVdBtn ) );
	panelBtnGUI( ApGetWidgetPtr( thisWindow, ABN_hzP1aExecuteBtn ) );
	/* 设置面板填充色 */
	PtSetResource(thisWindow, Pt_ARG_FILL_COLOR, PgRGB(191,191,191), 0);
	PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aPtRect ),Pt_ARG_LINE_WIDTH,1,0);
	PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aPtRect ),Pt_ARG_COLOR,PgRGB(0,0,0),0);

	/* 如果存在没有组的变量，则中断 */
	/*if ( PanelPName_NO[indexPNames] == 1 )
	{

		u32_ret = photon_show_msg("变量组态缺少!");
		PanelPName_NO[indexPNames] = 0;
		return( Pt_CONTINUE );
		return ( Pt_END );
	}*/

	for ( i = 0; i < PANEL_VAR_SIZE_MAX; i++ )
	{
		memset(&ack_hzPanel1a_data[indexPNames*PANEL_VAR_SIZE_MAX + i],UINT_0, sizeof(var_recv_des_t));
	}
	for ( i = 0; i < Input_PanelPName_Num[indexPNames]; i++ )
	{
		memcpy( &ack_hzPanel1a_data[indexPNames*PANEL_VAR_SIZE_MAX + i], &mq_cyc_recv_data.mq_recv_ddata.var_recv_buf[u32_recv_ddata_index[indexPNames] + i],
						sizeof(var_recv_des_t));
	}

	//change_widgetState_hzP1a( thisWindow, indexPNames, Input_PanelPName_Num[indexPNames] );/* 处理变量反馈 */

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}


int
start_dragging_hzP1a_win( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PhDim_t *dimension;
	PhRect_t rect;
	PhRect_t boundary;
	PhPointerEvent_t *cursor_event_data = P_NULL;
	PhPoint_t start_pos = {0};

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	/* Set the dragging rectangle to the position and size of
	   the widget being dragged. */
	cursor_event_data = (PhPointerEvent_t *)PhGetData(cbinfo->event);             /* get event data */
	start_pos.x = cursor_event_data->pos.x;
	start_pos.y = cursor_event_data->pos.y;

	PtWidgetExtent (widget, &rect);//Get a widget's extent
	rect.ul.x += start_pos.x;
	rect.lr.x += start_pos.x;
	rect.ul.y += start_pos.y;
	rect.lr.y += start_pos.y;
	rect.ul.y -= 80;
	rect.lr.y -= 80;

	//dimension->h = 0;
	//dimension->w = 0;
	//printf ("Start position: (%d, %d)\n", rect.ul.x, rect.ul.y);
	//printf ("Start position: (%d, %d)\n", rect.lr.x, rect.lr.y);

	/* Set the boundary for dragging to the boundary of the
	   window. */
	PtGetResource (thisFlowChart, Pt_ARG_DIM, &dimension, 0);
	boundary.ul.x = 0;
	boundary.ul.y = 0;
	boundary.lr.x = dimension->w - 1;//200
	boundary.lr.y = dimension->h - 1;//Rectangular area that limits the dragging

	/* Initiate outline dragging (Ph_DRAG_TRACK isn't
	   specified). */
	PhInitDrag (PtWidgetRid (thisFlowChart),//Get a widget's region ID
			    Ph_TRACK_DRAG | Ph_DRAG_TRACK,//Ph_TRACK_DRAG//Ph_TRACK_TOP
			 	&rect, &boundary,
				PhInputGroup( cbinfo->event ),
				NULL, NULL, NULL, NULL, NULL );

	/* Save a pointer to the widget being dragged. */
	dragged_panel = ApGetInstance( widget );
	//dragged_widget = widget;

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}


int
btnActivate_hzP1a_click( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtWidget_t *thisWindow;
	thisWindow = ApGetInstance( widget );
	u32_t  i;
	u32_t   indexPNames = 0;
	if(	trace_hzPanel1a_open_mode == NC_SCREEN_PANEL1A )
	{
		indexPNames = SET_UP_NC_SCREEN;
	}
	else
	{
		for ( i = 0; i < max_panel_num; i++ )
		{
			if ( windowEquipPanel[i] == thisWindow )
			{
				indexPNames = i;
				break;
			}
		}
	}

	if ( activateState[indexPNames] == 0 )		/*up*/
	{
		/*设置激活按钮的按下状态；面板边框为激活状态；所有按钮为弹起状态*/
		PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aActivateButton ),AB_STATE,AB_DOWN,0);
		PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aActivateButton ),AB_COLOR,PgRGB(255,255,255),0);
		activateState[indexPNames] = 1;

		PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aPtRect ),Pt_ARG_LINE_WIDTH,1,0);
		PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aPtRect ),Pt_ARG_COLOR,PgRGB(51,153,235),0);

		panelBtnFloat( ApGetWidgetPtr( thisWindow, ABN_hzP1aAutoManuBtn ) );
		panelBtnFloat( ApGetWidgetPtr( thisWindow, ABN_hzP1aFastSlowBtn ) );
		//与set_btnState_by_mode_hzP1a设置逻辑冲突
		//panelBtnFloat( ApGetWidgetPtr( thisWindow, ABN_hzP1aMViBtn ) );
		//panelBtnFloat( ApGetWidgetPtr( thisWindow, ABN_hzP1aMVdBtn ) );

		if(trace_hzPanel1a_MVd_btn_green[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aMVdBtn ),OB_BACKCOLOR,PgRGB(60,200,150),0);
			PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aMVdBtn ),OB_COLOR,Pg_BLACK,0);	/*wkt20240711*/
		}
		if(trace_hzPanel1a_MVi_btn_green[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aMViBtn ),OB_BACKCOLOR,PgRGB(60,200,150),0);
			PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aMVdBtn ),OB_COLOR,Pg_BLACK,0);	/*wkt20240711*/
		}
	}
	else if ( activateState[indexPNames] == 1 )	/*down*/
	{
		/*设置激活按钮的弹起状态；面板边框为未激活状态；所有按钮为闭锁状态*/
		PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aActivateButton ), AB_STATE, AB_UP, 0);
		PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aActivateButton ), AB_COLOR, PgRGB(0,0,0), 0);
		activateState[indexPNames] = 0;

		panelBtnGUI( ApGetWidgetPtr( thisWindow, ABN_hzP1aAutoManuBtn ) );
		panelBtnGUI( ApGetWidgetPtr( thisWindow, ABN_hzP1aFastSlowBtn ) );
		panelBtnGUINoClr( ApGetWidgetPtr( thisWindow, ABN_hzP1aMViBtn ) );
		panelBtnGUINoClr( ApGetWidgetPtr( thisWindow, ABN_hzP1aMVdBtn ) );
		panelBtnGUI( ApGetWidgetPtr( thisWindow, ABN_hzP1aExecuteBtn ) );

		trace_hzPanel1a_auto_manu_btn[indexPNames] = UP1a;

		PtSetResource(thisWindow, Pt_ARG_FILL_COLOR, PgRGB(191,191,191), 0);
		PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aPtRect ),Pt_ARG_LINE_WIDTH,1,0);
		PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aPtRect ),Pt_ARG_COLOR,PgRGB(0,0,0),0);
	}
	else
	{
		/*do nothing*/
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}


int
btnAutoManu_hzP1a_click( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtWidget_t *thisWindow;
	thisWindow = ApGetInstance( widget );
	u32_t  indexPNames = 0;
	u32_t  i;
	i32_t ret = UINT_0;

	if(	trace_hzPanel1a_open_mode == NC_SCREEN_PANEL1A )
	{
		indexPNames = SET_UP_NC_SCREEN;
	}
	else
	{
		for ( i = 0; i < max_panel_num; i++ )
		{
			if ( windowEquipPanel[i] == thisWindow )
			{
				indexPNames = i;
				break;
			}
		}
	}

	//traceBtnOp2[indexPNames] = C4_STATE;
	panelBtnSink( ApGetWidgetPtr( thisWindow, ABN_hzP1aAutoManuBtn ) );
	panelBtnFloat( ApGetWidgetPtr( thisWindow, ABN_hzP1aExecuteBtn ) );

	u32_hzPanel1a_btn_id[indexPNames] = 3;
	/*oper_count2[indexPNames]++;*/
	//oper_count++;
	//oper_time_count++;

	trace_hzPanel1a_auto_manu_btn[indexPNames] = DOWN1a;/*记录手自动按钮按下状态*/

	/*time(&timer5_startTime);
	StartTime2 = timer5_startTime;
	*/
	struct timeb timer_startTime;  /* millisecond time */
	memset(&timer_startTime, UINT_0, sizeof(struct timeb));
	ret = ftime(&timer_startTime);
	time_hzPanel1a_start_time[indexPNames] = timer_startTime.time*1000 + timer_startTime.millitm;//ms

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}

int
btnIncrease_hzP1a_arm( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtWidget_t *thisWindow;
	i32_t ret = UINT_0;
	thisWindow = ApGetInstance( widget );
//	PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aMViBtn ), OB_COLOR, PgRGB(255,255,255), 0);
//	PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aMViBtn ), OB_BACKCOLOR, PgRGB(97,97,97), 0);
	PtSetResource( ApGetWidgetPtr( thisWindow, ABN_hzP1aMViBtn ), OB_STATE, OB_DOWN, 0);	/*wkt*/		
	struct timeb timer_startTime;  /* millisecond time */
	memset(&timer_startTime, UINT_0, sizeof(struct timeb));
	ret = ftime(&timer_startTime);
	time_hzPanel1a_des_add_start_time = timer_startTime.time*1000 + timer_startTime.millitm;//记录增按钮的按下时间


	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}

int btnIncrease_hzP1a_disarm( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtWidget_t *thisWindow;
	thisWindow = ApGetInstance( widget );
	//u32_t  indexPNames = 0;
	//u32_t  i;
	scid_oper_log_t scid_oper_log_hzPanel1a = {0};
	char_t *device_id = P_NULL;
	char_t *device_sv = P_NULL;
	char_t *device_mv = P_NULL;
	u32_t u32_err = OK;
	u32_t temp_oper_time_count = 0;         /*操作计数器*/
	time_t time = 0;
	i32_t ret = UINT_0;
	time_t end_time = 0;

//	panelBtnFloatNoClr(ApGetWidgetPtr( thisWindow, ABN_hzP1aMViBtn ));
	PtSetResource( ApGetWidgetPtr( thisWindow, ABN_hzP1aMViBtn ), OB_STATE, OB_UP, 0);
	//panelBtnGUI( ApGetWidgetPtr( thisWindow, ABN_hzP1aMViBtn ) );
#if 0
	if(	trace_hzPanel1a_open_mode == NC_SCREEN_1a )
	{
		indexPNames = SET_UP_NC_SCREEN;
	}
	else
	{
		for ( i = 0; i < max_panel_num; i++ )
		{
			if ( windowEquipPanel[i] == thisWindow )
			{
				indexPNames = i;
				break;
			}
		}
	}
#endif

	struct timeb timer_endTime;  // millisecond time
	memset(&timer_endTime, UINT_0, sizeof(struct timeb));
	ret = ftime(&timer_endTime);
	end_time = timer_endTime.time*1000 + timer_endTime.millitm;//ms
	time = end_time - time_hzPanel1a_des_add_start_time;
	temp_oper_time_count = time / 400;/*400ms加1次*/
	if((time % 400) != 0)
	{
		temp_oper_time_count++;
	}
	oper_time_count = oper_time_count + temp_oper_time_count;
	time_hzPanel1a_des_add_start_time = 0;
	oper_count++;

	scid_oper_log_hzPanel1a.u16_oper_count = oper_time_count;/*操作计数器*/
	scid_oper_log_hzPanel1a.u8_scid_id = mq_recv_fdata.user_info.u32_station_id;/* station id */
	scid_oper_log_hzPanel1a.u8_device_type = 8;/*模拟量面板类型*/
	//PtGetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aEquipmentID ),Pt_ARG_TEXT_STRING,&device_id,0);
	scid_oper_log_hzPanel1a.u16_device_id = p_st_ana_hzPanel1a.u32_dev_id;/*设备ID*/
	scid_oper_log_hzPanel1a.u8_scid_pos = 0;
	scid_oper_log_hzPanel1a.u8_app_mode = atoi( mq_cyc_recv_data.mq_recv_ddata.var_recv_buf[UINT_1].ch_var_val);/*应用模式*/
	scid_oper_log_hzPanel1a.u32_btn_id = 5;/*MV/LV增指令*/
	PtGetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aAnalogDisplaySV ), Pt_ARG_TEXT_STRING, &device_sv, 0);
	if(P_NULL != device_sv)
	{
		f32_t f32_device_sv = atof(device_sv);
		memcpy(&scid_oper_log_hzPanel1a.u32_sv, &f32_device_sv, sizeof(f32_t));
	}
	//scid_oper_log_hzPanel1a.u32_sv = 0;//???应该是模拟量
	PtGetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aAnalogDisplayMV ), Pt_ARG_TEXT_STRING, &device_mv, 0);
	if(P_NULL != device_mv)
	{
		f32_t f32_device_mv = atof(device_mv);
		memcpy(&scid_oper_log_hzPanel1a.u32_MV, &f32_device_mv, sizeof(f32_t));
	}
	//scid_oper_log_hzPanel1a.u32_MV = 0;//???应该是模拟量
	scid_oper_log_hzPanel1a.u16_FAST_SLOW = 0;//？？？不应该是文本吗
	scid_oper_log_hzPanel1a.u16_res6 = 0;
	scid_oper_log_hzPanel1a.u32_res7 = 0;
	scid_oper_log_hzPanel1a.u32_res9 = oper_count;/*操作手操器按钮计数器*/
	scid_oper_log_hzPanel1a.u32_res10 = 0;

	u32_err = photon_send_oper_log( &scid_oper_log_hzPanel1a, &mq_cyc_send_data );/*发送操作指令*/
	if(u32_err != OK)
	{
		u32_err = photon_show_msg(MSG_SEND_MVLV_SUB_COMMOND_FAILED);
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}

int
btnDecrease_hzP1a_arm( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtWidget_t *thisWindow;
	i32_t ret = UINT_0;
	thisWindow = ApGetInstance( widget );

//	PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aMVdBtn ), OB_COLOR, PgRGB(255,255,255), 0);
//	PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aMVdBtn ), OB_BACKCOLOR, PgRGB(97,97,97), 0);
	PtSetResource( ApGetWidgetPtr( thisWindow, ABN_hzP1aMVdBtn ), OB_STATE, OB_DOWN, 0);	/*wkt*/

	struct timeb timer_startTime;  /* millisecond time */
	memset(&timer_startTime, UINT_0, sizeof(struct timeb));
	ret = ftime(&timer_startTime);
	time_hzPanel1a_des_add_start_time = timer_startTime.time*1000 + timer_startTime.millitm;//记录增按钮的按下时间


	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}

int
btnDecrease_hzP1a_disarm( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtWidget_t *thisWindow;
	thisWindow = ApGetInstance( widget );
	//u32_t  indexPNames = 0;
	//u32_t  i;
	scid_oper_log_t scid_oper_log_hzPanel1a = {0};
	char_t *device_id = P_NULL;
	char_t *device_sv = P_NULL;
	char_t *device_mv = P_NULL;
	u32_t u32_err = OK;
	i32_t ret = UINT_0;
	time_t end_time = 0;
	time_t time = 0;
	u32_t temp_oper_time_count = 0;         /*操作计数器*/

	PtSetResource( ApGetWidgetPtr( thisWindow, ABN_hzP1aMVdBtn ), OB_STATE, OB_UP, 0);	/*wkt*/
//	panelBtnFloatNoClr(ApGetWidgetPtr( thisWindow, ABN_hzP1aMVdBtn ));	/*wkt20240709:更改无效*/
	//panelBtnGUI( ApGetWidgetPtr( thisWindow, ABN_hzP1aMVdBtn ) );
#if 0
	if(	trace_hzPanel1a_open_mode == NC_SCREEN_1a )
	{
		indexPNames = SET_UP_NC_SCREEN;
	}
	else
	{
		for ( i = 0; i < max_panel_num; i++ )
		{
			if ( windowEquipPanel[i] == thisWindow )
			{
				indexPNames = i;
				break;
			}
		}
	}
#endif
	struct timeb timer_endTime;  // millisecond time
	memset(&timer_endTime, UINT_0, sizeof(struct timeb));
	ret = ftime(&timer_endTime);
	end_time = timer_endTime.time*1000 + timer_endTime.millitm;//ms
	time = end_time - time_hzPanel1a_des_add_start_time;
	temp_oper_time_count = time / 400;/*400ms加1次*/
	if((time % 400) != 0)
	{
		temp_oper_time_count++;
	}
	oper_time_count = oper_time_count + temp_oper_time_count;
	time_hzPanel1a_des_add_start_time = 0;

	oper_count++;

	scid_oper_log_hzPanel1a.u16_oper_count = oper_time_count;/*操作计数器*/
	scid_oper_log_hzPanel1a.u8_scid_id = mq_recv_fdata.user_info.u32_station_id;/* station id */
	scid_oper_log_hzPanel1a.u8_device_type = 8;/*模拟量面板类型*/
	//PtGetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aEquipmentID ),Pt_ARG_TEXT_STRING,&device_id,0);
	scid_oper_log_hzPanel1a.u16_device_id = p_st_ana_hzPanel1a.u32_dev_id;/*设备ID*/
	scid_oper_log_hzPanel1a.u8_scid_pos = 0;
	scid_oper_log_hzPanel1a.u8_app_mode = atoi( mq_cyc_recv_data.mq_recv_ddata.var_recv_buf[UINT_1].ch_var_val);/*应用模式*/
	scid_oper_log_hzPanel1a.u32_btn_id = 7;/*MV/LV减指令*/
	PtGetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aAnalogDisplaySV ), Pt_ARG_TEXT_STRING, &device_sv, 0);
	if(P_NULL != device_sv)
	{
		f32_t f32_device_sv = atof(device_sv);
		memcpy(&scid_oper_log_hzPanel1a.u32_sv, &f32_device_sv, sizeof(f32_t));
	}
	//scid_oper_log_hzPanel1a.u32_sv = 0;//???应该是模拟量
	PtGetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aAnalogDisplayMV ), Pt_ARG_TEXT_STRING, &device_mv, 0);
	if(P_NULL != device_mv)
	{
		f32_t f32_device_mv = atof(device_mv);
		memcpy(&scid_oper_log_hzPanel1a.u32_MV, &f32_device_mv, sizeof(f32_t));
	}
	//scid_oper_log_hzPanel1a.u32_MV = 0;//???应该是模拟量
	scid_oper_log_hzPanel1a.u16_FAST_SLOW = 0;//？？？不应该是文本吗
	scid_oper_log_hzPanel1a.u16_res6 = 0;
	scid_oper_log_hzPanel1a.u32_res7 = 0;
	scid_oper_log_hzPanel1a.u32_res9 = oper_count;/*操作手操器按钮计数器*/
	scid_oper_log_hzPanel1a.u32_res10 = 0;

	u32_err = photon_send_oper_log( &scid_oper_log_hzPanel1a, &mq_cyc_send_data );/*发送操作指令*/
	if(u32_err != OK)
	{
		u32_err = photon_show_msg(MSG_SEND_MVLV_SUB_COMMOND_FAILED);
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}


int
btnFastSlow_hzP1a_arm( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtWidget_t *thisWindow;
	thisWindow = ApGetInstance( widget );
	PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aFastSlowBtn ), OB_COLOR, PgRGB(255,255,255), 0);
	PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aFastSlowBtn ), OB_BACKCOLOR, PgRGB(97,97,97), 0);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}


int
btnFastSlow_hzP1a_disarm( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtWidget_t *thisWindow;
	thisWindow = ApGetInstance( widget );
	//u32_t  indexPNames = 0;
	//u32_t  i;
	scid_oper_log_t scid_oper_log_hzPanel1a = {0};
	char_t *device_id = P_NULL;
	char_t *device_sv = P_NULL;
	char_t *device_mv = P_NULL;
	u32_t u32_err = OK;

	panelBtnFloat(ApGetWidgetPtr( thisWindow, ABN_hzP1aFastSlowBtn ));
	//panelBtnGUI( ApGetWidgetPtr( thisWindow, ABN_hzP1aFastSlowBtn ) );
#if 0
	if(	trace_hzPanel1a_open_mode == NC_SCREEN_1a )
	{
		indexPNames = SET_UP_NC_SCREEN;
	}
	else
	{
		for ( i = 0; i < max_panel_num; i++ )
		{
			if ( windowEquipPanel[i] == thisWindow )
			{
				indexPNames = i;
				break;
			}
		}
	}
#endif
	oper_time_count++;
	oper_count++;

	scid_oper_log_hzPanel1a.u16_oper_count = oper_time_count;/*操作计数器*/
	scid_oper_log_hzPanel1a.u8_scid_id = mq_recv_fdata.user_info.u32_station_id;/* station id */
	scid_oper_log_hzPanel1a.u8_device_type = 8;/*模拟量面板类型*/
	//PtGetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aEquipmentID ),Pt_ARG_TEXT_STRING,&device_id,0);
	scid_oper_log_hzPanel1a.u16_device_id = p_st_ana_hzPanel1a.u32_dev_id;/*设备ID*/
	scid_oper_log_hzPanel1a.u8_scid_pos = 0;
	scid_oper_log_hzPanel1a.u8_app_mode = atoi( mq_cyc_recv_data.mq_recv_ddata.var_recv_buf[UINT_1].ch_var_val);/*应用模式*/
	scid_oper_log_hzPanel1a.u32_btn_id = 8;/*FAST/SLOW按钮ID*/
	PtGetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aAnalogDisplaySV ), Pt_ARG_TEXT_STRING, &device_sv, 0);
	if(P_NULL != device_sv)
	{
		f32_t f32_device_sv = atof(device_sv);
		memcpy(&scid_oper_log_hzPanel1a.u32_sv, &f32_device_sv, sizeof(f32_t));
	}
	//scid_oper_log_hzPanel1a.u32_sv = 0;//???应该是模拟量
	PtGetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aAnalogDisplayMV ), Pt_ARG_TEXT_STRING, &device_mv, 0);
	if(P_NULL != device_mv)
	{
		f32_t f32_device_mv = atof(device_mv);
		memcpy(&scid_oper_log_hzPanel1a.u32_MV, &f32_device_mv, sizeof(f32_t));
	}
	//scid_oper_log_hzPanel1a.u32_MV = 0;//???应该是模拟量
	scid_oper_log_hzPanel1a.u16_FAST_SLOW = 0;//？？？不应该是文本吗
	scid_oper_log_hzPanel1a.u16_res6 = 0;
	scid_oper_log_hzPanel1a.u32_res7 = 0;
	scid_oper_log_hzPanel1a.u32_res9 = oper_count;/*操作手操器按钮计数器*/
	scid_oper_log_hzPanel1a.u32_res10 = 0;

	u32_err = photon_send_oper_log( &scid_oper_log_hzPanel1a, &mq_cyc_send_data );/*发送操作指令*/
	if(u32_err != OK)
	{
		u32_err = photon_show_msg(MSG_SEND_FASTSLOW_COMMOND_FAILED);
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}


int
btnExecute_hzP1a_arm( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtWidget_t *thisWindow;
	thisWindow = ApGetInstance( widget );

	PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aExecuteBtn ), OB_COLOR, PgRGB(255,255,255), 0);
	PtSetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aExecuteBtn ), OB_BACKCOLOR, PgRGB(97,97,97), 0);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}


int
btnExecute_hzP1a_disarm( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	i32_t ret = UINT_0;      //function return value
	char_t *device_name = P_NULL;
	char_t device_id[VAR_NAME_SIZE_MAX] = {0};
	char_t *device_sv = P_NULL;
	char_t *device_mv = P_NULL;
	u32_t log_result = OK;
	u32_t   indexPNames = 0;
	u32_t   i;
	u32_t u32_err = OK;
	scid_oper_log_t scid_oper_log_hzPanel1a = {0};
	//char *switch_des = 0;	//operatingbuton text
	PtWidget_t *thisWindow;
	thisWindow = ApGetInstance( widget );

	panelBtnFloat(ApGetWidgetPtr( thisWindow, ABN_hzP1aExecuteBtn ));
	panelBtnGUI( ApGetWidgetPtr( thisWindow, ABN_hzP1aExecuteBtn ) );
	panelBtnFloat( ApGetWidgetPtr( thisWindow, ABN_hzP1aAutoManuBtn ) );

	if(	trace_hzPanel1a_open_mode == NC_SCREEN_PANEL1A )
	{
		indexPNames = SET_UP_NC_SCREEN;
	}
	else
	{
		for ( i = 0; i < max_panel_num; i++ )
		{
			if ( windowEquipPanel[i] == thisWindow )
			{
				indexPNames = i;
				break;
			}
		}
	}

	trace_hzPanel1a_auto_manu_btn[indexPNames] = UP1a;/*记录手自动按钮弹起状态*/

	memset(&mq_send_ldata_hzPanel1a, ' ', sizeof(mq_send_ldata_t));

	mq_send_ldata_hzPanel1a.ldata_send_buf.operation_log_frame[0].operation_log_frame_head.u32_operation_log_type = 1;
	// get current time
	struct timeb msec_time;  // millisecond time
	memset(&msec_time,UINT_0,sizeof(struct timeb));
	ret = ftime(&msec_time);
	scid_time_t scid_time;   // scid time structure
	memset(&scid_time,UINT_0,sizeof(scid_time_t));
	ret = std_sec_to_scid_time( &scid_time, msec_time.time, (u32_t)msec_time.millitm );
	u32_t logTimeSize = 23;
	memcpy(&mq_send_ldata_hzPanel1a.ldata_send_buf.operation_log_frame[0].operation_log_frame_head.ch_time_buf,&scid_time,logTimeSize);

	memcpy(&mq_send_ldata_hzPanel1a.ldata_send_buf.operation_log_frame[0].operation_log_frame_head.ch_cur_user,&mq_recv_fdata.user_info.ch_cur_user,strlen(mq_recv_fdata.user_info.ch_cur_user));

	PtGetResource( ApGetWidgetPtr( thisWindow, ABN_hzP1aActivateButton ),Pt_ARG_TEXT_STRING,&device_name,0);
	if(P_NULL != device_name)
	{
		memcpy(&mq_send_ldata_hzPanel1a.ldata_send_buf.operation_log_frame[0].operation_log_frame_app.device_log_frame_des.ch_device_name,device_name,strlen(device_name));
	}
	//PtGetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aEquipmentID ),Pt_ARG_TEXT_STRING,&device_id,0);
	itoa(p_st_ana_hzPanel1a.u32_dev_id, device_id, 10);;
	memcpy(mq_send_ldata_hzPanel1a.ldata_send_buf.operation_log_frame[0].operation_log_frame_app.device_log_frame_des.ch_device_id,device_id,strlen(device_id));

#if 0
	if ( traceBtnOp2[indexPNames] == C4_STATE  )//C4
	{
		PtGetResource(ApGetWidgetPtr(thisWindow, ABN_hzP2OperatingButtonC4), Pt_ARG_TEXT_STRING, &switch_des, 0);
//		strcpy(manu_auto,"manu");
		memcpy(mq_send_ldata_hzPanel1a.ldata_send_buf.operation_log_frame[0].operation_log_frame_app.device_log_frame_des.ch_switch_des,switch_des,strlen(switch_des));
	}
	else if ( traceBtnOp2[indexPNames] == C3_STATE )//C3
	{
		PtGetResource(ApGetWidgetPtr(thisWindow, ABN_hzP2OperatingButtonC3), Pt_ARG_TEXT_STRING, &switch_des, 0);
//		strcpy(manu_auto,"auto");
		memcpy(mq_send_ldata_hzPanel1a.ldata_send_buf.operation_log_frame[0].operation_log_frame_app.device_log_frame_des.ch_switch_des,switch_des,strlen(switch_des));
	}

	else if ( traceBtnOp2[indexPNames] == C1_STATE )//C1
	{
		PtGetResource(ApGetWidgetPtr(thisWindow, ABN_hzP2OperatingButtonC1), Pt_ARG_TEXT_STRING, &switch_des, 0);
//		strcpy(on_off,"on");
		memcpy(mq_send_ldata_hzPanel1a.ldata_send_buf.operation_log_frame[0].operation_log_frame_app.device_log_frame_des.ch_switch_des,switch_des,strlen(switch_des));
	}
	else if ( traceBtnOp2[indexPNames] == C2_STATE )//C2
	{
		PtGetResource(ApGetWidgetPtr(thisWindow, ABN_hzP2OperatingButtonC2), Pt_ARG_TEXT_STRING, &switch_des, 0);
//		strcpy(on_off,"off");
		memcpy(mq_send_ldata_hzPanel1a.ldata_send_buf.operation_log_frame[0].operation_log_frame_app.device_log_frame_des.ch_switch_des,switch_des,strlen(switch_des));
	}
	else
	{

	}
#endif

	if ( (trace_hzPanel1a_auto_manu_mode[indexPNames] == AUTO1a) && (u32_hzPanel1a_btn_id[indexPNames] == 3) )
	{
		/*自动状态下，再点击手自动按钮，应该是要切换手动操作*/
		memcpy(mq_send_ldata_hzPanel1a.ldata_send_buf.operation_log_frame[0].operation_log_frame_app.device_log_frame_des.ch_switch_des,
				"MANU",strlen("MANU"));
	}
	else if((trace_hzPanel1a_auto_manu_mode[indexPNames] == MANU1a) && (u32_hzPanel1a_btn_id[indexPNames] == 3))
	{
		/*手动状态下，再点击手自动按钮，应该是要切换自动操作*/
		memcpy(mq_send_ldata_hzPanel1a.ldata_send_buf.operation_log_frame[0].operation_log_frame_app.device_log_frame_des.ch_switch_des,
						"AUTO",strlen("AUTO"));
	}
	else if(trace_hzPanel1a_auto_manu_mode[indexPNames] == FORCEMANU1a)
	{
		memcpy(mq_send_ldata_hzPanel1a.ldata_send_buf.operation_log_frame[0].operation_log_frame_app.device_log_frame_des.ch_switch_des,
						"FORCEMANU",strlen("FORCEMANU"));
	}
	else
	{
		strcpy(mq_send_ldata_hzPanel1a.ldata_send_buf.operation_log_frame[0].operation_log_frame_app.device_log_frame_des.ch_switch_des,"");
	}

	// log type(1,2,4)
	mq_send_ldata_hzPanel1a.u32_log_type = 1;
	// log size(1~20)
	mq_send_ldata_hzPanel1a.u32_log_size = 1;

	log_result = photon_send_display_log(&mq_send_ldata_hzPanel1a, &ERR_QUEUE, &u32_head_send_tick);/*发送日志信息*/
	if(log_result != OK)
	{
		u32_err = photon_show_msg(MSG_SEND_OPERATE_LOG_FAILED);
	}

	oper_time_count++;
	oper_count++;

	scid_oper_log_hzPanel1a.u16_oper_count = oper_time_count;/*操作计数器*/
	scid_oper_log_hzPanel1a.u8_scid_id = mq_recv_fdata.user_info.u32_station_id;/* station id */
	scid_oper_log_hzPanel1a.u8_device_type = 8;/*模拟量面板面板类型为8*/
	scid_oper_log_hzPanel1a.u16_device_id = atoi( device_id );/*设备ID*/
	scid_oper_log_hzPanel1a.u8_scid_pos = 0;/*由小秦接口获得*/
	scid_oper_log_hzPanel1a.u8_app_mode = atoi( mq_cyc_recv_data.mq_recv_ddata.var_recv_buf[UINT_1].ch_var_val);/*应用模式*/
	scid_oper_log_hzPanel1a.u32_btn_id = u32_hzPanel1a_btn_id[indexPNames];

	PtGetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aAnalogDisplaySV ), Pt_ARG_TEXT_STRING, &device_sv, 0);
	if(P_NULL != device_sv)
	{
		f32_t f32_device_sv = atof(device_sv);
		memcpy(&scid_oper_log_hzPanel1a.u32_sv, &f32_device_sv, sizeof(f32_t));
	}
	//scid_oper_log_hzPanel1a.u32_sv = 0;//???应该是模拟量
	PtGetResource(ApGetWidgetPtr( thisWindow, ABN_hzP1aAnalogDisplayMV ), Pt_ARG_TEXT_STRING, &device_mv, 0);
	if(P_NULL != device_mv)
	{
		f32_t f32_device_mv = atof(device_mv);
		memcpy(&scid_oper_log_hzPanel1a.u32_MV, &f32_device_mv, sizeof(f32_t));
	}
	//scid_oper_log_hzPanel1a.u32_MV = 0;//???应该是模拟量

	scid_oper_log_hzPanel1a.u16_FAST_SLOW = 0;/*不需要填*/

	scid_oper_log_hzPanel1a.u16_res6 = 0;
	scid_oper_log_hzPanel1a.u32_res7 = 0;
	scid_oper_log_hzPanel1a.u32_res9 = oper_count;/*操作手操器按钮计数器*/
	scid_oper_log_hzPanel1a.u32_res10 = 0;

	u32_err = photon_send_oper_log( &scid_oper_log_hzPanel1a, &mq_cyc_send_data );/*发送操作指令*/
	if(u32_err != OK)
	{
		u32_err = photon_show_msg(MSG_SEND_OPERATE_COMMAND_FAILED);
	}

	u32_hzPanel1a_btn_id[indexPNames] = 0;//add 将需求二次确认的按钮ID再写回初始值

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}


int
btnExit_hzP1a_click( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtWidget_t *thisWidget = P_NULL;
	PtWidget_t *thisWindow = P_NULL;
	thisWindow = ApGetInstance( widget );
	u32_t  index;
	u32_t  i;
	u32_t  j;
	u32_t closeAllPanel;
	panelBtnSink( ApGetWidgetPtr( thisWindow, ABN_hzP1aExitBtn ) );

	if(	trace_hzPanel1a_open_mode == NC_SCREEN_PANEL1A )
	{
		i = SET_UP_NC_SCREEN;
	}
	else
	{
		for ( i = 0; i < max_panel_num; i++ )
		{
			if ( windowEquipPanel[i] == thisWindow )
			{
				thisWidget = widgetEquipPanel[i];
				break;
			}
		}
	}

	if ( P_NULL != thisWidget )
	{
		PtSetResource(thisWidget, Pt_ARG_FLAGS, 0, Pt_BLOCKED | Pt_GETS_FOCUS);
		PtSetResource(thisWidget, Pt_ARG_FLAGS, 0x2000180, 0x2000180);
	}

	/*关闭面板时，设置当前打开的设备图符（流程图）为未打开状态*/
	if( (P_NULL != thisFlowChart) && (P_NULL != thisWidget) )
	{
		set_hzPanels_equipment_widgets_close(PANEL_TYPE_1A, thisFlowChart, thisWidget, i);
	}

	photon_close_panel_by_widget_pointer( thisWindow );/*将关闭面板的指针传给后台*/

	widgetEquipPanel[i] = P_NULL;
	windowEquipPanel[i] = P_NULL;
	activateState[i] = 0;
	for ( index = 0; index < max_panel_num; index++)
	{
		if ( windowEquipPanel[index] != P_NULL )
		{
			closeAllPanel = 0;
			break;
		}
		closeAllPanel = 1;
	}

	if ( closeAllPanel == 1 )
	{
		photon_close_panel(&mq_cyc_send_data.mq_req_ddata);
	}

	/*for( index = 0; index < 16;index++)
	{
		if( windowEquipPanel[index] != P_NULL )
		{
			break;
		}
	}
	*/
	if ( index >= max_panel_num )
	{
		SYS_INFO.b_panel = FALSE;
	}

	for ( j = 0; j < PANELPNAME_REQUEST_NUM; j++ )
	{
		PanelPName_Requst[ i*PANELPNAME_REQUEST_NUM + j] = P_NULL;
		memset(PName_Request[i*PANELPNAME_REQUEST_NUM + j],0,VAR_NAME_SIZE_MAX);
		PanelPName_NO[i*PANELPNAME_REQUEST_NUM + j] = 0;
	}
	Input_PanelPName_Num[i] = 0;

	if(	trace_hzPanel1a_open_mode == FLOW_CHART_PANEL1A )
	{
		windowEquipPanelIndex--;
	}
	else
	{
		/*do thing*/
	}

	//fclose(pFile);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}


int hzPanel1a_setUp( PtWidget_t *link_instance, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	//设置打开面板的设备图符在列表中的位置
	/*count_open_hzPanel2_times();*/
	char    buffer[40];
	memset( buffer, UINT_0, sizeof(buffer));
	u32_t   indexPNames = 0;
	u32_t   indexValve;
	u32_t   i;
	u32_t   firstOpenPanel = 0;
	u32_t   ret_reset = OK;
	u32_t   ret_init = OK;
	u32_t   u32_ret = OK;
	panel_handle_t panel1a_handle = {0};
	dev_widget_color_t st_dev_widget_color = { 0 };

	/*对入参判断处理*/
	if(P_NULL == link_instance || P_NULL == apinfo || P_NULL == apinfo->widget)
	{
		return( Pt_END );
	}

	thisFlowChart = ApGetInstance( apinfo->widget );/*获取流程图指针*/
	if(P_NULL == thisFlowChart)
	{
		return( Pt_END );
	}

	trace_hzPanel1a_open_mode = FLOW_CHART_PANEL1A;

	for ( i = 0; i < max_panel_num; i++ )
	{
		if ( windowEquipPanel[i] != P_NULL )
		{
			firstOpenPanel = 0;
			break;
		}
		firstOpenPanel = 1;
	}
	for ( indexValve = 0; indexValve < max_panel_num; indexValve++ )
	{
		if ( windowEquipPanel[indexValve] == P_NULL )
		{
			windowEquipPanel[indexValve] = link_instance;
			widgetEquipPanel[indexValve] = apinfo->widget;
			SYS_INFO.b_panel = TRUE;
			//PanelPName_NO[indexValve] = 0;
			Input_PanelPName_Num[indexValve] = 0;
			indexPNames = indexValve;
			activateState[indexPNames] = 0;
			break;
		}
	}
	if ( windowEquipPanelIndex == max_panel_num )/*面板数量达到上限*/
	{
		//openAllPanelNum = 1;
		//PanelPName_NO[indexPNames] = 0;
		u32_ret = photon_show_msg(MSG_OPENING_PANEL_LIMITED);
		//		SYS_INFO.b_panel = FALSE;
//		windowEquipPanel[indexPNames] = P_NULL;//qinshiling 20240729 修改多次打开面板程序异常退出
//		widgetEquipPanel[indexPNames] = P_NULL;//qinshiling 20240729 修改多次打开面板程序异常退出
//		Input_PanelPName_Num[indexPNames] = 0; //qinshiling 20240729 修改多次打开面板程序异常退出

		return( Pt_END );
	}

	if ( indexValve >= windowEquipPanelIndex )
	{
		windowEquipPanel[windowEquipPanelIndex] = link_instance;
		widgetEquipPanel[windowEquipPanelIndex] = apinfo->widget;
		SYS_INFO.b_panel = TRUE;
		indexPNames = windowEquipPanelIndex;
	}
	/*设置当前打开的设备图符（流程图）及获得设备ID，写面板的设备ID*/
	set_hzPanel1a_equipmentWidget(thisFlowChart, link_instance, indexPNames, apinfo->widget);
	if(0 == equipment_widget_index_hzP1a)
	{
		//u32_ret = photon_show_msg(MSG_THIS_WIDGET_SHOULD_NOT_OPEN_PANEL_A);
		windowEquipPanel[indexPNames] = P_NULL;//qinshiling 20240729 修改多次打开面板程序异常退出
		widgetEquipPanel[indexPNames] = P_NULL;//qinshiling 20240729 修改多次打开面板程序异常退出
		Input_PanelPName_Num[indexPNames] = 0; //qinshiling 20240729 修改多次打开面板程序异常退出

		//openAllPanelNum = 1;
		//PanelPName_NO[indexPNames] = 0;
		SYS_INFO.b_panel = FALSE;
		return (Pt_END);
	}

	sprintf( buffer, "HZPanel1a%d", indexPNames + 1 );
	PtSetResource( link_instance, Pt_ARG_WINDOW_TITLE, buffer, 0 );       /*put in the text of window title(hzPanel2)*/
	PtSetResource( apinfo->widget, Pt_ARG_FLAGS, Pt_BLOCKED, Pt_BLOCKED );/*闭锁打开面板的设备图符*/

	/*get the PNames of the hzPanel1a configuration*/
	u32_ret = get_PNames_hzPanel1a_configuration(link_instance, indexPNames);			/*获取打开面板的设备图符组态的变量*/
	if ( OK != u32_ret)/*获取面板变量组态失败*/
	{
		u32_ret = photon_show_msg(MSG_GET_PANEL_SETTING_FAILED);
		windowEquipPanel[indexPNames] = P_NULL;//qinshiling 20240729 修改多次打开面板程序异常退出
		widgetEquipPanel[indexPNames] = P_NULL;//qinshiling 20240729 修改多次打开面板程序异常退出
		Input_PanelPName_Num[indexPNames] = 0; //qinshiling 20240729 修改多次打开面板程序异常退出

		SYS_INFO.b_panel = FALSE;
		return (Pt_END);
	}
	else
	{
		for ( i = 0; i < u32_max_pnames_num_hzPanel1a; i++ )
		{
			if(1 == PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + i]) /*组态变量点*/
			{
				PanelPName_Requst[indexPNames*PANELPNAME_REQUEST_NUM + Input_PanelPName_Num[indexPNames]] = PName_Request[indexPNames*PANELPNAME_REQUEST_NUM + i];
				Input_PanelPName_Num[indexPNames]++;
			}
		}

		/* 设置图符填充色 */
		st_dev_widget_color.st_fill_color_on = p_st_ana_hzPanel1a.st_fillcolor_open;
		st_dev_widget_color.st_fill_color_mid = p_st_ana_hzPanel1a.st_fillcolor_middle;
		st_dev_widget_color.st_fill_color_off = p_st_ana_hzPanel1a.st_fillcolor_close;
		st_dev_widget_color.st_fill_color_undef = p_st_ana_hzPanel1a.st_fillcolor_undefine;
		set_widget_color(now_equipmentWidget_hzP1a[indexPNames], WIDGET_TYPE_DEVICE, &st_dev_widget_color);
		set_dev_widget_direction(now_equipmentWidget_hzP1a[indexPNames], p_st_ana_hzPanel1a.u32_dev_type);
	}

	if ( firstOpenPanel == 1 )
	{
		u32_old_ddata_size = mq_cyc_send_data.mq_req_ddata.u32_num;
		ret_reset = photon_reset_panel_vars( u32_old_ddata_size, &mq_cyc_send_data.mq_req_ddata, &mq_recv_fdata.vars_info );
	}

	for (  i = 0; i < Input_PanelPName_Num[indexPNames]; i++)
	{
		strcpy(	panel1a_handle.panel_req_ddata.ch_var_name_buffer[i],PanelPName_Requst[indexPNames*PANELPNAME_REQUEST_NUM + i]);
	}

	panel1a_handle.panel_req_ddata.u32_num = Input_PanelPName_Num[indexPNames];
	panel1a_handle.pmq_req_ddata = &mq_cyc_send_data.mq_req_ddata;
	panel1a_handle.u32_panel_index = indexPNames;

	/*get the texts of the hzPanel1a configuration:设置设备名称(激活按钮的文本)、设备ID、模拟量单位、AUTO/MANU按钮文本、FAST/SLOW按钮文本、增减按钮文本*/
	set_hzPanel1a_equipmentName(link_instance/*, apinfo->widget*/);

	//getEquipmentInformationText2(apinfo->widget, &InformationDisplayText);	/*获取信息图符的文本*/
	//spilt_data_informationDisplay_hzP2(indexPNames, InformationDisplayText);

	set_hzPanel1a_quipment_hide(link_instance/*, apinfo->widget*/);		/*设置设备图符隐藏*/
	set_widget_show_hide_hzPanel1a(link_instance/*, apinfo->widget*/, indexPNames);   /*设置面板控件的显示和隐藏（包括设备图符）*/

	ret_init = photon_init_oper_panel( u32_old_ddata_size, &panel1a_handle, &mq_recv_fdata.vars_info);
	//ret_init = OK; /*temp,之后去掉*/
	if ( ret_init != OK )
	{
		//openAllPanelNum = 1;
		SYS_INFO.b_panel = FALSE;
		//PanelPName_NO[indexPNames] = 0;
		u32_ret = photon_show_msg(MSG_INIT_FAILED);
		windowEquipPanel[indexPNames] = P_NULL;//qinshiling 20240729 修改多次打开面板程序异常退出
		widgetEquipPanel[indexPNames] = P_NULL;//qinshiling 20240729 修改多次打开面板程序异常退出
		Input_PanelPName_Num[indexPNames] = 0; //qinshiling 20240729 修改多次打开面板程序异常退出

		return (Pt_END);
	}
	s_u32_panel1a_init_tick = u32_head_send_tick;
	u32_recv_ddata_index[indexPNames] = panel1a_handle.u32_recv_ddata_index;

	windowEquipPanelIndex++;
	//open_times ++;

	u32_ret = photon_open_panel_by_widget_pointer(link_instance);/*将打开面板的指针传给后台*/
	if(u32_ret != OK)
	{
		u32_ret = photon_show_msg(MSG_SET_PANEL_OPENING_STATE_FAILED);
	}

	/*写日志信息*/
	//pFile = fopen("C:\\ide-4.7-workspace\\hzPanel.txt","a");
	//write_log(pFile,"%s","1a:setup is running");


	/* eliminate 'unreferenced' warnings */
	/*link_instance = link_instance, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );*/

	/* eliminate 'unreferenced' warnings */
		cbinfo = cbinfo;
		return( Pt_CONTINUE );
}


int
hzPanel1a_periodRefersh( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	time_t *timer_hzPanel1a_PeriodTime = P_NULL;
	u32_t   indexPNames = 0;
	PtWidget_t *thisWindow;
	thisWindow = ApGetInstance( widget );/* 获取面板窗口实例 */
	u32_t  i;
	u32_t  u32_ret = OK;
	u32_t u32_app_mode = UINT_0;/* 运行模式 */

	u32_app_mode = check_app_mode();/* 获取运行模式 */

#if 0
	if (openAllPanelNum == 16)
	{
		//photon_show_msg("");
		return( Pt_CONTINUE );
	}
#endif
	if(	trace_hzPanel1a_open_mode == NC_SCREEN_PANEL1A )
	{
		indexPNames = SET_UP_NC_SCREEN;
	}
	else
	{
		for ( i = 0; i < max_panel_num; i++ )
		{
			if ( windowEquipPanel[i] == thisWindow )
			{
				indexPNames = i;
				break;
			}
		}
	}

#if 0
	if (PanelPName_NO[indexPNames] == 1)
	{
		u32_ret = photon_show_msg("变量组态缺少!");
		PanelPName_NO[indexPNames] = 0;
		return( Pt_END );
	}
#endif

	for ( i = 0; i < PANEL_VAR_SIZE_MAX; i++ )
	{
		memset(&ack_hzPanel1a_data[indexPNames*PANEL_VAR_SIZE_MAX + i],UINT_0, sizeof(var_recv_des_t));
	}

	if(mq_cyc_recv_data.mq_recv_ddata.u32_recvTick > s_u32_panel1a_init_tick)
	{
		for ( i = 0; i < Input_PanelPName_Num[indexPNames]; i++ )
		{
			memcpy( &ack_hzPanel1a_data[indexPNames*PANEL_VAR_SIZE_MAX + i], &mq_cyc_recv_data.mq_recv_ddata.var_recv_buf[u32_recv_ddata_index[indexPNames] + i],
							sizeof(var_recv_des_t));
		}
		change_widgetState_hzP1a( thisWindow, indexPNames, Input_PanelPName_Num[indexPNames] );

		u32_t u32_i = 0;
		time_t timer_periodTime =0;
		for (u32_i = OPERATOR_PANEL_1; u32_i < TIMER_NUM_MAX; u32_i++)
		{
			if (thisWindow == g_timerArray[u32_i].parent_widget)
			{
				timer_periodTime = g_timerArray[u32_i].timerPeriod;
				break;
			}
		}

		/*设置需要二次确认按钮的自动弹起*/
		trace_hzPanel1a_auto_manu_btn[indexPNames] = trace_hzP1a_orderBtn_is_auto_up(indexPNames, ApGetWidgetPtr( thisWindow, ABN_hzP1aPtTimerAutoManu ),
										   ApGetWidgetPtr( thisWindow, ABN_hzP1aAutoManuBtn ),
										   ApGetWidgetPtr( thisWindow, ABN_hzP1aExecuteBtn ),
										   trace_hzPanel1a_auto_manu_btn[indexPNames], timer_periodTime);

		/*依据模式设置按钮的状态*/
		set_btnState_by_mode_hzP1a(thisWindow, indexPNames);
	}
	u32_ret = set_hzPanel1a_block_state( thisWindow, u32_app_mode, indexPNames );/* 根据运行模式设置面板的闭锁状态 */

	/*"操作手操器按钮计数器"和“操作计数器”到65535，就从0开始重新计时*/
	if(oper_count >= 65535)
	{
		oper_count = 0;
	}
	if(oper_time_count >= 65535)
	{
		oper_time_count = 0;
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	increase_timer_calltimes(thisWindow);

	return( Pt_CONTINUE );
}

/*依据模式设置按钮的状态*/
static void set_btnState_by_mode_hzP1a( PtWidget_t *theWindow, u32_t indexCount )
{
	char *cur_btnstatei;	/*wkt,get current button state,up or down or gui*/
	char *cur_btnstated;	/*wkt,get current button state,up or down or gui*/
	if(trace_hzPanel1a_fast_slow_mode[indexCount] == NOFASTSLOW1a) /*无FAST/SLOW模式*/
	{
		panelBtnGUI(ApGetWidgetPtr( theWindow, ABN_hzP1aMViBtn ));/*MV/LV增、减按钮闭锁*/
		panelBtnGUI(ApGetWidgetPtr( theWindow, ABN_hzP1aMVdBtn ));
	}
	else if(trace_hzPanel1a_auto_manu_mode[indexCount] == AUTO1a) /*有FAST/SLOW模式，且，自动模式*/
	{
		panelBtnGUI(ApGetWidgetPtr( theWindow, ABN_hzP1aMViBtn ));/*MV/LV增、减按钮闭锁*/
		panelBtnGUI(ApGetWidgetPtr( theWindow, ABN_hzP1aMVdBtn ));
	}
	else if( (trace_hzPanel1a_auto_manu_mode[indexCount] == MANU1a)
			|| (trace_hzPanel1a_auto_manu_mode[indexCount] == FORCEMANU1a) ) /*有FAST/SLOW模式，且，手动模式*/
	{
		if ( activateState[indexCount] == 1 ) /*激活状态*/
		{

			/* MV增按钮 */
			PtGetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aMViBtn ), OB_STATE, &cur_btnstatei, 0);
			if (OB_DOWN == *cur_btnstatei)
			{
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aMViBtn ), OB_COLOR, PgRGB(255,255,255), 0);
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aMViBtn ), OB_BACKCOLOR, PgRGB(97,97,97), 0);
			}
			else
			{
				panelBtnFloat(ApGetWidgetPtr( theWindow, ABN_hzP1aMViBtn ));/*MV/LV增、减按钮弹起*/

				if (trace_hzPanel1a_MVi_btn_green[indexCount] == 1)
				{
//					PtSetResource(ApGetWidgetPtr(theWindow, ABN_hzP1aMViBtn), OB_BACKCOLOR, PgRGB(60, 200, 150), 0);
//					PtSetResource(ApGetWidgetPtr(theWindow, ABN_hzP1aMViBtn), OB_COLOR, Pg_BLACK, 0);
				}
			}
			/* MV减按钮 */
			PtGetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aMVdBtn ), OB_STATE, &cur_btnstated, 0);
			if (OB_DOWN == *cur_btnstated)
			{
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aMVdBtn ), OB_COLOR, PgRGB(255,255,255), 0);
				PtSetResource(ApGetWidgetPtr( theWindow, ABN_hzP1aMVdBtn ), OB_BACKCOLOR, PgRGB(97,97,97), 0);
			}
			else
			{
				panelBtnFloat(ApGetWidgetPtr( theWindow, ABN_hzP1aMVdBtn ));
				if (trace_hzPanel1a_MVd_btn_green[indexCount] == 1)
				{
//					PtSetResource(ApGetWidgetPtr(theWindow, ABN_hzP1aMVdBtn), OB_BACKCOLOR, PgRGB(60, 200, 150), 0);
//					PtSetResource(ApGetWidgetPtr(theWindow, ABN_hzP1aMVdBtn), OB_COLOR, Pg_BLACK, 0);
				}
			}
		}
		else
		{
			panelBtnGUI(ApGetWidgetPtr( theWindow, ABN_hzP1aMViBtn ));/*MV/LV增、减按钮闭锁*/
			panelBtnGUI(ApGetWidgetPtr( theWindow, ABN_hzP1aMVdBtn ));
		}
	}
	else
	{
		/*do nothing*/
	}

	/* MV减按钮 */
	if (trace_hzPanel1a_MVd_btn_green[indexCount] == 1)
	{
		PtSetResource(ApGetWidgetPtr(theWindow, ABN_hzP1aMVdBtn), OB_BACKCOLOR, PgRGB(60, 200, 150), 0);
	}

	/* MV增按钮 */
	if (trace_hzPanel1a_MVi_btn_green[indexCount] == 1)
	{
		PtSetResource(ApGetWidgetPtr(theWindow, ABN_hzP1aMViBtn), OB_BACKCOLOR, PgRGB(60, 200, 150), 0);
	}
}


/*NC调屏打开面板*/
int hzPanel1a_NC_screen_setUp( PtWidget_t *link_instance, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	//设置打开面板的设备图符在列表中的位置
	/*count_open_hzPanel2_times();*/
	char    buffer[40];
	memset( buffer, UINT_0, sizeof(buffer));
	u32_t   i;
	u32_t   firstOpenPanel = 0;
	u32_t   ret_reset = OK;
	u32_t   ret_init = OK;
	u32_t   u32_ret = OK;
	panel_handle_t panel1a_handle = {0};
	u32_t   indexPNames = 0;
	dev_widget_color_t st_dev_widget_color = { 0 };

	//thisFlowChart = ApGetInstance( apinfo->widget );/*获取流程图指针*/
	for ( i = 0; i < max_panel_num; i++ )
	{
		if ( windowEquipPanel[i] != P_NULL )
		{
			firstOpenPanel = 0;
			break;
		}
		firstOpenPanel = 1;
	}
	trace_hzPanel1a_open_mode = NC_SCREEN_PANEL1A;
	indexPNames = SET_UP_NC_SCREEN;
	windowEquipPanel[indexPNames] = link_instance;
	widgetEquipPanel[indexPNames] = P_NULL;
	SYS_INFO.b_panel = TRUE;
	Input_PanelPName_Num[indexPNames] = 0;
	activateState[indexPNames] = 0;

#if 0
	for ( indexValve = 0; indexValve < max_panel_num; indexValve++ )
	{
		if ( windowEquipPanel[indexValve] == P_NULL )
		{
			windowEquipPanel[indexValve] = link_instance;
			widgetEquipPanel[indexValve] = apinfo->widget;
			SYS_INFO.b_panel = TRUE;
			//PanelPName_NO[indexValve] = 0;
			Input_PanelPName_Num[indexValve] = 0;
			indexPNames = indexValve;
			activateState[indexPNames] = 0;
			break;
		}
	}
	if ( windowEquipPanelIndex == max_panel_num )/*面板数量达到上限*/
	{
		openAllPanelNum = 1;
		//PanelPName_NO[indexPNames] = 0;
		u32_ret = photon_show_msg("打开面板数达到上限!");
		//		SYS_INFO.b_panel = FALSE;
		return( Pt_END );
	}

	if ( indexValve >= windowEquipPanelIndex )
	{
		windowEquipPanel[windowEquipPanelIndex] = link_instance;
		widgetEquipPanel[windowEquipPanelIndex] = apinfo->widget;
		SYS_INFO.b_panel = TRUE;
		indexPNames = windowEquipPanelIndex;
	}
#endif

	/*NC调屏打开面板:获取设备图符（面板）*/
	set_hzPanel1a_NC_screen_equipmentWidget(link_instance);

#if 0
	if(0 == equipment_widget_index_hzP1a)
	{
		u32_ret = photon_show_msg("该设备图符无法打开1a号面板");
		windowEquipPanel[windowEquipPanelIndex] = P_NULL;
		widgetEquipPanel[windowEquipPanelIndex] = P_NULL;

		openAllPanelNum = 1;
		//PanelPName_NO[indexPNames] = 0;

		SYS_INFO.b_panel = FALSE;
		return (Pt_END);
	}
#endif

	sprintf( buffer, "HZPanel1a%s", "NC_screen" );
	PtSetResource( link_instance, Pt_ARG_WINDOW_TITLE, buffer, 0 );       /*put in the text of window title(hzPanel2)*/
	//PtSetResource( apinfo->widget, Pt_ARG_FLAGS, Pt_BLOCKED, Pt_BLOCKED );/*闭锁打开面板的设备图符*/

	/*get the PNames of the hzPanel1a configuration*/
	u32_ret = get_PNames_hzPanel1a_configuration(link_instance, indexPNames);	/*获取打开面板的设备图符组态的变量*/
	if ( OK != u32_ret)/*获取面板变量组态失败*/
	{
		u32_ret = photon_show_msg(MSG_GET_PANEL_SETTING_FAILED);
		//PanelPName_NO[indexPNames] = 0;
		windowEquipPanel[indexPNames] = P_NULL;//qinshiling 20240729 修改多次打开面板程序异常退出
		widgetEquipPanel[indexPNames] = P_NULL;//qinshiling 20240729 修改多次打开面板程序异常退出
		Input_PanelPName_Num[indexPNames] = 0; //qinshiling 20240729 修改多次打开面板程序异常退出

		SYS_INFO.b_panel = FALSE;
		return (Pt_END);
	}
	else
	{
		for ( i = 0; i < u32_max_pnames_num_hzPanel1a; i++ )
		{
			if(1 == PanelPName_NO[indexPNames*PANELPNAME_REQUEST_NUM + i]) /*组态变量点*/
			{
				PanelPName_Requst[indexPNames*PANELPNAME_REQUEST_NUM + Input_PanelPName_Num[indexPNames]] = PName_Request[indexPNames*PANELPNAME_REQUEST_NUM + i];
				Input_PanelPName_Num[indexPNames]++;
			}
		}
	}
	if(	trace_hzPanel1a_open_mode == NC_SCREEN_PANEL1A )
	{
		 devType = p_st_ana_hzPanel1a.u32_dev_type;
		 Set_hzPanel1a_NCWidgetIndex(link_instance, indexPNames);

		 /* 设置图符填充色 */
		 st_dev_widget_color.st_fill_color_on = p_st_ana_hzPanel1a.st_fillcolor_open;
		 st_dev_widget_color.st_fill_color_mid = p_st_ana_hzPanel1a.st_fillcolor_middle;
		 st_dev_widget_color.st_fill_color_off = p_st_ana_hzPanel1a.st_fillcolor_close;
		 st_dev_widget_color.st_fill_color_undef = p_st_ana_hzPanel1a.st_fillcolor_undefine;
		 set_widget_color(now_equipmentWidget_hzP1a[indexPNames], WIDGET_TYPE_DEVICE, &st_dev_widget_color);
		 set_dev_widget_direction(now_equipmentWidget_hzP1a[indexPNames], devType); // ljx
	}
	if ( firstOpenPanel == 1 )
	{
		u32_old_ddata_size = mq_cyc_send_data.mq_req_ddata.u32_num;
		ret_reset = photon_reset_panel_vars( u32_old_ddata_size, &mq_cyc_send_data.mq_req_ddata, &mq_recv_fdata.vars_info );
	}

	for ( i = 0; i < Input_PanelPName_Num[indexPNames]; i++)
	{
		strcpy(	panel1a_handle.panel_req_ddata.ch_var_name_buffer[i],PanelPName_Requst[indexPNames*PANELPNAME_REQUEST_NUM + i]);
	}

	panel1a_handle.panel_req_ddata.u32_num = Input_PanelPName_Num[indexPNames];
	panel1a_handle.pmq_req_ddata = &mq_cyc_send_data.mq_req_ddata;
	panel1a_handle.u32_panel_index = indexPNames;

	/*get the texts of the hzPanel1a configuration:设置设备名称(激活按钮的文本)、设备ID、模拟量单位、AUTO/MANU按钮文本、FAST/SLOW按钮文本、增减按钮文本*/
	set_hzPanel1a_equipmentName(link_instance/*, apinfo->widget*/);

	set_hzPanel1a_quipment_hide(link_instance/*, apinfo->widget*/);		/*设置设备图符隐藏*/
	set_widget_show_hide_hzPanel1a(link_instance/*, apinfo->widget*/, indexPNames);   /*设置面板控件的显示和隐藏（包括设备图符）*/

	ret_init = photon_init_oper_panel( u32_old_ddata_size, &panel1a_handle, &mq_recv_fdata.vars_info);
	//ret_init = OK; /*temp,之后去掉*/
	if ( ret_init != OK )
	{
		//openAllPanelNum = 1;
		SYS_INFO.b_panel = FALSE;
		//PanelPName_NO[indexPNames] = 0;
		u32_ret = photon_show_msg(MSG_INIT_FAILED);
		windowEquipPanel[indexPNames] = P_NULL;//qinshiling 20240729 修改多次打开面板程序异常退出
		widgetEquipPanel[indexPNames] = P_NULL;//qinshiling 20240729 修改多次打开面板程序异常退出
		Input_PanelPName_Num[indexPNames] = 0; //qinshiling 20240729 修改多次打开面板程序异常退出

		return (Pt_END);
	}
	s_u32_panel1a_init_tick = u32_head_send_tick;
	u32_recv_ddata_index[indexPNames] = panel1a_handle.u32_recv_ddata_index;

	//windowEquipPanelIndex++;
	//open_times ++;

	u32_ret = photon_open_panel_by_widget_pointer(link_instance);/*将打开面板的指针传给后台*/
	if(u32_ret != OK)
	{
		u32_ret = photon_show_msg(MSG_SET_PANEL_OPENING_STATE_FAILED);
	}

	/* eliminate 'unreferenced' warnings */
	/*link_instance = link_instance, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );*/

	/* eliminate 'unreferenced' warnings */
	cbinfo = cbinfo;
	return( Pt_CONTINUE );
}

/*NC调屏打开面板:获取设备图符（面板）*/
static void set_hzPanel1a_NC_screen_equipmentWidget( PtWidget_t* window )
{
	save_equip_widgets_hzP1a[0] = ABN_hzP1aAirOpDamper;
	save_equip_widgets_hzP1a[1] = ABN_hzP1aMtDrvDamper;
	save_equip_widgets_hzP1a[2] = ABN_hzP1aMtFireDamper;

	save_equip_widgets_hzP1a[3] = ABN_hzP1aHZ_AiroperatedControlValve;
	save_equip_widgets_hzP1a[4] = ABN_hzP1aHZ_AirOperatedValve;
	save_equip_widgets_hzP1a[5] = ABN_hzP1aHZ_MotorDriveValve;
	save_equip_widgets_hzP1a[6] = ABN_hzP1aHZ_PilotoperateOne;
	save_equip_widgets_hzP1a[7] = ABN_hzP1aHZ_PilotoperateTwo;
	save_equip_widgets_hzP1a[8] = ABN_hzP1aHZ_PistonDrivenValve;
	save_equip_widgets_hzP1a[9] = ABN_hzP1aHZ_SafetyValveOne;
	save_equip_widgets_hzP1a[10] = ABN_hzP1aHZ_SolenoidControlValve;
	save_equip_widgets_hzP1a[11] = ABN_hzP1aHZ_SolenoidDriveValve;

	save_equip_widgets_hzP1a[12] = ABN_hzP1aHZ_MotorDriveTreeWayValve;
	save_equip_widgets_hzP1a[13] = ABN_hzP1aHZ_VacuumBreakingValve;
	save_equip_widgets_hzP1a[14] = ABN_hzP1aHZ_MotorControlValve;
	save_equip_widgets_hzP1a[15] = ABN_hzP1aHZ_MotorDriveTreeWayControlValve;

	save_equip_widgets_hzP1a[16] = ABN_hzP1aHZManualValve;
	save_equip_widgets_hzP1a[17] = ABN_hzP1aHZ_Solenoid_Three_Valve;
	save_equip_widgets_hzP1a[18] = ABN_hzP1aHZPistonControlValve;

	save_equip_widgets_hzP1a[19] = ABN_hzP1aHZ_MotorThreeWayOne;
	save_equip_widgets_hzP1a[20] = ABN_hzP1aHZ_MotorThreeWayTwo;
	save_equip_widgets_hzP1a[21] = ABN_hzP1aHZ_MotorThreeWayThree;
	equipment_widget_index_hzP1a = 200;
}

/*关闭面板时，设置当前打开的设备图符（流程图）为未打开状态*/
void set_hzPanels_equipment_widgets_close(u32_t u32_panel_type, PtWidget_t *flowChart, PtWidget_t* widget, u32_t indexPNames)
{
	switch(u32_panel_type)
	{
	case PANEL_TYPE_1A:
	case PANEL_TYPE_1B:
	case PANEL_TYPE_1C:
	case PANEL_TYPE_1D:
		set_valves_widgets_close(flowChart, widget, indexPNames);
		set_dampers_widgets_close(flowChart, widget, indexPNames);
		set_specal_widgets_close(flowChart, widget, indexPNames);
		break;
	case PANEL_TYPE_1E:
	case PANEL_TYPE_1F:
	case PANEL_TYPE_1G:
		set_specal_widgets_close(flowChart, widget, indexPNames);
		break;
	case PANEL_TYPE_2:
	case PANEL_TYPE_7:
		set_pumps_widgets_close(flowChart, widget, indexPNames);
		set_valves_widgets_close(flowChart, widget, indexPNames);
		set_dampers_widgets_close(flowChart, widget, indexPNames);
		set_breakers_widgets_close(flowChart, widget, indexPNames);
		break;
	case PANEL_TYPE_3:
		set_pumps_widgets_close(flowChart, widget, indexPNames);
		set_valves_widgets_close(flowChart, widget, indexPNames);
		set_dampers_widgets_close(flowChart, widget, indexPNames);
		break;
	case PANEL_TYPE_4:
	case PANEL_TYPE_5:
	case PANEL_TYPE_6:
	case PANEL_TYPE_8:
	case PANEL_TYPE_9:
		set_specal_widgets_close(flowChart, widget, indexPNames);
		break;
	default:
		break;
	}
}


/*关闭面板时，设置当前打开的电机类图符（流程图）为未打开状态*/
static void set_pumps_widgets_close(PtWidget_t *flowChart, PtWidget_t* widget, u32_t indexPNames)
{
	//判断打开面板的设备图符是哪个设备图符
	if(PtWidgetIsClass(widget, HZPumpOne) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PO_DevOrDyn,HZ_PODyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PO_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ), HZ_PO_Q, HZ_PONoneS, 0);/*设置为默认状态*/
		}
		return;
	}

	if(PtWidgetIsClass(widget, HZVlumetricPump) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_DevOrDyn, HZ_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Q, Q1a, 0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ), HZ_Q, HZ_AONoneS, 0);/*设置为默认状态*/
		}
		return;
	}

	if(PtWidgetIsClass(widget, HZAirConditionOne) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_HZAirConditionOne_AO_DevOrDyn,HZ_HZAirConditionOne_AODyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_HZAirConditionOne_AO_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_HZAirConditionOne_AO_Q,HZ_HZAirConditionOne_AONone,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZFanOne) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_FO_DevOrDyn,HZ_FODyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_FO_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_FO_Q,HZ_FONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZMotorOne) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MO_DevOrDyn,HZ_MODyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MO_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MO_Q,HZ_MONoneS,0);/*设置为默认状态*/
		}
		return;
	}

	if(PtWidgetIsClass(widget, HZPowerOne) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PRO_DevOrDyn,HZ_PRODyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PRO_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PRO_Q, HZ_PRONoneS, 0);/*设置为默认状态*/
		}
		return;
	}

	if(PtWidgetIsClass(widget, HZCompressor) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ), HZ_HZCompressor_AO_DevOrDyn, HZ_HZCompressor_AODyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ), HZ_HZCompressor_AO_Q, Q1a, 0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ), HZ_HZCompressor_AO_Q, HZ_HZCompressor_AONoneS, 0);/*设置为默认状态*/
		}
		return;
	}
}

/*关闭面板时，设置当前打开的阀门类图符（流程图）为未打开状态*/
static void set_valves_widgets_close(PtWidget_t *flowChart, PtWidget_t* widget, u32_t indexPNames)
{
	//判断打开面板的设备图符是哪个设备图符
	if(PtWidgetIsClass(widget, HZ_AiroperatedControlValve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AIRTwo_SAV_DevOrDyn,HZ_AIRTwo_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AIRTwo_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AIRTwo_SAV_Q,HZ_AIRTwo_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_AirOperatedValve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AIROne_SAV_DevOrDyn,HZ_AIROne_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AIROne_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AIROne_SAV_Q,HZ_AIROne_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorDriveValve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Motor_SAV_DevOrDyn,HZ_Motor_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Motor_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Motor_SAV_Q,HZ_Motor_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_PilotoperateOne) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILOne_SAV_DevOrDyn,HZ_PILOne_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILOne_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILOne_SAV_Q,HZ_PILOne_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_PilotoperateTwo) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILTwo_SAV_DevOrDyn,HZ_PILTwo_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILTwo_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILTwo_SAV_Q,HZ_PILTwo_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_PistonDrivenValve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Piston_SAV_DevOrDyn,HZ_Piston_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Piston_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Piston_SAV_Q,HZ_Piston_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SafetyValveOne) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SAFE_SAV_DevOrDyn,HZ_SAFE_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SAFE_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SAFE_SAV_Q,HZ_SAFE_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SolenoidControlValve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SOLTwo_SAV_DevOrDyn,HZ_SOLTwo_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SOLTwo_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SOLTwo_SAV_Q,HZ_SOLTwo_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_SolenoidDriveValve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SOLOne_SAV_DevOrDyn,HZ_SOLOne_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SOLOne_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SOLOne_SAV_Q,HZ_SOLOne_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorDriveTreeWayValve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorOne_SAV_DevOrDyn,HZ_MotorOne_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorOne_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorOne_SAV_Q,HZ_MotorOne_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_VacuumBreakingValve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_VAC_SAV_DevOrDyn,HZ_VAC_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_VAC_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_VAC_SAV_Q,HZ_VAC_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorControlValve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorTwo_SAV_DevOrDyn,HZ_MotorTwo_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorTwo_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorTwo_SAV_Q,HZ_MotorTwo_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorDriveTreeWayControlValve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThree_SAV_DevOrDyn,HZ_MotorThree_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThree_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThree_SAV_Q,HZ_MotorThree_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZManualValve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZManualValve_SAV_DevOrDyn, HZManualValve_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZManualValve_SAV_Q, Q1a, 0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZManualValve_SAV_Q, HZManualValve_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_Solenoid_Three_Valve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SolenoidThree_SAV_DevOrDyn, HZ_SolenoidThree_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SolenoidThree_SAV_Q, Q1a, 0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SolenoidThree_SAV_Q, HZ_SolenoidThree_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZPistonControlValve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZPistonControlValve_SAV_DevOrDyn, HZPistonControlValve_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZPistonControlValve_SAV_Q, Q1a, 0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZPistonControlValve_SAV_Q, HZPistonControlValve_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}

	//ljx
	if(PtWidgetIsClass(widget, HZ_MotorThreeWayValve) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThreeWayOne_SAV_DevOrDyn, HZ_MotorThreeWayOne_AODDyn,0);/*设置为动态状态*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThreeWayOne_SAV_Q, Q1a, 0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThreeWayOne_SAV_Q, HZ_MotorThreeWayOne_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_MotorThreeWayControlValve) != 0)
	{
			/*设置流程图上打开面板的设备图符状态*/
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThreeWayTwo_SAV_DevOrDyn, HZ_MotorThreeWayTwo_AODDyn,0);/*设置为动态状态*/
			if(trace_hzPanels_ms_state[indexPNames] == 1)
			{
				PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThreeWayTwo_SAV_Q, Q1a, 0);/*设置为质量位为坏状态*/
			}
			else
			{
				PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThreeWayTwo_SAV_Q, HZ_MotorThreeWayTwo_AONoneS,0);/*设置为默认状态*/
			}
			return;
	}
	if(PtWidgetIsClass(widget, HZ_SolenoidThreeWayValve) != 0)
	{
			/*设置流程图上打开面板的设备图符状态*/
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThreeWayThree_SAV_DevOrDyn, HZ_MotorThreeWayThree_AODDyn,0);/*设置为动态状态*/
			if(trace_hzPanels_ms_state[indexPNames] == 1)
			{
				PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThreeWayThree_SAV_Q, Q1a, 0);/*设置为质量位为坏状态*/
			}
			else
			{
				PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MotorThreeWayThree_SAV_Q, HZ_MotorThreeWayThree_AONoneS,0);/*设置为默认状态*/
			}
			return;
	}
}

/*关闭面板时，设置当前打开的挡板类图符（流程图）为未打开状态*/
static void set_dampers_widgets_close(PtWidget_t *flowChart, PtWidget_t* widget, u32_t indexPNames)
{
	//判断打开面板的设备图符是哪个设备图符
	if(PtWidgetIsClass(widget, HZAirOpDamper) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AOD_DevOrDyn,HZ_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/

		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AOD_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_AOD_Q,HZ_AODNoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZMtDrvDamper) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MDD_DevOrDyn,HZ_MDDDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/

		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MDD_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MDD_Q,HZ_MDDNoneS,0);/*设置为默认状态*/
		}
		return;
	}

	if(PtWidgetIsClass(widget, HZMtFireDamper) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MFD_DevOrDyn,HZ_MFDDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/

		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MFD_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_MFD_Q,HZ_MFDNoneS,0);/*设置为默认状态*/
		}
		return;
	}
}

/*关闭面板时，设置当前打开的开关类和其它类图符（流程图）为未打开状态*/
static void set_breakers_widgets_close(PtWidget_t *flowChart, PtWidget_t* widget, u32_t indexPNames)
{
	if(PtWidgetIsClass(widget, HZ_Breaker) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Brea_SAV_DevOrDyn,HZ_Brea_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Brea_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Brea_SAV_Q,HZ_Brea_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_DisconnectorWithFuse) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Dis_SAV_DevOrDyn,HZ_Dis_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Dis_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Dis_SAV_Q,HZ_Dis_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_Contactor) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Contactor_SAV_DevOrDyn,HZ_Contactor_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Contactor_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_Contactor_SAV_Q,HZ_Contactor_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
	if(PtWidgetIsClass(widget, HZ_RawData) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_RawData_SAV_Q,HZ_RawData_AONoneS,0);/*设置为默认状态*/
		return;
	}
	if(PtWidgetIsClass(widget, HZ_IsolationSwitch) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_IsolationSwitch_SAV_DevOrDyn,HZ_IsolationSwitch_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		if(trace_hzPanels_ms_state[indexPNames] == 1)
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_IsolationSwitch_SAV_Q,Q1a,0);/*设置为质量位为坏状态*/
		}
		else
		{
			PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_IsolationSwitch_SAV_Q,HZ_IsolationSwitch_AONoneS,0);/*设置为默认状态*/
		}
		return;
	}
}

/*关闭面板时，设置当前打开的设定值等图符（流程图）为未打开状态*/
static void set_specal_widgets_close(PtWidget_t *flowChart, PtWidget_t* widget, u32_t indexPNames)
{
	if(PtWidgetIsClass(widget, HZGroupController) != 0)/*群组连续控制器图符*/
	{
		/*设置流程图上打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILOne_SAV_DevOrDyn,HZ_PILOne_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_GC_Q,HZ_GCNoneS,0);/*设置为默认状态*/
		return;
	}
	if(PtWidgetIsClass(widget, HZSetValue) != 0)/*设定值图符*/
	{
		/*设置流程图上打开面板的设备图符状态*/
		//PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PILOne_SAV_DevOrDyn,HZ_PILOne_AODDyn,0);/*设置为动态状态*//*widgetEquipPanel[windowEquipPanelIndex]*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_SV_Q, HZ_SVNoneS, 0);/*设置为默认状态*/
		return;
	}
	if(PtWidgetIsClass(widget, HZSRingController) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_RC_Q,HZ_RCNoneS,0);/*设置为默认状态*/
		return;
	}
	if(PtWidgetIsClass(widget, HZPreSelector) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_PS_Q,HZ_PSNoneS,0);/*设置为默认状态*/
		return;
	}
	if(PtWidgetIsClass(widget, HZGroupConSelector) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_GCS_Q,HZ_GCSNoneS,0);/*设置为默认状态*/
		return;
	}
	if(PtWidgetIsClass(widget, HZGpController) != 0)
	{
		/*设置流程图上打开面板的设备图符状态*/
		PtSetResource(ApGetWidgetPtr( flowChart, ApName(widget) ),HZ_GPC_Q,HZ_GPCNoneS,0);/*设置为默认状态*/
		return;
	}
}

static void Set_hzPanel1a_NCWidgetIndex(PtWidget_t* window, u32_t indexCount)
{
	now_equipmentWidget_hzP1a[indexCount] = P_NULL;

	if(devType == DEV_TYPE_AIR_OPERATED_DAMPER)
	{
		equipment_widget_index_hzP1a = 1;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aAirOpDamper);
	}

	if(devType == DEV_TYPE_MOTOR_DRIVE_DAMPER)
	{
		equipment_widget_index_hzP1a = 2;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aMtDrvDamper);
	}

	if(devType == DEV_TYPE_MOTOR_FIRE_DAMPER)
	{
		equipment_widget_index_hzP1a = 3;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aMtFireDamper);//hzP2AirOpDamper
	}

	if(devType == DEV_TYPE_AIR_OPERATED_CONTROL_VALVE)
	{
		equipment_widget_index_hzP1a = 4;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_AiroperatedControlValve);
	}

	if(devType == DEV_TYPE_AIR_OPERATED_VALVE)
	{
		equipment_widget_index_hzP1a = 5;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_AirOperatedValve);
	}

	if(devType == DEV_TYPE_MOTOR_DRIVEN_VALVE)
	{
		equipment_widget_index_hzP1a = 6;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_MotorDriveValve);
	}

	if(devType == DEV_TYPE_PILOT_OPERATED_SAFETY_VALVE1)
	{
		equipment_widget_index_hzP1a = 7;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_PilotoperateOne);
	}

	if(devType == DEV_TYPE_PILOT_OPERATED_SAFETY_VALVE2)
	{
		equipment_widget_index_hzP1a = 8;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_PilotoperateTwo);
	}

	if(devType == DEV_TYPE_PISTON_DRIVEN_VALVE)
	{
		equipment_widget_index_hzP1a = 9;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_PistonDrivenValve);
	}

	if(devType == DEV_TYPE_SAFETY_VALVE)
	{
		equipment_widget_index_hzP1a = 10;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_SafetyValveOne);
	}

	if(devType == DEV_TYPE_SOLENOID_CONTROL_VALVE)
	{
		equipment_widget_index_hzP1a = 11;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_SolenoidControlValve);
	}

	if(devType == DEV_TYPE_SOLENOID_DRIVE_VALVE)
	{
		equipment_widget_index_hzP1a = 12;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_SolenoidDriveValve);
	}

	if(devType == DEV_TYPE_MOTOR_DEIVE_THREE_WAY_VALVE)
	{
		equipment_widget_index_hzP1a = 13;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_MotorDriveTreeWayValve);
	}

	if(devType == DEV_TYPE_VACUUM_BRAKING_VALVE)
	{
		equipment_widget_index_hzP1a = 14;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_VacuumBreakingValve);
	}

	if(devType == DEV_TYPE_MOTOR_CONTROL_VALVE)
	{
		equipment_widget_index_hzP1a = 15;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_MotorControlValve);
	}

	if(devType == DEV_TYPE_MOTOR_DRIVE_THREE_WAY_CONTROL_VALVE)
	{
		equipment_widget_index_hzP1a = 16;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_MotorDriveTreeWayControlValve);
	}

	if(devType == DEV_TYPE_MANUAL_VALVE)
	{
		equipment_widget_index_hzP1a = 17;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZManualValve);
	}

	if(devType == DEV_TYPE_SOLENOID_THREE_WAY_VALVE)
	{
		equipment_widget_index_hzP1a = 18;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZ_Solenoid_Three_Valve);
	}

	if(devType == DEV_TYPE_PISTON_CONTROL_VALVE)
	{
		equipment_widget_index_hzP1a = 19;
		now_equipmentWidget_hzP1a[indexCount] = ApGetWidgetPtr(window, ABN_hzP1aHZPistonControlValve);
	}

	/*20240801wkt*/
	if(DEV_TYPE_MOTOR_THREE_WAY_VALVE_LR <= devType && DEV_TYPE_MOTOR_THREE_WAY_VALVE_UD >= devType)
	{
		equipment_widget_index_hzP1a = 20;
		now_equipmentWidget_hzP1a[indexCount] = P_NULL;
	}
	if(DEV_TYPE_MOTOR_THREE_WAY_CONTROL_VALVE_LR <= devType && DEV_TYPE_MOTOR_THREE_WAY_CONTROL_VALVE_UD >= devType)
	{
		equipment_widget_index_hzP1a = 21;
		now_equipmentWidget_hzP1a[indexCount] = P_NULL;
	}
	if(DEV_TYPE_MOTOR_SOLENOID_THREE_WAY_CONTROL_VALVE_LR <= devType && DEV_TYPE_MOTOR_SOLENOID_THREE_WAY_CONTROL_VALVE_UD >= devType)
	{
		equipment_widget_index_hzP1a = 22;
		now_equipmentWidget_hzP1a[indexCount] = P_NULL;
	}

	if(devType == DEV_TYPE_GROUP_CONTINUOUS_CONTROLLER)/*群组连续控制器图符*/
	{
		equipment_widget_index_hzP1a = 100;
		now_equipmentWidget_hzP1a[indexCount] = P_NULL;
	}

	if(devType == DEV_TYPE_SET_VALUE)/*设定值图符*/
	{
		equipment_widget_index_hzP1a = 100;
		now_equipmentWidget_hzP1a[indexCount] = P_NULL;
	}
}

/*******************************************************************************
* Function: replace_special_character
* Identifier: SSD-PDT- (Trace to: PDT-)
* Description: 替换字符串中特殊字符
* Input:  pch_str---待替换字符串
* Output: None
* Return: OK---替换成功或未找到特殊字符, ERR_FAILED---错误或替换失败
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
u32_t replace_special_character_hzp1a(char_t *pch_str)
{
	u32_t u32_err = OK;//函数返回值
	char_t *pch_ret = P_NULL;//函数返回值

	if( P_NULL == pch_str )
	{
		u32_err = ERR_FAILED;//无效输入
	}

	if( OK == u32_err )
	{
		if( P_NULL != (pch_ret = strstr( pch_str, M2)))
		{
			str_right_shift_one_hzp1a( pch_ret, UINT_2);//特殊字符占3格字节，需向后移动一位
			memcpy( pch_ret, M_SQUARE, strlen(M_SQUARE));//若找到m2,替换为m²
		}
		else if( P_NULL != (pch_ret = strstr( pch_str, M3)))
		{
			str_right_shift_one_hzp1a( pch_ret, UINT_2);//特殊字符占3格字节，需向后移动一位
			memcpy( pch_ret, M_CUBE, strlen(M_CUBE));//若找到m3,替换为m³
		}
		else
		{
			//待添加其他特殊字符
		}
	}

	return u32_err;
}

/*******************************************************************************
* Function: str_right_shift
* Identifier: SSD-PDT- (Trace to: PDT-)
* Description: 向右移动字符串
* Input:  pch_str---待移动字符串
* Output: None
* Return: OK---移动成功, ERR_FAILED---移动错误
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
u32_t str_right_shift_one_hzp1a( char_t *pch_str, u32_t u32_offset )
{
	u32_t u32_err= OK;//函数返回值
	u32_t u32_index = UINT_0;//计数值
	u32_t u32_len = UINT_0;//字符串长度

	if( P_NULL == pch_str )
	{
		u32_err = ERR_FAILED;//无效输入
	}

	if( OK == u32_err )
	{
		u32_len = strlen( pch_str );//获取字符串长度
		for( u32_index = u32_len+UINT_1; u32_index > u32_offset; u32_index-- )
		{
			pch_str[u32_index] = pch_str[u32_index-UINT_1];
		}
	}

	return u32_err;
}

/*写日志，用于查白屏问题*/
int write_log(FILE *pFile,const char *format,...)
{
	va_list arg;
	int done;

	va_start(arg,format);

	time_t time_log = time(NULL);
	struct tm *tm_log = localtime(&time_log);
	fprintf(pFile,"%04d-%02d-%02d %02d:%02d:%02d",tm_log->tm_year + 1900,tm_log->tm_mon+1,tm_log->tm_mday,
			                                      tm_log->tm_hour,tm_log->tm_min,tm_log->tm_sec);
	done = vfprintf(pFile,format,arg);
	va_end(arg);

	fflush(pFile);
	return done;
}





