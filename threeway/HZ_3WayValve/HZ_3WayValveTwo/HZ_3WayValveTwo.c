/*
 * HZ_3WayValveTwo.c
 *
 *  Created on: 2021-8-18
 *      Author: shixin
 */

#include "HZ_3WayValveTwo.h"

PtWidgetClass_t *CreateHZ_3WayValveTwoClass(wvtid);
PtWidgetClassRef_t WHZ_3WayValveTwo = { NULL,
		CreateHZ_3WayValveTwoClass, 0};
PtWidgetClassRef_t *HZ_3WayValveTwo = &WHZ_3WayValveTwo;


static void HZ_3WayValveTwo_dflts(PtWidget_t *widget )
{
	HZ_3WayValveTwoWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;

	/* 防止发布版本代码无改动 */
	if ( NULL == widget )
	{
		return;                     /* 20181022 防止指针为空 */
	}

	otherone = (HZ_3WayValveTwoWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;
	otherone->hz_height = 50;
	otherone->hz_width = 50;
	otherone->hz_rotates = HZ_3WayValveTwo_0;
	otherone->hz_color = PgRGB(192, 192, 192);
	otherone->hz_frame_color = PgRGB(0, 0, 0);
	otherone->hz_q = HZ_3WayValveTwo_NoneS;
//	label->basic.fill_color = PgRGB(222, 222, 222);
}

static void HZ_3WayValveTwo_draw(PtWidget_t *widget, PhTile_t *damage )
{
	HZ_3WayValveTwoWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;
	PhRect_t   rect, center,up_rect;

	if ( ( NULL == widget ) || ( NULL == damage ))
	{
		return;                      /* 20181022 防止指针为空*/
	}

	otherone = (HZ_3WayValveTwoWidget *) widget;
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
	PhPoint_t  c;
	PhPoint_t o = {0,0};
   	PhPoint_t  line_point1, line_point2,up_point1,up_point2;
   	PhPoint_t  left_point1, left_point2;
   	PhPoint_t  right_point1, right_point2;
	PhPoint_t  middle_point1, middle_point2;
   	PhPoint_t cir, rir;
	char *ss="S";
	switch(otherone->hz_rotates)
	{
		
		case HZ_3WayValveTwo_90:
		{
			
			c.x = rect.ul.x + width*(9+21)/74;
			c.y = rect.ul.y + height/2;
			
			up_rect.ul.x = c.x + width*15/74;
			up_rect.ul.y = c.y - height*10/60;
			up_rect.lr.x = c.x + width*35/74;
			up_rect.lr.y = c.y + height*10/60;
			
			line_point1.x = c.x + width*15/74;
			line_point1.y = c.y;
			line_point2.x = c.x;
			line_point2.y = c.y;
			
			left_point1.x = c.x + width*10/74;
			left_point1.y = c.y - height*21/60;
			left_point2.x = c.x - width*10/74;
			left_point2.y = c.y - height*21/60;
			
			middle_point1.x = c.x - width*21/74;
			middle_point1.y = c.y - height*10/60;
			middle_point2.x = c.x - width*21/74;
			middle_point2.y = c.y + height*10/60;
	
			right_point1.x = c.x + width*10/74;
			right_point1.y = c.y + height*21/60;
			right_point2.x = c.x - width*10/74;
			right_point2.y = c.y + height*21/60;
			
			PgDrawRect(&up_rect, Pg_DRAW_STROKE | Pg_DRAW_FILL );
			PhRect_t border_rect_two1 = {{cir.x - rir.x,cir.y- rir.y},
		    						{cir.x + rir.x,cir.y+ rir.y}};
			PgSetFont( label->font);
			PgSetTextColor(PgRGB( 0, 0, 0 ));
			PgDrawTextArea( ss, strlen( ss ), &up_rect, Pg_TEXT_CENTER | Pg_TEXT_MIDDLE);
			break;
		}
		case HZ_3WayValveTwo_180:
		{
			c.x = rect.ul.x + width/2;
			c.y = rect.ul.y + height*(9+21)/74;
	
			up_rect.ul.x = c.x - width*10/60;
			up_rect.ul.y = c.y + height*15/74;
			up_rect.lr.x = c.x + width*10/60;
			up_rect.lr.y = c.y + height*35/74;
			
			line_point1.x = c.x;
			line_point1.y = c.y + height*15/74;
			line_point2.x = c.x;
			line_point2.y = c.y;
			
			left_point1.x = c.x + width*21/60;
			left_point1.y = c.y + height*10/74;
			left_point2.x = c.x + width*21/60;
			left_point2.y = c.y - height*10/74;
			
			middle_point1.x = c.x + width*10/60;
			middle_point1.y = c.y - height*21/74;
			middle_point2.x = c.x - width*10/60;
			middle_point2.y = c.y - height*21/74;
			
			right_point1.x = c.x - width*21/60;
			right_point1.y = c.y + height*10/74;
			right_point2.x = c.x - width*21/60;
			right_point2.y = c.y - height*10/74;
			
			PgDrawRect(&up_rect, Pg_DRAW_STROKE | Pg_DRAW_FILL );
			PhRect_t border_rect_two1 = {{cir.x - rir.x,cir.y- rir.y},
		    						{cir.x + rir.x,cir.y+ rir.y}};
			PgSetFont( label->font);
			PgSetTextColor(PgRGB( 0, 0, 0 ));
			PgDrawTextArea( ss, strlen( ss ), &up_rect, Pg_TEXT_CENTER | Pg_TEXT_MIDDLE);
			break;
		}
		case HZ_3WayValveTwo_270:
		{
			c.x = rect.ul.x + width*(9+35)/74;
			c.y = rect.ul.y + height/2;
	
			up_rect.ul.x = c.x - width*35/74;
			up_rect.ul.y = c.y - height*10/60;
			up_rect.lr.x = c.x - width*15/74;
			up_rect.lr.y = c.y + height*10/60;
			
			line_point1.x = c.x - width*15/74;
			line_point1.y = c.y;
			line_point2.x = c.x;
			line_point2.y = c.y;
			
			left_point1.x = c.x - width*10/74;
			left_point1.y = c.y + height*21/60;
			left_point2.x = c.x + width*10/74;
			left_point2.y = c.y + height*21/60;
			
			middle_point1.x = c.x + width*21/74;
			middle_point1.y = c.y + height*10/60;
			middle_point2.x = c.x + width*21/74;
			middle_point2.y = c.y - height*10/60;
			
			right_point1.x = c.x - width*10/74;
			right_point1.y = c.y - height*21/60;
			right_point2.x = c.x + width*10/74;
			right_point2.y = c.y - height*21/60;
			
			PgDrawRect(&up_rect, Pg_DRAW_STROKE | Pg_DRAW_FILL );
			PhRect_t border_rect_two1 = {{cir.x - rir.x,cir.y- rir.y},
		    						{cir.x + rir.x,cir.y+ rir.y}};
			PgSetFont( label->font);
			PgSetTextColor(PgRGB( 0, 0, 0 ));
			PgDrawTextArea( ss, strlen( ss ), &up_rect, Pg_TEXT_CENTER | Pg_TEXT_MIDDLE);
			break;
		}
		default:
		{
			c.x = rect.ul.x + width/2;
			c.y = rect.ul.y + height*(9+35)/74;
	
			up_rect.ul.x = c.x - width*10/60;
			up_rect.ul.y = c.y - height*35/74;
			up_rect.lr.x = c.x + width*10/60;
			up_rect.lr.y = c.y - height*15/74;
			
			line_point1.x = c.x;
			line_point1.y = c.y - height*15/74;
			line_point2.x = c.x;
			line_point2.y = c.y;
			
			left_point1.x = c.x - width*21/60;
			left_point1.y = c.y - height*10/74;
			left_point2.x = c.x - width*21/60;
			left_point2.y = c.y + height*10/74;
			
			middle_point1.x = c.x - width*10/60;
			middle_point1.y = c.y + height*21/74;
			middle_point2.x = c.x + width*10/60;
			middle_point2.y = c.y + height*21/74;
			
			right_point1.x = c.x + width*21/60;
			right_point1.y = c.y - height*10/74;
			right_point2.x = c.x + width*21/60;
			right_point2.y = c.y + height*10/74;
			
  			cir.x = rect.ul.x + width/2;
  			cir.y = rect.ul.y + height/2 - height*13/60;//13
  			rir.x =  (width/2 + width*10/60)-(width/2);
  			rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);
			
			PhRect_t border_rect_two1 = {{cir.x - rir.x,cir.y- rir.y},
		    						{cir.x + rir.x,cir.y+ rir.y}};
			
			PgDrawRect(&up_rect, Pg_DRAW_STROKE | Pg_DRAW_FILL );
			
			PgSetFont( label->font);
			PgSetTextColor(PgRGB( 0, 0, 0 ));
			PgDrawTextArea( ss, strlen( ss ), &up_rect, Pg_TEXT_CENTER | Pg_TEXT_MIDDLE);
			
			break;
		}
	}
	
	PgDrawLine( &line_point1, &line_point2 );
	PhPoint_t  p[] = { left_point1, left_point2, line_point2 };
	PhPoint_t  q[] = { middle_point1, middle_point2, line_point2 };
	PhPoint_t  s[] = { right_point1, right_point2, line_point2 };
	PgDrawPolygon( p, 3, &o, Pg_DRAW_STROKE_FILL | Pg_CLOSED );
	PgDrawPolygon( q, 3, &o, Pg_DRAW_STROKE_FILL | Pg_CLOSED );
	PgDrawPolygon( s, 3, &o, Pg_DRAW_STROKE_FILL | Pg_CLOSED );
	
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

PtWidgetClass_t *CreateHZ_3WayValveTwoClass(void)
{
	static PtResourceRec_t resources[] = {
			{HZ_HEIGHT172, Pt_CHANGE_RESIZE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveTwoWidget, hz_height ), 0},
			{HZ_WIDTH172, Pt_CHANGE_RESIZE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveTwoWidget, hz_width ), 0},
			{HZ_ROTATE172, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveTwoWidget, hz_rotates ), 0},
			{HZ_COLOR172, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveTwoWidget, hz_color ), 0},
			{HZ_FRAME_COLOR172, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveTwoWidget, hz_frame_color ), 0},
			{HZ_Q172, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveTwoWidget, hz_q ), 0},
			};


	static PtArg_t args[] = {
			{ Pt_SET_VERSION, 110, 0},
			{ Pt_SET_STATE_LEN, sizeof(HZ_3WayValveTwoWidget ), 0},
			{ Pt_SET_DFLTS_F, (long)HZ_3WayValveTwo_dflts, 0 },
			{ Pt_SET_DRAW_F, (long)HZ_3WayValveTwo_draw, 0 },
			{ Pt_SET_NUM_RESOURCES,
			sizeof( resources ) / sizeof( resources[0] ), 0 },
			{ Pt_SET_RESOURCES, (long)resources,
			sizeof( resources ) / sizeof( resources[0] )},
			};
	return (HZ_3WayValveTwo->wclass = PtCreateWidgetClass(
			PtLabel, 0, sizeof(args)/sizeof(args[0]), args));
}
