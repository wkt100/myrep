/******************************************************************************
* Copyright (C): CTEC
* Filename: Log.c
* Author: Xie Xinxin
* Date: 2017/10/12  11:31
* Version: A001
* Description : log list
* History:  <author>     <date>            <version>      <description>
*           Xie Xinxin   2017/10/12        A001           create this file
*
*****************************************************************************/
/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <AP.h>
#include <stdbool.h>

/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"
#include "button.h"
#include "LogManage.h"
#include "Log.h"
#include "typedef.h"
#include "LangSwitch.h"

/*******************************************************************************/
static char_t logSearchStartTime[UINT_32] = {0};           //时间查询的起始时间
static char_t logSearchEndTime[UINT_32] = {0};			    //时间查询的结束时间
static u32_t logTimeSearchNum = 0;					        //日志时间查询的总条数
static time_adjust_t setLogStartTime;                      //画面输入的时间查询的开始时间
static time_adjust_t setLogEndTime;                        //画面输入的时间查询的结束时间
static scid_time_t scidCurrentLogTime;                     //scid time structure(当前的系统时间)
static logInfo allLogInfo[ALL_LOG_NUM];    			    //全部日志
static logInfo logScreenInfo[LOG_ROW_MAX];				    //当前屏显示日志信息
static logInfo logTemScreenInfo[LOG_ROW_MAX];				//当前屏显示日志信息（带有时间的标志位a/b/c...）
/*1表示全日志	2表示操作日志	3表示故障日志	4表示运行日志*/
static u32_t logType = 255;				 				//日志类型,默认为全日志
static u32_t screenlogNum = 0;								//当前屏显示日志的数量
//static char_t Simsun14[MAX_FONT_TAG];                      //字体
static i32_t bCheckedLog = -1;                             //复选框的状态:1:选中;-1:未选中(默认未选中状态)
static firm_bool_t logTimeSearch = false;					//是否需要根据起止时间筛选
static firm_bool_t logRealOrHistory = true;				//true:实时状态;false:历史状态(默认为true)
static u32_t firstlogNum = 0;								//本页第一条
static u32_t currentlogNum = 0;							//当前日志数量
static mq_req_ldata_t  requireLogData;                     //日志请求的结构体
static u32_t replayLogALLNum = 0;                          //接口返回日志总条数
static u32_t replayLogNum = 0;                             //接口返回日志应答条数
static u32_t replayLogPeriodNum = 0;                       //接口返回日志应答时间段内的总条数
static u32_t traceFirstRequire = 1;                        //跟踪第一拍日志请求(有日志信息:0;日志信息为0条:1)
//static u32_t traceBtnSearch = 0;                           //跟踪查询按钮的状态(up:0;down:1)
static void SetLogNewStartTime(void);
u32_t log_covered_flag = 0;									/* add by zsl 20210401 日志是否被覆盖标志 */

/*******************************************************************************/
/***************************控件事件函数*******************************************/
/*******************************************************************************
* Function: Log_Init
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description: 进入日志画面时的初始化函数
* Input:	PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
Log_Init( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	u32_t iLog = 0;  //索引
	u32_t retLog = OK;
	u32_t u32_ret = OK;
	firm_bool_t b_ret = SUCCESS;
	traceFirstRequire = 1;

	logRealOrHistory = true;
	logTimeSearch = false;
	//scid_time_t scidCurrentTime;   // scid time structure
	InitTemplate_Log(widget);
	/* get current time */
	memset(&scidCurrentLogTime, UINT_0, sizeof(scid_time_t));
	/* time string, format:"YYYY-MM-DD HH:MM:SS:MMM" */
	b_ret = getLogCurrentScid(&scidCurrentLogTime);
	/* qishiling 20190114 start: set current time */
	SetLogNewStartTime();
	/* qishiling 20190114 end: set current time */
	if ( b_ret != SUCCESS )
	{
		/*20200709*/
		/*u32_ret = photon_show_msg( "获取当前时间失败" );*/
		u32_ret = photon_show_msg( MSG_GET_TIME_FAILURE );
	}
	else
	{
		memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
		strcpy(requireLogData.ch_start_time, "0");//起始时间无效,填充0;按结束时间向前查询
		memcpy(&requireLogData.ch_end_time, &scidCurrentLogTime, SCID_TIME_SIZE);//当前的系统时间作为结束时间

		//requireLogNum = LOG_REAL_NUM;
		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 2;
		requireLogData.u32_req_flag = 2;
		requireLogData.u32_log_size = LOG_REAL_NUM;
		memset(&allLogInfo[0], '\0', ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		//start_xiexinxin_20180903_第一拍请求数据为0条时的处理
//qinshiling_20180907：先判断是否为0
		if ((OK != retLog) || (replayLogALLNum == 0))//第一拍请求日志,返回0条数据,此时,第二拍按照第一拍请求日志,即起始时间无效,填充0,当前的系统时间作为结束时间,按结束时间向前查询
		{
			traceFirstRequire = 1;
			screenlogNum = 0;
		}
		else
		{
			traceFirstRequire = 0;
			screenlogNum = replayLogNum;
		}
//qinshiling_20180907：先判断是否为0

		//返回的"应答条数replayLogNum"是当前屏的日志信息


		setlogBtnDisable();
		currentlogNum = replayLogALLNum;
		firstlogNum = currentlogNum - screenlogNum;
		memset(&logScreenInfo[0],'\0',LOG_ROW_MAX*sizeof(logInfo));
		memset(&logTemScreenInfo[0],'\0',LOG_ROW_MAX*sizeof(logInfo));
		for( iLog = 0; iLog < screenlogNum; iLog++ )
		{
			logScreenInfo[iLog] = allLogInfo[iLog];
			logTemScreenInfo[iLog] = allLogInfo[iLog];
		}
		UpdateLogInfo();
	}

	if ( logType == MQ_LOG_TYPE_OPERATION )
	{
		/*20200710*/
		/*PtSetResource(ABW_logTitle, Pt_ARG_TEXT_STRING, "Operation Logs", 0);*/
		PtSetResource(ABW_logTitle, Pt_ARG_TEXT_STRING, MSG_LogPage_Title_OPER, 0);
	}
	else if ( logType == MQ_LOG_TYPE_FAULT )
	{
		/*20200710*/
		/*PtSetResource(ABW_logTitle, Pt_ARG_TEXT_STRING, "Error Logs", 0);*/
		PtSetResource(ABW_logTitle, Pt_ARG_TEXT_STRING, MSG_LogPage_Title_FAULT, 0);
	}
	else if ( logType == MQ_LOG_TYPE_SYS )
	{
		/*20200710*/
		/*PtSetResource(ABW_logTitle, Pt_ARG_TEXT_STRING, "Run Logs", 0);*/
		PtSetResource(ABW_logTitle, Pt_ARG_TEXT_STRING, MSG_LogPage_Title_SYS, 0);
	}
	else//logType==MQ_LOG_TYPE_ALL
	{
		/*20200710*/
		/*PtSetResource(ABW_logTitle, Pt_ARG_TEXT_STRING, "All Logs", 0);*/
		PtSetResource(ABW_logTitle, Pt_ARG_TEXT_STRING, MSG_LogPage_Title_ALL, 0);
	}

	/* (255U) 表示全日志	(1U)表示操作日志	(4U)表示故障日志	 (2U)表示运行日志*/
	if ( logType == MQ_LOG_TYPE_ALL )
	{
		/*PtSetResources(ABW_All_Log, 6, sink_args);
		PtSetResources(ABW_Operation_Log, 6, float_args);
		PtSetResources(ABW_System_Log, 6, float_args);
		PtSetResources(ABW_Falut_Log, 6, float_args);
		*/
		PtSetResource( ABW_All_Log, OB_STATE, OB_DOWN, 0);//按下状态
		logBtnSink( ABW_All_Log );//设置按钮按下状态
		logBtnFloat( ABW_Operation_Log );//设置按钮弹起启用状态
		logBtnFloat( ABW_System_Log );//设置按钮弹起启用状态
		logBtnFloat( ABW_Falut_Log );//设置按钮弹起启用状态

		/*20200710*/
		/*PtSetResource(ABW_PtLabelName, Pt_ARG_TEXT_STRING, "用户名/故障码", 0);
		PtSetResource(ABW_PtLabelObject, Pt_ARG_TEXT_STRING, "标识符", 0);*/
		PtSetResource(ABW_PtLabelName, Pt_ARG_TEXT_STRING, MSG_LogPage_LabelName_ALL, 0);
		PtSetResource(ABW_PtLabelObject, Pt_ARG_TEXT_STRING, MSG_LogPage_LabelObject_ALL, 0);
	}else if ( logType == MQ_LOG_TYPE_OPERATION )
	{
		/*PtSetResources(ABW_All_Log, 6, float_args);
		PtSetResources(ABW_Operation_Log, 6, sink_args);
		PtSetResources(ABW_System_Log, 6, float_args);
		PtSetResources(ABW_Falut_Log, 6, float_args);
		*/
		PtSetResource( ABW_Operation_Log, OB_STATE, OB_DOWN, 0);//按下状态
		logBtnSink( ABW_Operation_Log );//设置按钮按下状态
		logBtnFloat( ABW_All_Log );//设置按钮弹起启用状态
		logBtnFloat( ABW_System_Log );//设置按钮弹起启用状态
		logBtnFloat( ABW_Falut_Log );//设置按钮弹起启用状态

		/*20200710*/
		/*PtSetResource(ABW_PtLabelName, Pt_ARG_TEXT_STRING, "用户名", 0);
		PtSetResource(ABW_PtLabelObject, Pt_ARG_TEXT_STRING, "标识符", 0);*/
		PtSetResource(ABW_PtLabelName, Pt_ARG_TEXT_STRING, MSG_LogPage_LabelName_OPER, 0);
		PtSetResource(ABW_PtLabelObject, Pt_ARG_TEXT_STRING, MSG_LogPage_LabelObject_OPER, 0);
	}else if ( logType == MQ_LOG_TYPE_SYS )
	{
		/*PtSetResources(ABW_All_Log, 6, float_args);
		PtSetResources(ABW_Operation_Log, 6, float_args);
		PtSetResources(ABW_System_Log, 6, sink_args);
		PtSetResources(ABW_Falut_Log, 6, float_args);
		*/
		PtSetResource( ABW_System_Log, OB_STATE, OB_DOWN, 0);//按下状态
		logBtnSink( ABW_System_Log );//设置按钮按下状态
		logBtnFloat( ABW_All_Log );//设置按钮弹起启用状态
		logBtnFloat( ABW_Operation_Log );//设置按钮弹起启用状态
		logBtnFloat( ABW_Falut_Log );//设置按钮弹起启用状态

		/*20200710*/
		/*PtSetResource(ABW_PtLabelName, Pt_ARG_TEXT_STRING, "固定值", 0);
		PtSetResource(ABW_PtLabelObject, Pt_ARG_TEXT_STRING, "标识符", 0);*/
		PtSetResource(ABW_PtLabelName, Pt_ARG_TEXT_STRING, MSG_LogPage_LabelName_SYS, 0);
		PtSetResource(ABW_PtLabelObject, Pt_ARG_TEXT_STRING, MSG_LogPage_LabelObject_SYS, 0);
	}else if ( logType == MQ_LOG_TYPE_FAULT )
	{
		/*PtSetResources(ABW_All_Log, 6, float_args);
		PtSetResources(ABW_Operation_Log, 6, float_args);
		PtSetResources(ABW_System_Log, 6, float_args);
		PtSetResources(ABW_Falut_Log, 6, sink_args);
		*/
		PtSetResource( ABW_Falut_Log, OB_STATE, OB_DOWN, 0);//按下状态
		logBtnSink( ABW_Falut_Log );//设置按钮按下状态
		logBtnFloat( ABW_All_Log );//设置按钮弹起启用状态
		logBtnFloat( ABW_Operation_Log );//设置按钮弹起启用状态
		logBtnFloat( ABW_System_Log );//设置按钮弹起启用状态

		/*20200710*/
		/*PtSetResource(ABW_PtLabelName, Pt_ARG_TEXT_STRING, "故障码", 0);
		PtSetResource(ABW_PtLabelObject, Pt_ARG_TEXT_STRING, "标识符", 0);*/
		PtSetResource(ABW_PtLabelName, Pt_ARG_TEXT_STRING, MSG_LogPage_LabelName_FAULT, 0);
		PtSetResource(ABW_PtLabelObject, Pt_ARG_TEXT_STRING, MSG_LogPage_LabelObject_FAULT, 0);
	}
	else
	{
		//不做处理
	}
	//logBtnFloat( ABW_btnLogSearch );//设置按钮弹起启用状态

	// ljx
	g_timerArray[LOG_DATA_REFRESH].widget = ABW_RealTimer;
	g_timerArray[LOG_DATA_REFRESH].parent_widget = widget;
	g_timerArray[LOG_DATA_REFRESH].timerCallBackFunc = periodRefersh;
	g_timerArray[LOG_DATA_REFRESH].data = NULL;
	g_timerArray[LOG_DATA_REFRESH].timerPeriod = UINT_500;

	char_t ch_label_text[UINT_30] = { 0 };
	char_t* pch_label_text = ch_label_text;
	PtGetResource(ABW_LogListProjectName, Pt_ARG_TEXT_STRING, &pch_label_text, 0);
	if ((NULL  != strstr(pch_label_text, UNIT_STAR)) && (g_ch_project_info[0] != '\0'))
	{
		PtSetResource(ABW_LogListProjectName, Pt_ARG_TEXT_STRING, g_ch_project_info, UINT_0);
	}
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}

