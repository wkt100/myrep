/*
 * HZ_3WayValveThree.c
 *
 *  Created on: 2021-7-30
 *      Author: shixin
 */

#include "HZ_3WayValveThree.h"

PtWidgetClass_t *CreateHZ_3WayValveThreeClass(void);
PtWidgetClassRef_t WHZ_3WayValveThree = { NULL,
                                    CreateHZ_3WayValveThreeClass, 0};
PtWidgetClassRef_t *HZ_3WayValveThree = &WHZ_3WayValveThree;

static void HZ_3WayValveThree_dflts(PtWidget_t *widget )
{
	HZ_3WayValveThreeWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;

	/* 防止发布版本代码无改动 */
	if ( NULL == widget )
	{
		return;                     /* 20181022 防止指针为空 */
	}

	otherone = (HZ_3WayValveThreeWidget *) widget;
	label = ( PtLabelWidget_t * ) widget;
	otherone->hz_height = 50;
	otherone->hz_width = 50;
	otherone->hz_rotates = HZ_3WayValveThree_0;
	otherone->hz_color = PgRGB(110, 110, 110);
	otherone->hz_frame_color = PgRGB(0, 0, 0);
	otherone->hz_q = HZ_3WayValveThree_NoneS;
//	label->basic.fill_color = PgRGB(222, 222, 222);
}

