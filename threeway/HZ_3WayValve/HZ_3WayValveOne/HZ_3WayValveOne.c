/*
 * HZ_3WayValveOne.c
 *
 *  Created on: 2021-7-13
 *      Author: shixin
 */

#include "HZ_3WayValveOne.h"
PtWidgetClass_t *CreateHZ_3WayValveOneClass(void);

PtWidgetClassRef_t WHZ_3WayValveOne = {NULL, CreateHZ_3WayValveOneClass, 0};
PtWidgetClassRef_t *HZ_3WayValveOne = &WHZ_3WayValveOne;


static void HZ_3WayValveOne_dflts(PtWidget_t *widget )
{
	HZ_3WayValveOneWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;

	/* 防止发布版本代码无改动 */
	if ( NULL == widget )
	{
		return;                     /* 20181022 防止指针为空 */
	}

	otherone = (HZ_3WayValveOneWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;
	otherone->hz_height = 1256;
	otherone->hz_width = 242;
	otherone->hz_rotates = HZ_3WayValveOne_0;
	otherone->hz_color = PgRGB(128, 138, 135);
	otherone->hz_frame_color = PgRGB(0, 0, 0);
	otherone->hz_q = HZ_3WayValveOne_NoneS;
//	label->basic.fill_color = PgRGB(222, 222, 222);	// 背景颜色，ken
}

static void HZ_3WayValveOne_draw(PtWidget_t *widget, PhTile_t *damage )
{
	HZ_3WayValveOneWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;
	PhRect_t   rect, center;

	if ( ( NULL == widget ) || ( NULL == damage ))
	{
		return;                      /* 20181022 防止指针为空*/
	}

	otherone = (HZ_3WayValveOneWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;
	//很重要的,避免看到label自身的文本
	label->basic.color = Pg_TRANSPARENT;
	PtSuperClassDraw(PtLabel, widget, damage );
	PtCalcCanvas(widget, &rect);
	PtClipAdd(widget, &rect);
	short width = rect.lr.x - rect.ul.x;
	short height = rect.lr.y - rect.ul.y;

	PgSetFillColor(otherone->hz_color);
	PgSetStrokeColor( Pg_BLACK );//线颜色
	PgSetStrokeWidth(1);//线粗细
	PhPoint_t o = {0, 0};
	switch(otherone->hz_rotates)
	{
		case HZ_3WayValveOne_90:
		{
				PgDrawILine(rect.lr.x - width / 4, rect.ul.y + height / 4, rect.lr.x - width / 4, rect.lr.y - height / 4);
				PgDrawILine(rect.lr.x - width / 4, rect.ul.y + height / 2, rect.lr.x - width / 2, rect.ul.y + height / 2);
				PhPoint_t p1[] = {	{rect.lr.x - width / 4, rect.ul.y},
									{-width / 2, 0},
									{width / 4, height / 2}};
				PhPoint_t p2[] = {	{rect.lr.x - width / 4, rect.lr.y},
									{-width / 2}, {0},
									{width / 4, -height / 2}};
				PhPoint_t p3[] = {	{rect.lr.x - width / 2, rect.ul.y + height / 2},
									{-width / 2}, {-height / 4},
									{0, height / 2}};
				PgDrawPolygon(p1, 3, &o, Pg_DRAW_FILL | Pg_RELATIVE);
				PgDrawPolygon(p2, 3, &o, Pg_DRAW_FILL | Pg_RELATIVE);
				PgDrawPolygon(p3, 3, &o, Pg_DRAW_FILL | Pg_RELATIVE);
				break;
		}
		case HZ_3WayValveOne_180:
		{
				PgDrawILine(rect.ul.x + width / 4, rect.lr.y - height / 4, rect.lr.x - width / 4, rect.lr.y - height / 4);
				PgDrawILine(rect.ul.x + width / 2, rect.lr.y - height / 4, rect.ul.x + width / 2, rect.lr.y - height / 2);
				PhPoint_t p1[] = {	{rect.ul.x, rect.lr.y - height / 4},
									{0, -height / 2},
									{width / 2, height / 4}	};
				PhPoint_t p2[] = {	{rect.lr.x, rect.lr.y - height / 4},
									{0, -height / 2},
									{-width / 2, height / 4}	};
				PhPoint_t p3[] = {	{rect.ul.x + width / 2, rect.lr.y - height / 2},
									{-width / 4, -height / 2},
									{width / 2, 0}	};
				PgDrawPolygon(p1, 3, &o, Pg_DRAW_FILL | Pg_RELATIVE);
				PgDrawPolygon(p2, 3, &o, Pg_DRAW_FILL | Pg_RELATIVE);
				PgDrawPolygon(p3, 3, &o, Pg_DRAW_FILL | Pg_RELATIVE);
				break;
		}
		case HZ_3WayValveOne_270:
		{
				PgDrawILine(rect.ul.x + width / 4, rect.ul.y + height / 4, rect.ul.x + width / 4, rect.lr.y - height / 4);
				PgDrawILine(rect.ul.x + width / 4, rect.ul.y + height / 2, rect.ul.x + width / 2, rect.ul.y + height / 2);
				PhPoint_t p1[] = {	{rect.ul.x + width / 4, rect.ul.y},
									{width / 2, 0},
									{-width / 4, height / 2}	};
				PhPoint_t p2[] = {	{rect.ul.x + width / 4, rect.lr.y},
									{width / 2, 0},
									{-width / 4, -height / 2}	};
				PhPoint_t p3[] = {	{rect.ul.x + width / 2, rect.ul.y + height / 2},
									{width / 2, -height / 4},
									{0, height / 2}	};
				PgDrawPolygon(p1, 3, &o, Pg_DRAW_FILL | Pg_RELATIVE);
				PgDrawPolygon(p2, 3, &o, Pg_DRAW_FILL | Pg_RELATIVE);
				PgDrawPolygon(p3, 3, &o, Pg_DRAW_FILL | Pg_RELATIVE);
				break;
		}
		default:
		{
			PgDrawILine(rect.ul.x + width / 4, rect.ul.y + height / 4, rect.lr.x - width / 4, rect.ul.y + height / 4);
			PgDrawILine(rect.ul.x + width / 2, rect.ul.y + height / 4, rect.ul.x + width / 2, rect.ul.y + height / 2);
			PhPoint_t p1[] = {	{rect.ul.x, rect.ul.y + height / 4},
								{0, height / 2},
								{width / 2, - height / 4}	};
			PhPoint_t p2[] = {	{rect.lr.x, rect.ul.y + height / 4},
								{0, height / 2},
								{-width / 2, - height / 4}	};
			PhPoint_t p3[] = {	{rect.ul.x + width / 2, rect.ul.y + height / 2},
								{-width / 4, height / 2},
								{width / 2, 0}	};

			PgDrawPolygon(p1, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED | Pg_RELATIVE);
			PgDrawPolygon(p2, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED | Pg_RELATIVE);
			PgDrawPolygon(p3, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED | Pg_RELATIVE);

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

PtWidgetClass_t *CreateHZ_3WayValveOneClass(void)
{
	static PtResourceRec_t resources[] = {
			{HZ_HEIGHT155, Pt_CHANGE_RESIZE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveOneWidget, hz_height ), 0},
			{HZ_WIDTH155, Pt_CHANGE_RESIZE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveOneWidget, hz_width ), 0},
			{HZ_ROTATE155, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveOneWidget, hz_rotates ), 0},
			{HZ_COLOR155, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveOneWidget, hz_color ), 0},
			{HZ_FRAME_COLOR155, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveOneWidget, hz_frame_color ), 0},
			{HZ_Q155, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveOneWidget, hz_q ), 0},
			};


	static PtArg_t args[] = {
			{ Pt_SET_VERSION, 110, 0},
			{ Pt_SET_STATE_LEN, sizeof(HZ_3WayValveOneWidget ), 0},
			{ Pt_SET_DFLTS_F, (long)HZ_3WayValveOne_dflts, 0 },
			{ Pt_SET_DRAW_F, (long)HZ_3WayValveOne_draw, 0 },
			{ Pt_SET_NUM_RESOURCES,
			sizeof( resources ) / sizeof( resources[0] ), 0 },
			{ Pt_SET_RESOURCES, (long)resources,
			sizeof( resources ) / sizeof( resources[0] )},
			};
	return (HZ_3WayValveOne->wclass = PtCreateWidgetClass(
			PtLabel, 0, sizeof(args)/sizeof(args[0]), args));
}
