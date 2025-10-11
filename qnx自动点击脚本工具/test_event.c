/* M a i n l i n e                                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.03  */

#ifdef __USAGE
%C - This is a QNX/Photon Application.
%C [options]

Options:
  -s server   Server node or device name
  -x x        Initial x position
  -y y        Initial y position
  -h h        Initial h dimension
  -w w        Initial w dimension

Examples:
%C -s4
  Run using Photon server on node 4

%C -s//4/dev/photon
  Same as above

%C -x10 -y10 -h200 -w300
  Run at initial position 10,10 with initial
  dimension of 200x300.
#endif

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h>
#include <Ph.h>
#include <Pt.h>
#include <photon/PtButton.h>
#include <photon/PtTimer.h>

/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"
//#include "photon_data_com.h"
//#include "button.h"


//static PtWidget_t *g_window = NULL;
//static PtWidget_t *g_button = NULL;
//static PtWidget_t *g_button2 = NULL; // 新增第二个按钮
//static PtWidget_t *g_button3 = NULL; // 新增第三个按钮

// 全局变量定义
static PtWidget_t *g_button = NULL;    // 第一个按钮控件指针
static PtWidget_t *g_button1 = NULL;   // 第二个按钮控件指针
static PtWidget_t *g_global_timer = NULL; // 全局定时器变量
static int *g_global_widgets_number = NULL; // 全局控件编号数组指针

/**
 * 函数原型声明区
 */
void emit_click(PtWidget_t *widget);                    // 触发控件点击事件
int start_timer(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo);  // 启动定时器
int stop_timer(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo);   // 停止定时器
int on_timer_widgets(PtWidget_t *w, void *data, PtCallbackInfo_t *cbinfo);  // 定时器回调函数
PtWidget_t *create_periodic_control_caller(int *widget_number, PtWidget_t **widgets, int count, int interval_ms, PtWidget_t *parent_window); // 创建周期性控件调用器

//const int ABN_oper_dialog = 1718;
//const int ABN_oper_dialog_contain1 = 1719;
//const int ABN_oper_dialog1_button1 = 1720;
//const int ABN_oper_dialog1_button2 = 1721;
//const int ABN_oper_dialog1_button3 = 1722;
//const int ABN_oper_dialog1_button4 = 1723;
//const int ABN_oper_dialog1_button5 = 1724;
//const int ABN_oper_dialog1_button6 = 1725;
//const int ABN_oper_dialog1_button7 = 1726;
//const int ABN_oper_dialog1_button8 = 1727;
//const int ABN_oper_dialog1_button9 = 1728;
//const int ABN_oper_dialog1_button10 = 1729;
//const int ABN_oper_dialog_contain2 = 1730;
//const int ABN_oper_dialog2_button1 = 1731;
//const int ABN_oper_dialog2_button2 = 1732;
//const int ABN_oper_dialog2_button3 = 1733;
//const int ABN_oper_dialog2_button4 = 1734;
//const int ABN_oper_dialog2_button5 = 1735;
//const int ABN_oper_dialog2_button6 = 1736;
//const int ABN_oper_dialog2_button7 = 1737;
//const int ABN_oper_dialog2_button8 = 1738;
//const int ABN_oper_dialog2_button9 = 1739;
//const int ABN_oper_dialog2_button10 = 1740;

// 创建控件编号静态数组
static int widgets_number[] = {
//		1720, 1721, 1722, 1723, 1724  // 控件编号(导航栏前五个按钮)
		1735,
		1736,
		1734,

		61//screen_clear_button
};

// 用于保存控件数组和执行周期的结构体
typedef struct {
    int *widget_number;     // 控件编号数组指针
    PtWidget_t **widgets;   // 控件指针数组指针
    int count;              // 控件总数
    int current_index;      // 当前要执行的控件索引
    int base_interval;      // 基础周期时间（毫秒）
} WidgetsData;

// 按钮回调函数 - 打印hello信息
int prthello(PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo)
{
	static int i=0;
	printf("hello, %d~\n", i++);
	return Pt_CONTINUE;
}

// 按钮回调函数 - 打印goodbye信息
int prtbye(PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo)
{
	static int j=0;
	printf("goodbye, %d~\n", j++);
	return Pt_CONTINUE;
}

// 按钮回调函数 - 打印welcome信息（当前未使用）
int prtwelcome(PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo)
{
	static int k=0;
	printf("welcome, %d~\n", k++);
	return Pt_CONTINUE;
}

/**
 * 触发控件的点击事件
 *
 * @param widget 要触发点击事件的Photon控件指针
 */
void emit_click(PtWidget_t *widget) {
	// 获取当前时间并打印调试信息
	time_t current_time = time(NULL);
	struct tm *local_time = localtime(&current_time);
	char time_str[64];
	strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);
	printf("[点击时间] %s\n", time_str);

	// 直接触发控件的激活回调，等效于一次用户点击操作
	PtInvokeCallbackType(widget, OB_ACTIVATE, NULL);
}

