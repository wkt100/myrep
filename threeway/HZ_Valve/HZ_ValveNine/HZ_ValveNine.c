/*
 * HZ_ValveNine.c
 *
 *  Created on: 2021-8-20
 *      Author: shixin
 */

#include "HZ_ValveNine.h"

PtWidgetClass_t *CreateHZ_ValveNineClass(void);
PtWidgetClassRef_t WHZ_ValveNine = { NULL,
		CreateHZ_ValveNineClass, 0};
PtWidgetClassRef_t *HZ_ValveNine = &WHZ_ValveNine;


static void HZ_ValveNine_dflts(PtWidget_t *widget )
{
	HZ_ValveNineWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;


	if ( NULL == widget )
	{
		return;
	}

	otherone = (HZ_ValveNineWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;
	otherone->hz_height = 50;
	otherone->hz_width = 50;
	otherone->hz_rotates = HZ_ValveNine_0;
	otherone->hz_color = PgRGB(128, 138, 135);
	otherone->hz_frame_color = PgRGB(0, 0, 0);
	otherone->hz_q = HZ_ValveNine_NoneS;
//	label->basic.fill_color = PgRGB(222, 222, 222);
}

static void HZ_ValveNine_draw(PtWidget_t *widget, PhTile_t *damage )
{
	HZ_ValveNineWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;
	PhRect_t   rect, center,canvas;

	if ( ( NULL == widget ) || ( NULL == damage ))
	{
		return;
	}

	otherone = (HZ_ValveNineWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;

	label->basic.color = Pg_TRANSPARENT;
	PtSuperClassDraw(PtLabel, widget, damage );
	PtCalcCanvas(widget, &rect);
	PtClipAdd(widget, &rect);
	short width = rect.lr.x - rect.ul.x;
	short height = rect.lr.y - rect.ul.y;
	
	short dynSymbolWidth = 54;
	short dynSymbolHeight = 58;
	short handAutoHeight=0;
	short CircleDiameter = 42;

	PgSetFillColor(otherone->hz_color);
	PgSetStrokeColor( Pg_BLACK );//线颜色
	PgSetStrokeWidth(1);//线粗细
	PhPoint_t o = {0, 0};
	PhPoint_t  up_triangle1_point1, up_triangle1_point2, up_triangle1_point3;
	PhPoint_t cir, rir,c,r;
   	char *ss = "S";
	c.x = rect.ul.x + width / 2;
			c.y = rect.ul.y + height / 3;
			r.x = width / 4;
			r.y = height / 8;
	switch(otherone->hz_rotates)
	{
		
		case HZ_ValveNine_90:
		{
			cir.x = rect.ul.x + width/2 + width*15/60;
			cir.y = rect.ul.y + height/2;
			rir.x =  (width/2 + width*13/60)-(width/2);
			rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);
			PgDrawEllipse(&cir, &rir, Pg_DRAW_FILL_STROKE);
					
			up_triangle1_point1.x = c.x - width*-2/40;
			up_triangle1_point1.y = c.y - height*0/30;
			up_triangle1_point2.x = c.x + width*-7/40;
			up_triangle1_point2.y = c.y + height*5/30;
			up_triangle1_point3.x = c.x - width*-1.5/30;
			up_triangle1_point3.y = c.y +height*10/30;
			PhPoint_t  p[] = { up_triangle1_point1, up_triangle1_point2, up_triangle1_point3 };//多边形点的坐标
			PgDrawPolygon( &p, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED );
			PhRect_t border_rect_two1 = {{cir.x - rir.x,cir.y- rir.y},
		    				{cir.x + rir.x,cir.y+ rir.y}};
			
			PgSetFont( label->font);
			PgSetTextColor(PgRGB( 0, 0, 0 ));
			PgDrawTextArea( ss, strlen( ss ), &border_rect_two1, Pg_TEXT_CENTER | Pg_TEXT_MIDDLE);
			PhPoint_t p1[] = {rect.lr.x - width / 3 * 2, rect.ul.y + height / 2, width / 3, -height / 2, -width / 3 * 2, 0};
			PgDrawPolygon(&p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			PhPoint_t p2[] = {rect.lr.x - width / 3 * 2, rect.ul.y + height / 2, width / 3, height / 2, -width / 3 * 2, 0};
			PgDrawPolygon(&p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			break;
		}
		case HZ_ValveNine_180:
		{
			cir.x = rect.ul.x + width/2 - width*1/60;
			cir.y = rect.ul.y + height/1.5;
			rir.x =  (width/2 + width*10/60)-(width/2);
			rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);
			PgDrawEllipse(&cir, &rir, Pg_DRAW_FILL_STROKE);
					
			up_triangle1_point1.x = c.x - width*8/50;
			up_triangle1_point1.y = c.y - height*-5/30;
			up_triangle1_point2.x = c.x + width*8/50;
			up_triangle1_point2.y = c.y - height*-5/30;
			up_triangle1_point3.x = c.x + width*0/40;
			up_triangle1_point3.y = c.y - height*0/30;
			PhPoint_t  p[] = { up_triangle1_point1, up_triangle1_point2, up_triangle1_point3 };//多边形点的坐标
			PgDrawPolygon( &p, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED );
			PhRect_t border_rect_two1 = {{cir.x - rir.x,cir.y- rir.y},
		    				{cir.x + rir.x,cir.y+ rir.y}};
			
			PgSetFont( label->font);
			PgSetTextColor(PgRGB( 0, 0, 0 ));
			PgDrawTextArea( ss, strlen( ss ), &border_rect_two1, Pg_TEXT_CENTER | Pg_TEXT_MIDDLE);
			PhPoint_t p1[] = {rect.ul.x + width / 2, rect.lr.y - height / 3 * 2, -width / 2, -height / 3, 0, height / 3 * 2};
			PgDrawPolygon(&p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			PhPoint_t p2[] = {rect.ul.x + width / 2, rect.lr.y - height / 3 * 2, width / 2, -height / 3, 0, height / 3 * 2};
			PgDrawPolygon(&p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			break;
		}
		case HZ_ValveNine_270:
		{
			cir.x = rect.ul.x + width/4;
			cir.y = rect.ul.y + height/2 + height*2/60;
			rir.x =  (width/2 + width*10/60)-(width/2);
			rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);
			PgDrawEllipse(&cir, &rir, Pg_DRAW_FILL_STROKE);
			
			up_triangle1_point1.x = c.x - width*4/40;
			up_triangle1_point1.y = c.y - height*0/30;
			up_triangle1_point2.x = c.x + width*6/40;
			up_triangle1_point2.y = c.y + height*5/30;
			up_triangle1_point3.x = c.x - width*3/30;
			up_triangle1_point3.y = c.y +height*10/30;
			PhPoint_t  p[] = { up_triangle1_point1, up_triangle1_point2, up_triangle1_point3 };//多边形点的坐标
			PgDrawPolygon( &p, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED );
			PhRect_t border_rect_two1 = {{cir.x - rir.x,cir.y- rir.y},
		    				{cir.x + rir.x,cir.y+ rir.y}};
			
			PgSetFont( label->font);
			PgSetTextColor(PgRGB( 0, 0, 0 ));
			PgDrawTextArea( ss, strlen( ss ), &border_rect_two1, Pg_TEXT_CENTER | Pg_TEXT_MIDDLE);
			PhPoint_t p1[] = {rect.ul.x + width / 3 * 2, rect.ul.y + height / 2, width / 3, -height / 2, -width / 3 * 2, 0};
			PgDrawPolygon(&p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			PhPoint_t p2[] = {rect.ul.x + width / 3 * 2, rect.ul.y + height / 2, width / 3, height / 2, -width / 3 * 2, 0};
			PgDrawPolygon(&p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			break;
		}
		default:
		{
			cir.x = rect.ul.x + width/2;
			cir.y = rect.ul.y + height/2 - height*13/60;
			rir.x =  (width/2 + width*10/60)-(width/2);
			rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);
			PgDrawEllipse(&cir, &rir, Pg_DRAW_FILL_STROKE);
			
			up_triangle1_point1.x = c.x - width*8/50;
			up_triangle1_point1.y = c.y - height*-4/30;
			up_triangle1_point2.x = c.x + width*8/50;
			up_triangle1_point2.y = c.y - height*-4/30;
			up_triangle1_point3.x = c.x;
			up_triangle1_point3.y = c.y +height*10/30;
			PhPoint_t  p[] = { up_triangle1_point1, up_triangle1_point2, up_triangle1_point3 };//多边形点的坐标
			PgDrawPolygon( &p, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED );
			
			PhRect_t border_rect_two1 = {{cir.x - rir.x,cir.y- rir.y},
		    						{cir.x + rir.x,cir.y+ rir.y}};
			
			PgSetFont( label->font);
			PgSetTextColor(PgRGB( 0, 0, 0 ));
			PgDrawTextArea( ss, strlen( ss ), &border_rect_two1, Pg_TEXT_CENTER | Pg_TEXT_MIDDLE);
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

PtWidgetClass_t *CreateHZ_ValveNineClass(void)
{
	static PtResourceRec_t resources[] = {
			{HZ_ROTATE177, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveNineWidget, hz_rotates ), 0},
			{HZ_COLOR177, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveNineWidget, hz_color ), 0},
			{HZ_FRAME_COLOR177, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveNineWidget, hz_frame_color ), 0},
			{HZ_Q177, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveNineWidget, hz_q ), 0},
			};


	static PtArg_t args[] = {
			{ Pt_SET_VERSION, 110, 0},
			{ Pt_SET_STATE_LEN, sizeof(HZ_ValveNineWidget ), 0},
			{ Pt_SET_DFLTS_F, (long)HZ_ValveNine_dflts, 0 },
			{ Pt_SET_DRAW_F, (long)HZ_ValveNine_draw, 0 },
			{ Pt_SET_NUM_RESOURCES,
			sizeof( resources ) / sizeof( resources[0] ), 0 },
			{ Pt_SET_RESOURCES, (long)resources,
			sizeof( resources ) / sizeof( resources[0] )},
			};
	return (HZ_ValveNine->wclass = PtCreateWidgetClass(
			PtLabel, 0, sizeof(args)/sizeof(args[0]), args));
}
