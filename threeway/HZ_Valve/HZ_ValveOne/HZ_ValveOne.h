/*
 * HZ_ValveOne.h
 *
 *  Created on: 2021-7-13
 *      Author: shixin
 */

#ifndef HZ_VALVEONE_H_
#define HZ_VALVEONE_H_

#include <Pt.h>
#include <photon/PhProto.h>
#define HZ_HEIGHT173    Pt_RESOURCE(  Pt_USER( 173 ), 0 )
#define HZ_WIDTH173     Pt_RESOURCE( Pt_USER( 173 ), 1 )
#define HZ_COLOR173     Pt_RESOURCE( Pt_USER( 173 ), 2 )
#define HZ_ROTATE173     Pt_RESOURCE( Pt_USER( 173 ), 3 )
#define HZ_FRAME_COLOR173     Pt_RESOURCE( Pt_USER( 173 ), 4 )
#define HZ_Q173     Pt_RESOURCE( Pt_USER( 173 ), 5 )

#define HZ_ValveOne_NoneS            	0x0001
#define HZ_ValveOne_Q               	0x0002
#define HZ_ValveOne_OPEN             	0x0003
#define HZ_ValveOne_RECTS          	0x0004

#define HZ_ValveOne_0		    0x01	/* 顺时针旋转0度(默认状态)*/
#define HZ_ValveOne_90		    0x02	/* 顺时针旋转90度 */
#define HZ_ValveOne_180			0x03	/* 顺时针旋转180度 */
#define HZ_ValveOne_270			0x04	/* 顺时针旋转270度 */

typedef struct HZ_ValveOne_widget{
	PtLabelWidget_t		label;
	ushort_t            hz_height;//高
	ushort_t            hz_width;//宽
	PgColor_t           hz_color;//填充颜色
	PgColor_t           hz_frame_color;//边框颜色
	uchar_t             hz_q;
	uchar_t             hz_rotates;
	//PtCallbackList_t   *my_activate;
}HZ_ValveOneWidget;

extern PtWidgetClassRef_t *HZ_ValveOne;

#endif /* HZ_VALVEONE_H_ */
