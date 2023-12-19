/*
 * HZ_ValveSix.h
 *
 *  Created on: 2021-8-4
 *      Author: shixin
 */

#ifndef HZ_VALVESIX_H_
#define HZ_VALVESIX_H_

#include <Pt.h>
#include <photon/PhProto.h>
#define HZ_COLOR183    Pt_RESOURCE( Pt_USER( 183 ), 0 )
#define HZ_ROTATE183     Pt_RESOURCE( Pt_USER( 183 ), 1 )
#define HZ_FRAME_COLOR183     Pt_RESOURCE( Pt_USER( 183 ), 4 )
#define HZ_Q183     Pt_RESOURCE( Pt_USER( 183 ), 5 )

#define HZ_ValveSix_NoneS            	0x0001
#define HZ_ValveSix_Q               	0x0002
#define HZ_ValveSix_OPEN             	0x0003
#define HZ_ValveSix_RECTS          	0x0004

#define HZ_ValveSix_0		    0x01	/* 顺时针旋转0度(默认状态)*/
#define HZ_ValveSix_90		    0x02	/* 顺时针旋转90度 */
#define HZ_ValveSix_180			0x03	/* 顺时针旋转180度 */
#define HZ_ValveSix_270			0x04	/* 顺时针旋转270度 */

typedef struct HZ_ValveSix_widget{
	PtLabelWidget_t		labHZ_el;
	ushort_t            hz_height;//高
	ushort_t            hz_width;//宽
	PgColor_t           hz_color;//填充颜色
	PgColor_t           hz_frame_color;//边框颜色
	uchar_t             hz_q;
	uchar_t             hz_rotates;
	//PtCallbackList_t   *my_activate;
}HZ_ValveSixWidget;

extern PtWidgetClassRef_t *HZ_ValveSix;

#endif /* HZ_VALVESIX_H_ */
