#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Ph.h>
#include <Pt.h>

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
            printf("绘制微笑表情\n");
            PgDrawILine(160, 260, 180, 280);
            PgDrawILine(180, 280, 200, 290);
            PgDrawILine(200, 290, 220, 280);
            PgDrawILine(220, 280, 240, 260);
            break;
            
        case CRY:    /* 哭泣表情 - 嘴角下垂，添加眼泪 */
            printf("绘制哭泣表情\n");
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
            
        default:     /* 普通表情 - 直线嘴巴 */
            printf("绘制普通表情\n");
            PgDrawILine(160, 260, 200, 280);
            PgDrawILine(200, 280, 240, 260);
            break;
    }
}

/* 定义海绵宝宝绘图函数 */
int draw_spongebob(PtWidget_t *widget, PhTile_t *damage) {
    /* 使用标准矩形结构 */
    PhRect_t rect;
    PhRect_t canvasRect;
    
    printf("开始绘制海绵宝宝...\n");
    
    /* 获取窗口区域大小 */
    PtCalcCanvas(widget, &canvasRect);
    printf("画布区域: (%d,%d) - (%d,%d)\n", 
           canvasRect.ul.x, canvasRect.ul.y, 
           canvasRect.lr.x, canvasRect.lr.y);
    
    /* 设置绘图上下文 */
    PgSetDrawBufferSize(0);
    PgSetDrawMode(Pg_DRAWMODE_OPAQUE);
    
    /* 清除背景 - 使用整个窗口区域而不是NULL */
    PgSetFillColor(Pg_DGRAY);
    if (PgDrawRect(&canvasRect, Pg_DRAW_FILL) == -1) {
        fprintf(stderr, "背景绘制失败\n");
    } else {
        printf("背景绘制成功\n");
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
        printf("身体绘制成功\n");
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
    printf("绘图完成，刷新显示...\n");
    PgFlush();
    
    return Pt_CONTINUE;
}

/* 微笑按钮回调 */
int smile_cb(PtWidget_t *widget, void *data, PtCallbackInfo_t *cbinfo) {
    printf("微笑按钮被点击\n");
    current_face = SMILE;
    /* 重新绘制海绵宝宝 */
    PtWidget_t *window = PtFindDisjoint(widget);
    PtCalcCanvas(window, NULL);
    draw_spongebob(window, NULL);
    return Pt_CONTINUE;
}

/* 哭泣按钮回调 */
int cry_cb(PtWidget_t *widget, void *data, PtCallbackInfo_t *cbinfo) {
    printf("哭泣按钮被点击\n");
    current_face = CRY;
    /* 重新绘制海绵宝宝 */
    PtWidget_t *window = PtFindDisjoint(widget);
    PtCalcCanvas(window, NULL);
    draw_spongebob(window, NULL);
    return Pt_CONTINUE;
}

/* 普通表情按钮回调 */
int normal_cb(PtWidget_t *widget, void *data, PtCallbackInfo_t *cbinfo) {
    printf("普通表情按钮被点击\n");
    current_face = NORMAL;
    /* 重新绘制海绵宝宝 */
    PtWidget_t *window = PtFindDisjoint(widget);
    PtCalcCanvas(window, NULL);
    draw_spongebob(window, NULL);
    return Pt_CONTINUE;
}

/* 事件处理回调函数 */
int event_handler(PtWidget_t *widget, void *data, PtCallbackInfo_t *cbinfo) {
    PhEvent_t *event = cbinfo->event;
    
    printf("接收到事件: %d\n", event->type);
    
    /* 检查是否为曝光(expose)事件 */
    if (event->type == Ph_EV_EXPOSE) {
        printf("处理Expose事件\n");
        /* 在每次重绘前设置绘图目标 */
        PtCalcCanvas(widget, NULL);
        draw_spongebob(widget, NULL);
    } else if (event->type == Ph_EV_WM) {
        printf("处理窗口管理事件\n");
        PtCalcCanvas(widget, NULL);
        draw_spongebob(widget, NULL);
    }
    
    return Pt_CONTINUE;
}

/* 窗口实现时回调函数 */
int realized_cb(PtWidget_t *widget, void *data, PtCallbackInfo_t *cbinfo) {
    /* 窗口第一次显示时调用绘图函数 */
    printf("窗口已实现回调被触发\n");
    PtCalcCanvas(widget, NULL);
    draw_spongebob(widget, NULL);
    return Pt_CONTINUE;
}

/* 绘图回调函数 - 尝试另一种方法 */
int draw_cb(PtWidget_t *widget, void *data, PtCallbackInfo_t *cbinfo) {
    printf("绘图回调被触发\n");
    PtCalcCanvas(widget, NULL);
    draw_spongebob(widget, NULL);
    return Pt_CONTINUE;
}

/* 应用程序入口 */
int main(int argc, char *argv[]) {
    PtWidget_t *window;
    PtWidget_t *smile_btn;
    PtWidget_t *cry_btn;
    PtWidget_t *normal_btn;
    PhDim_t dim = {400, 600}; /* 增加窗口高度，为按钮留出空间 */
    
    printf("程序开始运行...\n");
    
    /* 初始化Photon应用程序 */
    if (PtInit(NULL) == -1) {
        fprintf(stderr, "Photon初始化失败\n");
        return EXIT_FAILURE;
    }
    printf("Photon初始化成功\n");
    
    /* 创建主窗口 */
    window = PtCreateWidget(PtWindow, NULL, 0, 0);
    if (!window) {
        fprintf(stderr, "无法创建窗口\n");
        return EXIT_FAILURE;
    }
    printf("窗口创建成功\n");
    
    /* 设置窗口属性 */
    PtSetResource(window, Pt_ARG_WINDOW_TITLE, "海绵宝宝表情控制", 0);
    PtSetResource(window, Pt_ARG_DIM, &dim, 0);
    printf("窗口属性设置完成\n");
    
    /* 创建微笑按钮 */
    PhPoint_t btn_pos = {100, 400};
    PhDim_t btn_dim = {80, 30};
    
    smile_btn = PtCreateWidget(PtButton, window, 2, 0);
    PtSetResource(smile_btn, Pt_ARG_POS, &btn_pos, 0);
    PtSetResource(smile_btn, Pt_ARG_DIM, &btn_dim, 0);
    PtSetResource(smile_btn, Pt_ARG_TEXT_STRING, "微笑", 0);
    PtAddCallback(smile_btn, Pt_CB_ACTIVATE, smile_cb, NULL);
    
    /* 创建哭泣按钮 */
    btn_pos.x = 220;
    cry_btn = PtCreateWidget(PtButton, window, 2, 0);
    PtSetResource(cry_btn, Pt_ARG_POS, &btn_pos, 0);
    PtSetResource(cry_btn, Pt_ARG_DIM, &btn_dim, 0);
    PtSetResource(cry_btn, Pt_ARG_TEXT_STRING, "哭泣", 0);
    PtAddCallback(cry_btn, Pt_CB_ACTIVATE, cry_cb, NULL);
    
    /* 创建普通表情按钮 */
    btn_pos.x = 160;
    btn_pos.y = 450;
    normal_btn = PtCreateWidget(PtButton, window, 2, 0);
    PtSetResource(normal_btn, Pt_ARG_POS, &btn_pos, 0);
    PtSetResource(normal_btn, Pt_ARG_DIM, &btn_dim, 0);
    PtSetResource(normal_btn, Pt_ARG_TEXT_STRING, "普通", 0);
    PtAddCallback(normal_btn, Pt_CB_ACTIVATE, normal_cb, NULL);
    
    /* 添加事件处理器 */
    PtAddEventHandler(window, Ph_EV_EXPOSE, event_handler, NULL);
    PtAddEventHandler(window, Ph_EV_WM, event_handler, NULL);
    
    /* 添加回调函数 */
    PtAddCallback(window, Pt_CB_REALIZED, realized_cb, NULL);
    PtAddCallback(window, Pt_CB_ACTIVATE, draw_cb, NULL);
    PtAddCallback(window, Pt_CB_GOT_FOCUS, draw_cb, NULL);
    
    printf("事件处理器和回调设置完成\n");
    
    /* 显示窗口 */
    printf("正在显示窗口...\n");
    PtRealizeWidget(window);
    printf("窗口已显示\n");
    
    /* 强制绘制一次 */
    printf("强制进行初始绘制...\n");
    PtCalcCanvas(window, NULL);
    draw_spongebob(window, NULL);
    
    /* 进入主循环 */
    printf("进入事件循环...\n");
    PtMainLoop();
    
    return EXIT_SUCCESS;
}
