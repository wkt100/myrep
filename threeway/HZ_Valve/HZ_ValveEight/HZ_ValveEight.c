/*
 * HZ_ValveEight.c
 *
 *  Created on: 2021-8-6
 *      Author: shixin
 */

#include "HZ_ValveEight.h"

PtWidgetClass_t *CreateHZ_ValveEightClass(void);
PtWidgetClassRef_t WHZ_ValveEight = { NULL,
                                    CreateHZ_ValveEightClass, 0};
PtWidgetClassRef_t *HZ_ValveEight = &WHZ_ValveEight;

static void HZ_ValveEight_dflts(PtWidget_t *widget )
{
	HZ_ValveEightWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;


	if ( NULL == widget )
	{
		return;
	}

	otherone = (HZ_ValveEightWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;
	otherone->hz_height = 50;
	otherone->hz_width = 50;
	otherone->hz_rotates = HZ_ValveEight_0;
	otherone->hz_color = PgRGB(128, 138, 135);
	otherone->hz_frame_color = PgRGB(0, 0, 0);
	otherone->hz_q = HZ_ValveEight_NoneS;
//	label->basic.fill_color = PgRGB(222, 222, 222);
}

static void HZ_ValveEight_draw(PtWidget_t *widget, PhTile_t *damage )
{
	HZ_ValveEightWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;
	PhRect_t   rect, center;

	if ( ( NULL == widget ) || ( NULL == damage ))
	{
		return;
	}

	otherone = (HZ_ValveEightWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;

	label->basic.color = Pg_TRANSPARENT;
	PtSuperClassDraw(PtLabel, widget, damage );
	PtCalcCanvas(widget, &rect);
	PtClipAdd(widget, &rect);
	short width = rect.lr.x - rect.ul.x;
	short height = rect.lr.y - rect.ul.y;

	PgSetFillColor(otherone->hz_color);
	PgSetStrokeColor( Pg_BLACK );//����ɫ
	PgSetStrokeWidth(1);//�ߴ�ϸ
	PhPoint_t o = {0, 0};
	PhPoint_t  up_triangle1_point1, up_triangle1_point2, up_triangle1_point3;
	PhPoint_t c,r,cir,rir;
	c.x = rect.ul.x + width / 2;
			c.y = rect.ul.y + height / 3;
			r.x = width / 4;
			r.y = height / 8;
	
	PhPoint_t p_0[] = {rect.ul.x + width / 8 * 3, rect.lr.y - height / 1.8, rect.ul.x + width / 8 * 3, rect.ul.y + height / 4, rect.ul.x + width / 2, rect.lr.y - height / 1.5, rect.lr.x - width / 8 * 3, rect.ul.y + height / 4, rect.lr.x - width / 8 * 3, rect.lr.y - height / 1.8};
	PhPoint_t p_90[] = {rect.ul.x + width / 1.6, rect.ul.y + height / 8 * 3, rect.lr.x - width / 4.9, rect.ul.y + height / 8 * 3, rect.ul.x + width / 1.5, rect.ul.y + height / 2, rect.lr.x - width / 4.9, rect.lr.y - height / 8 * 3, rect.ul.x + width / 1.6, rect.lr.y - height / 8 * 3};
	PhPoint_t p_180[] = {rect.ul.x + width / 8 * 3, rect.ul.y + height / 1.8, rect.ul.x + width / 8 * 3, rect.lr.y - height / 4, rect.ul.x + width / 2, rect.ul.y + height / 1.5, rect.lr.x - width / 8 * 3.1, rect.lr.y - height / 4, rect.lr.x - width / 8 * 3.1, rect.ul.y + height / 1.8};
	PhPoint_t p_270[] = {rect.lr.x - width / 1.6, rect.ul.y + height / 8 * 3.5, rect.ul.x + width / 5.6, rect.ul.y + height / 8 * 3.5, rect.lr.x - width / 1.5, rect.ul.y + height / 1.9, rect.ul.x + width / 5.6, rect.lr.y - height / 8 * 3, rect.lr.x - width / 1.6, rect.lr.y - height / 8 * 3};
	switch(otherone->hz_rotates)
	{
		
		case HZ_ValveEight_90:
		{
					cir.x = rect.ul.x + width/2 + width*13/60;
							cir.y = rect.ul.y + height/2;
							rir.x =  (width/2 + width*10/60)-(width/2);
							rir.y =  (height/2 - height*13.5/60)-(height/2 - height*3/60);
			PgDrawEllipse(&cir, &rir, Pg_DRAW_FILL_STROKE);
			
					up_triangle1_point1.x = c.x - width*-2/40;
					up_triangle1_point1.y = c.y - height*0/30;
					up_triangle1_point2.x = c.x + width*-7/40;
					up_triangle1_point2.y = c.y + height*5/30;
					up_triangle1_point3.x = c.x - width*-1.5/30;
					up_triangle1_point3.y = c.y +height*10/30;
					PhPoint_t  p[] = { up_triangle1_point1, up_triangle1_point2, up_triangle1_point3 };//����ε������
					PgDrawPolygon( &p, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED );
					
					//PgDrawILine(rect.lr.x - width / 3, rect.ul.y + height / 2, rect.lr.x - width / 3 * 2, rect.ul.y + height / 2);
					PhPoint_t p1[] = {rect.lr.x - width / 3 * 2, rect.ul.y + height / 2, width / 3, -height / 2, -width / 3 * 2, 0};
					PgDrawPolygon(&p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					PhPoint_t p2[] = {rect.lr.x - width / 3 * 2, rect.ul.y + height / 2, width / 3, height / 2, -width / 3 * 2, 0};
					PgDrawPolygon(&p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					
					PgDrawPolygon(&p_90, 5, &o, Pg_DRAW_STROKE);
					break;
		}
		case HZ_ValveEight_180:
		{
			cir.x = rect.ul.x + width/2 - width*1/60;
							cir.y = rect.ul.y + height/1.5;
							rir.x =  (width/2 + width*10/60)-(width/2.2);
							rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);
			PgDrawEllipse(&cir, &rir, Pg_DRAW_FILL_STROKE);
			PgDrawPolygon(&p_180, 5, &o, Pg_DRAW_STROKE);
					
					up_triangle1_point1.x = c.x - width*8/50;
					up_triangle1_point1.y = c.y - height*-5/30;
					up_triangle1_point2.x = c.x + width*8/50;
					up_triangle1_point2.y = c.y - height*-5/30;
					up_triangle1_point3.x = c.x + width*0/40;
					up_triangle1_point3.y = c.y - height*0/30;
					PhPoint_t  p[] = { up_triangle1_point1, up_triangle1_point2, up_triangle1_point3 };//����ε������
					PgDrawPolygon( &p, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED );
					//PgDrawILine(rect.ul.x + width / 2, rect.lr.y - height / 3, rect.ul.x + width / 2, rect.lr.y - height / 3 * 2);
					PhPoint_t p1[] = {rect.ul.x + width / 2, rect.lr.y - height / 3 * 2, -width / 2, -height / 3, 0, height / 3 * 2};
					PgDrawPolygon(&p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					PhPoint_t p2[] = {rect.ul.x + width / 2, rect.lr.y - height / 3 * 2, width / 2, -height / 3, 0, height / 3 * 2};
					PgDrawPolygon(&p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					break;
		}
		case HZ_ValveEight_270:
		{
			cir.x = rect.ul.x + width/4.2;
							cir.y = rect.ul.y + height/2 + height*2/60;
							rir.x =  (width/2 + width*10/60)-(width/2);
							rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);
			PgDrawEllipse(&cir, &rir, Pg_DRAW_FILL_STROKE);
			PgDrawPolygon(&p_270, 5, &o, Pg_DRAW_STROKE);
					up_triangle1_point1.x = c.x - width*4/40;
					up_triangle1_point1.y = c.y - height*0/30;
					up_triangle1_point2.x = c.x + width*6/40;
					up_triangle1_point2.y = c.y + height*5/30;
					up_triangle1_point3.x = c.x - width*3/30;
					up_triangle1_point3.y = c.y +height*10/30;
					PhPoint_t  p[] = { up_triangle1_point1, up_triangle1_point2, up_triangle1_point3 };//����ε������
					PgDrawPolygon( &p, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED );
					//PgDrawILine(rect.ul.x + width / 3, rect.ul.y + height / 2, rect.ul.x + width / 3 * 2, rect.ul.y + height / 2);
					PhPoint_t p1[] = {rect.ul.x + width / 3 * 2, rect.ul.y + height / 2, width / 3, -height / 2, -width / 3 * 2, 0};
					PgDrawPolygon(&p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					PhPoint_t p2[] = {rect.ul.x + width / 3 * 2, rect.ul.y + height / 2, width / 3, height / 2, -width / 3 * 2, 0};
					PgDrawPolygon(&p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					break;
		}
		default:
		{
			c.x = rect.ul.x + width / 2;
			c.y = rect.ul.y + height / 3;
			r.x = width / 4;
			r.y = height / 8;
			PgDrawEllipse(&c, &r, Pg_DRAW_FILL_STROKE);
			PgDrawPolygon(&p_0, 5, &o, Pg_DRAW_STROKE);
			
			up_triangle1_point1.x = c.x - width*8/50;
			up_triangle1_point1.y = c.y - height*-4/30;
			up_triangle1_point2.x = c.x + width*8/50;
			up_triangle1_point2.y = c.y - height*-4/30;
			up_triangle1_point3.x = c.x;
			up_triangle1_point3.y = c.y +height*10/30;
			PhPoint_t  p[] = { up_triangle1_point1, up_triangle1_point2, up_triangle1_point3 };//����ε������
			PgDrawPolygon( &p, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED );
			
			
			PhPoint_t p1[] = {rect.ul.x + width / 2, rect.ul.y + height / 3 * 2, -width / 2, -height / 3, 0, height / 3 * 2};
			PgDrawPolygon(&p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			PhPoint_t p2[] = {rect.ul.x + width / 2, rect.ul.y + height / 3 * 2, width / 2, -height / 3, 0, height / 3 * 2};
			PgDrawPolygon(&p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			
			
			break;
		}
	}

	center.ul.x = rect.ul.x + 1;
	center.ul.y = rect.ul.y + 1;
	center.lr.x = rect.lr.x;
	center.lr.y = rect.lr.y;
	unsigned int b_rect_show_s = 1;
	//q(255,0,0),open(0,176,80)
	PgColor_t frame_color_temp = otherone->hz_frame_color;
	switch(otherone->hz_q)
	{
		case 0x01:
			b_rect_show_s = 0;
			break;
		case 0x02:
			b_rect_show_s = 1;
			frame_color_temp = PgRGB(255,0,0);
			break;
		case 0x03:
			b_rect_show_s = 1;
			frame_color_temp = PgRGB(0,130,130);
			break;
		default:
			b_rect_show_s = 1;
			break;
	}
	//outside frame
	if( b_rect_show_s == 1 )
	{
		PgSetStrokeWidth(2);
		PgSetStrokeColor(frame_color_temp);
		PgDrawRect( &center, Pg_DRAW_STROKE );
	}
	PtClipRemove();
}

PtWidgetClass_t *CreateHZ_ValveEightClass(void)
{
	static PtResourceRec_t resources[] = {
			{HZ_ROTATE185, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveEightWidget, hz_rotates ), 0},
			{HZ_COLOR185, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveEightWidget, hz_color ), 0},
			{HZ_FRAME_COLOR185, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveEightWidget, hz_frame_color ), 0},
			{HZ_Q185, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveEightWidget, hz_q ), 0},
			};


	static PtArg_t args[] = {
			{ Pt_SET_VERSION, 110, 0},
			{ Pt_SET_STATE_LEN, sizeof(HZ_ValveEightWidget ), 0},
			{ Pt_SET_DFLTS_F, (long)HZ_ValveEight_dflts, 0 },
			{ Pt_SET_DRAW_F, (long)HZ_ValveEight_draw, 0 },
			{ Pt_SET_NUM_RESOURCES,
			sizeof( resources ) / sizeof( resources[0] ), 0 },
			{ Pt_SET_RESOURCES, (long)resources,
			sizeof( resources ) / sizeof( resources[0] )},
			};
	return (HZ_ValveEight->wclass = PtCreateWidgetClass(
			PtLabel, 0, sizeof(args)/sizeof(args[0]), args));
}