/*******************************************************************************
* Function: InitTemplate_Log
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description: 初始化页眉
* Input:	PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
void InitTemplate_Log(PtWidget_t *widget)
{
	u32_t u32_err = OK;
	SYS_INFO.pwork_mode_label = ABW_workmode_Log;
	SYS_INFO.papp_mode_label = ABW_appmode_Log;
	SYS_INFO.perr_label = ABW_error_Log;
	SYS_INFO.parc = ABW_arc_Log;
	SYS_INFO.pdate_label = ABW_date_Log;
	if ( 0 != strcmp( "init_page", ApInstanceName( pCUR_WIN)))
	{
		clean_timer(pCUR_WIN);
		PtDestroyWidget( pCUR_WIN); //销毁当前页面
	}
	pCUR_WIN = widget;
	u32_err = photon_init_sys_info( &SYS_INFO, &mq_cyc_send_data.mq_req_ddata );//初始化系统信息

}
//周期刷新
/*******************************************************************************
* Function: periodRefersh
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:	PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
periodRefersh( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	u32_t iLog = 0;
	u32_t retLog = OK;
	u32_t u32_ret = OK;

	if ( logRealOrHistory )//实时状态
	{
		if ( logTimeSearch )//时间查询
		{
			return( Pt_CONTINUE );
		}

		firm_bool_t b_ret = SUCCESS;
		/* get current time */
		memset(&scidCurrentLogTime,UINT_0,sizeof(scid_time_t));
		/* time string, format:"YYYY-MM-DD HH:MM:SS:MMM" */
		b_ret = getLogCurrentScid(&scidCurrentLogTime);
		if ( b_ret != SUCCESS )
		{

			/*20200710*/
			/*u32_ret = photon_show_msg( "获取当前时间失败" );*/
			u32_ret = photon_show_msg( MSG_GET_TIME_FAILURE );
		}
		else
		{
			//start_xiexinxin_20180903_第一拍请求数据为0条时的处理
			if ( traceFirstRequire == 1 )
			{

				memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
				strcpy(requireLogData.ch_start_time, "0");//起始时间无效,填充0;按结束时间向前查询
				memcpy(&requireLogData.ch_end_time, &scidCurrentLogTime, SCID_TIME_SIZE);//当前的系统时间作为结束时间

				//requireLogNum = LOG_REAL_NUM;
				requireLogData.u32_log_type = logType;
				requireLogData.u32_init_flag = 2;
				requireLogData.u32_req_flag = 2;
				requireLogData.u32_log_size = LOG_REAL_NUM;
				memset(&allLogInfo, '\0', ALL_LOG_NUM*sizeof(logInfo));
				retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
				if ( retLog != OK )
				{
					traceFirstRequire = 1;
					return( Pt_CONTINUE );
				}
				//start_xiexinxin_20180904_第一拍请求数据为0条时的处理
				if ( replayLogALLNum == 0 )//第一拍请求日志,返回0条数据,此时,第二拍按照第一拍请求日志,即起始时间无效,填充0,当前的系统时间作为结束时间,按结束时间向前查询
				{
					traceFirstRequire = 1;
					screenlogNum = 0;
				}
				else
				{
					//返回的"应答条数replayLogNum"是当前屏的日志信息
					traceFirstRequire = 0;
					screenlogNum = replayLogNum;
				}
				//end_xiexinxin_20180904_第一拍请求数据为0条时的处理

				//setlogBtnDisable();
				currentlogNum = replayLogALLNum;
				firstlogNum = currentlogNum - screenlogNum;
				memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
				memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
				for ( iLog = 0; iLog < screenlogNum; iLog++ )
				{
					logScreenInfo[iLog] = allLogInfo[iLog];
					logTemScreenInfo[iLog] = allLogInfo[iLog];
				}
				UpdateLogInfo();
			}
			//end_xiexinxin_20180903_第一拍请求数据为0条时的处理
			else
			{
				memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
				strncpy(requireLogData.ch_start_time, logTemScreenInfo[0].logTime, UINT_24);//起始时间,结束时间都有效;按起始时间向后查询
				memcpy(&requireLogData.ch_end_time,&scidCurrentLogTime,SCID_TIME_SIZE);//当前的系统时间作为结束时间

				requireLogData.u32_log_type = logType;
				requireLogData.u32_init_flag = 1;
				requireLogData.u32_req_flag = 1;
				requireLogData.u32_log_size = getLogRequireNum(logType);

				memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
				retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
				if ( retLog != OK )
				{
					return( Pt_CONTINUE );
				}
				//返回的"应答条数replayLogNum"是当前屏的日志信息
				currentlogNum = replayLogALLNum;
				if ( replayLogNum > LOG_ROW_MAX )
				{
					//screenlogNum = replayLogNum % (LOG_ROW_MAX - 1);
					screenlogNum = LOG_REAL_NUM;
				}
				else
				{
					screenlogNum = replayLogNum;
				}
				firstlogNum = currentlogNum - screenlogNum;
				memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
				memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
				for ( iLog = 0; iLog < screenlogNum; iLog++ )
				{
					logScreenInfo[iLog] = allLogInfo[ replayLogNum - screenlogNum + iLog ];
					logTemScreenInfo[iLog] = allLogInfo[replayLogNum - screenlogNum + iLog ];
				}
				UpdateLogInfo();
			}
		}
	}

	g_timerArray[LOG_DATA_REFRESH].callBackTimes++;
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}
//切换到实时模式下
/*******************************************************************************
* Function: btnLogReal_click
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:	PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
btnLogReal_click( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	u32_t iLog = 0;
	u32_t retLog = OK;
	u32_t u32_ret = OK;
	firm_bool_t b_ret = SUCCESS;

	/*PtSetResources(ABW_btnLogReal, 6, sink_args);
	PtSetResources(ABW_btnLogHistory, 6, float_args);
	*/
	logBtnSink( ABW_btnLogReal );//设置按钮按下状态
	logBtnFloat( ABW_btnLogHistory );//设置按钮弹起启用状态
	logRealOrHistory = true;
	setlogBtnDisable();
	//logBtnFloat( ABW_btnLogSearch );//设置按钮弹起启用状态

	logTimeSearch = false;
	bCheckedLog = -1;
	SetCheckBoxState(ABW_CheckLine_Time, bCheckedLog);

	/* get current time */
	//scid_time_t scidCurrentLogTime;   // scid time structure
	memset(&scidCurrentLogTime, UINT_0, sizeof(scid_time_t));
	/* time string, format:"YYYY-MM-DD HH:MM:SS:MMM" */
	b_ret = getLogCurrentScid(&scidCurrentLogTime);
	if ( b_ret != SUCCESS )
	{
		/*20200710*/
		/*u32_ret = photon_show_msg( "获取当前时间失败" );*/
		u32_ret = photon_show_msg( MSG_GET_TIME_FAILURE );
	}
	else
	{
		memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
		strcpy(requireLogData.ch_start_time,"0");//起始时间无效,填充0;终止时间为当前系统时间;按终止时间向前查询
		memcpy(&requireLogData.ch_end_time,&scidCurrentLogTime,SCID_TIME_SIZE);

		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 2;
		requireLogData.u32_req_flag = 2;
		requireLogData.u32_log_size = LOG_REAL_NUM;//getLogRequireNum(logType);

		memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		if ( retLog != OK )
		{
			return( Pt_CONTINUE );
		}
		//返回的"应答条数replayLogNum"是当前屏的日志信息
		currentlogNum = replayLogALLNum;
		screenlogNum = replayLogNum;
		firstlogNum = currentlogNum - screenlogNum;
		memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		for ( iLog = 0; iLog < screenlogNum; iLog++ )
		{
			logScreenInfo[iLog] = allLogInfo[iLog];
			logTemScreenInfo[iLog] = allLogInfo[iLog];
		}
		UpdateLogInfo();
	}

	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );

}

