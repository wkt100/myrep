/*
 * HZ_3WayValveTwo.h
 *
 *  Created on: 2021-8-18
 *      Author: shixin
 */

#ifndef HZ_3WAYVALVETWO_H_
#define HZ_3WAYVALVETWO_H_

#include <Pt.h>
#include <photon/PhProto.h>
#define HZ_HEIGHT172    Pt_RESOURCE(  Pt_USER( 172 ), 0 )
#define HZ_WIDTH172     Pt_RESOURCE( Pt_USER( 172 ), 1 )
#define HZ_COLOR172     Pt_RESOURCE( Pt_USER( 172 ), 2 )
#define HZ_ROTATE172     Pt_RESOURCE( Pt_USER( 172 ), 3 )
#define HZ_FRAME_COLOR172     Pt_RESOURCE( Pt_USER( 172 ), 4 )
#define HZ_Q172     Pt_RESOURCE( Pt_USER( 172 ), 5 )

#define HZ_3WayValveTwo_NoneS            	0x0001
#define HZ_3WayValveTwo_Q               	0x0002
#define HZ_3WayValveTwo_OPEN             	0x0003
#define HZ_3WayValveTwo_RECTS          	0x0004

#define HZ_3WayValveTwo_0		    0x01	/* 顺时针旋转0度(默认状态)*/
#define HZ_3WayValveTwo_90		    0x02	/* 顺时针旋转90度 */
#define HZ_3WayValveTwo_180			0x03	/* 顺时针旋转180度 */
#define HZ_3WayValveTwo_270			0x04	/* 顺时针旋转270度 */

typedef struct HZ_3WayValveTwo_widget{
	PtLabelWidget_t		label;
	ushort_t            hz_height;//高
	ushort_t            hz_width;//宽
	PgColor_t           hz_color;//填充颜色
	PgColor_t           hz_frame_color;//边框颜色
	uchar_t             hz_q;
	uchar_t             hz_rotates;
	//PtCallbackList_t   *my_activate;
}HZ_3WayValveTwoWidget;

extern PtWidgetClassRef_t *HZ_3WayValveTwo;

#endif /* HZ_3WAYVALVETWO_H_ */
