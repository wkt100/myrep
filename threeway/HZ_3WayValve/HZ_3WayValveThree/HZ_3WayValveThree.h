/*
 * HZ_3WayValveThree.h
 *
 *  Created on: 2021-7-30
 *      Author: shixin
 */

#ifndef HZ_3WAYVALVETHREE_H_
#define HZ_3WAYVALVETHREE_H_

#include <Pt.h>
#include <photon/PhProto.h>
#define HZ_HEIGHT181    Pt_RESOURCE(  Pt_USER( 181 ), 0 )
#define HZ_WIDTH181     Pt_RESOURCE( Pt_USER( 181 ), 1 )
#define HZ_COLOR181     Pt_RESOURCE( Pt_USER( 181 ), 2 )
#define HZ_ROTATE181     Pt_RESOURCE( Pt_USER( 181 ), 3 )
#define HZ_FRAME_COLOR181     Pt_RESOURCE( Pt_USER( 181 ), 4 )
#define HZ_Q181     Pt_RESOURCE( Pt_USER( 181 ), 5 )

#define HZ_3WayValveThree_NoneS            	0x0001
#define HZ_3WayValveThree_Q               	0x0002
#define HZ_3WayValveThree_OPEN             	0x0003
#define HZ_3WayValveThree_RECTS          	0x0004

#define HZ_3WayValveThree_0		    0x01	/* 顺时针旋转0度(默认状态)*/
#define HZ_3WayValveThree_90		    0x02	/* 顺时针旋转90度 */
#define HZ_3WayValveThree_180			0x03	/* 顺时针旋转180度 */
#define HZ_3WayValveThree_270			0x04	/* 顺时针旋转270度 */

typedef struct HZ_3WayValveThree_widget{
	PtLabelWidget_t		label;
	ushort_t            hz_height;//高
	ushort_t            hz_width;//宽
	PgColor_t           hz_color;//填充颜色
	PgColor_t           hz_frame_color;//边框颜色
	uchar_t             hz_q;
	uchar_t             hz_rotates;
	//PtCallbackList_t   *my_activate;
}HZ_3WayValveThreeWidget;

extern PtWidgetClassRef_t *HZ_3WayValveThree;
#endif /* HZ_3WAYVALVETHREE_H_ */