//切换到历史模式下
/*******************************************************************************
* Function: btnLogHistoryClick
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:	PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
btnLogHistoryClick( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	u32_t iLog = 0;
	u32_t retLog = OK;
	u32_t u32_ret = OK;
	firm_bool_t b_ret = SUCCESS;
	/*PtSetResources(ABW_btnLogHistory, 6, sink_args);
	PtSetResources(ABW_btnLogReal, 6, float_args);
	*/
	logBtnSink( ABW_btnLogHistory );//设置按钮按下状态
	logBtnFloat( ABW_btnLogReal );//设置按钮弹起启用状态
	logRealOrHistory = false;
	setlogBtnEnable();
	//logBtnFloat( ABW_btnLogSearch );//设置按钮弹起启用状态

	logTimeSearch = false;
	bCheckedLog = -1;
	SetCheckBoxState(ABW_CheckLine_Time, bCheckedLog);

	//scid_time_t scidCurrentLogTime;   // scid time structure
	memset(&scidCurrentLogTime,UINT_0,sizeof(scid_time_t));
	b_ret = getLogCurrentScid(&scidCurrentLogTime);
	if ( b_ret != SUCCESS )
	{
		/*20200710*/
		/*u32_ret = photon_show_msg( "获取当前时间失败" );*/
		u32_ret = photon_show_msg( MSG_GET_TIME_FAILURE );
	}
	else
	{
		memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
		strcpy(requireLogData.ch_start_time,"0");//起始时间无效,填充0;终止时间为当前系统时间;按终止时间向前查询
		memcpy(&requireLogData.ch_end_time,&scidCurrentLogTime,SCID_TIME_SIZE);

		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 2;
		requireLogData.u32_req_flag = 2;
		requireLogData.u32_log_size = LOG_REAL_NUM;//getLogRequireNum(logType);

		memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		if ( retLog != OK )
		{
			return( Pt_CONTINUE );
		}
		//返回的"应答条数replayLogNum"是当前屏的日志信息
		currentlogNum = replayLogALLNum;
		screenlogNum = replayLogNum;
		firstlogNum = currentlogNum - screenlogNum;
		memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		for ( iLog = 0; iLog < screenlogNum; iLog++ )
		{
			logScreenInfo[iLog] = allLogInfo[iLog];
			logTemScreenInfo[iLog] = allLogInfo[iLog];
		}
		UpdateLogInfo();

	/*	memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
		strncpy(requireLogData.ch_start_time, logTemScreenInfo[0].logTime, UINT_24);//起始时间,终止时间都有效;按起始时间向后查询
		memcpy(&requireLogData.ch_end_time,&scidCurrentLogTime,SCID_TIME_SIZE);

		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 1;
		requireLogData.u32_req_flag = 1;
		requireLogData.u32_log_size = getLogRequireNum(logType);

		memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		if(retLog != OK)
		{
			return( Pt_CONTINUE );
		}
		currentlogNum = replayLogALLNum;
		if(replayLogNum > LOG_REAL_NUM)
		{
			screenlogNum = LOG_REAL_NUM;
		}
		else
		{
			screenlogNum = replayLogNum;
		}
		firstlogNum = currentlogNum - screenlogNum;
		memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		for(iLog = 0;iLog < screenlogNum;iLog++)
		{
			logScreenInfo[iLog] = allLogInfo[replayLogNum - screenlogNum + iLog];
			logTemScreenInfo[iLog] = allLogInfo[replayLogNum - screenlogNum + iLog];
		}
		UpdateLogInfo();
	*/
	}


	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );

}

//前一页
/*******************************************************************************
* Function: btnLogPrePageClick
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:	PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
btnLogPrePageClick( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	u32_t iLog = 0;
	u32_t retLog = OK;
	u32_t u32_ret = OK;

	//logBtnSink( ABW_btnLogPrePage );//设置按钮按下状态
	//logBtnFloat( ABW_btnLogPageUp );//设置按钮弹起启用状态
	//logBtnFloat( ABW_btnLogPageDown );//设置按钮弹起启用状态
	//logBtnFloat( ABW_btnLogNextPage );//设置按钮弹起启用状态
	//logBtnFloat( ABW_btnLogPreRow );//设置按钮弹起启用状态
	//logBtnFloat( ABW_btnLogNexRow );//设置按钮弹起启用状态
	//traceBtnPage = 1;
	/* add by zsl 20210402 start 保留当前屏第一条的序号 */
	u32_t temp_first_lognum = firstlogNum;
	/* add by zsl 20210402 end 保留当前屏第一条的序号 */

	if ( firstlogNum < 1 )
	{
		//弹框提示已经是第一页了

		/*20200710*/
		/*u32_ret = photon_show_msg( "已经是第一页了!" );*/
		u32_ret = photon_show_msg( MSG_IS_FIRST_PAGE );
		return( Pt_CONTINUE );
	}
	memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
	if ( logTimeSearch )//时间查询
	{
		//“前一页”满屏时:起始时间为输入的时间查询的起始时间;终止时间为上一拍当前屏的第一条信息的时间;按终止时间向前查询
		//“前一页”未满屏时:起始时间为输入的时间查询的起始时间;终止时间为上一拍当前屏的最后一条信息的时间;按终止时间向前查询
		strncpy(requireLogData.ch_start_time, logSearchStartTime, UINT_24);
//		strncpy(requireLogData.ch_start_time, "", 0);
//		if ( firstlogNum >= LOG_ROW_MAX - 1 )//“前一页”满屏的情况
//		{
//			strncpy(requireLogData.ch_end_time, logTemScreenInfo[0].logTime, UINT_24);
//			requireLogData.u32_log_size = LOG_ROW_MAX;
//		}
//		else//“前一页”未满屏的情况
//		{
//			strncpy(requireLogData.ch_end_time, logTemScreenInfo[screenlogNum - 1].logTime ,UINT_24);
//			requireLogData.u32_log_size = 2*LOG_ROW_MAX;
//		}
		/* add by zsl 202100402 start 统一按照15条进行请求 */
		strncpy(requireLogData.ch_end_time, logTemScreenInfo[0].logTime, UINT_24);
		requireLogData.u32_log_size = LOG_ROW_MAX;
		/* add by zsl 202100402 end 统一按照15条进行请求 */

		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 2;
		requireLogData.u32_req_flag = 2;
		memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		if ( retLog != OK )
		{
			return( Pt_CONTINUE );
		}
		if ( firstlogNum >= LOG_ROW_MAX - 1 )//“前一页”满屏的情况
		{
			//返回的"应答条数replayLogNum"是当前屏的日志信息
			screenlogNum = replayLogNum;
			currentlogNum = logTimeSearchNum;
			firstlogNum = firstlogNum - (screenlogNum - 1);
		}
		else//“前一页”未满屏的情况
		{
			if ( replayLogNum >= LOG_ROW_MAX )
			{
				screenlogNum = LOG_ROW_MAX;
			}
			else
			{
				screenlogNum = replayLogNum;
			}
			currentlogNum = logTimeSearchNum;
			firstlogNum = 0;
		}
		memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		for ( iLog = 0; iLog < screenlogNum; iLog++ )
		{
			logScreenInfo[iLog] = allLogInfo[iLog];
			logTemScreenInfo[iLog] = allLogInfo[iLog];
		}

		/* add by zsl 20210402 start 当向前翻的条数不足15条时，对返回数据进行判断，请求日志时间与返回日志时间进行比较，避免弹出错误的覆盖提示 */
		if(!strncmp(requireLogData.ch_end_time,logScreenInfo[temp_first_lognum].logTime, UINT_24))
		{
			log_covered_flag = 0;
		}
		/* add by zsl 20210402 end 当向前翻的条数不足15条时，对返回数据进行判断，请求日志时间与返回日志时间进行比较，避免弹出错误的覆盖提示 */
		UpdateLogInfo();
		return( Pt_CONTINUE );
	}

	//“前一页”满屏时:起始时间无效,填充0;终止时间为上一拍当前屏的第一条信息的时间;按终止时间向前查询
	//“前一页”未满屏时:起始时间无效,填充0;终止时间为上一拍当前屏的最后一条信息的时间;按终止时间向前查询
	strcpy(requireLogData.ch_start_time, "0");
//	if ( firstlogNum >= LOG_ROW_MAX -1 )//“前一页”满屏的情况
//	{
//		strncpy(requireLogData.ch_end_time, logTemScreenInfo[0].logTime, UINT_24);
//		requireLogData.u32_log_size = LOG_ROW_MAX;
//	}
//	else//“前一页”未满屏的情况
//	{
//		strncpy(requireLogData.ch_end_time, logTemScreenInfo[screenlogNum - 1].logTime, UINT_24);
//		requireLogData.u32_log_size = 2*LOG_ROW_MAX;
//	}
	/* add by zsl 20210402 start 统一按照15条进行请求 */
	strncpy(requireLogData.ch_end_time, logTemScreenInfo[0].logTime, UINT_24);
	requireLogData.u32_log_size = LOG_ROW_MAX;
	/* add by zsl 20210402 end 统一按照15条进行请求 */

	/* add by zsl 20210402 start 当向前翻的条数不足15条时，对返回数据进行判断，请求日志时间与返回日志时间进行比较，避免弹出错误的覆盖提示 */
	if(!strncmp(requireLogData.ch_end_time,logScreenInfo[temp_first_lognum].logTime, UINT_24))
	{
		log_covered_flag = 0;
	}
	/* add by zsl 20210402 end 当向前翻的条数不足15条时，对返回数据进行判断，请求日志时间与返回日志时间进行比较，避免弹出错误的覆盖提示 */

	//strcpy(requireLogData.ch_end_time, logTemScreenInfo[0].logTime);
	requireLogData.u32_log_type = logType;
	requireLogData.u32_init_flag = 2;
	requireLogData.u32_req_flag = 2;
	//requireLogData.u32_log_size = LOG_ROW_MAX;
	memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
	retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
	if ( retLog != OK )
	{
		return( Pt_CONTINUE );
	}
	if ( firstlogNum >= LOG_ROW_MAX -1 )//“前一页”满屏的情况
	{
		//返回的"应答条数replayLogNum"是当前屏的日志信息
		screenlogNum = replayLogNum;
		currentlogNum = replayLogALLNum;
		firstlogNum = firstlogNum - (screenlogNum -1);
	}
	else//“前一页”未满屏的情况
	{
		if ( replayLogNum >= LOG_ROW_MAX )
		{
			screenlogNum = LOG_ROW_MAX;
		}
		else
		{
			screenlogNum = replayLogNum;
		}
		currentlogNum = replayLogALLNum;
		firstlogNum = 0;
	}
	memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
	memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
	for ( iLog = 0; iLog < screenlogNum; iLog++ )
	{
		logScreenInfo[iLog] = allLogInfo[iLog];
		logTemScreenInfo[iLog] = allLogInfo[iLog];
	}
	/* add by zsl 20210402 start 当向前翻的条数不足15条时，对返回数据进行判断，请求日志时间与返回日志时间进行比较，避免弹出错误的覆盖提示 */
	if(!strncmp(requireLogData.ch_end_time,logScreenInfo[temp_first_lognum].logTime, UINT_24))
	{
		log_covered_flag = 0;
	}
	/* add by zsl 20210402 end 当向前翻的条数不足15条时，对返回数据进行判断，请求日志时间与返回日志时间进行比较，避免弹出错误的覆盖提示 */
	UpdateLogInfo();


	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}