// 定时器回调函数 - 按顺序执行控件数组中的控件回调
int on_timer_widgets(PtWidget_t *w, void *data, PtCallbackInfo_t *cbinfo) {
	(void)w; (void)cbinfo;
	WidgetsData *widgets_data = (WidgetsData *)data;

	// 检查数据有效性
	if (widgets_data && widgets_data->widget_number && widgets_data->count > 0) {
		// 执行当前索引的控件回调
		if (AbGetABW(widgets_data->widget_number[widgets_data->current_index])) {
			emit_click(AbGetABW(widgets_data->widget_number[widgets_data->current_index]));
			printf("cur num : %d\n", widgets_data->current_index);
		}

		// 更新到下一个控件索引
		widgets_data->current_index++;
		if (widgets_data->current_index >= widgets_data->count) {
			widgets_data->current_index = 0; // 循环回到第一个控件
		}
	}
	return Pt_CONTINUE;
}

/**
 * 创建定时器并按顺序执行控件数组中的控件回调
 *
 * @param widgets_num 控件编号数组
 * @param widgets 控件指针数组
 * @param count 控件数量
 * @param interval_ms 基础周期时间（毫秒）
 * @param parent_window 定时器所在页面指针
 * @return 创建的定时器控件指针，如果创建失败则返回NULL
 */
PtWidget_t *create_periodic_control_caller(int *widgets_num, PtWidget_t **widgets, int count, int interval_ms, PtWidget_t *parent_window) {
	// 参数有效性检查
	if (!widgets || count <= 0 || interval_ms <= 0 || !parent_window) {
		return NULL;
	}

	// 为控件数据分配内存
	WidgetsData *widgets_data = (WidgetsData *)malloc(sizeof(WidgetsData));
	if (!widgets_data) {
		return NULL;
	}

	// 初始化控件数据
	widgets_data->widget_number = widgets_num;
	widgets_data->widgets = widgets;
	widgets_data->count = count;
	widgets_data->current_index = 0; // 从第一个控件开始
	widgets_data->base_interval = interval_ms;

	// 创建定时器控件
	PtWidget_t *timer = PtCreateWidget(PtTimer, parent_window, 0, NULL);
	if (!timer) {
		free(widgets_data);
		return NULL;
	}

	// 实际创建定时器控件
	PtRealizeWidget(timer);

	// 添加定时器回调函数
	PtAddCallback(timer, Pt_CB_TIMER_ACTIVATE, on_timer_widgets, widgets_data);

	// 设置定时器参数
	PtArg_t args[2];
	PtSetArg(&args[0], Pt_ARG_TIMER_INITIAL, interval_ms, 0);
	PtSetArg(&args[1], Pt_ARG_TIMER_REPEAT, interval_ms, 0);
	PtSetResources(timer, 2, args);

	// 启动定时器
	PtTimerArm(timer, interval_ms);

	return timer;
}

// 启动定时器函数 - 初始化控件并创建周期性调用器
int start_timer( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	// 消除未引用参数警告
	widget = widget; apinfo = apinfo; cbinfo = cbinfo;

    // 如果定时器已经存在，则先关闭
    if (g_global_timer) {
	PtTimerArm(g_global_timer, 0);
        PtDestroyWidget(g_global_timer);
        g_global_timer = NULL;
    }

    // 初始化按钮控件指针
//    g_button = ABW_btn_one;
//    g_button1 = ABW_btn_two;

    // 创建控件指针数组
    PtWidget_t *widgets[] = {g_button, g_button1};

//    // 创建控件编号静态数组
//    static int widgets_number[] = {
//    		3, 6  // 控件编号
//    };

    // 为全局指针赋值，使定时器回调可访问控件数组
    g_global_widgets_number = widgets_number;

    // 计算控件数量
    int widget_count = sizeof(widgets_number) / sizeof(widgets_number[0]);

    // 从传入的按钮控件获取父窗口
    PtWidget_t *parent_window = PtWidgetParent(widget);

    // 打印调试信息
//    printf("%s\t%s\t%s\n", ApInstanceName(ABW_btn_one), ApInstanceName(ABW_btn_two),
//           parent_window ? ApInstanceName(parent_window) : "NULL");

    // 创建并启动定时器，周期为1000ms
    if(parent_window) {
	g_global_timer = create_periodic_control_caller(widgets_number, widgets, widget_count, 1000, parent_window);
    }

    // 检查定时器创建结果
    if (g_global_timer) {
        printf("定时器已成功启动\n");
    } else {
        printf("定时器启动失败\n");
    }

	return Pt_CONTINUE;
}
// 停止定时器函数 - 关闭并销毁已存在的定时器
int stop_timer( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	// 消除未引用参数警告
	widget = widget; apinfo = apinfo; cbinfo = cbinfo;

    // 关闭并销毁定时器
    if (g_global_timer) {
    	PtTimerArm(g_global_timer, 0);
        PtDestroyWidget(g_global_timer);
        g_global_timer = NULL;
        printf("定时器已成功关闭\n");
    } else {
        printf("定时器未运行\n");
    }

	return Pt_CONTINUE;
}

