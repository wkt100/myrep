#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Ph.h>
#include <Pt.h>

/* 调试宏定义 */
/* #define NOWDEBUG */  /* 取消注释此行以启用调试输出 */

/* 根据NOWDEBUG宏定义调试输出函数 */
#ifdef NOWDEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...) /* 不做任何事 */
#endif

/* 表情状态 */
typedef enum {
    NORMAL = 0,  /* 普通表情 */
    SMILE,       /* 微笑表情 */
    CRY          /* 哭泣表情 */
} FaceState;

/* 自定义颜色，使用PgRGB函数 */
#define Pg_SPONGE_YELLOW   PgRGB(255, 222, 0)   /* 海绵宝宝的黄色 */
#define Pg_DARKBROWN       PgRGB(76, 43, 0)     /* 深棕色 */
#define Pg_LIGHTBROWN      PgRGB(150, 85, 0)    /* 浅棕色 */
#define Pg_PINK            PgRGB(255, 150, 150) /* 粉红色 */
#define Pg_SKY_BLUE        PgRGB(135, 206, 235) /* 天蓝色 */
#define Pg_TEAR_BLUE       PgRGB(0, 191, 255)   /* 眼泪蓝色 */

/* 全局变量 - 当前表情状态 */
FaceState current_face = NORMAL;

/* 绘制不同表情的嘴巴 */
void draw_mouth(FaceState state) {
    /* 设置黑色绘图 */
    PgSetStrokeColor(Pg_BLACK);
    PgSetStrokeWidth(2);
    
    switch(state) {
        case SMILE:  /* 微笑表情 - 更自然的微笑曲线 */
            DEBUG_PRINT("绘制微笑表情\n");
            /* 绘制笑脸 - 使用扇形 */
            {
                PhPoint_t center = {200, 250};  /* 中心点 */
                PhPoint_t radius = {40, 20};    /* 半径 */
                PgSetFillColor(Pg_BLACK);
                PgDrawArc(&center, &radius, 0, 180, Pg_DRAW_FILL | Pg_ARC_CHORD);
            }
            break;
            
        case CRY:    /* 哭泣表情 - 更夸张的哭泣效果 */
            DEBUG_PRINT("绘制哭泣表情\n");
            /* 嘴巴下垂 - 使用扇形 */
            {
                PhPoint_t center = {200, 250};  /* 中心点 */
                PhPoint_t radius = {40, 20};    /* 半径 */
                PgSetFillColor(Pg_BLACK);
                PgDrawArc(&center, &radius, 180, 360, Pg_DRAW_FILL | Pg_ARC_CHORD);
            }
            
            /* 眼泪 - 蓝色，更大更夸张 */
            PgSetFillColor(Pg_TEAR_BLUE);
            
            /* 左眼泪 */
            {
                PhPoint_t leftTearCenter = {150, 210};
                PhPoint_t leftTearRadius = {10, 20};
                PgDrawArc(&leftTearCenter, &leftTearRadius, 0, 360, Pg_DRAW_FILL | Pg_ARC_PIE);
            }
            
            /* 左泪滴 */
            {
                PhPoint_t leftDropPoints[] = {
                    {150, 245},  /* 顶点 */
                    {140, 260},  /* 左侧 */
                    {150, 270},  /* 底部 */
                    {160, 260}   /* 右侧 */
                };
                PhPoint_t leftDropOffset = {0, 0};
                PgDrawPolygon(leftDropPoints, 4, &leftDropOffset, Pg_DRAW_FILL | Pg_CLOSED);
            }
            
            /* 右眼泪 */
            {
                PhPoint_t rightTearCenter = {250, 210};
                PhPoint_t rightTearRadius = {10, 20};
                PgDrawArc(&rightTearCenter, &rightTearRadius, 0, 360, Pg_DRAW_FILL | Pg_ARC_PIE);
            }
            
            /* 右泪滴 */
            {
                PhPoint_t rightDropPoints[] = {
                    {250, 245},  /* 顶点 */
                    {240, 260},  /* 左侧 */
                    {250, 270},  /* 底部 */
                    {260, 260}   /* 右侧 */
                };
                PhPoint_t rightDropOffset = {0, 0};
                PgDrawPolygon(rightDropPoints, 4, &rightDropOffset, Pg_DRAW_FILL | Pg_CLOSED);
            }
            
            break;
            
        case NORMAL:     /* 普通表情 - 简单的直线 */
            DEBUG_PRINT("绘制普通表情\n");
            PgDrawILine(160, 240, 240, 240);
            break;
            
        default:     /* 默认也是普通表情 - 保障安全性 */
            DEBUG_PRINT("绘制默认表情（普通）\n");
            PgDrawILine(160, 240, 240, 240);
            break;
    }
}