//最前页
/*******************************************************************************
* Function: btnLogPageUpClick
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:	PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
btnLogPageUpClick( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	u32_t iLog = 0;
	u32_t retLog = OK;
	u32_t u32_ret = OK;

	/*logBtnSink( ABW_btnLogPageUp );//设置按钮按下状态
	logBtnFloat( ABW_btnLogPrePage );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogPageDown );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogNextPage );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogPreRow );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogNexRow );//设置按钮弹起启用状态
	traceBtnPage = 1;*/
	if ( firstlogNum < 1 )
	{
		//弹框提示当前已经是第一页了

		/*20200710*/
		/*u32_ret = photon_show_msg( "已经是第一页了!" );*/
		u32_ret = photon_show_msg( MSG_IS_FIRST_PAGE );
		return( Pt_CONTINUE );
	}
	memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
	if ( logTimeSearch )
	{
		//起始时间为输入的时间查询的起始时间;终止时间为上一拍当前屏的最后一条信息的时间;按终止时间向前查询
		strncpy(requireLogData.ch_start_time, logSearchStartTime, UINT_24);
		//strncpy(requireLogData.ch_end_time, logTemScreenInfo[screenlogNum - 1].logTime, UINT_24);
		strncpy(requireLogData.ch_end_time, logSearchEndTime, UINT_24);

		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 2;
		requireLogData.u32_req_flag = 2;
		requireLogData.u32_log_size = getLogRequireNum(logType);

		memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		if ( retLog != OK )
		{
			return( Pt_CONTINUE );
		}
		currentlogNum = logTimeSearchNum;
		firstlogNum = 0;
		if ( replayLogNum > LOG_ROW_MAX )
		{
			screenlogNum = LOG_ROW_MAX;
		}
		else
		{
			screenlogNum = replayLogNum;
		}
		memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		for (iLog = 0; iLog < screenlogNum; iLog++ )
		{
			logScreenInfo[iLog] = allLogInfo[iLog];
			logTemScreenInfo[iLog] = allLogInfo[iLog];
		}
		UpdateLogInfo();
		return( Pt_CONTINUE );
	}

	//起始时间无效,填充0;终止时间为上一拍当前屏的最后一条信息的时间;按终止时间向前查询
	/* get current time */
	memset(&scidCurrentLogTime, UINT_0, sizeof(scid_time_t));
	/* time string, format:"YYYY-MM-DD HH:MM:SS:MMM" */
	getLogCurrentScid(&scidCurrentLogTime);
	strcpy(requireLogData.ch_start_time,"0");
	memcpy(requireLogData.ch_end_time, &scidCurrentLogTime, SCID_TIME_SIZE);//使用当前时间向前翻页

	requireLogData.u32_log_type = logType;
	requireLogData.u32_init_flag = 2;
	requireLogData.u32_req_flag = 2;
	requireLogData.u32_log_size = getLogRequireNum(logType);

	memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
	retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
	if ( retLog != OK )
	{
		return( Pt_CONTINUE );
	}
	currentlogNum = replayLogALLNum;
	firstlogNum = 0;
	if ( replayLogNum > LOG_ROW_MAX )
	{
		screenlogNum = LOG_ROW_MAX;
	}
	else
	{
		screenlogNum = replayLogNum;
	}
	memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
	memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
	for ( iLog = 0; iLog < screenlogNum; iLog++ )
	{
		logScreenInfo[iLog] = allLogInfo[iLog];
		logTemScreenInfo[iLog] = allLogInfo[iLog];
	}
	UpdateLogInfo();


	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}
