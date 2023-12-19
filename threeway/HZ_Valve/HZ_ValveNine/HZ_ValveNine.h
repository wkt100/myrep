/*
 * HZ_ValveNine.h
 *
 *  Created on: 2021-8-20
 *      Author: shixin
 */

#ifndef HZ_VALVENINE_H_
#define HZ_VALVENINE_H_

#include <Pt.h>
#include <photon/PhProto.h>
#define HZ_COLOR177    Pt_RESOURCE( Pt_USER( 177 ), 0 )
#define HZ_ROTATE177     Pt_RESOURCE( Pt_USER( 177 ), 1 )
#define HZ_FRAME_COLOR177     Pt_RESOURCE( Pt_USER( 177 ), 4 )
#define HZ_Q177     Pt_RESOURCE( Pt_USER( 177 ), 5 )

#define HZ_ValveNine_NoneS            	0x0001
#define HZ_ValveNine_Q               	0x0002
#define HZ_ValveNine_OPEN             	0x0003
#define HZ_ValveNine_RECTS          	0x0004

#define HZ_ValveNine_0		    0x01	/* ˳ʱ����ת0��(Ĭ��״̬)*/
#define HZ_ValveNine_90		    0x02	/* ˳ʱ����ת90�� */
#define HZ_ValveNine_180			0x03	/* ˳ʱ����ת180�� */
#define HZ_ValveNine_270			0x04	/* ˳ʱ����ת270�� */

typedef struct HZ_ValveNine_widget{
	PtLabelWidget_t		label;
	ushort_t            hz_height;//��
	ushort_t            hz_width;//��
	PgColor_t           hz_color;//�����ɫ
	PgColor_t           hz_frame_color;//�߿���ɫ
	uchar_t             hz_q;
	uchar_t             hz_rotates;
	//PtCallbackList_t   *my_activate;
}HZ_ValveNineWidget;

extern PtWidgetClassRef_t *HZ_ValveNine;

#endif /* HZ_VALVENINE_H_ */