/* 绘制海绵宝宝 */
void draw_spongebob(PtWidget_t *widget, void *data) {
    PhRect_t canvasRect = {{0, 0}, {0, 0}}; /* 初始化为空矩形 */
    int i = 0;
    short pos_x = 0, pos_y = 0;
    
    DEBUG_PRINT("开始绘制海绵宝宝...\n");
    DEBUG_PRINT("当前表情状态: %d\n", current_face);
    
    /* 检查widget指针 */
    if (!widget) {
        fprintf(stderr, "无效的widget指针\n");
        return;
    }
    
    /* 使用固定的画布区域 - 避免使用可能有问题的函数 */
    canvasRect.ul.x = 50;
    canvasRect.ul.y = 50;
    canvasRect.lr.x = 350;
    canvasRect.lr.y = 380;
    
    DEBUG_PRINT("画布区域: (%d,%d) - (%d,%d)\n",
            canvasRect.ul.x, canvasRect.ul.y, 
            canvasRect.lr.x, canvasRect.lr.y);
    
    /* 设置背景色为天蓝色 */
    PgSetFillColor(Pg_SKY_BLUE);
    if (PgDrawRect(&canvasRect, Pg_DRAW_FILL) != 0) {
        fprintf(stderr, "背景绘制失败\n");
    } else {
        DEBUG_PRINT("背景绘制成功\n");
    }
    
    /* 绘制斑点 - 海绵宝宝特有的孔洞 */
    PgSetFillColor(PgRGB(255, 200, 0)); /* 稍深的黄色 */
    PhRect_t spots[] = {
        {{110, 110}, {120, 120}},
        {{150, 130}, {160, 140}},
        {{200, 115}, {215, 130}},
        {{250, 140}, {265, 155}},
        {{130, 190}, {145, 205}},
        {{230, 240}, {245, 255}},
        {{180, 280}, {195, 295}}
    };
    
    for (i = 0; i < sizeof(spots)/sizeof(spots[0]); i++) {
        PgDrawRect(&spots[i], Pg_DRAW_FILL);
    }
    
    /* 绘制身体 - 使用更圆润的矩形 */
    PhRect_t bodyRect = {{100, 100}, {300, 300}};
    PgSetFillColor(Pg_SPONGE_YELLOW);
    if (PgDrawRect(&bodyRect, Pg_DRAW_FILL) != 0) {
        fprintf(stderr, "身体绘制失败\n");
    } else {
        DEBUG_PRINT("身体绘制成功\n");
    }
    
    /* 绘制衬衫 - 白色 */
    PhRect_t shirtRect = {{100, 220}, {300, 300}};
    PgSetFillColor(Pg_WHITE);
    if (PgDrawRect(&shirtRect, Pg_DRAW_FILL) != 0) {
        fprintf(stderr, "衣服绘制失败\n");
    }
    
    /* 绘制衣服褶皱 */
    PgSetStrokeColor(Pg_DGREY);
    PgDrawILine(150, 220, 150, 300);  /* 左侧褶皱 */
    PgDrawILine(250, 220, 250, 300);  /* 右侧褶皱 */
    PgDrawILine(100, 260, 300, 260);  /* 中间褶皱 */
    
    /* 绘制领带 - 红色 */
    PgSetFillColor(Pg_RED);
    
    /* 领带结 */
    PhRect_t tieKnot = {{185, 220}, {215, 240}};
    PgDrawRect(&tieKnot, Pg_DRAW_FILL);
    
    /* 领带带子 - 使用多边形 */
    PhPoint_t tiePoints[] = {
        {200, 240},  /* 起点 */
        {185, 260},  /* 左点 */
        {200, 280},  /* 中点 */
        {215, 260}   /* 右点 */
    };
    PhPoint_t tieOffset = {0, 0};
    PgDrawPolygon(tiePoints, 4, &tieOffset, Pg_DRAW_FILL | Pg_CLOSED);
    
    /* 绘制眼睛 - 大大的白色眼睛 */
    PgSetFillColor(Pg_WHITE);
    
    /* 左眼白 - 椭圆形 */
    {
        PhPoint_t leftEyeCenter = {150, 175};
        PhPoint_t leftEyeRadius = {30, 25};
        PgDrawEllipse(&leftEyeCenter, &leftEyeRadius, Pg_DRAW_FILL);
    }
    
    /* 右眼白 - 椭圆形 */
    {
        PhPoint_t rightEyeCenter = {250, 175};
        PhPoint_t rightEyeRadius = {30, 25};
        PgDrawEllipse(&rightEyeCenter, &rightEyeRadius, Pg_DRAW_FILL);
    }
    
    /* 绘制眼睛轮廓 */
    PgSetStrokeColor(Pg_BLACK);
    {
        PhPoint_t leftEyeCenter = {150, 175};
        PhPoint_t leftEyeRadius = {30, 25};
        PgDrawEllipse(&leftEyeCenter, &leftEyeRadius, Pg_DRAW_STROKE);
    }
    {
        PhPoint_t rightEyeCenter = {250, 175};
        PhPoint_t rightEyeRadius = {30, 25};
        PgDrawEllipse(&rightEyeCenter, &rightEyeRadius, Pg_DRAW_STROKE);
    }
    
    /* 绘制瞳孔 - 蓝色 */
    PgSetFillColor(Pg_DBLUE);
    
    /* 左瞳孔 */
    {
        PhPoint_t leftPupilCenter = {150, 175};
        PhPoint_t leftPupilRadius = {10, 10};
        PgDrawEllipse(&leftPupilCenter, &leftPupilRadius, Pg_DRAW_FILL);
    }
    
    /* 右瞳孔 */
    {
        PhPoint_t rightPupilCenter = {250, 175};
        PhPoint_t rightPupilRadius = {10, 10};
        PgDrawEllipse(&rightPupilCenter, &rightPupilRadius, Pg_DRAW_FILL);
    }
    
    /* 高光效果 */
    PgSetFillColor(Pg_WHITE);
    {
        PhPoint_t leftHighlightCenter = {147, 172};
        PhPoint_t leftHighlightRadius = {2, 2};
        PgDrawEllipse(&leftHighlightCenter, &leftHighlightRadius, Pg_DRAW_FILL);
    }
    {
        PhPoint_t rightHighlightCenter = {247, 172};
        PhPoint_t rightHighlightRadius = {2, 2};
        PgDrawEllipse(&rightHighlightCenter, &rightHighlightRadius, Pg_DRAW_FILL);
    }
    
    /* 绘制眉毛 */
    PgSetStrokeColor(Pg_BLACK);
    PgSetStrokeWidth(2);
    
    /* 绘制睫毛 */
    PgSetStrokeWidth(1);
    PgDrawILine(130, 145, 140, 140);  /* 左眼左侧睫毛 */
    PgDrawILine(150, 140, 150, 130);  /* 左眼中间睫毛 */
    PgDrawILine(170, 145, 160, 140);  /* 左眼右侧睫毛 */
    
    PgDrawILine(230, 145, 240, 140);  /* 右眼左侧睫毛 */
    PgDrawILine(250, 140, 250, 130);  /* 右眼中间睫毛 */
    PgDrawILine(270, 145, 260, 140);  /* 右眼右侧睫毛 */
    
    /* 绘制鼻子 - 黄色 */
    PgSetFillColor(Pg_SPONGE_YELLOW);
    {
        PhPoint_t noseCenter = {200, 200};
        PhPoint_t noseRadius = {10, 20};
        PgDrawEllipse(&noseCenter, &noseRadius, Pg_DRAW_FILL);
    }
    PgSetStrokeColor(Pg_BLACK);
    {
        PhPoint_t noseCenter = {200, 200};
        PhPoint_t noseRadius = {10, 20};
        PgDrawEllipse(&noseCenter, &noseRadius, Pg_DRAW_STROKE);
    }
    
    /* 绘制腮红 */
    PgSetFillColor(Pg_PINK);
    {
        PhPoint_t leftCheekCenter = {130, 210};
        PhPoint_t leftCheekRadius = {10, 10};
        PgDrawEllipse(&leftCheekCenter, &leftCheekRadius, Pg_DRAW_FILL);
    }
    {
        PhPoint_t rightCheekCenter = {270, 210};
        PhPoint_t rightCheekRadius = {10, 10};
        PgDrawEllipse(&rightCheekCenter, &rightCheekRadius, Pg_DRAW_FILL);
    }
    
    /* 绘制牙齿 */
    PgSetFillColor(Pg_WHITE);
    PhRect_t leftToothRect = {{170, 250}, {185, 260}};
    PhRect_t rightToothRect = {{215, 250}, {230, 260}};
    PgDrawRect(&leftToothRect, Pg_DRAW_FILL);
    PgDrawRect(&rightToothRect, Pg_DRAW_FILL);
    PgSetStrokeColor(Pg_BLACK);
    PgDrawRect(&leftToothRect, Pg_DRAW_STROKE);
    PgDrawRect(&rightToothRect, Pg_DRAW_STROKE);
    
    /* 绘制嘴巴 - 根据当前表情状态 */
    DEBUG_PRINT("调用draw_mouth绘制嘴巴，表情状态: %d\n", current_face);
    draw_mouth(current_face);
    
    /* 绘制手臂 */
    PgSetFillColor(Pg_SPONGE_YELLOW);
    PhRect_t leftArmRect = {{80, 230}, {100, 290}};
    PhRect_t rightArmRect = {{300, 230}, {320, 290}};
    PgDrawRect(&leftArmRect, Pg_DRAW_FILL);
    PgDrawRect(&rightArmRect, Pg_DRAW_FILL);
    
    /* 绘制手 */
    {
        PhPoint_t leftHandCenter = {70, 290};
        PhPoint_t leftHandRadius = {10, 10};
        PgDrawEllipse(&leftHandCenter, &leftHandRadius, Pg_DRAW_FILL);
    }
    {
        PhPoint_t rightHandCenter = {330, 290};
        PhPoint_t rightHandRadius = {10, 10};
        PgDrawEllipse(&rightHandCenter, &rightHandRadius, Pg_DRAW_FILL);
    }
    
    /* 绘制裤子 - 棕色 */
    PhRect_t pantsRect = {{100, 300}, {300, 330}};
    PgSetFillColor(Pg_BROWN);
    if (PgDrawRect(&pantsRect, Pg_DRAW_FILL) != 0) {
        fprintf(stderr, "裤子绘制失败\n");
    }
    
    /* 绘制腰带 */
    PgSetFillColor(Pg_BLACK);
    PhRect_t beltRect = {{100, 300}, {300, 310}};
    PgDrawRect(&beltRect, Pg_DRAW_FILL);
    
    /* 绘制裤腿 */
    PgSetFillColor(Pg_BROWN);
    PhRect_t leftLegRect = {{120, 330}, {150, 350}};
    PhRect_t rightLegRect = {{250, 330}, {280, 350}};
    PgDrawRect(&leftLegRect, Pg_DRAW_FILL);
    PgDrawRect(&rightLegRect, Pg_DRAW_FILL);
    
    /* 绘制鞋子 */
    PgSetFillColor(Pg_BLACK);
    PhRect_t leftShoeRect = {{110, 350}, {160, 370}};
    PhRect_t rightShoeRect = {{240, 350}, {290, 370}};
    PgDrawRect(&leftShoeRect, Pg_DRAW_FILL);
    PgDrawRect(&rightShoeRect, Pg_DRAW_FILL);
    
    /* 刷新绘图 */
    DEBUG_PRINT("绘图完成，刷新显示...\n");
    PgFlush();
}