//下一页
/*******************************************************************************
* Function: btnLogNextPage
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:	PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
btnLogNextPage( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	u32_t iLog = 0;
	u32_t retLog = OK;
	u32_t u32_ret = OK;
	firm_bool_t b_ret = SUCCESS;

	/*logBtnSink( ABW_btnLogNextPage );//设置按钮按下状态
	logBtnFloat( ABW_btnLogPrePage );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogPageDown );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogPageUp );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogPreRow );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogNexRow );//设置按钮弹起启用状态
	traceBtnPage = 1;*/
	if ( logTimeSearch )
	{
		if ( logTimeSearchNum - firstlogNum <= LOG_ROW_MAX )
		{
			//弹框提示这是最后一页了

			/*20200710*/
			/*u32_ret = photon_show_msg( "已经是最后一页了!" );*/
			u32_ret = photon_show_msg( MSG_IS_LAST_PAGE );
			return ( Pt_CONTINUE );
		}
		memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
		//起始时间为上一拍当前屏的最后一条信息的时间;终止时间为输入的时间查询的结束时间;按起始时间向后查询
		strncpy(requireLogData.ch_start_time, logTemScreenInfo[screenlogNum-1].logTime, UINT_24);
		strncpy(requireLogData.ch_end_time, logSearchEndTime, UINT_24);//当前的系统时间作为结束时间

		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 1;
		requireLogData.u32_req_flag = 1;
		requireLogData.u32_log_size = LOG_ROW_MAX;

		memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		if ( retLog != OK )
		{
			return( Pt_CONTINUE );
		}

		//返回的"应答条数replayLogNum"是当前屏的日志信息
		firstlogNum = firstlogNum + screenlogNum - 1;
		screenlogNum = replayLogNum;
		currentlogNum = logTimeSearchNum;
		memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		for ( iLog = 0; iLog < screenlogNum; iLog++ )
		{
			logScreenInfo[iLog] = allLogInfo[iLog];
			logTemScreenInfo[iLog] = allLogInfo[iLog];
		}
		UpdateLogInfo();
		return( Pt_CONTINUE );
	}

	/* get current time */
	memset(&scidCurrentLogTime,UINT_0,sizeof(scid_time_t));
	/* time string, format:"YYYY-MM-DD HH:MM:SS:MMM" */
	b_ret = getLogCurrentScid(&scidCurrentLogTime);
	if ( b_ret != SUCCESS )
	{
		/*20200710*/
		/*u32_ret = photon_show_msg( "获取当前时间失败" );*/
		u32_ret = photon_show_msg( MSG_GET_TIME_FAILURE );
	}
	else
	{
		memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
		//起始时间为上一拍当前屏的最后一条信息的时间;终止时间为当前的系统时间;按起始时间向后查询
		strncpy(requireLogData.ch_start_time, logTemScreenInfo[screenlogNum-1].logTime, UINT_24);
		memcpy(&requireLogData.ch_end_time,&scidCurrentLogTime,SCID_TIME_SIZE);//当前的系统时间作为结束时间

		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 1;
		requireLogData.u32_req_flag = 1;
		requireLogData.u32_log_size = LOG_ROW_MAX;

		memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		if ( retLog != OK )
		{
			return( Pt_CONTINUE );
		}
		if ( replayLogALLNum - firstlogNum <= LOG_ROW_MAX )
		{
			//弹框提示这是最后一页了

			/*20200710*/
			/*u32_ret = photon_show_msg( "已经是最后一页了!" );*/
			u32_ret = photon_show_msg( MSG_IS_LAST_PAGE );
		}
		else
		{
			//返回的"应答条数replayLogNum"是当前屏的日志信息
			firstlogNum = firstlogNum + screenlogNum - 1;
			screenlogNum = replayLogNum;
			currentlogNum = replayLogALLNum;
			memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
			memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
			for ( iLog = 0; iLog < screenlogNum; iLog++ )
			{
				logScreenInfo[iLog] = allLogInfo[iLog];
				logTemScreenInfo[iLog] = allLogInfo[iLog];
			}
			UpdateLogInfo();
		}
	}


	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );

}
//最后页
/*******************************************************************************
* Function: btnLogPageDown
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:	PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
btnLogPageDown( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	u32_t iLog = 0;
	u32_t retLog = OK;
	u32_t u32_ret = OK;
	firm_bool_t b_ret = SUCCESS;
	/*logBtnSink( ABW_btnLogPageDown );//设置按钮按下状态
	logBtnFloat( ABW_btnLogPrePage );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogNextPage );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogPageUp );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogPreRow );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogNexRow );//设置按钮弹起启用状态
	traceBtnPage = 1;*/
	if ( logTimeSearch )
	{
		if ( logTimeSearchNum - firstlogNum <= LOG_ROW_MAX )
		{
			//弹框提示这是最后一页了

			/*20200710*/
			/*u32_ret = photon_show_msg( "已经是最后一页了!" );*/
			u32_ret = photon_show_msg( MSG_IS_LAST_PAGE );
			return ( Pt_CONTINUE );
		}
		memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
		//起始时间为输入的时间查询的起始时间;终止时间为输入的时间查询的结束时间;按终止时间向前查询
		strncpy(requireLogData.ch_start_time, logSearchStartTime, UINT_24);
		strncpy(requireLogData.ch_end_time, logSearchEndTime, UINT_24);

		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 1;
		requireLogData.u32_req_flag = 2;
		requireLogData.u32_log_size = LOG_ROW_MAX;

		memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		if ( retLog != OK )
		{
			return( Pt_CONTINUE );
		}
		screenlogNum = replayLogNum;
		currentlogNum = logTimeSearchNum;
		firstlogNum = currentlogNum - screenlogNum;
		memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		for ( iLog = 0; iLog < screenlogNum; iLog++ )
		{
			logScreenInfo[iLog] = allLogInfo[iLog];
			logTemScreenInfo[iLog] = allLogInfo[iLog];
		}
		UpdateLogInfo();

		return( Pt_CONTINUE );
	}


	/* get current time */
	memset(&scidCurrentLogTime,UINT_0,sizeof(scid_time_t));
	/* time string, format:"YYYY-MM-DD HH:MM:SS:MMM" */
	b_ret = getLogCurrentScid(&scidCurrentLogTime);
	if( b_ret != SUCCESS )
	{
		/*20200710*/
		/*u32_ret = photon_show_msg( "获取当前时间失败" );*/
		u32_ret = photon_show_msg( MSG_GET_TIME_FAILURE );
	}
	else
	{
		memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
		//起始时间无效;终止时间为当前的系统时间;按终止时间向前查询
		strcpy(requireLogData.ch_start_time,"0");
		memcpy(&requireLogData.ch_end_time,&scidCurrentLogTime,SCID_TIME_SIZE);//当前的系统时间作为结束时间

		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 2;
		requireLogData.u32_req_flag = 2;
		requireLogData.u32_log_size = LOG_ROW_MAX;

		memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		if ( retLog != OK )
		{
			return( Pt_CONTINUE );
		}
		if ( replayLogALLNum - firstlogNum <= LOG_ROW_MAX )
		{
			//弹框提示这是最后一页了

			/*20200710*/
			/*u32_ret = photon_show_msg( "已经是最后一页了!" );*/
			u32_ret = photon_show_msg( MSG_IS_LAST_PAGE );
		}
		else
		{
			screenlogNum = replayLogNum;
			currentlogNum = replayLogALLNum;
			firstlogNum = currentlogNum - screenlogNum;
			memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
			memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
			for ( iLog = 0; iLog < screenlogNum; iLog++ )
			{
				logScreenInfo[iLog] = allLogInfo[iLog];
				logTemScreenInfo[iLog] = allLogInfo[iLog];
			}
			UpdateLogInfo();
		}
	}



	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );

}
//前一条
/*******************************************************************************
* Function: btnLogPreRowClick
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:	PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
btnLogPreRowClick( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	u32_t iLog = 0;
	u32_t retLog = OK;
	u32_t u32_ret = OK;
	/*logBtnSink( ABW_btnLogPreRow );//设置按钮按下状态
	logBtnFloat( ABW_btnLogPrePage );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogNextPage );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogPageUp );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogPageDown );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogNexRow );//设置按钮弹起启用状态
	traceBtnPage = 1;*/

	/* add by zsl 20210402 start 保留当前屏第一条的序号 */
	u32_t temp_first_lognum = firstlogNum;
	/* add by zsl 20210402 start 保留当前屏第一条的序号 */

	if ( firstlogNum < 1 )
	{
		//弹框提示已经是第一页了

		/*20200710*/
		/*u32_ret = photon_show_msg( "已经是第一条了!" );*/
//		u32_ret = photon_show_msg( MSG_IS_FIRST_ITEM );
		u32_ret = photon_show_msg( MSG_IS_FIRST_PAGE );	/*wkt20240903:first page*/
		return( Pt_CONTINUE );
	}
	memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));

	if ( logTimeSearch )
	{
		//上一拍当前屏为满屏时：起始时间为输入的时间查询的起始时间;终止时间为上一拍当前屏的倒数第二条信息的时间;按终止时间向前查询
		//上一拍当前屏未满屏时：起始时间为输入的时间查询的起始时间;终止时间为上一拍当前屏的倒数第一条信息的时间;按终止时间向前查询
		strncpy(requireLogData.ch_start_time, logSearchStartTime, UINT_24);
		if ( screenlogNum < LOG_ROW_MAX )//未满屏时
		{
			strncpy(requireLogData.ch_end_time, logTemScreenInfo[screenlogNum-1].logTime, UINT_24);
			requireLogData.u32_log_size = screenlogNum + 1;
		}
		else//满屏时
		{
			strncpy(requireLogData.ch_end_time, logTemScreenInfo[screenlogNum-2].logTime, UINT_24);
			requireLogData.u32_log_size = LOG_ROW_MAX;
		}
		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 1;
		requireLogData.u32_req_flag = 2;

		memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		if ( retLog != OK )
		{
			return( Pt_CONTINUE );
		}
		//返回的"应答条数replayLogNum"是当前屏的日志信息
		firstlogNum = firstlogNum - 1;
		screenlogNum = replayLogNum;
		currentlogNum = logTimeSearchNum;
		memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		for (iLog = 0; iLog < screenlogNum; iLog++ )
		{
			logScreenInfo[iLog] = allLogInfo[iLog];
			logTemScreenInfo[iLog] = allLogInfo[iLog];
		}

		/* add by zsl 20210402 start 当向前翻的条数不足15条时，对返回数据进行判断，请求日志时间与返回日志时间进行比较，避免弹出错误的覆盖提示 */
		if(!strncmp(requireLogData.ch_end_time,logScreenInfo[temp_first_lognum].logTime, UINT_24))
		{
			log_covered_flag = 0;
		}
		/* add by zsl 20210402 end 当向前翻的条数不足15条时，对返回数据进行判断，请求日志时间与返回日志时间进行比较，避免弹出错误的覆盖提示 */
		UpdateLogInfo();
		return( Pt_CONTINUE );
	}

	//上一拍当前屏为满屏时：起始时间无效,填充0;终止时间为上一拍当前屏的倒数第二条信息的时间;按终止时间向前查询
	//上一拍当前屏未满屏时：起始时间无效,填充0;终止时间为上一拍当前屏的倒数第一条信息的时间;按终止时间向前查询
	strcpy(requireLogData.ch_start_time,"0");
	if ( screenlogNum < LOG_ROW_MAX )//未满屏时
	{
		strncpy(requireLogData.ch_end_time, logTemScreenInfo[screenlogNum-1].logTime, UINT_24);
		requireLogData.u32_log_size = screenlogNum + 1;
	}
	else//满屏时
	{
		strncpy(requireLogData.ch_end_time, logTemScreenInfo[screenlogNum-2].logTime, UINT_24);
		requireLogData.u32_log_size = LOG_ROW_MAX;
	}
	requireLogData.u32_log_type = logType;
	requireLogData.u32_init_flag = 2;
	requireLogData.u32_req_flag = 2;

	memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
	retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
	if ( retLog != OK )
	{
		return( Pt_CONTINUE );
	}
	//返回的"应答条数replayLogNum"是当前屏的日志信息
	firstlogNum = firstlogNum - 1;
	screenlogNum = replayLogNum;
	currentlogNum = replayLogALLNum;
	memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
	memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
	for ( iLog = 0; iLog < screenlogNum; iLog++ )
	{
		logScreenInfo[iLog] = allLogInfo[iLog];
		logTemScreenInfo[iLog] = allLogInfo[iLog];
	}

	/* add by zsl 20210402 start 当向前翻的条数不足15条时，对返回数据进行判断，请求日志时间与返回日志时间进行比较，避免弹出错误的覆盖提示 */
	if(!strncmp(requireLogData.ch_end_time,logScreenInfo[temp_first_lognum].logTime, UINT_24))
	{
		log_covered_flag = 0;
	}
	/* add by zsl 20210402 end 当向前翻的条数不足15条时，对返回数据进行判断，请求日志时间与返回日志时间进行比较，避免弹出错误的覆盖提示 */
	UpdateLogInfo();


	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );

}
//后一条
/*******************************************************************************
* Function: btnLogNexRowClick
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:	PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
btnLogNexRowClick( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	u32_t iLog = 0;
	u32_t retLog = OK;
	u32_t u32_ret = OK;
	firm_bool_t b_ret = SUCCESS;
	/*logBtnSink( ABW_btnLogNexRow );//设置按钮按下状态
	logBtnFloat( ABW_btnLogPrePage );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogNextPage );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogPageUp );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogPageDown );//设置按钮弹起启用状态
	logBtnFloat( ABW_btnLogPreRow );//设置按钮弹起启用状态
	traceBtnPage = 1;*/
	if ( firstlogNum + 1 >= currentlogNum )
	{
		//弹框提示这是最后一条了

		/*20200710*/
		/*u32_ret = photon_show_msg( "已经是最后一条了!" );*/
//		u32_ret = photon_show_msg( MSG_IS_LAST_ITEM );
		u32_ret = photon_show_msg( MSG_IS_LAST_PAGE );	/*wkt20240903, this does not execute*/
		return( Pt_CONTINUE );
	}
	if ( logTimeSearch )
	{
		memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
		//起始时间为上一拍当前屏的第二条信息的时间;终止时间为输入的时间查询的结束时间;按起始时间向后查询
		strncpy(requireLogData.ch_start_time, logTemScreenInfo[1].logTime, UINT_24);
		strncpy(requireLogData.ch_end_time, logSearchEndTime, UINT_24);

		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 1;
		requireLogData.u32_req_flag = 1;
		requireLogData.u32_log_size = LOG_ROW_MAX;

		memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		if ( retLog != OK )
		{
			return( Pt_CONTINUE );
		}

		if ( logTimeSearchNum - firstlogNum <= LOG_ROW_MAX )//时间查询下，翻页限制的总数改为当前时间查询的总数
		{
			//弹框提示这是最后一页了
			/*20200710*/
			/*u32_ret = photon_show_msg( "已经是最后一页了!" );*/
			u32_ret = photon_show_msg( MSG_IS_LAST_PAGE );
		}
		else
		{

			firstlogNum = firstlogNum + 1;
			screenlogNum = replayLogNum;
			currentlogNum = logTimeSearchNum;
			memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
			memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
			for ( iLog = 0; iLog < screenlogNum; iLog++ )
			{
				logScreenInfo[iLog] = allLogInfo[iLog];
				logTemScreenInfo[iLog] = allLogInfo[iLog];
			}
			UpdateLogInfo();
		}
		return( Pt_CONTINUE );
	}

	/* get current time */
	memset(&scidCurrentLogTime,UINT_0,sizeof(scid_time_t));
	/* time string, format:"YYYY-MM-DD HH:MM:SS:MMM" */
	b_ret = getLogCurrentScid(&scidCurrentLogTime);
	if ( b_ret != SUCCESS )
	{
		/*20200710*/
		/*u32_ret = photon_show_msg( "获取当前时间失败" );*/
		u32_ret = photon_show_msg( MSG_GET_TIME_FAILURE );
	}
	else
	{
		memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
		//起始时间为上一拍当前屏的第二条信息的时间;终止时间为当前的系统时间;按起始时间向后查询
		strncpy(requireLogData.ch_start_time, logTemScreenInfo[1].logTime, UINT_24);
		memcpy(&requireLogData.ch_end_time,&scidCurrentLogTime,SCID_TIME_SIZE);//当前的系统时间作为结束时间

		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 1;
		requireLogData.u32_req_flag = 1;
		requireLogData.u32_log_size = LOG_ROW_MAX;

		memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		if ( retLog != OK )
		{
			return( Pt_CONTINUE );
		}

		if ( replayLogALLNum - firstlogNum <= LOG_ROW_MAX )
		{
			//弹框提示这是最后一页了
			/*20200710*/
			/*u32_ret = photon_show_msg( "已经是最后一页了!" );*/
			u32_ret = photon_show_msg( MSG_IS_LAST_PAGE );
		}
		else
		{

			firstlogNum = firstlogNum + 1;
			screenlogNum = replayLogNum;
			currentlogNum = replayLogALLNum;
			memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
			memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
			for ( iLog = 0; iLog < screenlogNum; iLog++ )
			{
				logScreenInfo[iLog] = allLogInfo[iLog];
				logTemScreenInfo[iLog] = allLogInfo[iLog];
			}
			UpdateLogInfo();
		}
	}


	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );

}

