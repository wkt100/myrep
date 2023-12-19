/*
 * HZ_ValveThree.h
 *
 *  Created on: 2021-8-27
 *      Author: shixin
 */

#ifndef HZ_ValveThree_H_
#define HZ_ValveThree_H_

#include <Pt.h>
#include <photon/PhProto.h>
#define HZ_HEIGHT174    Pt_RESOURCE(  Pt_USER( 174 ), 0 )
#define HZ_WIDTH174     Pt_RESOURCE( Pt_USER( 174 ), 1 )
#define HZ_COLOR174     Pt_RESOURCE( Pt_USER( 174 ), 2 )
#define HZ_ROTATE174     Pt_RESOURCE( Pt_USER( 174 ), 3 )
#define HZ_FRAME_COLOR174     Pt_RESOURCE( Pt_USER( 174 ), 4 )
#define HZ_Q174     Pt_RESOURCE( Pt_USER( 174 ), 5 )

#define HZ_ValveThree_NoneS            	0x0001
#define HZ_ValveThree_Q               	0x0002
#define HZ_ValveThree_OPEN             	0x0003
#define HZ_ValveThree_RECTS          	0x0004

#define HZ_ValveThree_0		    0x01	/* ˳ʱ����ת0��(Ĭ��״̬)*/
#define HZ_ValveThree_90		    0x02	/* ˳ʱ����ת90�� */
#define HZ_ValveThree_180			0x03	/* ˳ʱ����ת180�� */
#define HZ_ValveThree_270			0x04	/* ˳ʱ����ת270�� */

typedef struct HZ_ValveThree_widget{
	PtLabelWidget_t		label;
	ushort_t            hz_height;//��
	ushort_t            hz_width;//��
	PgColor_t           hz_color;//�����ɫ
	PgColor_t           hz_frame_color;//�߿���ɫ
	uchar_t             hz_q;
	uchar_t             hz_rotates;
	//PtCallbackList_t   *my_activate;
}HZ_ValveThreeWidget;

extern PtWidgetClassRef_t *HZ_ValveThree;

#endif /* HZ_ValveThree_H_ */
