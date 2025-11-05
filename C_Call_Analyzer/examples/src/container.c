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
#include "container.h"


//static PtWidget_t *wig[CONTAINER_VAR_SIZE_MAX] = {P_NULL, P_NULL, P_NULL, P_NULL, P_NULL, P_NULL, P_NULL, P_NULL, P_NULL, P_NULL};/*容器内的文本图符*/
static char pcontainer_last_widget[65] = {0};/*上一拍的容器*/
static char pcontainer_this_widget[65] = {0};/*容器*/                                /* 上一张页面 */
static PtWidget_t *wig[CONTAINER_VAR_SIZE_MAX] = {P_NULL};/*容器内的文本图符or 开关量图符*/
static container_list_t container_list = {0};
extern firm_bool_t b_new_open;

/*******************************************************************************
* Function: container_click
* Identifier: SSD-PDT- (Trace to: )
* Description: parms container widget click function
* Input: widget---parms container widget
*        apinfo---callback reason
*        cbinfo---callback data
* Output: None
* Return: PtContinue
* Call:
* Others: None
* Log: 2018/11/01
*******************************************************************************/
int container_click( PtWidget_t *link_instance, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	/* eliminate 'unreferenced' warnings */
    cbinfo = cbinfo;
    apinfo = apinfo;
    cbinfo = cbinfo;
    char_t ch_cur_win[65] = {0};                                               /* 当前页面 */
    unsigned int         wig_ctr = 0;/*计数容器内的文本图符*/
	i32_t i32_ret1 = INT_N1;
	i32_t i32_ret2 = INT_N1;
	PtWidget_t *child = P_NULL;
	int index = 0;
	u32_t u32_container_pos = UINT_0;

	if (pcontainer_this_window != P_NULL)
	{
		PtDestroyWidget( pcontainer_this_window); /*销毁打开的页面(link_instance指向之前打开的页面)*/
	}

	memset(wig, 0, sizeof(wig));
	strncpy(ch_cur_win, ApInstanceName(pCUR_WIN), UINT_64);                                    /* 当前页面 */
	pcontainer_this_window = link_instance;
	strncpy( pcontainer_this_widget, ApInstanceName(apinfo->widget), UINT_64);/*获取当前容器名称*/
	for (index = 0; index < container_list.u32_container_size; index++)
	{
		i32_ret1 = strncmp(pcontainer_this_widget, container_list.container_des_buf[index].ch_container_name, UINT_64);
		if (0 == i32_ret1)
		{
			u32_container_pos = index;                                         /* 定位当前容器 */
			break;
		}
	}
	if (index == container_list.u32_container_size)
	{
		u32_container_pos = container_list.u32_container_size;
		container_list.container_des_buf[u32_container_pos].u32_var_pos = 0;
		strncpy(container_list.container_des_buf[u32_container_pos].ch_container_name, pcontainer_this_widget, UINT_64);
		container_list.u32_container_size++;                                   /* 新增容器 */
	}

	i32_ret1 = strncmp(pcontainer_this_widget, pcontainer_last_widget, UINT_64);
	if (((0 != i32_ret1) || (b_new_open == TRUE)) && (container_list.container_des_buf[u32_container_pos].u32_var_pos > 0))
	{
		container_list.container_des_buf[u32_container_pos].u32_var_pos--;               /* 若由其他页面或控件再次点击至本控件，计数减一以保持记忆 */
	}
	strncpy( pcontainer_last_widget, pcontainer_this_widget, UINT_64);/*替换上一个容器的名称*/

	for (child = PtWidgetChildBack(apinfo->widget); child; child = PtWidgetBrotherInFront(child))
	{
		if (wig_ctr >= CONTAINER_VAR_SIZE_MAX)
		{
			break;
		}
		i32_ret1 = PtWidgetIsClass(child, AnalogDisplay);
		i32_ret2 = PtWidgetIsClass(child, DigitalDisplay);
		if ((1 == i32_ret1) || (1 == i32_ret2))
		{
			 wig[wig_ctr] = child;
			 wig_ctr++;
		}
	}

	if (container_list.container_des_buf[u32_container_pos].u32_var_pos == wig_ctr)/*wig_ctr周期长度,调整wig_index*/
	{
		container_list.container_des_buf[u32_container_pos].u32_var_pos = UINT_0;/* qinshiling 20181017 变量计数超过单个容器最大值，重新计数 */
		//wig_index = wig_index - wig_ctr;
	}
	PtGetResource(wig[container_list.container_des_buf[u32_container_pos].u32_var_pos], Pt_ARG_USER_DATA, &pContainerName, 0);/*从文本图符获得点名*/
	for (index = 0; index < strlen(pContainerName); index++)
	{
		if ( pContainerName[index] == ';' )
		{
			pContainerName[index] = '\0';
			break;
		}
	}
	b_new_open = FALSE;
	container_list.container_des_buf[u32_container_pos].u32_var_pos++;

	return( Pt_CONTINUE );
}