//日志查询,目前只有时间查询
/*******************************************************************************
* Function: btnLogSearch
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
btnLogSearch( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	u32_t iLog = 0;
	u32_t retLog = OK;
	u32_t u32_ret = OK;
	memset(&setLogStartTime,UINT_0,sizeof(time_adjust_t));
	memset(&setLogEndTime,UINT_0,sizeof(time_adjust_t));
	firm_bool_t retLogStart = SUCCESS;
	PtWidget_t *pWidLogStart[UINT_6] = { ABW_PtSetYear1, ABW_PtSetMonth1, ABW_PtSetDay1, ABW_PtSetHour1, ABW_PtSetMinute1, ABW_PtSetSecond1 };//起始时间控件
	u32_t *ptmLogStart[UINT_6] = { &setLogStartTime.u32_time_year, &setLogStartTime.u32_time_mon,
	&setLogStartTime.u32_time_day, &setLogStartTime.u32_time_hour, &setLogStartTime.u32_time_min, &setLogStartTime.u32_time_sec };//起始时间
	u32_t u32_index = UINT_0;
	firm_bool_t retLogEnd = SUCCESS;
	PtWidget_t *pWidLogEnd[UINT_6] = { ABW_PtSetYear2, ABW_PtSetMonth2,	ABW_PtSetDay2, ABW_PtSetHour2, ABW_PtSetMinute2, ABW_PtSetSecond2 };//结束时间控件
	u32_t *ptmLogEnd[UINT_6] = { &setLogEndTime.u32_time_year, &setLogEndTime.u32_time_mon,
	&setLogEndTime.u32_time_day, &setLogEndTime.u32_time_hour, &setLogEndTime.u32_time_min, &setLogEndTime.u32_time_sec };//结束时间
	u32_t flagLogTime = UINT_0;//UINT_1:起始时间;UINT_2:终止时间
	firm_bool_t retTimeValid = SUCCESS;

	//logBtnSink( ABW_btnLogSearch );//设置按钮按下状态
	//traceBtnSearch = 1;

	if ( bCheckedLog == 1 )
	{
		logTimeSearch = true;//根据起止时间筛选
		memset(logSearchStartTime,0,sizeof(logSearchStartTime));//清空旧的开始时间
		memset(logSearchEndTime,0,sizeof(logSearchEndTime));//清空旧的终止时间
	}
	else
	{
		logTimeSearch = false;
	}
	if ( logTimeSearch )
	{
		//获得起始时间e.g/2018-02-09 10:28:56:000)
		for ( u32_index = TM_LOG_YEAR; u32_index < TM_LOG_SIZE; u32_index++ )
		{
			if ( retLogStart == FAILURE )
			{
				break;
			}
			flagLogTime = UINT_1;
			retLogStart = checkLogTime( pWidLogStart[u32_index], u32_index, ptmLogStart[u32_index],flagLogTime); //检查输入的起始时间
		}
		if ( retLogStart == FAILURE )
		{
			/*20200713*/
			/*u32_ret = photon_show_msg( "请检查起始时间!" );*/
			u32_ret = photon_show_msg( MSG_CHECK_START_TIME );
			logTimeSearch = false;
			return( Pt_CONTINUE );
		}
		else
		{
			sprintf(logSearchStartTime, "%04d-%02d-%02d %02d:%02d:%02d.%03d", setLogStartTime.u32_time_year,
					setLogStartTime.u32_time_mon,setLogStartTime.u32_time_day,setLogStartTime.u32_time_hour,
					setLogStartTime.u32_time_min,setLogStartTime.u32_time_sec,setLogStartTime.u32_time_msec);
		}

		//获得终止时间e.g/2018-02-09 10:28:56:000)
		for ( u32_index = TM_LOG_YEAR; u32_index < TM_LOG_SIZE; u32_index++ )
		{
			if ( retLogEnd == FAILURE )
			{
				break;
			}
			flagLogTime = UINT_2;
			retLogEnd = checkLogTime( pWidLogEnd[u32_index], u32_index, ptmLogEnd[u32_index],flagLogTime); //检查输入的终止时间
		}
		if ( retLogEnd == FAILURE )
		{
			/*20200713*/
			/*u32_ret = photon_show_msg( "请检查结束时间!" );*/
			u32_ret = photon_show_msg( MSG_CHECK_END_TIME );
			logTimeSearch = false;
			return( Pt_CONTINUE );
		}
		else
		{
			sprintf(logSearchEndTime, "%04d-%02d-%02d %02d:%02d:%02d.%03d", setLogEndTime.u32_time_year,
					setLogEndTime.u32_time_mon,setLogEndTime.u32_time_day,setLogEndTime.u32_time_hour,
					setLogEndTime.u32_time_min,setLogEndTime.u32_time_sec,setLogEndTime.u32_time_msec);
		}

		retTimeValid = CheckLogTimeValid(&setLogStartTime, &setLogEndTime);
		if ( retTimeValid == FAILURE )
		{
			logTimeSearch = false;
			return( Pt_CONTINUE );
		}
		else
		{

		}

		memset(&requireLogData, UINT_0, sizeof(mq_req_ldata_t));
		strncpy(requireLogData.ch_start_time, logSearchStartTime, UINT_24);//起始时间,终止时间都有效;按终止时间向前 查询
		strncpy(requireLogData.ch_end_time, logSearchEndTime, UINT_24);

		requireLogData.u32_log_type = logType;
		requireLogData.u32_init_flag = 1;
		requireLogData.u32_req_flag = 2;
		if ( logRealOrHistory )
		{
			requireLogData.u32_log_size = getLogRequireNum(logType);
		}
		else
		{
			requireLogData.u32_log_size = getLogRequireNum(logType);
		}
		memset(&allLogInfo,'\0',ALL_LOG_NUM*sizeof(logInfo));
		retLog = ReadLogByType(logType, requireLogData, allLogInfo, &replayLogALLNum, &replayLogNum, &replayLogPeriodNum);
		if ( retLog != OK )
		{
			return( Pt_CONTINUE );
		}
		currentlogNum = replayLogPeriodNum;
		logTimeSearchNum = replayLogPeriodNum;//存储日志时间查询的总条数
		if(LOG_ROW_MAX < replayLogNum)
		{
			screenlogNum = LOG_ROW_MAX;
		}
		else
		{
			screenlogNum = replayLogNum;
		}
		firstlogNum = currentlogNum - screenlogNum;
		memset(&logScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		memset(&logTemScreenInfo,'\0',LOG_ROW_MAX*sizeof(logInfo));
		int index_log = 0;
		u32_t screen_start_index = 0;
		/* add by zsl 20210402 start 判断日志回包条数是否大于15条，大于时，取后15条进行显示 */
		if(replayLogNum >= 15)
		{
			screen_start_index = replayLogNum - 15;
		}

		for ( iLog = screen_start_index; iLog < replayLogNum; iLog++ )
		{
			logScreenInfo[index_log] = allLogInfo[iLog];
			logTemScreenInfo[index_log] = allLogInfo[iLog];
			index_log++;
		}
		/* add by zsl 20210402 start 判断日志回包条数是否大于15条，大于时，取后15条进行显示 */
		UpdateLogInfo();
	}
	else
	{
		//不做处理
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}
//日志类型切换
/*******************************************************************************
* Function: switchLogByType
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
switchLogByType( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	char_t *plogName = P_NULL;
	plogName = ApInstanceName(widget);
	/*按类型读取日志，其中：1表示全日志	2表示操作日志	3表示故障日志	4表示运行日志*/
	if ( strcmp(plogName,"All_Log") == 0 )
	{
		logType = MQ_LOG_TYPE_ALL;
	}
	else if ( strcmp(plogName,"Operation_Log") == 0 )
	{
		logType = MQ_LOG_TYPE_OPERATION;
	}
	else if ( strcmp(plogName,"System_Log") == 0 )
	{
		logType = MQ_LOG_TYPE_SYS;
	}
	else if ( strcmp(plogName,"Falut_Log") == 0 )
	{
		logType = MQ_LOG_TYPE_FAULT;
	}
	else
	{
		//不做处理
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

//切换日志类型时,销毁当前的日志
/*******************************************************************************
* Function: SwitchLog
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
SwitchLog( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtDestroyWidget(ABW_LogList);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );

}


//CheckBox单击事件函数
/*******************************************************************************
* Function: CheckBox_Time_Click
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
int
CheckBox_Time_Click( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtWidget_t *CheckWidget = P_NULL;
	CheckWidget = ABW_CheckLine_Time;
	//logBtnFloat( ABW_btnLogSearch );//设置按钮弹起启用状态
	bCheckedLog = InverseValve(bCheckedLog);
	SetCheckBoxState(CheckWidget, bCheckedLog);

	if ( (logTimeSearch == true) && (bCheckedLog == -1) )
	{
		logTimeSearch = false;
	}


	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	return( Pt_CONTINUE );
}


/*******************************************************************************/
/**********************自定义函数**************************************************/

/*清空显示的集合*/
/*******************************************************************************
* Function: clearInfo
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
void clearInfo()
{
	u32_t iLog = 0;
	logInfo loginfo = {{0},{0},{0},{0},{0}};
	for( iLog = 0; iLog < LOG_ROW_MAX; iLog++)
	{
		logScreenInfo[iLog] = loginfo;
	}
	PtListDeleteAllItems(ABW_PtLogInfoList);
}
/*判断是否有空元素*/
/*******************************************************************************
* Function: existsNull
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
i32_t existsNull(logInfo loginfo)
{
    if ( (loginfo.eventDesc == NULL) || (loginfo.logTime == NULL)
       || (loginfo.userName == NULL) || (loginfo.object == NULL) )    /* qinshiling 20181017 静态检查异常：loginfo.logType == NULL恒为假*/
//	if ( (loginfo.logType == NULL) || (loginfo.eventDesc == NULL) || (loginfo.logTime == NULL)
//	   || (loginfo.userName == NULL) || (loginfo.object == NULL) )
	{
		return -1;
	}
	return 0;
}

