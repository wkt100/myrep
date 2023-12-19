/*
 * HZ_ValveTen.c
 *
 *  Created on: 2021-7-7
 *      Author: shixin
 */

#include "HZ_ValveTwo.h"
PtWidgetClass_t *CreateHZ_ValveTwoClass(void);

PtWidgetClassRef_t WHZ_ValveTwo = {NULL, CreateHZ_ValveTwoClass, 0};
PtWidgetClassRef_t *HZ_ValveTwo = &WHZ_ValveTwo;


static void HZ_ValveTwo_dflts(PtWidget_t *widget )
{
	HZ_ValveTwoWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;


	if ( NULL == widget )
	{
		return;
	}

	otherone = (HZ_ValveTwoWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;
	otherone->hz_height = 50;
	otherone->hz_width = 50;
	otherone->hz_rotates = HZ_ValveTwo_0;
	otherone->hz_color = PgRGB(128, 138, 135);
	otherone->hz_frame_color = PgRGB(0, 0, 0);
	otherone->hz_q = HZ_ValveTwo_NoneS;
//	label->basic.fill_color = PgRGB(222, 222, 222);
}

static void HZ_ValveTwo_draw(PtWidget_t *widget, PhTile_t *damage )
{
	HZ_ValveTwoWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;
	PhRect_t   rect, center;

	if ( ( NULL == widget ) || ( NULL == damage ))
	{
		return;
	}

	otherone = (HZ_ValveTwoWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;

	label->basic.color = Pg_TRANSPARENT;
	PtSuperClassDraw(PtLabel, widget, damage );
	PtCalcCanvas(widget, &rect);
	PtClipAdd(widget, &rect);
	short width = rect.lr.x - rect.ul.x;
	short height = rect.lr.y - rect.ul.y;

	PgSetFillColor(otherone->hz_color);
	PgSetStrokeColor( Pg_BLACK );//ÏßÑÕÉ«
	PgSetStrokeWidth(2);//Ïß´ÖÏ¸
	PhPoint_t o = {0, 0};
	switch(otherone->hz_rotates)
	{
		case HZ_ValveTwo_90:
		{
					PhPoint_t c,r;
					c.x = rect.lr.x - width / 3;
					c.y = rect.ul.y + height / 2;
					r.x = width / 3;
					r.y = height / 4;
					PgDrawArc(&c, &r, 0xBFFF, 0x3FFF, Pg_DRAW_FILL_STROKE | Pg_ARC);
					PgDrawILine(c.x , c.y - r.y, c.x, c.y + r.y);
					PgDrawILine(rect.lr.x - width / 3, rect.ul.y + height / 2, rect.lr.x - width / 3 * 2, rect.ul.y + height / 2);
					PhPoint_t p1[] = {rect.lr.x - width / 3 * 2, rect.ul.y + height / 2, width / 3, -height / 2, -width / 3 * 2, 0};
					PgDrawPolygon(p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					PhPoint_t p2[] = {rect.lr.x - width / 3 * 2, rect.ul.y + height / 2, width / 3, height / 2, -width / 3 * 2, 0};
					PgDrawPolygon(p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					break;
				}
		case HZ_ValveTwo_180:
		{
					PhPoint_t c,r;
					c.x = rect.ul.x + width / 2;
					c.y = rect.lr.y - height / 3;
					r.x = width / 4;
					r.y = height / 3;
					PgDrawArc(&c, &r, 0x7FFF, 0x0000, Pg_DRAW_FILL_STROKE | Pg_ARC);
					PgDrawILine(c.x - r.x, c.y, c.x + r.x, c.y);
					PgDrawILine(rect.ul.x + width / 2, rect.lr.y - height / 3, rect.ul.x + width / 2, rect.lr.y - height / 3 * 2);
					PhPoint_t p1[] = {rect.ul.x + width / 2, rect.lr.y - height / 3 * 2, -width / 2, -height / 3, 0, height / 3 * 2};
					PgDrawPolygon(p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					PhPoint_t p2[] = {rect.ul.x + width / 2, rect.lr.y - height / 3 * 2, width / 2, -height / 3, 0, height / 3 * 2};
					PgDrawPolygon(p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					break;
				}
		case HZ_ValveTwo_270:
		{
					PhPoint_t c,r;
					c.x = rect.ul.x + width / 3;
					c.y = rect.ul.y + height / 2;
					r.x = width / 3;
					r.y = height / 4;
					PgDrawArc(&c, &r, 0x3FFF, 0xBFFF, Pg_DRAW_FILL_STROKE | Pg_ARC);
					PgDrawILine(c.x , c.y - r.y, c.x, c.y + r.y);
					PgDrawILine(rect.ul.x + width / 3, rect.ul.y + height / 2, rect.ul.x + width / 3 * 2, rect.ul.y + height / 2);
					PhPoint_t p1[] = {rect.ul.x + width / 3 * 2, rect.ul.y + height / 2, width / 3, -height / 2, -width / 3 * 2, 0};
					PgDrawPolygon(p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
					PhPoint_t p2[] = {rect.ul.x + width / 3 * 2, rect.ul.y + height / 2, width / 3, height / 2, -width / 3 * 2, 0};
					PgDrawPolygon(p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
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
			PgDrawILine(rect.ul.x + width / 2, rect.ul.y + height / 3, rect.ul.x + width / 2, rect.ul.y + height / 3 * 2);
			PhPoint_t p1[] = {rect.ul.x + width / 2, rect.ul.y + height / 3 * 2, -width / 2, -height / 3, 0, height / 3 * 2};
			PgDrawPolygon(p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			PhPoint_t p2[] = {rect.ul.x + width / 2, rect.ul.y + height / 3 * 2, width / 2, -height / 3, 0, height / 3 * 2};
			PgDrawPolygon(p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
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

PtWidgetClass_t *CreateHZ_ValveTwoClass(void)
{
	static PtResourceRec_t resources[] = {
			{HZ_HEIGHT175, Pt_CHANGE_RESIZE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveTwoWidget, hz_height ), 0},
			{HZ_WIDTH175, Pt_CHANGE_RESIZE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveTwoWidget, hz_width ), 0},
			{HZ_ROTATE175, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveTwoWidget, hz_rotates ), 0},
			{HZ_COLOR175, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveTwoWidget, hz_color ), 0},
			{HZ_FRAME_COLOR175, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveTwoWidget, hz_frame_color ), 0},
			{HZ_Q175, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveTwoWidget, hz_q ), 0},
			};


	static PtArg_t args[] = {
			{ Pt_SET_VERSION, 110, 0},
			{ Pt_SET_STATE_LEN, sizeof(HZ_ValveTwoWidget ), 0},
			{ Pt_SET_DFLTS_F, (long)HZ_ValveTwo_dflts, 0 },
			{ Pt_SET_DRAW_F, (long)HZ_ValveTwo_draw, 0 },
			{ Pt_SET_NUM_RESOURCES,
			sizeof( resources ) / sizeof( resources[0] ), 0 },
			{ Pt_SET_RESOURCES, (long)resources,
			sizeof( resources ) / sizeof( resources[0] )},
			};
	return (HZ_ValveTwo->wclass = PtCreateWidgetClass(
			PtLabel, 0, sizeof(args)/sizeof(args[0]), args));
}