/* 微笑按钮回调 */
int smile_cb(PtWidget_t *widget, void *data, PtCallbackInfo_t *cbinfo) {
    DEBUG_PRINT("微笑按钮被点击\n");
    
    if (!widget) {
        fprintf(stderr, "错误：无效的控件指针\n");
        return Pt_CONTINUE;
    }
    
    /* 安全地切换表情状态 */
    current_face = SMILE;
    DEBUG_PRINT("将表情状态设置为微笑: %d\n", current_face);
    
    /* 直接使用传入的data作为绘图区域 */
    PtWidget_t *draw_area = (PtWidget_t *)data;
    DEBUG_PRINT("从回调数据中获取绘图区域: %p\n", draw_area);
    
    if (!draw_area) {
        fprintf(stderr, "无法获取绘图区域，尝试使用主窗口\n");
        /* 如果无法获取绘图区域，尝试获取父窗口 */
        draw_area = PtFindDisjoint(widget);
        DEBUG_PRINT("尝试获取父窗口, 结果: %p\n", draw_area);
        
        if (!draw_area) {
            fprintf(stderr, "无法获取父窗口，无法继续绘图\n");
            return Pt_CONTINUE;
        }
    }
    
    /* 绘图前先确认控件有效 */
    DEBUG_PRINT("检查draw_area是否已实现: %d\n", PtWidgetIsRealized(draw_area));
    if (PtWidgetIsRealized(draw_area)) {
        /* 使用try-catch风格保护关键操作 */
        DEBUG_PRINT("正在绘制微笑表情...\n");
        
        /* 直接调用绘图函数 */
        draw_spongebob(draw_area, NULL);
        DEBUG_PRINT("微笑表情绘制完成\n");
    } else {
        fprintf(stderr, "绘图区域未初始化\n");
        /* 尝试直接绘制，即使控件未实现 */
        DEBUG_PRINT("尝试强制绘制...\n");
        draw_spongebob(draw_area, NULL);
    }
    
    /* 确保在回调函数结束前回到一个稳定状态 */
    PgFlush();
    
    return Pt_CONTINUE;
}