/*显示当前屏信息*/
/*******************************************************************************
* Function: UpdateLogInfo
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
void UpdateLogInfo()
{
	logInfo loginfo; //一条日志
	char_t *logitem[LOG_ROW_MAX];	//当前屏显示的日志
	u32_t	showlogNum = 0; //当前屏显示的日志数量
	char_t templogNum[20] = {0};//备用char类型数组
	u32_t jLog = 0;
	u32_t iLog = 0;
	u32_t u32_index = UINT_0;
	//clearInfo();
	PtListDeleteAllItems(ABW_PtLogInfoList);//清空日志显示
	memset(&loginfo, 0, sizeof(logInfo));
	memset(logitem, 0, sizeof(logitem));
	memset(templogNum, 0, sizeof(templogNum));
	if ( currentlogNum == 0 )
	{
		itoa(0, templogNum, 10);
	}
	else
	{
		/* add by zsl 20210401 start 向前翻页的日志数据被覆盖时，直接将右上角日志标志跳转到第一页 */
		if(log_covered_flag)
		{
			firstlogNum = 0;
			log_covered_flag = 0;
			//photon_show_msg("日志内容更新，已导航至首页！");
			photon_show_msg(MSG_LOG_UPDATED_TURN_TO_FIRSTPAGE);
		}
		/* add by zsl 20210401 end 向前翻页的日志数据被覆盖时，直接将右上角日志标志跳转到第一页 */
		itoa(firstlogNum + 1, templogNum, 10);
	}
	//start_xiexinxin_20180912_临时解决:校时后,出现ABW_firstlogNum > ABW_alllogNum的情况
	if ( firstlogNum + 1 > currentlogNum )
	{
		memset(templogNum, 0, sizeof(templogNum));
		itoa(0, templogNum, 10);
	}
	//end_xiexinxin_20180912_临时解决:校时后,出现ABW_firstlogNum > ABW_alllogNum的情况
	PtSetResource(ABW_firstlogNum, Pt_ARG_TEXT_STRING, templogNum, 0);
	itoa(currentlogNum, templogNum, 10);
	PtSetResource(ABW_alllogNum, Pt_ARG_TEXT_STRING, templogNum, 0);
	if ( currentlogNum <= 0 )
	{
		return;
	}

	for ( iLog = 0; iLog < screenlogNum; iLog++ )
	{
		logScreenInfo[iLog].logTime[UINT_23] = '\0';//去掉时间标志位
	}

	//日志表格赋值
	showlogNum = 0;
	char_t str[TMP_INFO_LEN] = {0};
	for ( jLog = 0; jLog < screenlogNum; jLog++ )
	{
		loginfo = logScreenInfo[jLog];
		if ( existsNull(loginfo) == -1 )
		{
			for ( u32_index = UINT_0; u32_index < showlogNum; u32_index++ )
			{
				free( logitem[u32_index] );/* 20181022 qinshiling 静态分析异常：内存未释放 */
			}
			return;
		}

		memset(str,0,TMP_INFO_LEN);
		strcat(str,loginfo.logType);
		strcat(str,"	");
		strcat(str,loginfo.eventDesc);
		strcat(str,"	");
		strcat(str,loginfo.logTime);
		strcat(str,"	");
		strcat(str,loginfo.userName);
		strcat(str,"	");
		strcat(str,loginfo.object);
		strcat(str,"\0");
		//logitem[jLog] = (char_t *)malloc(TMP_INFO_LEN*sizeof(char_t));
		logitem[jLog] = (char_t *)malloc(TMP_INFO_LEN*sizeof(char_t));
		if (P_NULL == logitem[jLog])
		{
			continue;
		}
		memset(logitem[jLog], 0, TMP_INFO_LEN);
		memcpy(logitem[jLog], str, TMP_INFO_LEN);
		showlogNum++;
	}
	PtListAddItems(ABW_PtLogInfoList,(const char_t **)logitem,showlogNum,1);//添加日志内容
	for ( jLog = 0; jLog < screenlogNum; jLog++ )
	{
/*
		if ( NULL != logitem[jLog] )
		{
			free(logitem[jLog]);
			logitem[jLog] = NULL;
		}
*/
		free(logitem[jLog]);
		logitem[jLog] = NULL;/* qinshiling 20181019 静态分析修正:字符串指针是否为空都需要释放内存*/
	}
}
/*设置筛选按钮不可用*/
/*******************************************************************************
* Function: setlogBtnDisable
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
void setlogBtnDisable()
{
	/*PtSetResources(ABW_btnLogPrePage, 6, disable_args);
	PtSetResources(ABW_btnLogPageUp,  6, disable_args);
	PtSetResources(ABW_btnLogPageDown, 6, disable_args);
	PtSetResources(ABW_btnLogNextPage, 6, disable_args);
	PtSetResources(ABW_btnLogPreRow, 6, disable_args);
	PtSetResources(ABW_btnLogNexRow, 6, disable_args);
	*/
	logBtnGUI( ABW_btnLogPrePage );
	logBtnGUI( ABW_btnLogNextPage );
	logBtnGUI( ABW_btnLogPageUp );
	logBtnGUI( ABW_btnLogPageDown );
	logBtnGUI( ABW_btnLogPreRow );
	logBtnGUI( ABW_btnLogNexRow );
}
/*设置筛选按钮启用*/
/*******************************************************************************
* Function: setlogBtnEnable
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
void setlogBtnEnable()
{
	/*PtSetResources(ABW_btnLogPrePage, 6, float_args);
	PtSetResources(ABW_btnLogPageUp,  6, float_args);
	PtSetResources(ABW_btnLogPageDown, 6, float_args);
	PtSetResources(ABW_btnLogNextPage, 6, float_args);
	PtSetResources(ABW_btnLogPreRow, 6, float_args);
	PtSetResources(ABW_btnLogNexRow, 6, float_args);
	*/
	logBtnFloat( ABW_btnLogPrePage );
	logBtnFloat( ABW_btnLogNextPage );
	logBtnFloat( ABW_btnLogPageUp );
	logBtnFloat( ABW_btnLogPageDown );
	logBtnFloat( ABW_btnLogPreRow );
	logBtnFloat( ABW_btnLogNexRow );
}

//设置按钮闭锁状态
/*******************************************************************************
* Function: logBtnGUI
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
void logBtnGUI( PtWidget_t *PWidget )
{
	PtSetResource( PWidget, OB_STATE, OB_GUI, 0);//闭锁状态
	PtSetResource( PWidget, Pt_ARG_FLAGS, Pt_BLOCKED, Pt_BLOCKED);
	PtSetResource( PWidget, OB_BACKCOLOR, PgRGB(166,166,166), 0);//背景色
	PtSetResource( PWidget, OB_COLOR, PgRGB(255,255,255), 0);//文本颜色
}
//设置按钮弹起启用状态
/*******************************************************************************
* Function: logBtnFloat
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
void logBtnFloat( PtWidget_t *PWidget )
{
	PtSetResource(PWidget, Pt_ARG_FLAGS, 0, Pt_SET);                           /* qinshiling 20190614: delete Pt_SET */
	PtSetResource( PWidget, Pt_ARG_FLAGS, 0, Pt_BLOCKED );//解禁
	PtSetResource( PWidget, Pt_ARG_FLAGS, 0x2000580, 0x2000580 );
	PtSetResource( PWidget, OB_STATE, OB_UP, 0);//弹起状态
	PtSetResource( PWidget, OB_BACKCOLOR, PgRGB(191,191,191), 0);//背景色
	PtSetResource( PWidget, OB_COLOR, Pg_BLACK, 0);//文本颜色
}
//设置按钮按下状态
/*******************************************************************************
* Function: logBtnSink
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
void logBtnSink( PtWidget_t *PWidget )
{
	//PtSetResource( PWidget, OB_STATE, OB_DOWN, 0);//按下状态
	PtSetResource( PWidget, Pt_ARG_FLAGS, Pt_BLOCKED, Pt_BLOCKED );
	PtSetResource( PWidget, OB_BACKCOLOR, PgRGB(191,191,191), 0);//背景色
	PtSetResource( PWidget, OB_COLOR, Pg_BLACK, 0);//文本颜色
}

//设置按钮的状态
/*void logbutton_state()
{

    PtSetArg( &disable_args[0], Pt_ARG_CONTRAST, 0, 0 );
	PtSetArg( &disable_args[1], Pt_ARG_BORDER_CONTRAST, 20, 0 );
	PtSetArg( &disable_args[2], Pt_ARG_BASIC_FLAGS, 0x1100fff, 0x1100fff );
	//PtSetArg( &disable_args[3], Pt_ARG_FILL_COLOR, Pg_GRAY, 0 );
	PtSetArg( &disable_args[3], Pt_ARG_FILL_COLOR, PgRGB(166,166,166), 0 );
	PtSetArg( &disable_args[4], Pt_ARG_COLOR, Pg_WHITE, 0 );
	PtSetArg( &disable_args[5], Pt_ARG_FLAGS, 0x2020580, 0x2020580 );

	PtSetArg( &float_args[0], Pt_ARG_CONTRAST, 20, 0 );
	PtSetArg( &float_args[1], Pt_ARG_BORDER_CONTRAST, 20, 0 );
	PtSetArg( &float_args[2], Pt_ARG_BASIC_FLAGS, 0x1100fff, 0x1100fff );
	//PtSetArg( &float_args[3], Pt_ARG_FILL_COLOR, 0xD9D9D9, 0 );
	PtSetArg( &float_args[3], Pt_ARG_FILL_COLOR, PgRGB(191,191,191), 0 );
	PtSetArg( &float_args[4], Pt_ARG_COLOR, Pg_BLACK, 0 );
	PtSetArg( &float_args[5], Pt_ARG_FLAGS, 0, Pt_BLOCKED );

	PtSetArg( &sink_args[0], Pt_ARG_CONTRAST, 20, 0 );
	PtSetArg( &sink_args[1], Pt_ARG_BORDER_CONTRAST, 250, 0 );
	PtSetArg( &sink_args[2], Pt_ARG_BASIC_FLAGS, 0, Pt_ALL_BEVELS|Pt_BOTTOM_RIGHT_OUTLINE );
	//PtSetArg( &sink_args[3], Pt_ARG_FILL_COLOR, 0xD9D9D9, 0 );
	PtSetArg( &sink_args[3], Pt_ARG_FILL_COLOR, PgRGB(191,191,191), 0 );
	PtSetArg( &sink_args[4], Pt_ARG_COLOR, Pg_BLACK, 0 );
	PtSetArg( &sink_args[5], Pt_ARG_FLAGS, 0, Pt_BLOCKED );
}
*/
//设置CheckBox状态
/*******************************************************************************
* Function: SetCheckBoxState
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
void SetCheckBoxState(PtWidget_t *widget, i32_t bCheck)
{
	if (bCheck == 1)
	{
		PtSetResource( widget, Pt_ARG_COLOR, Pg_BLACK, 0 );//选中
	}
	else if (bCheck == -1)
	{
		PtSetResource( widget, Pt_ARG_COLOR, Pg_TRANSPARENT, 0 );//未选中
	}
	else
	{
		//不做处理
	}
}
//取反函数
/*******************************************************************************
* Function: InverseValve
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
i32_t InverseValve(i32_t shVal)
{
	if (shVal == 1)
	{
		shVal = -1;
	}
	else if (shVal == -1)
	{
		shVal = 1;
	}
	else
	{
		//不做处理
	}
	return shVal;
}

//检查输入时间是否有效
/*******************************************************************************
* Function: checkLogTime
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
firm_bool_t checkLogTime( PtWidget_t *pWidget, u32_t tmType, u32_t *pTime,u32_t tmFlag)
{
	firm_bool_t b_ret = SUCCESS;           /* fucntion return value */
	char_t ch_time_buf[UINT_5] = {0};      /* time buffer */
	char_t *pch_text = ch_time_buf;        /* label text string pointer */
	u32_t u32_index = UINT_0;              /* index */
	i32_t u32_tm_label = UINT_0;           /* time label(uint) */

	if ( (P_NULL == pWidget) || (P_NULL == pTime) )
	{
		b_ret = FAILURE;
	}

	if ( SUCCESS == b_ret )
	{
		PtGetResource( pWidget, Pt_ARG_TEXT_STRING, &pch_text, UINT_5); /* get year label text */
		/* check label text string */
		for ( u32_index = UINT_0; u32_index < strlen(pch_text); u32_index++ )
		{
			if ( '0' > pch_text[u32_index] || '9' < pch_text[u32_index] )
			{
				b_ret = FAILURE;                 /* compare char to '0'&'9'*/
				break;
			}
		}
	}

	if ( SUCCESS == b_ret )
	{
		u32_tm_label = atoi( pch_text);           /* convert char to int*/
		/* check time value */
		switch ( tmType )
		{
		case TM_LOG_YEAR:
			if ( SYS_YEAR_BASE > u32_tm_label )
			{
				b_ret = FAILURE;
			}
			break;
		case TM_LOG_MON:
			if ( (JAN > u32_tm_label) || (DEC < u32_tm_label) )
			{
				b_ret = FAILURE;
			}
			break;
		case TM_LOG_DAY:
			if ( tmFlag == UINT_1 )
			{
				b_ret = checkLogDay( setLogStartTime.u32_time_year, setLogStartTime.u32_time_mon, u32_tm_label );
			}
			else//UINT_2
			{
				b_ret = checkLogDay( setLogEndTime.u32_time_year, setLogEndTime.u32_time_mon, u32_tm_label );
			}
			break;
		case TM_LOG_HOUR:
//			if ( (UINT_0 > u32_tm_label) || (TIME_HOUR_MAX < u32_tm_label) )
			if ( TIME_HOUR_MAX < u32_tm_label )                     /* qinshiling 20181017 静态检查异常：UINT_0 > u32_tm_label恒为假 */
			{
				b_ret = FAILURE;
			}
			break;
		case TM_LOG_MIN:
//			if ( (UINT_0 > u32_tm_label) || (TIME_MIN_MAX < u32_tm_label) )
			if ( TIME_MIN_MAX < u32_tm_label )
			{
				b_ret = FAILURE;
			}
			break;
		case TM_LOG_SEC:
//			if ( (UINT_0 > u32_tm_label) || (TIME_SEC_MAX < u32_tm_label) )
			if ( TIME_SEC_MAX < u32_tm_label )
			{
				b_ret = FAILURE;
			}
			break;
		default:
			b_ret = FAILURE;
			break;
		}
	}

	if ( SUCCESS == b_ret )
	{
		*pTime = u32_tm_label;               /* set time label */
	}

	return b_ret;
}

