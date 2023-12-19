/*
 * HZ_ValveFour.c
 *
 *  Created on: 2021-8-2
 *      Author: shixin
 */

#include "HZ_ValveFour.h"

PtWidgetClass_t *CreateHZ_ValveFourClass(void);
PtWidgetClassRef_t WHZ_ValveFour = { NULL,
                                    CreateHZ_ValveFourClass, 0};
PtWidgetClassRef_t *HZ_ValveFour = &WHZ_ValveFour;


static void HZ_ValveFour_dflts(PtWidget_t *widget )
{
	HZ_ValveFourWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;


	if ( NULL == widget )
	{
		return;
	}

	otherone = (HZ_ValveFourWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;
	otherone->hz_height = 50;
	otherone->hz_width = 50;
	otherone->hz_rotates = HZ_ValveFour_0;
	otherone->hz_color = PgRGB(128, 138, 135);
	otherone->hz_frame_color = PgRGB(0, 0, 0);
	otherone->hz_q = HZ_ValveFour_NoneS;
//	label->basic.fill_color = PgRGB(222, 222, 222);
}

static void HZ_ValveFour_draw(PtWidget_t *widget, PhTile_t *damage )
{
	HZ_ValveFourWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;
	PhRect_t   rect, center;

	if ( ( NULL == widget ) || ( NULL == damage ))
	{
		return;
	}

	otherone = (HZ_ValveFourWidget *) widget;
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
	PhPoint_t  line_point1, line_point2;
   	PhPoint_t  up_line_point1, up_line_point2;
	PhPoint_t c,r,cir,rir;
	int radiiX = 0;
	int radiiY = 0;
	
	PhPoint_t p_0[] = {rect.ul.x + width / 8 * 3, rect.lr.y - height / 1.7, rect.ul.x + width / 8 * 3, rect.ul.y + height / 4, rect.ul.x + width / 2, rect.lr.y - height / 1.5, rect.lr.x - width / 8 * 3, rect.ul.y + height / 4, rect.lr.x - width / 8 * 3, rect.lr.y - height / 1.7};
	PhPoint_t p_90[] = {rect.ul.x + width / 1.5, rect.ul.y + height / 8 * 3, rect.lr.x - width / 4.9, rect.ul.y + height / 8 * 3, rect.ul.x + width / 1.5, rect.ul.y + height / 2, rect.lr.x - width / 4.9, rect.lr.y - height / 8 * 3, rect.ul.x + width / 1.5, rect.lr.y - height / 8 * 3};
	PhPoint_t p_180[] = {rect.ul.x + width / 8 * 3, rect.ul.y + height / 1.7, rect.ul.x + width / 8 * 3, rect.lr.y - height / 4, rect.ul.x + width / 2, rect.ul.y + height / 1.5, rect.lr.x - width / 8 * 3.1, rect.lr.y - height / 4, rect.lr.x - width / 8 * 3.1, rect.ul.y + height / 1.7};
	PhPoint_t p_270[] = {rect.lr.x - width / 1.5, rect.ul.y + height / 8 * 3.5, rect.ul.x + width / 5.6, rect.ul.y + height / 8 * 3.5, rect.lr.x - width / 1.5, rect.ul.y + height / 1.9, rect.ul.x + width / 5.6, rect.lr.y - height / 8 * 3, rect.lr.x - width / 1.5, rect.lr.y - height / 8 * 3};
	switch(otherone->hz_rotates)
	{
		
		case HZ_ValveFour_90:
		{
			c.x = rect.ul.x + width*(9 + 46)/74;
   			c.y = rect.ul.y + height/2;
   			radiiX = width*10/74;
   			radiiY = height*10/60;
   			if(radiiX < 0)
   			{
   				radiiX = 0;
   			}
   			if(radiiY < 0)
   			{
   				radiiY = 0;
   			}
			cir.x = rect.ul.x + width/2 + width*13/60;
			cir.y = rect.ul.y + height/2;
			rir.x =  (width/2 + width*10/60)-(width/2);
			rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);
			PgDrawEllipse(&cir, &rir, Pg_DRAW_FILL_STROKE);
					
			up_line_point1.x = c.x + width*-14/74;
			up_line_point1.y = c.y + height*8/50;
			up_line_point2.x = c.x + width*-14/74;
			up_line_point2.y = c.y - height*8/50;

   			line_point1.x = c.x - radiiX-6.5;
   			line_point1.y = c.y;
   			line_point2.x = c.x - width*29/74;
   			line_point2.y = c.y;
			
			//PgDrawILine(rect.lr.x - width / 3, rect.ul.y + height / 2, rect.lr.x - width / 3 * 2, rect.ul.y + height / 2);
			PhPoint_t p1[] = {rect.lr.x - width / 3 * 2, rect.ul.y + height / 2, width / 3, -height / 2, -width / 3 * 2, 0};
			PgDrawPolygon(&p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			PhPoint_t p2[] = {rect.lr.x - width / 3 * 2, rect.ul.y + height / 2, width / 3, height / 2, -width / 3 * 2, 0};
			PgDrawPolygon(&p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			
			PgDrawPolygon(&p_90, 5, &o, Pg_DRAW_STROKE);
			
			PgDrawLine(&up_line_point1, &up_line_point2);
			PgDrawLine( &line_point1, &line_point2 );
			break;
		}
		case HZ_ValveFour_180:
		{
			c.x = rect.ul.x + width/2;
   			c.y = rect.ul.y + height*(9 + 46)/74;
   			radiiX = width*10/60;
   			radiiY = height*10/74;
   			if(radiiX < 0)
   			{
   				radiiX = 0;
   			}
   			if(radiiY < 0)
   			{
   				radiiY = 0;
   			}
			cir.x = rect.ul.x + width/2 - width*1/60;
			cir.y = rect.ul.y + height/1.5;
			rir.x =  (width/2 + width*10/60)-(width/2.2);
			rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);
			PgDrawEllipse(&cir, &rir, Pg_DRAW_FILL_STROKE);
			PgDrawPolygon(&p_180, 5, &o, Pg_DRAW_STROKE);
					
			up_line_point1.x = c.x + width*11/74;
			up_line_point1.y = c.y + height*-12/50;
			up_line_point2.x = c.x - width*11/74;
			up_line_point2.y = c.y + height*-12/50;

   			line_point1.x = c.x;
   			line_point1.y = c.y - radiiY-11;
   			line_point2.x = c.x;
   			line_point2.y = c.y - height*29/74;
			
			//PgDrawILine(rect.ul.x + width / 2, rect.lr.y - height / 3, rect.ul.x + width / 2, rect.lr.y - height / 3 * 2);
			PhPoint_t p1[] = {rect.ul.x + width / 2, rect.lr.y - height / 3 * 2, -width / 2, -height / 3, 0, height / 3 * 2};
			PgDrawPolygon(&p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			PhPoint_t p2[] = {rect.ul.x + width / 2, rect.lr.y - height / 3 * 2, width / 2, -height / 3, 0, height / 3 * 2};
			PgDrawPolygon(&p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			
			PgDrawLine(&up_line_point1, &up_line_point2);
			PgDrawLine( &line_point1, &line_point2 );
					break;
		}
		case HZ_ValveFour_270:
		{
			c.x = rect.ul.x + width*(9 + 10)/74;
   			c.y = rect.ul.y + height/2;
   			radiiX = width*10/74;
   			radiiY = height*10/60;
   			if(radiiX < 0)
   			{
   				radiiX = 0;
   			}
   			if(radiiY < 0)
   			{
   				radiiY = 0;
   			}
			cir.x = rect.ul.x + width/4.2;
			cir.y = rect.ul.y + height/2 + height*2/60;
			rir.x =  (width/2 + width*10/60)-(width/2);
			rir.y =  (height/2 - height*13/60)-(height/2 - height*3/60);
			PgDrawEllipse(&cir, &rir, Pg_DRAW_FILL_STROKE);
			PgDrawPolygon(&p_270, 5, &o, Pg_DRAW_STROKE);
			
			up_line_point1.x = c.x + width*12/74;
			up_line_point1.y = c.y + height*8/50;
			up_line_point2.x = c.x + width*12/74;
			up_line_point2.y = c.y - height*8/50;

   			line_point1.x = c.x + radiiX;
   			line_point1.y = c.y;
   			line_point2.x = c.x + width*29/74;
   			line_point2.y = c.y;
			
			//PgDrawILine(rect.ul.x + width / 3, rect.ul.y + height / 2, rect.ul.x + width / 3 * 2, rect.ul.y + height / 2);
			PhPoint_t p1[] = {rect.ul.x + width / 3 * 2, rect.ul.y + height / 2, width / 3, -height / 2, -width / 3 * 2, 0};
			PgDrawPolygon(&p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			PhPoint_t p2[] = {rect.ul.x + width / 3 * 2, rect.ul.y + height / 2, width / 3, height / 2, -width / 3 * 2, 0};
			PgDrawPolygon(&p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			
			PgDrawLine(&up_line_point1, &up_line_point2);
			PgDrawLine( &line_point1, &line_point2 );
			break;
		}
		default:
		{
			c.x = rect.ul.x + width/2;
   			c.y = rect.ul.y + height*19/74;
   			radiiX = width*10/60;
   			radiiY = height*10/74;
   			if(radiiX < 0)
   			{
   				radiiX = 0;
   			}
   			if(radiiY < 0)
   			{
   				radiiY = 0;
   			}
			c.x = rect.ul.x + width / 2;
			c.y = rect.ul.y + height / 3;
			r.x = width / 4;
			r.y = height / 8;
			PgDrawEllipse(&c, &r, Pg_DRAW_FILL_STROKE);
			PgDrawPolygon(&p_0, 5, &o, Pg_DRAW_STROKE);
			
			up_line_point1.x = c.x + width*11/74;
			up_line_point1.y = c.y + height*7/50;
			up_line_point2.x = c.x - width*11/74;
			up_line_point2.y = c.y + height*7/50;

   			line_point1.x = c.x;
   			line_point1.y = c.y + radiiY;
   			line_point2.x = c.x;
   			line_point2.y = c.y + height*25/74;
			
			PhPoint_t p1[] = {rect.ul.x + width / 2, rect.ul.y + height / 3 * 2, -width / 2, -height / 3, 0, height / 3 * 2};
			PgDrawPolygon(&p1, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			PhPoint_t p2[] = {rect.ul.x + width / 2, rect.ul.y + height / 3 * 2, width / 2, -height / 3, 0, height / 3 * 2};
			PgDrawPolygon(&p2, 3, &o, Pg_DRAW_FILL | Pg_CLOSED | Pg_RELATIVE);
			
			PgDrawLine(&up_line_point1, &up_line_point2);
			PgDrawLine( &line_point1, &line_point2 );
			
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

PtWidgetClass_t *CreateHZ_ValveFourClass(void)
{
	static PtResourceRec_t resources[] = {
			{HZ_ROTATE182, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveFourWidget, hz_rotates ), 0},
			{HZ_COLOR182, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveFourWidget, hz_color ), 0},
			{HZ_FRAME_COLOR182, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveFourWidget, hz_frame_color ), 0},
			{HZ_Q182, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_ValveFourWidget, hz_q ), 0},
			};


	static PtArg_t args[] = {
			{ Pt_SET_VERSION, 110, 0},
			{ Pt_SET_STATE_LEN, sizeof(HZ_ValveFourWidget ), 0},
			{ Pt_SET_DFLTS_F, (long)HZ_ValveFour_dflts, 0 },
			{ Pt_SET_DRAW_F, (long)HZ_ValveFour_draw, 0 },
			{ Pt_SET_NUM_RESOURCES,
			sizeof( resources ) / sizeof( resources[0] ), 0 },
			{ Pt_SET_RESOURCES, (long)resources,
			sizeof( resources ) / sizeof( resources[0] )},
			};
	return (HZ_ValveFour->wclass = PtCreateWidgetClass(
			PtLabel, 0, sizeof(args)/sizeof(args[0]), args));
}