/* 哭泣按钮回调 */
int cry_cb(PtWidget_t *widget, void *data, PtCallbackInfo_t *cbinfo) {
    DEBUG_PRINT("哭泣按钮被点击\n");
    
    if (!widget) {
        fprintf(stderr, "错误：无效的控件指针\n");
        return Pt_CONTINUE;
    }
    
    /* 安全地切换表情状态 */
    current_face = CRY;
    DEBUG_PRINT("将表情状态设置为哭泣: %d\n", current_face);
    
    /* 直接使用传入的data作为绘图区域 */
    PtWidget_t *draw_area = (PtWidget_t *)data;
    DEBUG_PRINT("从回调数据中获取绘图区域: %p\n", draw_area);
    
    if (!draw_area) {
        fprintf(stderr, "无法获取绘图区域，尝试使用主窗口\n");
        /* 如果无法获取绘图区域，尝试获取父窗口 */
        draw_area = PtFindDisjoint(widget);
        DEBUG_PRINT("尝试获取父窗口, 结果: %p\n", draw_area);
        
        if (!draw_area) {
            fprintf(stderr, "无法获取父窗口，无法继续绘图\n");
            return Pt_CONTINUE;
        }
    }
    
    /* 绘图前先确认控件有效 */
    DEBUG_PRINT("检查draw_area是否已实现: %d\n", PtWidgetIsRealized(draw_area));
    if (PtWidgetIsRealized(draw_area)) {
        /* 使用try-catch风格保护关键操作 */
        DEBUG_PRINT("正在绘制哭泣表情...\n");
        
        /* 直接调用绘图函数 */
        draw_spongebob(draw_area, NULL);
        DEBUG_PRINT("哭泣表情绘制完成\n");
    } else {
        fprintf(stderr, "绘图区域未初始化\n");
        /* 尝试直接绘制，即使控件未实现 */
        DEBUG_PRINT("尝试强制绘制...\n");
        draw_spongebob(draw_area, NULL);
    }
    
    /* 确保在回调函数结束前回到一个稳定状态 */
    PgFlush();
    
    return Pt_CONTINUE;
}

