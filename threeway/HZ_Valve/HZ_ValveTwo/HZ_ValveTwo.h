/*
 * HZ_ValveTen.h
 *
 *  Created on: 2021-7-7
 *      Author: shixin
 */

#ifndef HZ_VALVETWO_H_
#define HZ_VALVETWO_H_


#include <Pt.h>
#include <photon/PhProto.h>
#define HZ_HEIGHT175    Pt_RESOURCE(  Pt_USER( 175 ), 0 )
#define HZ_WIDTH175     Pt_RESOURCE( Pt_USER( 175 ), 1 )
#define HZ_COLOR175     Pt_RESOURCE( Pt_USER( 175 ), 2 )
#define HZ_ROTATE175     Pt_RESOURCE( Pt_USER( 175 ), 3 )
#define HZ_FRAME_COLOR175     Pt_RESOURCE( Pt_USER( 175 ), 4 )
#define HZ_Q175     Pt_RESOURCE( Pt_USER( 175 ), 5 )

#define HZ_ValveTwo_NoneS            	0x0001
#define HZ_ValveTwo_Q               	0x0002
#define HZ_ValveTwo_OPEN             	0x0003
#define HZ_ValveTwo_RECTS          	0x0004

#define HZ_ValveTwo_0		    0x01	/* ˳ʱ����ת0��(Ĭ��״̬)*/
#define HZ_ValveTwo_90		    0x02	/* ˳ʱ����ת90�� */
#define HZ_ValveTwo_180			0x03	/* ˳ʱ����ת180�� */
#define HZ_ValveTwo_270			0x04	/* ˳ʱ����ת270�� */

typedef struct HZ_ValveTwo_widget{
	PtLabelWidget_t		label;
	ushort_t            hz_height;//��
	ushort_t            hz_width;//��
	PgColor_t           hz_color;//�����ɫ
	PgColor_t           hz_frame_color;//�߿���ɫ
	uchar_t             hz_q;
	uchar_t             hz_rotates;
	//PtCallbackList_t   *my_activate;
}HZ_ValveTwoWidget;

extern PtWidgetClassRef_t *HZ_ValveTwo;

#endif /* HZ_VALVETEN_H_ */