//判断是否为闰年
/*******************************************************************************
* Function: checkLogLeapYear
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
firm_bool_t checkLogLeapYear( u32_t u32_year )
{
	firm_bool_t b_ret = TRUE;            /* function return value */

	if ( ( (0 == u32_year%4) && (0 != u32_year%100) ) || ( 0 == u32_year%400 ) )
	{
		b_ret = TRUE;
	}
	else
	{
		b_ret = FALSE;
	}

	return b_ret;
}

//判断DAY是否有效
/*******************************************************************************
* Function: checkLogDay
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
firm_bool_t checkLogDay( u32_t u32_year, u32_t u32_mon, u32_t u32_day )
{
	firm_bool_t b_ret = SUCCESS;           /* function return value */

	switch ( u32_mon )
	{
	case JAN:
	case MAR:
	case MAY:
	case JUL:
	case AUG:
	case OCT:
	case DEC:
		if ( (UINT_1 > u32_day) || (UINT_31 < u32_day) )
		{
			b_ret = FAILURE;
		}
		break;
	case APR:
	case JUN:
	case SEP:
	case NOV:
		if ( (UINT_1 > u32_day) || (UINT_30 < u32_day) )
		{
			b_ret = FAILURE;
		}
		break;
	case FEB:
		if ( checkLogLeapYear( u32_year ) )
		{
			if ( (UINT_1 > u32_day) || (UINT_29 < u32_day) )
			{
				b_ret = FAILURE;
			}
		}
		else
		{
			if ( (UINT_1 > u32_day) || (UINT_28 < u32_day) )
			{
				b_ret = FAILURE;
			}
		}
		break;
	default:
		b_ret = FAILURE;
		break;
	}

	return b_ret;
}
//获得当前的系统时间
/*******************************************************************************
* Function: getLogCurrentScid
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
firm_bool_t getLogCurrentScid(scid_time_t *logCurrentTime)
{
/* get current time */
	firm_bool_t b_ret = SUCCESS;
	i32_t ret = UINT_0;
	struct timeb currentTime;  // millisecond time
	memset(&currentTime, UINT_0, sizeof(struct timeb));
	ret = ftime(&currentTime); //获得目前时间
	b_ret = std_sec_to_scid_time( logCurrentTime, currentTime.time, (u32_t)currentTime.millitm );
	/* time string, format:"YYYY-MM-DD HH:MM:SS:MMM" */
	return b_ret;
}
//获得请求的条数
/*******************************************************************************
* Function: getLogRequireNum
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
u32_t getLogRequireNum(u32_t type)
{
	u32_t requireNum = 0;
	/*1表示全日志	2表示操作日志	3表示故障日志	4表示运行日志*/
	switch (type)
	{
	case 2:
		requireNum = OPER_LOG_NUM;
		break;
	case 3:
		requireNum = ERR_LOG_NUM;
		break;
	case 4:
		requireNum = SYS_LOG_NUM;
		break;
	default:
		requireNum = ALL_LOG_NUM;
		break;
	}
	return requireNum;
}

//输入时间是否早于系统时间,是否小于180天
/*******************************************************************************
* Function: CheckLogTimeValid
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description:
* Input:
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2017/10/12
*******************************************************************************/
firm_bool_t CheckLogTimeValid(time_adjust_t *logStartTime, time_adjust_t *logEndTime)
{
	u32_t u32_ret = OK;
	/* get current time */
	firm_bool_t b_ret = SUCCESS;
	memset(&scidCurrentLogTime, UINT_0, sizeof(scid_time_t));
	/* time string, format:"YYYY-MM-DD HH:MM:SS:MMM" */
	b_ret = getLogCurrentScid( &scidCurrentLogTime );
	if ( b_ret != SUCCESS )
	{
		/*20200713*/
		/*u32_ret = photon_show_msg( "获取当前时间失败" );*/
		u32_ret = photon_show_msg( MSG_GET_TIME_FAILURE );
		return b_ret;
	}
	else
	{

	}
	//struct tm currentLogTm;
	/* to sturct tm */
	//b_ret = scid_time_to_tm(&scidCurrentLogTime, &currentLogTm);
	time_t time_ret = 0U;
	time_ret = scid_time_to_std_sec( &scidCurrentLogTime );

	struct tm setLogStartTm;
	setLogStartTm.tm_year = setLogStartTime.u32_time_year - TM_YEAR_BASE;
	setLogStartTm.tm_mon = setLogStartTime.u32_time_mon - 1;
	setLogStartTm.tm_mday = setLogStartTime.u32_time_day;
	setLogStartTm.tm_hour = setLogStartTime.u32_time_hour;
	setLogStartTm.tm_min = setLogStartTime.u32_time_min;
	setLogStartTm.tm_sec = setLogStartTime.u32_time_sec;
	/* get second */
	time_t setLogStartSec = mktime( &setLogStartTm );

	struct tm setLogEndTm;
	setLogEndTm.tm_year = setLogEndTime.u32_time_year - TM_YEAR_BASE;
	setLogEndTm.tm_mon = setLogEndTime.u32_time_mon - 1;
	setLogEndTm.tm_mday = setLogEndTime.u32_time_day;
	setLogEndTm.tm_hour = setLogEndTime.u32_time_hour;
	setLogEndTm.tm_min = setLogEndTime.u32_time_min;
	setLogEndTm.tm_sec = setLogEndTime.u32_time_sec;
	/* get second */
	time_t setLogEndSec = mktime( &setLogEndTm );

	if ((time_ret <= setLogStartSec) || (time_ret <= setLogEndSec) || (setLogStartSec >= setLogEndSec))
	{
		/*20200713*/
		/*u32_ret = photon_show_msg( "请检查输入时间!" );*/
		u32_ret = photon_show_msg( MSG_CHECK_INPUT_TIME );
		b_ret = FAILURE;
		return b_ret;
	}
	else
	{

	}
	time_t log180day = 180*24*3600;
	if ((time_ret - setLogStartSec > log180day) || (time_ret - setLogEndSec > log180day))
	{
		/*20200713*/
		/*u32_ret = photon_show_msg( "输入时间超出180天!" );*/
		u32_ret = photon_show_msg( MSG_INPUT_TIME_EXCEED_180DAYS );
		b_ret = FAILURE;
		return b_ret;
	}
	else
	{

	}
	return b_ret;
}

/*
int
BtnPeriodRefresh( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if( traceBtnPage == 1 )
	{
		logBtnFloat( ABW_btnLogPrePage );//设置按钮弹起启用状态
		logBtnFloat( ABW_btnLogPageUp );//设置按钮弹起启用状态
		logBtnFloat( ABW_btnLogPageDown );//设置按钮弹起启用状态
		logBtnFloat( ABW_btnLogNextPage );//设置按钮弹起启用状态
		logBtnFloat( ABW_btnLogPreRow );//设置按钮弹起启用状态
		logBtnFloat( ABW_btnLogNexRow );//设置按钮弹起启用状态
		traceBtnPage = 0;
	}
	if( traceBtnSearch == 1 )
	{
		logBtnFloat( ABW_btnLogSearch );//设置按钮弹起启用状态
		traceBtnSearch = 0;
	}

	// eliminate 'unreferenced' warnings //
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );

}
*/

/*******************************************************************************
* Function: SetLogNewStartTime
* Identifier: SSD-PDT-017 (Trace to: PDT-022、PDT-023、PDT-024、PDT-025、PDT-026、PDT-027、PDT-028、PDT-050)
* Description: set log start and end time to current time
* Input: None
* Output: None
* Return: None
* Call:
* Others: None
* Log: 2018/11/01
*******************************************************************************/
static void SetLogNewStartTime(void)
{
	int logStartTime[6] = {0};//年 月 日 时 分 秒
	struct timeb timepCur_b = {0};
	ftime(&timepCur_b);
	time_t timepSys;
	timepSys = timepCur_b.time;

	struct tm *pTime;
	pTime = localtime(&timepSys);

	logStartTime[0] = 1900 + pTime->tm_year;
	logStartTime[1] = 1 + pTime->tm_mon;
	logStartTime[2] = pTime->tm_mday;
	logStartTime[3] = pTime->tm_hour;
	logStartTime[4] = pTime->tm_min;
	logStartTime[5] = pTime->tm_sec;

	PtWidget_t *TimeWidget[12] = {P_NULL};
	TimeWidget[0] = ABW_PtSetYear1;
	TimeWidget[1] = ABW_PtSetMonth1;
	TimeWidget[2] = ABW_PtSetDay1;
	TimeWidget[3] = ABW_PtSetHour1;
	TimeWidget[4] = ABW_PtSetMinute1;
	TimeWidget[5] = ABW_PtSetSecond1;
	TimeWidget[6] = ABW_PtSetYear2;
	TimeWidget[7] = ABW_PtSetMonth2;
	TimeWidget[8] = ABW_PtSetDay2;
	TimeWidget[9] = ABW_PtSetHour2;
	TimeWidget[10] = ABW_PtSetMinute2;
	TimeWidget[11] = ABW_PtSetSecond2;

	int i = 0;
	for (i = 0; i < 6; i++) {
		int number;
		char_t strVal[5] = {0};
		number = logStartTime[i];
		sprintf(strVal, "%02d", number);
		PtSetResource(TimeWidget[i], Pt_ARG_TEXT_STRING, 0, 0);
		PtSetResource(TimeWidget[i+6], Pt_ARG_TEXT_STRING, 0, 0);
		PtSetResource(TimeWidget[i], Pt_ARG_TEXT_STRING, strVal, 0);
		PtSetResource(TimeWidget[i+6], Pt_ARG_TEXT_STRING, strVal, 0);
	}
}