/* 普通表情按钮回调 */
int normal_cb(PtWidget_t *widget, void *data, PtCallbackInfo_t *cbinfo) {
    DEBUG_PRINT("普通表情按钮被点击\n");
    
    if (!widget) {
        fprintf(stderr, "错误：无效的控件指针\n");
        return Pt_CONTINUE;
    }
    
    /* 安全地切换表情状态 */
    current_face = NORMAL;
    DEBUG_PRINT("将表情状态设置为普通: %d\n", current_face);
    
    /* 直接使用传入的data作为绘图区域 */
    PtWidget_t *draw_area = (PtWidget_t *)data;
    DEBUG_PRINT("从回调数据中获取绘图区域: %p\n", draw_area);
    
    if (!draw_area) {
        fprintf(stderr, "无法获取绘图区域，尝试使用主窗口\n");
        /* 如果无法获取绘图区域，尝试获取父窗口 */
        draw_area = PtFindDisjoint(widget);
        DEBUG_PRINT("尝试获取父窗口, 结果: %p\n", draw_area);
        
        if (!draw_area) {
            fprintf(stderr, "无法获取父窗口，无法继续绘图\n");
            return Pt_CONTINUE;
        }
    }
    
    /* 绘图前先确认控件有效 */
    DEBUG_PRINT("检查draw_area是否已实现: %d\n", PtWidgetIsRealized(draw_area));
    if (PtWidgetIsRealized(draw_area)) {
        /* 使用try-catch风格保护关键操作 */
        DEBUG_PRINT("正在绘制普通表情...\n");
        
        /* 直接调用绘图函数 */
        draw_spongebob(draw_area, NULL);
        DEBUG_PRINT("普通表情绘制完成\n");
    } else {
        fprintf(stderr, "绘图区域未初始化\n");
        /* 尝试直接绘制，即使控件未实现 */
        DEBUG_PRINT("尝试强制绘制...\n");
        draw_spongebob(draw_area, NULL);
    }
    
    /* 确保在回调函数结束前回到一个稳定状态 */
    PgFlush();
    
    return Pt_CONTINUE;
}

