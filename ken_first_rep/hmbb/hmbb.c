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

/* 全局变量 - 当前表情状态 */
FaceState current_face = NORMAL;

/* 绘制不同表情的嘴巴 */
void draw_mouth(FaceState state) {
    /* 设置黑色绘图 */
    PgSetStrokeColor(Pg_BLACK);
    
    switch(state) {
        case SMILE:  /* 微笑表情 - 嘴角上扬 */
            DEBUG_PRINT("绘制微笑表情\n");
            PgDrawILine(160, 260, 180, 280);
            PgDrawILine(180, 280, 200, 290);
            PgDrawILine(200, 290, 220, 280);
            PgDrawILine(220, 280, 240, 260);
            break;
            
        case CRY:    /* 哭泣表情 - 嘴角下垂，添加眼泪 */
            DEBUG_PRINT("绘制哭泣表情\n");
            /* 嘴巴下垂 */
            PgDrawILine(160, 280, 180, 270);
            PgDrawILine(180, 270, 200, 260);
            PgDrawILine(200, 260, 220, 270);
            PgDrawILine(220, 270, 240, 280);
            
            /* 眼泪 - 蓝色 */
            PgSetFillColor(Pg_BLUE);
            PhRect_t tear1 = {{165, 195}, {175, 230}};
            PhRect_t tear2 = {{225, 195}, {235, 230}};
            PgDrawRect(&tear1, Pg_DRAW_FILL);
            PgDrawRect(&tear2, Pg_DRAW_FILL);
            break;
            
        case NORMAL:     /* 普通表情 - 直线嘴巴 */
            DEBUG_PRINT("绘制普通表情\n");
            PgDrawILine(160, 260, 200, 280);
            PgDrawILine(200, 280, 240, 260);
            break;
            
        default:     /* 默认也是普通表情 - 保障安全性 */
            DEBUG_PRINT("绘制默认表情（普通）\n");
            PgDrawILine(160, 260, 200, 280);
            PgDrawILine(200, 280, 240, 260);
            break;
    }
}

/* 定义海绵宝宝绘图函数 */
int draw_spongebob(PtWidget_t *widget, PhTile_t *damage) {
    /* 使用标准矩形结构 */
    PhRect_t rect;
    PhRect_t canvasRect = {{0, 0}, {0, 0}}; /* 初始化为空矩形 */
    
    DEBUG_PRINT("开始绘制海绵宝宝...\n");
    DEBUG_PRINT("当前表情状态: %d\n", current_face);
    
    /* 检查widget指针 */
    if (!widget) {
        fprintf(stderr, "警告：draw_spongebob 收到空widget指针，尝试直接绘制\n");
        /* 即使没有widget，也尝试绘制到默认图形上下文 */
    }
    
    /* 固定使用画布大小 */
    canvasRect.ul.x = 0; 
    canvasRect.ul.y = 0;
    canvasRect.lr.x = 400; 
    canvasRect.lr.y = 380;
    
    DEBUG_PRINT("画布区域: (%d,%d) - (%d,%d)\n", 
           canvasRect.ul.x, canvasRect.ul.y, 
           canvasRect.lr.x, canvasRect.lr.y);
    
    /* 设置绘图上下文 */
    PgSetDrawBufferSize(0);
    PgSetDrawMode(Pg_DRAWMODE_OPAQUE);
    
    /* 清除背景 */
    PgSetFillColor(Pg_DGRAY);
    rect.ul.x = 50;
    rect.ul.y = 50;
    rect.lr.x = 350;
    rect.lr.y = 380;
    if (PgDrawRect(&rect, Pg_DRAW_FILL) == -1) {
        fprintf(stderr, "背景绘制失败\n");
    } else {
        DEBUG_PRINT("背景绘制成功\n");
    }
    
    /* 绘制身体（黄色矩形） */
    PgSetFillColor(0xFFFF00);
    rect.ul.x = 100;
    rect.ul.y = 100;
    rect.lr.x = 300;
    rect.lr.y = 350;
    if (PgDrawRect(&rect, Pg_DRAW_FILL) == -1) {
        fprintf(stderr, "身体绘制失败\n");
    } else {
        DEBUG_PRINT("身体绘制成功\n");
    }
    
    /* 绘制眼睛（白色圆形） */
    PgSetFillColor(Pg_WHITE);
    
    /* 左眼 */
    rect.ul.x = 140;
    rect.ul.y = 150;
    rect.lr.x = 200;
    rect.lr.y = 210;
    PgDrawRect(&rect, Pg_DRAW_FILL);
    
    /* 右眼 */
    rect.ul.x = 200;
    rect.ul.y = 150;
    rect.lr.x = 260;
    rect.lr.y = 210;
    PgDrawRect(&rect, Pg_DRAW_FILL);
    
    /* 眼球（蓝色圆形） */
    PgSetFillColor(Pg_BLUE);
    
    /* 左眼球 */
    rect.ul.x = 155;
    rect.ul.y = 165;
    rect.lr.x = 185;
    rect.lr.y = 195;
    PgDrawRect(&rect, Pg_DRAW_FILL);
    
    /* 右眼球 */
    rect.ul.x = 215;
    rect.ul.y = 165;
    rect.lr.x = 245;
    rect.lr.y = 195;
    PgDrawRect(&rect, Pg_DRAW_FILL);
    
    /* 鼻子（棕色） */
    PgSetFillColor(0xA52A2A);
    rect.ul.x = 190;
    rect.ul.y = 210;
    rect.lr.x = 210;
    rect.lr.y = 230;
    PgDrawRect(&rect, Pg_DRAW_FILL);
    
    /* 绘制嘴巴 - 根据当前表情状态 */
    DEBUG_PRINT("调用draw_mouth绘制嘴巴，表情状态: %d\n", current_face);
    draw_mouth(current_face);
    
    /* 绘制斑点（棕色小点） */
    PgSetFillColor(0xA52A2A);
    
    rect.ul.x = 125;
    rect.ul.y = 145;
    rect.lr.x = 135;
    rect.lr.y = 155;
    PgDrawRect(&rect, Pg_DRAW_FILL);
    
    rect.ul.x = 265;
    rect.ul.y = 145;
    rect.lr.x = 275;
    rect.lr.y = 155;
    PgDrawRect(&rect, Pg_DRAW_FILL);
    
    rect.ul.x = 145;
    rect.ul.y = 295;
    rect.lr.x = 155;
    rect.lr.y = 305;
    PgDrawRect(&rect, Pg_DRAW_FILL);
    
    rect.ul.x = 245;
    rect.ul.y = 295;
    rect.lr.x = 255;
    rect.lr.y = 305;
    PgDrawRect(&rect, Pg_DRAW_FILL);
    
    rect.ul.x = 195;
    rect.ul.y = 315;
    rect.lr.x = 205;
    rect.lr.y = 325;
    PgDrawRect(&rect, Pg_DRAW_FILL);
    
    /* 绘制睫毛（黑色线条） */
    PgSetStrokeColor(Pg_BLACK);
    
    /* 左眼睫毛 */
    PgDrawILine(150, 150, 160, 140);
    PgDrawILine(170, 150, 170, 140);
    PgDrawILine(190, 150, 180, 140);
    
    /* 右眼睫毛 */
    PgDrawILine(210, 150, 220, 140);
    PgDrawILine(230, 150, 230, 140);
    PgDrawILine(250, 150, 240, 140);
    
    /* 刷新绘图 */
    DEBUG_PRINT("绘图完成，刷新显示...\n");
    PgFlush();
    
    return Pt_CONTINUE;
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
    PtCalcCanvas(widget, NULL);
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
