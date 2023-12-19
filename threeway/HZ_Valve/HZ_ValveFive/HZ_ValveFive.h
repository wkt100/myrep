/*
 * HZ_ValveFive.h
 *
 *  Created on: 2021-8-6
 *      Author: shixin
 */

#ifndef HZ_VALVEFIVE_H_
#define HZ_VALVEFIVE_H_

#include <Pt.h>
#include <photon/PhProto.h>
#define HZ_HEIGHT186    Pt_RESOURCE(  Pt_USER( 186 ), 0 )
#define HZ_WIDTH186     Pt_RESOURCE( Pt_USER( 186 ), 1 )
#define HZ_COLOR186     Pt_RESOURCE( Pt_USER( 186 ), 2 )
#define HZ_ROTATE186     Pt_RESOURCE( Pt_USER( 186 ), 3 )
#define HZ_FRAME_COLOR186     Pt_RESOURCE( Pt_USER( 186 ), 4 )
#define HZ_Q186     Pt_RESOURCE( Pt_USER( 186 ), 5 )

#define HZ_ValveFive_NoneS            	0x0001
#define HZ_ValveFive_Q               	0x0002
#define HZ_ValveFive_OPEN             	0x0003
#define HZ_ValveFive_RECTS          	0x0004

#define HZ_ValveFive_0		    0x01	/* ˳ʱ����ת0��(Ĭ��״̬)*/
#define HZ_ValveFive_90		    0x02	/* ˳ʱ����ת90�� */
#define HZ_ValveFive_180			0x03	/* ˳ʱ����ת180�� */
#define HZ_ValveFive_270			0x04	/* ˳ʱ����ת270�� */

typedef struct HZ_ValveFive_widget{
	PtLabelWidget_t		label;
	ushort_t            hz_height;//��
	ushort_t            hz_width;//��
	PgColor_t           hz_color;//�����ɫ
	PgColor_t           hz_frame_color;//�߿���ɫ
	uchar_t             hz_q;
	uchar_t             hz_rotates;
	//PtCallbackList_t   *my_activate;
}HZ_ValveFiveWidget;

extern PtWidgetClassRef_t *HZ_ValveFive;

#endif /* HZ_VALVEFIVE_H_ */
