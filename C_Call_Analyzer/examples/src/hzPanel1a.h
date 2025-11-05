/*******************************************************************************
* Copyright (C): CTEC
* Filename: hzPanel1a.h
* Author: Xie Xinxin
* Date:   2021/07/26 11:20
* Version: A001
* Description : HZPanel1a
* History:  <author>     <date>            <version>      <description>
*           Xie Xinxin  2021/07/26         A001           create this file
*******************************************************************************/
#ifndef HZPANEL1A_H_
#define HZPANEL1A_H_


/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>


/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"
#include "typedef.h"
#include "photon_data_com.h"
#include "button.h"
#include "time.h"
#include "scid_time.h"
#include "photon_file_parse.h"
#include "photon_mq_protocol.h"
#include "photon_module_parse.h"


#define  PANELPNAME_REQUEST_NUM  24 /*点名数*/
//#define  AUTO_MANU_STATE           (6U) /*AUTO/MANU的状态*/
#define  SET_UP_NC_SCREEN        15    /*定义存储调屏的数据索引为15*/

#define UPPER_TRIANGULAR  "▲"  /*"&#x25B2;"*/  /*▲*/
#define LOWER_TRIANGULAR  "▼"  /*"&#x25BC;"*/  /*▼*/

#define ONE_TRIANGULAR "&#x2594;" /*▔*/
#define EQUAL_TRIANGULAR "&#x3013;" /*〓*/


i32_t  trace_hzPanels_ms_state[16] = {0U};/*跟踪设备图符的质量位状态，默认为0质量位是好的，若为1则为质量位坏*/
extern u32_t windowEquipPanelIndex;                         	    //打开面板计数器,确保<=3
extern PtWidget_t *windowEquipPanel[16];            	        //存储打开的面板
extern PtWidget_t *widgetEquipPanel[16];             	    //存储打开面板的图符(设备图符)
extern u32_t Input_PanelPName_Num[16];                           //跟踪图符绑定点数
extern char_t *PanelPName_Requst[16*PANELPNAME_REQUEST_NUM];   //存储图符绑定的点名
extern char_t PName_Request[16*PANELPNAME_REQUEST_NUM][VAR_NAME_SIZE_MAX];
extern u32_t PanelPName_NO[16*PANELPNAME_REQUEST_NUM];                              //跟踪图符组点情况(1:存在没有组点的图符;0:不存在没有组点的图符)
extern u32_t activateState[16];                              //跟踪激活按钮的状态(up:0;down:1)
extern u32_t u32_old_ddata_size;
extern u32_t oper_count;              /*操作手操器按钮计数器*/
extern u32_t oper_time_count;         /*操作计数器*/
extern u32_t max_panel_num;
extern PtWidget_t *dragged_panel; //指向待平移的面板
extern PtWidget_t *thisFlowChart;  //获得流程图指针

//FILE *pFile;

enum testAndInsulation1a
{
	TSRSNone = 1,	  /*设备正常*/
	TS1a = 2,		  /*处于试验状态*/
	RS1a = 3,		  /*处于隔离状态*/
	TSandRS1a = 4,	  /*处于试验隔离状态*/
	TSRSRect1a = 5,    /*处于默认状态*/
};

enum QState1a
{
	QNone = 1,	  /*无外框*/
	Q1a = 2,       /*外框为质量位坏*/
	QOpen1a = 3,   /*外框架为打开状态*/
	QRect1a = 4,   /*外框为默认*/
};

enum state1a
{
	START1a = 1,				//START OR FULLON
	STOP1a = 2, 				//STOP OR FULLOFF
	UNDEFINED1a = 3,			//UNDEFINED
	MIDDLE1a = 4,            /*middle state*/
};

enum mode_auto_manu_panel1a
{
	AUTO1a = 6,				/*auto mode*/
	MANU1a = 7,				/*manu mode*/
	FORCEMANU1a = 8,        /*force manu mode*/
	NOAUTOMANU1a = 9,       /*no auto/manu mode*/
};

enum mode_fast_slow_panel1a
{
	FAST1a = 10,				/*fast mode*/
	SLOW1a = 11,				/*slow mode*/
	NOFASTSLOW1a = 12,       /*no fast/slow mode*/
};

enum btn_auto_manu_panel1a
{
	UP1a = -1,			 /*弹起状态*/
	DOWN1a = 1,			/*按下状态*/
};

enum set_up_mode_panel1a
{
	NC_SCREEN_PANEL1A = 13,			/*调屏打开面板*/
	FLOW_CHART_PANEL1A = 14,			/*流程图图符切换打开*/
};