/* 事件处理回调函数 */
int event_handler(PtWidget_t *widget, void *data, PtCallbackInfo_t *cbinfo) {
    if (!widget || !cbinfo || !cbinfo->event) {
        fprintf(stderr, "事件处理器：无效的参数\n");
        return Pt_CONTINUE;
    }
    
    PhEvent_t *event = cbinfo->event;
    
    DEBUG_PRINT("接收到事件类型: %ld\n", event->type);
    
    /* 处理不同类型的事件 */
    switch (event->type) {
        case Ph_EV_EXPOSE:
            DEBUG_PRINT("处理Expose事件\n");
            /* 直接调用绘图函数 */
            draw_spongebob(widget, NULL);
            break;
            
        case Ph_EV_WM:
            /* 检查是否是关闭窗口请求 */
            {
                PhWindowEvent_t *wm_event = PhGetData(event);
                if (wm_event && wm_event->event_f == Ph_WM_CLOSE) {
                    DEBUG_PRINT("接收到窗口关闭事件，忽略它以防止窗口关闭\n");
                    return Pt_END;  /* 阻止关闭事件被处理 */
                }
                
                DEBUG_PRINT("处理窗口管理事件\n");
                /* 直接调用绘图函数 */
                draw_spongebob(widget, NULL);
            }
            break;
            
        case Ph_EV_BUT_PRESS:
            DEBUG_PRINT("接收到按钮按下事件\n");
            /* 按钮事件已在按钮回调中处理，这里不需要额外操作 */
            break;
            
        case Ph_EV_BUT_RELEASE:
            DEBUG_PRINT("接收到按钮释放事件\n");
            /* 按钮事件已在按钮回调中处理，这里不需要额外操作 */
            break;
            
        case Ph_EV_PTR_MOTION:
            /* 鼠标移动事件 - 可以忽略，避免日志过多 */
            break;
            
        default:
            DEBUG_PRINT("接收到其他类型事件: %ld\n", event->type);
            break;
    }
    
    /* 确保所有图形操作都被刷新 */
    PgFlush();
    
    return Pt_CONTINUE;
}

