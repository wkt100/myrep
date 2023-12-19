/*
 * HZ_ValveSix.c
 *
 *  Created on: 2021-8-4
 *      Author: shixin
 */

#include "HZ_ValveSix.h"
PtWidgetClass_t *CreateHZ_ValveSixClass(void);

PtWidgetClassRef_t WHZ_ValveSix = {NULL, CreateHZ_ValveSixClass, 0};
PtWidgetClassRef_t *HZ_ValveSix = &WHZ_ValveSix;


static void HZ_ValveSix_dflts(PtWidget_t *widget )
{
	HZ_ValveSixWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;


	if ( NULL == widget )
	{
		return;
	}

	otherone = (HZ_ValveSixWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;
	otherone->hz_height = 50;
	otherone->hz_width = 50;
	otherone->hz_rotates = HZ_ValveSix_0;
	otherone->hz_color = PgRGB(128, 138, 135);
	otherone->hz_frame_color = PgRGB(0, 0, 0);
	otherone->hz_q = HZ_ValveSix_NoneS;
//	label->basic.fill_color = PgRGB(222, 222, 222);
}

static void HZ_ValveSix_draw(PtWidget_t *widget, PhTile_t *damage )
{
	HZ_ValveSixWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;
	PhRect_t   rect, center;

	if ( ( NULL == widget ) || ( NULL == damage ))
	{
		return;
	}

	otherone = (HZ_ValveSixWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;

	label->basic.color = Pg_TRANSPARENT;
	PtSuperClassDraw(PtLabel, widget, damage );
	PtCalcCanvas(widget, &rect);
	PtClipAdd(widget, &rect);
	short width = rect.lr.x - rect.ul.x;
	short height = rect.lr.y - rect.ul.y;

	PgSetFillColor(otherone->hz_color);
	PgSetStrokeColor( Pg_BLACK );//线颜色
	PgSetStrokeWidth(2);//线粗细
	PhPoint_t o = {0, 0};
	PhPoint_t  up_triangle1_point1, up_triangle1_point2, up_triangle1_point3;
	switch(otherone->hz_rotates)
	{
		case HZ_ValveSix_90:
		{
					PhPoint_t c,r;
					c.x = rect.lr.x - width / 3;
					c.y = rect.ul.y + height / 2;
					r.x = width / 3;
					r.y = height / 4;
					PgDrawArc(&c, &r, 0xBFFF, 0x3FFF, Pg_DRAW_FILL_STROKE | Pg_ARC);
					PgDrawILine(c.x , c.y - r.y, c.x, c.y + r.y);
					up_triangle1_point1.x = c.x - width*0/40;
					up_triangle1_point1.y = c.y - height*8/30;
					up_triangle1_point2.x = c.x + width*0/40;
					up_triangle1_point2.y = c.y + height*8/30;
					up_triangle1_point3.x = c.x - width*10/30;
					up_triangle1_point3.y = c.y +height*0/30;
					PhPoint_t  p[] = { up_triangle1_point1, up_triangle1_point2, up_triangle1_point3 };//多边形点的坐标
					PgDrawPolygon( &p, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED );
					//PgDrawILine(rect.lr.x - width / 3, rect.ul.y + height / 2, rect.lr.x - width / 3 * 2, rect.ul.y + height / 2);
					PhPoint_t p1[] = {rect.lr.x - width / 3 * 2, rect.ul.y + height / 2, width / 3, -height / 2, -width / 3 * 2, 0};
					PgDrawPolygon(&p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					PhPoint_t p2[] = {rect.lr.x - width / 3 * 2, rect.ul.y + height / 2, width / 3, height / 2, -width / 3 * 2, 0};
					PgDrawPolygon(&p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					break;
				}
		case HZ_ValveSix_180:
		{
					PhPoint_t c,r;
					c.x = rect.ul.x + width / 2;
					c.y = rect.lr.y - height / 3;
					r.x = width / 4;
					r.y = height / 3;
					PgDrawArc(&c, &r, 0x7FFF, 0x0000, Pg_DRAW_FILL_STROKE | Pg_ARC);
					PgDrawILine(c.x - r.x, c.y, c.x + r.x, c.y);
					up_triangle1_point1.x = c.x - width*13/50;
					up_triangle1_point1.y = c.y - height*0/30;
					up_triangle1_point2.x = c.x + width*13/50;
					up_triangle1_point2.y = c.y - height*0/30;
					up_triangle1_point3.x = c.x + width*0/40;
					up_triangle1_point3.y = c.y -height*10/30;
					PhPoint_t  p[] = { up_triangle1_point1, up_triangle1_point2, up_triangle1_point3 };//多边形点的坐标
					PgDrawPolygon( &p, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED );
					//PgDrawILine(rect.ul.x + width / 2, rect.lr.y - height / 3, rect.ul.x + width / 2, rect.lr.y - height / 3 * 2);
					PhPoint_t p1[] = {rect.ul.x + width / 2, rect.lr.y - height / 3 * 2, -width / 2, -height / 3, 0, height / 3 * 2};
					PgDrawPolygon(&p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					PhPoint_t p2[] = {rect.ul.x + width / 2, rect.lr.y - height / 3 * 2, width / 2, -height / 3, 0, height / 3 * 2};
					PgDrawPolygon(&p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					break;
				}
		case HZ_ValveSix_270:
		{
					PhPoint_t c,r;
					c.x = rect.ul.x + width / 3;
					c.y = rect.ul.y + height / 2;
					r.x = width / 3;
					r.y = height / 4;
					PgDrawArc(&c, &r, 0x3FFF, 0xBFFF, Pg_DRAW_FILL_STROKE | Pg_ARC);
					PgDrawILine(c.x , c.y - r.y, c.x, c.y + r.y);
					up_triangle1_point1.x = c.x - width*0/40;
					up_triangle1_point1.y = c.y - height*8/30;
					up_triangle1_point2.x = c.x + width*13/40;
					up_triangle1_point2.y = c.y + height*0/30;
					up_triangle1_point3.x = c.x;
					up_triangle1_point3.y = c.y +height*8/30;
					PhPoint_t  p[] = { up_triangle1_point1, up_triangle1_point2, up_triangle1_point3 };//多边形点的坐标
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
			PhPoint_t c,r;
			c.x = rect.ul.x + width / 2;
			c.y = rect.ul.y + height / 3;
			r.x = width / 4;
			r.y = height / 3;
			PgDrawArc(&c, &r, 0x0000, 0x7FFF, Pg_DRAW_FILL_STROKE | Pg_ARC);
			PgDrawILine(c.x - r.x, c.y, c.x + r.x, c.y);
			up_triangle1_point1.x = c.x - width*13/50;
			up_triangle1_point1.y = c.y - height*0/30;
			up_triangle1_point2.x = c.x + width*13/50;
			up_triangle1_point2.y = c.y - height*0/30;
			up_triangle1_point3.x = c.x;
			up_triangle1_point3.y = c.y +height*10/30;
			PhPoint_t  p[] = { up_triangle1_point1, up_triangle1_point2, up_triangle1_point3 };//多边形点的坐标
			PgDrawPolygon( &p, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED );
			//PgDrawILine(rect.ul.x + width / 2, rect.ul.y + height / 3, rect.ul.x + width / 2, rect.ul.y + height / 3 * 2);
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

PtWidgetClass_t *CreateHZ_ValveSixClass(void)
{
	static PtResourceRec_t resources[] = {
			{HZ_ROTATE183, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveSixWidget, hz_rotates ), 0},
			{HZ_COLOR183, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveSixWidget, hz_color ), 0},
			{HZ_FRAME_COLOR183, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveSixWidget, hz_frame_color ), 0},
			{HZ_Q183, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveSixWidget, hz_q ), 0},
			};


	static PtArg_t args[] = {
			{ Pt_SET_VERSION, 110, 0},
			{ Pt_SET_STATE_LEN, sizeof(HZ_ValveSixWidget ), 0},
			{ Pt_SET_DFLTS_F, (long)HZ_ValveSix_dflts, 0 },
			{ Pt_SET_DRAW_F, (long)HZ_ValveSix_draw, 0 },
			{ Pt_SET_NUM_RESOURCES,
			sizeof( resources ) / sizeof( resources[0] ), 0 },
			{ Pt_SET_RESOURCES, (long)resources,
			sizeof( resources ) / sizeof( resources[0] )},
			};
	return (HZ_ValveSix->wclass = PtCreateWidgetClass(
			PtLabel, 0, sizeof(args)/sizeof(args[0]), args));
}
