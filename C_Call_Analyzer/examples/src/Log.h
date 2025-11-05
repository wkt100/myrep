/******************************************************************************
* Copyright (C): CTEC
* Filename: Log.h
* Author: Xie Xinxin
* Date: 2017/10/12  11:31
* Version: A001
* Description : log list
* History:  <author>     <date>            <version>      <description>
*           Xie Xinxin   2017/10/12        A001           create this file
*
*****************************************************************************/
#ifndef LOG_H_
#define LOG_H_

#include "photon_data_com.h"

/*******************************************************************************/
#define LOG_ROW_MAX         15				/*操作日志画面每页最多显示15条*/
#define TMP_INFO_LEN        270U			/*每条日志长度为270字节*/
#define LOG_REAL_NUM        10				/*实时显示10条*/

enum TM_LOG
{
	TM_LOG_YEAR = 0U,
	TM_LOG_MON,
	TM_LOG_DAY,
	TM_LOG_HOUR,
	TM_LOG_MIN,
	TM_LOG_SEC,
	TM_LOG_SIZE,
};

/*******************************************************************************/

void clearInfo();//清空显示的集合
i32_t existsNull(logInfo loginfo);//判断是否有空元素
void InitTemplate_Log(PtWidget_t *widget);
void UpdateLogInfo();//显示当前屏信息
void setlogBtnDisable();//设置筛选按钮不可用
void setlogBtnEnable();//设置筛选按钮启用
void logBtnGUI( PtWidget_t *PWidget );//设置按钮闭锁状态(使用于无ARM时)
void logBtnFloat( PtWidget_t *PWidget );//设置按钮弹起启用状态(使用于无ARM时)
void logBtnSink( PtWidget_t *PWidget );//设置按钮按下状态(使用于无ARM时)
//void logbutton_state();//设置按钮的状态
void SetCheckBoxState(PtWidget_t *widget, i32_t bCheck);//设置CheckBox状态
i32_t InverseValve(i32_t shVal);//取反函数
u32_t getLogRequireNum(u32_t type);//获得请求的条数
firm_bool_t getLogCurrentScid(scid_time_t *logCurrentTime);//获得当前的系统时间
firm_bool_t checkLogTime( PtWidget_t *pWidget, u32_t tmType, u32_t *pTime,u32_t tmFlag);//检查输入时间是否有效
firm_bool_t checkLogDay( u32_t u32_year, u32_t u32_mon, u32_t u32_day );//判断DAY是否有效
firm_bool_t checkLogLeapYear( u32_t u32_year );//判断是否为闰年
firm_bool_t CheckLogTimeValid(time_adjust_t *logStartTime, time_adjust_t *logEndTime);//输入时间是否早于系统时间,是否小于180天

#endif /*LOG_H_ */