/* 窗口实现时回调函数 */
int realized_cb(PtWidget_t *widget, void *data, PtCallbackInfo_t *cbinfo) {
    /* 窗口第一次显示时调用绘图函数 */
    DEBUG_PRINT("窗口已实现回调被触发\n");
    
    if (!widget) {
        fprintf(stderr, "realized_cb: 收到空widget指针\n");
        return Pt_CONTINUE;
    }
    
    /* 直接调用绘图函数 */
    draw_spongebob(widget, NULL);
    
    /* 确保刷新所有绘图操作 */
    PgFlush();
    
    return Pt_CONTINUE;
}

/* 绘图回调函数 - 尝试另一种方法 */
int draw_cb(PtWidget_t *widget, void *data, PtCallbackInfo_t *cbinfo) {
    DEBUG_PRINT("绘图回调被触发\n");
    /* 不使用PtCalcCanvas，直接调用绘图函数 */
    draw_spongebob(widget, NULL);
    return Pt_CONTINUE;
}

/* 主函数 - 修改窗口和按钮初始化部分 */
int main(int argc, char *argv[]) {
    PtWidget_t *window;
    PtWidget_t *smile_btn;
    PtWidget_t *cry_btn;
    PtWidget_t *normal_btn;
    PtWidget_t *draw_area;
    PtWidget_t *btn_area;
    PhDim_t dim = {400, 600}; /* 增加窗口高度，为按钮留出空间 */
    PtArg_t args[10];
    int n;
    
    DEBUG_PRINT("程序开始运行...\n");
    
    /* 初始化Photon应用程序 */
    if (PtInit(NULL) == -1) {
        fprintf(stderr, "Photon初始化失败\n");
        return EXIT_FAILURE;
    }
    DEBUG_PRINT("Photon初始化成功\n");
    
    /* 创建主窗口，使用参数数组 */
    n = 0;
    PtSetArg(&args[n++], Pt_ARG_WINDOW_TITLE, "海绵宝宝表情控制", 0);
    PtSetArg(&args[n++], Pt_ARG_DIM, &dim, 0);
    PtSetArg(&args[n++], Pt_ARG_WINDOW_RENDER_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    window = PtCreateWidget(PtWindow, NULL, n, args);
    
    if (!window) {
        fprintf(stderr, "无法创建窗口\n");
        return EXIT_FAILURE;
    }
    DEBUG_PRINT("窗口创建成功\n");
    
    /* 创建绘图区域 */
    PhPoint_t draw_pos = {0, 0};
    PhDim_t draw_dim = {400, 380};
    n = 0;
    PtSetArg(&args[n++], Pt_ARG_POS, &draw_pos, 0);
    PtSetArg(&args[n++], Pt_ARG_DIM, &draw_dim, 0);
    PtSetArg(&args[n++], Pt_ARG_FILL_COLOR, Pg_DGRAY, 0);
    draw_area = PtCreateWidget(PtRaw, window, n, args);
    
    if (!draw_area) {
        fprintf(stderr, "无法创建绘图区域\n");
        return EXIT_FAILURE;
    }
    
    /* 添加绘图区域的回调和事件处理 */
    PtAddCallback(draw_area, Pt_CB_REALIZED, realized_cb, NULL);
    PtAddEventHandler(draw_area, Ph_EV_EXPOSE, event_handler, NULL);
    PtAddEventHandler(draw_area, Ph_EV_WM, event_handler, NULL);
    PtAddEventHandler(draw_area, Ph_EV_BUT_PRESS, event_handler, NULL);
    
    /* 创建按钮区域背景 */
    PhPoint_t btn_area_pos = {0, 380};
    PhDim_t btn_area_dim = {400, 220};
    n = 0;
    PtSetArg(&args[n++], Pt_ARG_POS, &btn_area_pos, 0);
    PtSetArg(&args[n++], Pt_ARG_DIM, &btn_area_dim, 0);
    PtSetArg(&args[n++], Pt_ARG_FILL_COLOR, Pg_LGRAY, 0);
    btn_area = PtCreateWidget(PtRaw, window, n, args);
    
    if (!btn_area) {
        fprintf(stderr, "无法创建按钮区域\n");
        return EXIT_FAILURE;
    }
    
    /* 创建微笑按钮 */
    PhPoint_t btn_pos = {100, 400};
    PhDim_t btn_dim = {80, 30};
    
    n = 0;
    PtSetArg(&args[n++], Pt_ARG_POS, &btn_pos, 0);
    PtSetArg(&args[n++], Pt_ARG_DIM, &btn_dim, 0);
    PtSetArg(&args[n++], Pt_ARG_TEXT_STRING, "微笑", 0);
    
    DEBUG_PRINT("创建微笑按钮，绘图区域: %p\n", draw_area);
    smile_btn = PtCreateWidget(PtButton, window, n, args);
    
    if (!smile_btn) {
        fprintf(stderr, "无法创建微笑按钮\n");
        return EXIT_FAILURE;
    }
    
    DEBUG_PRINT("添加微笑按钮回调，传递draw_area: %p\n", draw_area);
    PtAddCallback(smile_btn, Pt_CB_ACTIVATE, smile_cb, draw_area);
    
    /* 创建哭泣按钮 */
    btn_pos.x = 220;
    n = 0;
    PtSetArg(&args[n++], Pt_ARG_POS, &btn_pos, 0);
    PtSetArg(&args[n++], Pt_ARG_DIM, &btn_dim, 0);
    PtSetArg(&args[n++], Pt_ARG_TEXT_STRING, "哭泣", 0);
    
    DEBUG_PRINT("创建哭泣按钮，绘图区域: %p\n", draw_area);
    cry_btn = PtCreateWidget(PtButton, window, n, args);
    
    if (!cry_btn) {
        fprintf(stderr, "无法创建哭泣按钮\n");
        return EXIT_FAILURE;
    }
    
    DEBUG_PRINT("添加哭泣按钮回调，传递draw_area: %p\n", draw_area);
    PtAddCallback(cry_btn, Pt_CB_ACTIVATE, cry_cb, draw_area);
    
    /* 创建普通表情按钮 */
    btn_pos.x = 160;
    btn_pos.y = 450;
    n = 0;
    PtSetArg(&args[n++], Pt_ARG_POS, &btn_pos, 0);
    PtSetArg(&args[n++], Pt_ARG_DIM, &btn_dim, 0);
    PtSetArg(&args[n++], Pt_ARG_TEXT_STRING, "普通", 0);
    
    /* 不再使用USER_DATA存储绘图区域 */
    DEBUG_PRINT("创建普通按钮，绘图区域: %p\n", draw_area);
    normal_btn = PtCreateWidget(PtButton, window, n, args);
    
    if (!normal_btn) {
        fprintf(stderr, "无法创建普通按钮\n");
        return EXIT_FAILURE;
    }
    
    /* 直接将draw_area作为回调数据传递 */
    DEBUG_PRINT("添加普通按钮回调，传递draw_area: %p\n", draw_area);
    PtAddCallback(normal_btn, Pt_CB_ACTIVATE, normal_cb, draw_area);
    
    DEBUG_PRINT("事件处理器和回调设置完成\n");
    
    /* 显示窗口 */
    DEBUG_PRINT("正在显示窗口...\n");
    PtRealizeWidget(window);
    DEBUG_PRINT("窗口已显示\n");
    
    /* 显示初始内容 */
    DEBUG_PRINT("准备初始绘图，draw_area: %p\n", draw_area);
    if (PtWidgetIsRealized(draw_area)) {
        DEBUG_PRINT("绘图区域已实现，开始初始绘制\n");
    } else {
        DEBUG_PRINT("警告：绘图区域未实现，但仍将尝试绘制\n");
    }
    
    /* 确保当前表情为普通 */
    current_face = NORMAL;
    DEBUG_PRINT("初始表情状态设置为普通: %d\n", current_face);
    
    /* 绘制初始表情 */
    draw_spongebob(draw_area, NULL);
    DEBUG_PRINT("初始绘图完成\n");
    
    /* 进入主循环 */
    DEBUG_PRINT("进入事件循环...\n");
    PtMainLoop();
    
    return EXIT_SUCCESS;
}