static void HZ_3WayValveThree_draw(PtWidget_t *widget, PhTile_t *damage )
{
	HZ_3WayValveThreeWidget *otherone = NULL;
	PtLabelWidget_t        *label = NULL;
	PhRect_t   rect, center;

	if ( ( NULL == widget ) || ( NULL == damage ))
	{
		return;                      /* 20181022 防止指针为空*/
	}

	otherone = (HZ_3WayValveThreeWidget *) widget;
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
	PhPoint_t  c,r;
	PhPoint_t o = {0,0};
	PhPoint_t  line_point1, line_point2;
   	PhPoint_t  left_point1, left_point2;
   	PhPoint_t  right_point1, right_point2;
	PhPoint_t  middle_point1, middle_point2;
	PhPoint_t up_triangle1_point1,up_triangle1_point2,up_triangle1_point3;
	int radiiX = 0;
	int radiiY = 0;
	
	
	PhPoint_t p_0[] = {rect.ul.x + width / 8 * 3, rect.lr.y - height / 1.7, rect.ul.x + width / 8 * 3, rect.ul.y + height / 4, rect.ul.x + width / 2, rect.lr.y - height / 1.5, rect.lr.x - width / 8 * 3, rect.ul.y + height / 4, rect.lr.x - width / 8 * 3, rect.lr.y - height / 1.7};
	PhPoint_t p_90[] = {rect.ul.x + width / 1.5, rect.ul.y + height / 8 * 3, rect.lr.x - width / 4.9, rect.ul.y + height / 8 * 3, rect.ul.x + width / 1.5, rect.ul.y + height / 2, rect.lr.x - width / 4.9, rect.lr.y - height / 8 * 3, rect.ul.x + width / 1.5, rect.lr.y - height / 8 * 3};
	PhPoint_t p_180[] = {rect.ul.x + width / 8 * 3, rect.ul.y + height / 1.7, rect.ul.x + width / 8 * 3, rect.lr.y - height / 4, rect.ul.x + width / 2, rect.ul.y + height / 1.5, rect.lr.x - width / 8 * 3.1, rect.lr.y - height / 4, rect.lr.x - width / 8 * 3.1, rect.ul.y + height / 1.7};
	PhPoint_t p_270[] = {rect.lr.x - width / 1.5, rect.ul.y + height / 8 * 3.5, rect.ul.x + width / 5.6, rect.ul.y + height / 8 * 3.5, rect.lr.x - width / 1.5, rect.ul.y + height / 1.9, rect.ul.x + width / 5.6, rect.lr.y - height / 8 * 3, rect.lr.x - width / 1.5, rect.lr.y - height / 8 * 3};
	
	switch(otherone->hz_rotates)
	{
		
		case HZ_3WayValveThree_90:
		{
			
			c.x = rect.ul.x + width/2 + width*13/60;
			c.y = rect.ul.y + height/2;
			r.x =  (width/2 + width*7/60)-(width/2);
			r.y =  (height/2 - height*13/60)-(height/2 - height*2/60);
			PgDrawEllipse(&c, &r, Pg_DRAW_FILL_STROKE);
			PgDrawPolygon(p_90, 5, &o, Pg_DRAW_STROKE);
			
			up_triangle1_point1.x = c.x - width*5/40;
			up_triangle1_point1.y = c.y - height*5/30;
			up_triangle1_point2.x = c.x + width*(-5)/40;
			up_triangle1_point2.y = c.y + height*5/30;
			up_triangle1_point3.x = c.x - width*10/30;
			up_triangle1_point3.y = c.y +height*0/30;
			
			line_point1.x = c.x - radiiX;
			line_point1.y = c.y;
			line_point2.x = c.x - width*25/74;
			line_point2.y = c.y;
			
			left_point1.x = c.x - width*15/74;
			left_point1.y = c.y - height*21/60;
			left_point2.x = c.x - width*35/74;
			left_point2.y = c.y - height*21/60;
			middle_point1.x = c.x - width*46/74;
			middle_point1.y = c.y - height*10/60;
			middle_point2.x = c.x - width*46/74;
			middle_point2.y = c.y + height*10/60;
			right_point1.x = c.x - width*15/74;
			right_point1.y = c.y + height*21/60;
			right_point2.x = c.x - width*35/74;
			right_point2.y = c.y + height*21/60;
			break;
		}
		case HZ_3WayValveThree_180:
		{
			c.x = rect.ul.x + width/2 - width*1/60;
			c.y = rect.ul.y + height/1.5;
			r.x =  (width/2 + width*10/60)-(width/2.2);
			r.y =  (height/2 - height*10/60)-(height/2 - height*3/60);
			PgDrawEllipse(&c, &r, Pg_DRAW_FILL_STROKE);
			PgDrawPolygon(p_180, 5, &o, Pg_DRAW_STROKE);
			
			up_triangle1_point1.x = c.x - width*8/50;
			up_triangle1_point1.y = c.y - height*4/30;
			up_triangle1_point2.x = c.x + width*8/50;
			up_triangle1_point2.y = c.y - height*4/30;
			up_triangle1_point3.x = c.x + width*0/40;
			up_triangle1_point3.y = c.y -height*10/30;
			
			line_point1.x = c.x;
			line_point1.y = c.y - radiiY;
			line_point2.x = c.x;
			line_point2.y = c.y - height*25/74;
			
			left_point1.x = c.x + width*21/60;
			left_point1.y = c.y - height*15/74;
			left_point2.x = c.x + width*21/60;
			left_point2.y = c.y - height*35/74;
			middle_point1.x = c.x + width*10/60;
			middle_point1.y = c.y - height*46/74;
			middle_point2.x = c.x - width*10/60;
			middle_point2.y = c.y - height*46/74;
			right_point1.x = c.x - width*21/60;
			right_point1.y = c.y - height*15/74;
			right_point2.x = c.x - width*21/60;
			right_point2.y = c.y - height*35/74;
			break;
		}
		case HZ_3WayValveThree_270:
		{
			c.x = rect.ul.x + width/4.2;
			c.y = rect.ul.y + height/2 + height*2/60;
			r.x =  (width/2 + width*7/60)-(width/2);
			r.y =  (height/2 - height*13/60)-(height/2 - height*3/60);
			PgDrawEllipse(&c, &r, Pg_DRAW_FILL_STROKE);
			PgDrawPolygon(p_270, 5, &o, Pg_DRAW_STROKE);
			
			up_triangle1_point1.x = c.x - width*(-5)/40;
			up_triangle1_point1.y = c.y - height*5/30;
			up_triangle1_point2.x = c.x + width*13/40;
			up_triangle1_point2.y = c.y + height*0/30;
			up_triangle1_point3.x = c.x - width*(-5)/40;
			up_triangle1_point3.y = c.y +height*5/30;
			
			line_point1.x = c.x + radiiX;
			line_point1.y = c.y;
			line_point2.x = c.x + width*25/74;
			line_point2.y = c.y;
			
			left_point1.x = c.x + width*15/74;
			left_point1.y = c.y + height*21/60;
			left_point2.x = c.x + width*35/74;
			left_point2.y = c.y + height*21/60;
			
			middle_point1.x = c.x + width*46/74;
			middle_point1.y = c.y + height*10/60;
			middle_point2.x = c.x + width*46/74;
			middle_point2.y = c.y - height*10/60;
			
			right_point1.x = c.x + width*15/74;
			right_point1.y = c.y - height*21/60;
			right_point2.x = c.x + width*35/74;
			right_point2.y = c.y - height*21/60;
			break;
		}
		default:
		{
			c.x = rect.ul.x + width / 2;
			c.y = rect.ul.y + height / 3;
			r.x = width / 4;
			r.y = height / 8;
			
			
			up_triangle1_point1.x = c.x - width*5/50;
			up_triangle1_point1.y = c.y - height*(-4)/30;
			up_triangle1_point2.x = c.x + width*5/50;
			up_triangle1_point2.y = c.y - height*(-4)/30;
			up_triangle1_point3.x = c.x;
			up_triangle1_point3.y = c.y +height*10/30;
			
			line_point1.x = c.x;
			line_point1.y = c.y + radiiY;
			line_point2.x = c.x;
			line_point2.y = c.y + height*25/74;
			
			left_point1.x = c.x - width*21/60;
			left_point1.y = c.y + height*15/74;
			left_point2.x = c.x - width*21/60;
			left_point2.y = c.y + height*35/74;
			middle_point1.x = c.x - width*10/60;
			middle_point1.y = c.y + height*46/74;
			middle_point2.x = c.x + width*10/60;
			middle_point2.y = c.y + height*46/74;
			right_point1.x = c.x + width*21/60;
			right_point1.y = c.y + height*15/74;
			right_point2.x = c.x + width*21/60;
			right_point2.y = c.y + height*35/74;
			
			PgDrawEllipse( &c, &r, Pg_DRAW_FILL_STROKE );
			PgDrawPolygon(p_0, 5, &o, Pg_DRAW_STROKE);
			break;
		}
	}
	
	//PhPoint_t  r = { radiiX, radiiY };
	
	PhPoint_t  pp[] = { up_triangle1_point1, up_triangle1_point2, up_triangle1_point3 };//多边形点的坐标
	PgDrawPolygon( pp, 3, &o, Pg_DRAW_FILL_STROKE | Pg_CLOSED );
	PhPoint_t  p[] = { left_point1, left_point2, line_point2 };
	PhPoint_t  q[] = { middle_point1, middle_point2, line_point2 };
	PhPoint_t  s[] = { right_point1, right_point2, line_point2 };
	PgDrawPolygon( p, 3, &o, Pg_DRAW_FILL | Pg_CLOSED );
	PgDrawPolygon( q, 3, &o, Pg_DRAW_FILL | Pg_CLOSED );
	PgDrawPolygon( s, 3, &o, Pg_DRAW_FILL | Pg_CLOSED );

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

PtWidgetClass_t *CreateHZ_3WayValveThreeClass(void)
{
	static PtResourceRec_t resources[] = {
			{HZ_HEIGHT181, Pt_CHANGE_RESIZE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveThreeWidget, hz_height ), 0},
			{HZ_WIDTH181, Pt_CHANGE_RESIZE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveThreeWidget, hz_width ), 0},
			{HZ_ROTATE181, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveThreeWidget, hz_rotates ), 0},
			{HZ_COLOR181, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveThreeWidget, hz_color ), 0},
			{HZ_FRAME_COLOR181, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveThreeWidget, hz_frame_color ), 0},
			{HZ_Q181, Pt_CHANGE_REDRAW, 0,
			Pt_ARG_IS_NUMBER( HZ_3WayValveThreeWidget, hz_q ), 0},
			};


	static PtArg_t args[] = {
			{ Pt_SET_VERSION, 110, 0},
			{ Pt_SET_STATE_LEN, sizeof(HZ_3WayValveThreeWidget ), 0},
			{ Pt_SET_DFLTS_F, (long)HZ_3WayValveThree_dflts, 0 },
			{ Pt_SET_DRAW_F, (long)HZ_3WayValveThree_draw, 0 },
			{ Pt_SET_NUM_RESOURCES,
			sizeof( resources ) / sizeof( resources[0] ), 0 },
			{ Pt_SET_RESOURCES, (long)resources,
			sizeof( resources ) / sizeof( resources[0] )},
			};
	return (HZ_3WayValveThree->wclass = PtCreateWidgetClass(
			PtLabel, 0, sizeof(args)/sizeof(args[0]), args));
}