extern void set_operate_button();//导航栏解锁
extern void set_all_disable();////导航栏闭锁
extern u32_t check_app_mode();//获取当前应用模式
extern void panelBtnGUI( PtWidget_t *PWidget );//设置按钮闭锁状态(使用于无ARM时)
extern void panelBtnFloat( PtWidget_t *PWidget );//设置按钮弹起启用状态(使用于无ARM时)
extern void panelBtnSink( PtWidget_t *PWidget );//设置按钮按下闭锁状态(使用于无ARM时)
extern void panelBtnGUINoClr( PtWidget_t *PWidget );
extern void panelBtnFloatNoClr( PtWidget_t *PWidget );
extern void panelBtnSinkNoClr( PtWidget_t *PWidget );

static void change_widgetState_hzP1a( PtWidget_t *theWindow, u32_t indexCount, u32_t PNameNum );//解析获得的数据
static i32_t trace_hzP1a_orderBtn_is_auto_up( u32_t indexPName, PtWidget_t *widget1, PtWidget_t *widget2, PtWidget_t *widget3, i32_t trace, time_t PeriodTime );//命令按钮是否自动弹起
static u32_t set_hzPanel1a_block_state( PtWidget_t *pwidget, u32_t u32_app_mode, u32_t u32_pname_index );//根据当前应用模式设置控件闭锁状态
static void set_widget_show_hide_hzPanel1a(PtWidget_t * link_instance/*, PtWidget_t *widget*/, u32_t indexPNames); //按照设备图符的组态设置面板控件的显示和隐藏
static u32_t get_PNames_hzPanel1a_configuration(PtWidget_t *window, u32_t indexPNames);
static void set_hzPanel1a_ts_rs_state(PtWidget_t * window, PtWidget_t * widget, u32_t state, u32_t equip_index);
static void set_equip_status_hzPanel1a(PtWidget_t *window, PtWidget_t *widget, u32_t status, u32_t equip_index);/*设置设备图符的状态*/
static void set_hzPanel1a_Q_State(PtWidget_t * window, PtWidget_t * widget, u32_t state, u32_t indexCount);/*设置设备图符的质量位*/
static void set_hzPanel1a_equipmentWidget(PtWidget_t *flowChart, PtWidget_t* window, u32_t indexPNames, PtWidget_t* widget);	//设置面板对应的设备图符以及打开当前面板的设备图符
static void set_hzPanel1a_NC_screen_equipmentWidget( PtWidget_t* window );/*NC调屏打开面板:获取设备图符（面板）*/
static void set_hzPanel1a_equipmentName( PtWidget_t* window/*, PtWidget_t *widget*/);	/*设置设备名称(激活按钮的文本)、设备ID、操作按钮C1、C2文本*/
static void set_hzPanel1a_quipment_hide(PtWidget_t * link_instance/*, PtWidget_t *widget*/);	/*按照设备图符的组态设置面板设备图符的隐藏*/
static void set_btnState_by_mode_hzP1a( PtWidget_t *theWindow, u32_t indexCount );/*依据模式设置按钮的状态*/
static void Set_hzPanel1a_NCWidgetIndex(PtWidget_t* window, u32_t indexCount);//设置调屏下打开面板的设备图符号

/*关闭面板时，设置当前打开的电机类图符（流程图）为未打开状态*/
static void set_pumps_widgets_close(PtWidget_t *flowChart, PtWidget_t* widget, u32_t indexPNames);
/*关闭面板时，设置当前打开的阀门类图符（流程图）为未打开状态*/
static void set_valves_widgets_close(PtWidget_t *flowChart, PtWidget_t* widget, u32_t indexPNames);
/*关闭面板时，设置当前打开的挡板类图符（流程图）为未打开状态*/
static void set_dampers_widgets_close(PtWidget_t *flowChart, PtWidget_t* widget, u32_t indexPNames);
/*关闭面板时，设置当前打开的开关类和其它类图符（流程图）为未打开状态*/
static void set_breakers_widgets_close(PtWidget_t *flowChart, PtWidget_t* widget, u32_t indexPNames);
/*关闭面板时，设置当前打开的设定值等图符（流程图）为未打开状态*/
static void set_specal_widgets_close(PtWidget_t *flowChart, PtWidget_t* widget, u32_t indexPNames);


void set_hzPanels_equipment_widgets_close(u32_t u32_panel_type, PtWidget_t *flowChart, PtWidget_t* widget, u32_t indexPNames);/*关闭面板时，设置当前打开的设备图符（流程图）为未打开状态*/

u32_t replace_special_character_hzp1a(char_t *pch_str);
u32_t str_right_shift_one_hzp1a(char_t *pch_str, u32_t u32_offset);

//int write_log(FILE *pFile,const char *format,...);/*写日志，用于查白屏问题*/


#endif /* HZPANEL1A_H_ */
