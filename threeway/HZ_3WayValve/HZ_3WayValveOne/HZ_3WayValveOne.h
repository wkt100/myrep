/*
 * HZ_3WayValveOne.h
 *
 *  Created on: 2021-7-13
 *      Author: shixin
 */

#ifndef HZ_3WAYVALVEONE_H_
#define HZ_3WAYVALVEONE_H_

#include <Pt.h>
#include <photon/PhProto.h>
#define HZ_HEIGHT155    Pt_RESOURCE(  Pt_USER( 155 ), 0 )
#define HZ_WIDTH155     Pt_RESOURCE( Pt_USER( 155 ), 1 )
#define HZ_COLOR155     Pt_RESOURCE( Pt_USER( 155 ), 2 )
#define HZ_ROTATE155     Pt_RESOURCE( Pt_USER( 155 ), 3 )
#define HZ_FRAME_COLOR155     Pt_RESOURCE( Pt_USER( 155 ), 4 )
#define HZ_Q155     Pt_RESOURCE( Pt_USER( 155 ), 5 )

#define HZ_3WayValveOne_NoneS            	0x0001
#define HZ_3WayValveOne_Q               	0x0002
#define HZ_3WayValveOne_OPEN             	0x0003
#define HZ_3WayValveOne_RECTS          	0x0004

#define HZ_3WayValveOne_0		    0x01	/* ˳ʱ����ת0��(Ĭ��״̬)*/
#define HZ_3WayValveOne_90		    0x02	/* ˳ʱ����ת90�� */
#define HZ_3WayValveOne_180			0x03	/* ˳ʱ����ת180�� */
#define HZ_3WayValveOne_270			0x04	/* ˳ʱ����ת270�� */

typedef struct HZ_3WayValveOne_widget{
	PtLabelWidget_t		label;
	ushort_t            hz_height;//��
	ushort_t            hz_width;//��
	PgColor_t           hz_color;//�����ɫ
	PgColor_t           hz_frame_color;//�߿���ɫ
	uchar_t             hz_q;
	uchar_t             hz_rotates;
	//PtCallbackList_t   *my_activate;
}HZ_3WayValveOneWidget;

extern PtWidgetClassRef_t *HZ_3WayValveOne;

#endif /* HZ_3WAYVALVEONE_H_ */
