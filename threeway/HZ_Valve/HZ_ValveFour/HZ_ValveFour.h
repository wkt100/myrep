/*
 * HZ_ValveFour.h
 *
 *  Created on: 2021-8-2
 *      Author: shixin
 */

#ifndef HZ_VALVEFOUR_H_
#define HZ_VALVEFOUR_H_

#include <Pt.h>
#include <photon/PhProto.h>
#define HZ_COLOR182    Pt_RESOURCE( Pt_USER( 182 ), 0 )
#define HZ_ROTATE182     Pt_RESOURCE( Pt_USER( 182 ), 1 )
#define HZ_FRAME_COLOR182     Pt_RESOURCE( Pt_USER( 182 ), 4 )
#define HZ_Q182     Pt_RESOURCE( Pt_USER( 182 ), 5 )

#define HZ_ValveFour_NoneS            	0x0001
#define HZ_ValveFour_Q               	0x0002
#define HZ_ValveFour_OPEN             	0x0003
#define HZ_ValveFour_RECTS          	0x0004

#define HZ_ValveFour_0		    0x01	/* 顺时针旋转0度(默认状态)*/
#define HZ_ValveFour_90		    0x02	/* 顺时针旋转90度 */
#define HZ_ValveFour_180			0x03	/* 顺时针旋转180度 */
#define HZ_ValveFour_270			0x04	/* 顺时针旋转270度 */

typedef struct HZ_ValveFour_widget{
	PtLabelWidget_t		label;
	ushort_t            hz_height;//高
	ushort_t            hz_width;//宽
	PgColor_t           hz_color;//填充颜色
	PgColor_t           hz_frame_color;//边框颜色
	uchar_t             hz_q;
	uchar_t             hz_rotates;
	//PtCallbackList_t   *my_activate;
}HZ_ValveFourWidget;

extern PtWidgetClassRef_t *HZ_ValveFour;

#endif /* HZ_VALVEFOUR_H_ */
