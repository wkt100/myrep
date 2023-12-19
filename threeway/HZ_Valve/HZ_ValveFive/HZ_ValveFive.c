/*
 * HZ_MotorDriveValve1.c
 *
 *  Created on: 2021-7-12
 *      Author: shixin
 */

#include "HZ_ValveFive.h"

PtWidgetClass_t *CreateHZ_ValveFiveClass(void);

PtWidgetClassRef_t WHZ_ValveFive = {NULL, CreateHZ_ValveFiveClass, 0};
PtWidgetClassRef_t *HZ_ValveFive = &WHZ_ValveFive;


static void HZ_ValveFive_dflts(PtWidget_t *widget )
{
	HZ_ValveFiveWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;

	/* 防止发布版本代码无改动 */
	if ( NULL == widget )
	{
		return;                     /* 20181022 防止指针为空 */
	}

	otherone = (HZ_ValveFiveWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;
	otherone->hz_height = 50;
	otherone->hz_width = 50;
	otherone->hz_rotates = HZ_ValveFive_0;
	otherone->hz_color = PgRGB(128, 138, 135);
	otherone->hz_frame_color = PgRGB(0, 0, 0);
	otherone->hz_q = HZ_ValveFive_NoneS;
//	label->basic.fill_color = PgRGB(222, 222, 222);
}

static void HZ_ValveFive_draw(PtWidget_t *widget, PhTile_t *damage )
{
	HZ_ValveFiveWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;
	PhRect_t   rect, center,canvas;

	if ( ( NULL == widget ) || ( NULL == damage ))
	{
		return;                      /* 20181022 防止指针为空*/
	}

	otherone = (HZ_ValveFiveWidget *) widget;
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
	PgSetStrokeWidth(2);//线粗细
	PhRect_t rect_m;
	PhPoint_t o = {0, 0};
	PhPoint_t cir, rir;
	//char *ss = "S";
	switch(otherone->hz_rotates)
	{
		case HZ_ValveFive_90:
		{
				rect_m.ul.x = rect.lr.x;
				rect_m.ul.y = rect.ul.y + height / 3;
				rect_m.lr.x = rect.lr.x - width / 2;
				rect_m.lr.y = rect.lr.y - height / 3;
				cir.x = rect.ul.x + width/2 + width*13/60;
				cir.y = rect.ul.y + height/2;//13
				rir.x =  (width/2 + width*10/60)-(width/2);
				rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);
				PhPoint_t p1[] = {rect.ul.x, rect.ul.y, width / 2, 0, -width / 4, height / 2};
				PhPoint_t p2[] = {rect.ul.x, rect.lr.y, width / 2, 0, -width / 4, -height / 2};
				PgDrawRect(&rect_m, Pg_DRAW_STROKE_FILL);
				PgDrawPolygon(p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
				PgDrawPolygon(p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);

				PgDrawILine(rect.ul.x + width / 4, rect.ul.y + height / 2, rect_m.lr.x, rect.ul.y + height / 2);

				PgDrawILine((rect_m.ul.x + rect_m.lr.x) / 2, rect_m.ul.y, (rect_m.ul.x + rect_m.lr.x) / 2, rect_m.lr.y);
				PgDrawILine(rect_m.lr.x, (rect_m.ul.y + rect_m.lr.y) / 2, (rect_m.ul.x + rect_m.lr.x) / 2, (rect_m.ul.y + rect_m.lr.y) / 2);
				break;
			}
		case HZ_ValveFive_180:
		{
				rect_m.ul.x = rect.ul.x + width / 3;
				rect_m.ul.y = rect.lr.y - height / 2;
				rect_m.lr.x = rect.lr.x - width / 3;
				rect_m.lr.y = rect.lr.y;

				cir.x = rect.ul.x + width/2 - width*1/60;
								cir.y = rect.ul.y + height/1.5;
								rir.x =  (width/2 + width*10/60)-(width/2);
								rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);

				PhPoint_t p1[] = {rect.ul.x, rect.ul.y, 0, height / 2, width / 2, -height / 4};
				PhPoint_t p2[] = {rect.lr.x, rect.ul.y, 0, height / 2, -width / 2, -height / 4};
				PgDrawRect(&rect_m, Pg_DRAW_STROKE_FILL);
				PgDrawPolygon(p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
				PgDrawPolygon(p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);

				PgDrawILine(rect.ul.x + width / 2, rect_m.ul.y, rect.ul.x + width / 2, rect.ul.y + height / 4);
				PgDrawILine((rect_m.ul.x + rect_m.lr.x) / 2, (rect_m.ul.y + rect_m.lr.y) / 2, (rect_m.ul.x + rect_m.lr.x) / 2, rect_m.ul.y);
				PgDrawILine(rect_m.lr.x, (rect_m.ul.y + rect_m.lr.y) / 2, rect_m.ul.x, (rect_m.ul.y + rect_m.lr.y) / 2);
				break;
			}
		case HZ_ValveFive_270:
		{
				rect_m.ul.x = rect.ul.x;
				rect_m.ul.y = rect.lr.y - height / 3;
				rect_m.lr.x = rect.ul.x + width / 2;
				rect_m.lr.y = rect.ul.y + height / 3;

				cir.x = rect.ul.x + width/4.2;
								cir.y = rect.ul.y + height/2 + height*2/60;
								rir.x =  (width/2 + width*10/60)-(width/2);
								rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);

				PhPoint_t p1[] = {rect.lr.x, rect.ul.y, -width / 2, 0, width / 4, height / 2};
				PhPoint_t p2[] = {rect.lr.x, rect.lr.y, -width / 2, 0, width / 4, -height / 2};
				PgDrawRect(&rect_m, Pg_DRAW_STROKE_FILL);
				PgDrawPolygon(p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
				PgDrawPolygon(p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);

				PgDrawILine(rect.ul.x + width / 2, rect.ul.y + height / 2, rect.lr.x - width / 4, rect.ul.y + height / 2);
				PgDrawILine((rect_m.ul.x + rect_m.lr.x) / 2, rect_m.ul.y, (rect_m.ul.x + rect_m.lr.x) / 2, rect_m.lr.y);
				PgDrawILine(rect_m.lr.x, (rect_m.ul.y + rect_m.lr.y) / 2, (rect_m.ul.x + rect_m.lr.x) / 2, (rect_m.ul.y + rect_m.lr.y) / 2);
				break;
		}
		default:
		{
			rect_m.ul.x = rect.ul.x + width / 3;
			rect_m.ul.y = rect.ul.y;
			rect_m.lr.x = rect.lr.x - width / 3;
			rect_m.lr.y = rect.ul.y + height / 2;

			cir.x = rect.ul.x + width/2;
			cir.y = rect.ul.y + height/2 - height*13/60;//13
			rir.x =  (width/2 + width*10/60)-(width/2);
			rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);
			PhPoint_t p1[] = {rect.ul.x, rect.lr.y - height / 2, width / 2, height / 4, -width / 2, height / 4};
			PhPoint_t p2[] = {rect.lr.x, rect.lr.y - height / 2, -width / 2, height / 4, width / 2, height / 4};
			PgDrawRect(&rect_m, Pg_DRAW_STROKE_FILL);

			PgDrawPolygon(p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			PgDrawPolygon(p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);

			PgDrawILine(rect.ul.x + width / 2, rect_m.lr.y, rect.ul.x + width / 2, rect.lr.y - height / 4);
			PgDrawILine((rect_m.ul.x + rect_m.lr.x) / 2, (rect_m.ul.y + rect_m.lr.y) / 2, (rect_m.ul.x + rect_m.lr.x) / 2, rect_m.lr.y);
			PgDrawILine(rect_m.lr.x, (rect_m.ul.y + rect_m.lr.y) / 2, rect_m.ul.x, (rect_m.ul.y + rect_m.lr.y) / 2);
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

PtWidgetClass_t *CreateHZ_ValveFiveClass(void)
{
	static PtResourceRec_t resources[] = {
			{HZ_HEIGHT186, Pt_CHANGE_RESIZE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveFiveWidget, hz_height ), 0},
			{HZ_WIDTH186, Pt_CHANGE_RESIZE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveFiveWidget, hz_width ), 0},
			{HZ_ROTATE186, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveFiveWidget, hz_rotates ), 0},
			{HZ_COLOR186, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveFiveWidget, hz_color ), 0},
			{HZ_FRAME_COLOR186, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveFiveWidget, hz_frame_color ), 0},
			{HZ_Q186, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveFiveWidget, hz_q ), 0},
			};


	static PtArg_t args[] = {
			{ Pt_SET_VERSION, 110, 0},
			{ Pt_SET_STATE_LEN, sizeof(HZ_ValveFiveWidget ), 0},
			{ Pt_SET_DFLTS_F, (long)HZ_ValveFive_dflts, 0 },
			{ Pt_SET_DRAW_F, (long)HZ_ValveFive_draw, 0 },
			{ Pt_SET_NUM_RESOURCES,
			sizeof( resources ) / sizeof( resources[0] ), 0 },
			{ Pt_SET_RESOURCES, (long)resources,
			sizeof( resources ) / sizeof( resources[0] )},
			};
	return (HZ_ValveFive->wclass = PtCreateWidgetClass(
			PtLabel, 0, sizeof(args)/sizeof(args[0]), args));
}
