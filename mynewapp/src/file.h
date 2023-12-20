#ifndef FILE_H
#define FILE_H

#include "typedef.h"
#include "uthash.h"


#define FILE_NAME_TREND_VAR "/SCID300/Config/download_trend_var.csv"
//#define FILE_NAME_TREND_VAR "/SCID300/Config/16-download_trend_var.csv"
#define PATENT_VAR_SIZE_MAX (200U)          /* 鍙傛暟鏁村畾鍙橀噺 */
#define DOWNLOAD_VAR_SIZE_MAX (24U * 1000U) /* SICD300 var szie max */
#define VAR_NAME_SIZE_MAX (33U)             /* var name max size */
#define VAR_NOTE_SIZE_MAX (65U)             /* var note max size */
#define VAR_UNIT_SIZE_MAX (33U)             /* var unit max size */
#define VAR_MAX_SIZE_MAX (129U)             /* var max data size max */
#define VAR_MINI_SIZE_MAX (129U)            /* var min data size max */
#define VAR_VAL_SIZE_MAX (129U)             /* var value size max */
#define MODULE_WIDGET_MAX_SIZE (500U)       /* widget max size in one module */
#define PT_TREND_VAR_SIZE (8U)              /* PT trend var size */
#define TREND_TYPE_SIZE_MAX (6U)            /* trend type(only "PT") */
#define TREND_VAR_SIZE_MAX (2000U)          /* trend var size max */
#define GROUP_VAR_SIZE_MAX (5U)             /* trend group var max size */
#define TREND_GROUP_SIZE_MAX (1500U)         /* trend group max size */
#define TREND_GROUP_NAME_SIZE_MAX (33U)     /* trend group name size max */
#define TREND_GROUP_DES_SIZE_MAX (65U)      /* trend group description size max */

#define VAR_TYPE_REAL_SIGNAL (1U)   /* real signal */
#define VAR_TYPE_INT_SIGNAL (2U)    /* int signal */
#define VAR_TYPE_REAL (3U)          /* real */
#define VAR_TYPE_BOOL_SIGNAL (4U)   /* bool signal */
#define VAR_TYPE_INT (5U)           /* int */
#define VAR_TYPE_BOOL (6U)          /* bool */
#define VAR_TYPE_DEVICE_SIGNAL (7U) /* device signal */

#define INT_N1                          (-1)
#define INT_0                           (0)
#define UINT_0                          (0U)
#define UINT_1                          (1U)
#define UINT_2                          (2U)
#define UINT_3                          (3U)
#define UINT_4                          (4U)
#define UINT_5                          (5U)
#define UINT_6                          (6U)
#define UINT_7                          (7U)
#define UINT_8                          (8U)
#define UINT_9                          (9U)
#define UINT_10                         (10U)
#define UINT_11                         (11U)
#define UINT_12                         (12U)
#define UINT_13                         (13U)
#define UINT_14                         (14U)
#define UINT_16                         (16U)
#define UINT_19                         (19U)
#define UINT_20                         (20U)
#define UINT_23                         (23U)
#define UINT_24                         (24U)
#define UINT_28                         (28U)
#define UINT_29                         (29U)
#define UINT_30                         (30U)
#define UINT_31                         (31U)
#define UINT_32                         (32U)
#define UINT_33                         (33U)
#define UINT_40                         (40U)
#define UINT_50                         (50U)
#define UINT_52                         (52U)
#define UINT_60                         (60U)
#define UINT_64                         (64U)
#define UINT_65                         (65U)
#define UINT_70                         (70U)
#define UINT_71                         (71U)
#define UINT_75                         (75U)
#define UINT_76                         (76U)
#define UINT_80                         (80U)
#define UINT_90                         (90U)
#define UINT_92                         (92U)
#define UINT_100                        (100U)
#define UINT_124                        (124U)
#define UINT_128                        (128U)
#define UINT_129						(129U)
#define UINT_140                        (140U)
#define UINT_148                        (148U)
#define UINT_150                        (150U)
#define UINT_156                        (156U)
#define UINT_168                        (168U)
#define UINT_190                        (190U)
#define UINT_200                        (200U)
#define UINT_219                        (219U)
#define UINT_220                        (220U)
#define UINT_256                        (256U)
#define UINT_300                        (300U)
#define UINT_400                        (400U)
#define UINT_500                        (500U)
#define UINT_600                        (600U)
#define UINT_800                        (800U)
#define UINT_1000                       (1000U)
#define UINT_1200                       (1200U)
#define UINT_1500                       (1500U)
#define UINT_2000                       (2000U)
#define UINT_3000                       (3000U)
#define UINT_1KB                        (1024U)
#define UINT_8KB                        (8192U)
#define UINT_10000                      (10000U)
#define UINT_32KB                       (32768U)
#define UINT_64KB                       (65536U)
#define UINT_10MB                       (10485760U)
#define UNAVA_COLOR						(0xFFFFFFFF)

#define TREND_GROUP_TITLE_HZ "Group,GroupType,VarSize,Var1,Var2,Var3,Var4,Var5,Var6,Var7,Var8,Var9,Var10,GroupDes"
#define TREND_GROUP_TITLE "Group,VarSize,Var1,Var2,Var3,Var4,Var5,Var6,Var7,Var8,Var9,Var10,GroupDes"
#define TREND_VAR_TITLE "Name,MaxData,MinData,Unit,TrendType,Format,TagName,,,,,,"                 /* qinshiling 20191011 add TagName*/
#define VAR_INFO_TITLE "Name,DataID,Offset,DataType,Note"
#define M2 "m2"
#define M3 "m3"
#define M_SQUARE "m虏"
#define M_CUBE "m鲁"
#define GREATER_HTML "&gt;"
#define LESS_HTML "&lt;"
#define GREATER_SYMBOL ">"
#define LESS_SYMBOL "<"

typedef struct var_hash_tag
{
    char_t a_ch_var_name[VAR_NAME_SIZE_MAX]; // 变量名称
    u32_t u32_var_id;                        // 变量区号
    u32_t u32_var_offset;                    // 变量偏移
    u32_t u32_var_type;                      // 变量类型
    UT_hash_handle hh;                       // hash句柄
} var_hash_t;

typedef struct var_des_tag
{
    u32_t u32_area_id; /* area id */
    u32_t u32_offset;  /* variable address offset */
    u32_t u32_type;    /* variable type (1-7) */
} var_des_t;

typedef struct trend_var_des
{
    char_t ch_var_name[VAR_NAME_SIZE_MAX];     /* trend var var name */
    u32_t u32_var_id;                          /* trend var data id */
    u32_t u32_var_offset;                      /* trend var offset */
    u32_t u32_var_type;                        /* trend var data type */
    f32_t f32_max_data;                        /* trend var max data */
    f32_t f32_mini_data;                       /* trend var mini data */
    char_t ch_unit[VAR_UNIT_SIZE_MAX];         /* trend var unit buffer */
    char_t ch_trend_type[TREND_TYPE_SIZE_MAX]; /* trend type(only "PT") */
    char_t ch_note[VAR_NOTE_SIZE_MAX];         /* 鍙橀噺鎻忚堪 */
    u32_t u32_display_format;                  /* var value display format(0~24) */
    char_t ch_tag_name[VAR_NAME_SIZE_MAX];     /* qinshiling 20191011 add trend var tag name */
} trend_var_des_t;

typedef struct trend_var_file_head_tag
{
    u32_t u32_version_id;     /* file version */
    u32_t u32_trend_var_size; /* trend var size */
    u32_t u32_data_len;       /* file data length */
    u32_t u32_data_crc;       /* file data crc */
    u32_t u32_head_crc;       /* file head crc */
} trend_var_file_head_t;

typedef struct trend_var_info_tag
{
    u32_t u32_trend_data_len;                          /* trend data length( trend and pt ) */
    u32_t u32_trend_var_size;                          /* trend var size */
    trend_var_des_t trend_var_buf[TREND_VAR_SIZE_MAX]; /* trend var descriptor buffer */
} trend_var_info_t;

typedef struct trend_group_des
{
    char_t ch_group_name[TREND_GROUP_NAME_SIZE_MAX];         /* photon trend group name */
    char_t ch_group_des[TREND_GROUP_DES_SIZE_MAX];           /* photon trend group descriptor buffer */
    u32_t u32_var_size;                                      /* group var size(1~5) */
    trend_var_des_t trend_group_var_buf[GROUP_VAR_SIZE_MAX]; /* photon trend group var buffer */
} trend_gtoup_des_t;

typedef struct trend_group_info_tag
{
    u32_t u32_trend_data_len;                                    /* trend data length( trend and pt ) */
    u32_t u32_group_size;                                        /* trend group size */
    trend_gtoup_des_t trend_group_val_buf[TREND_GROUP_SIZE_MAX]; /* trend group descriptor buffer */
} trend_group_info_t;

typedef struct trend_info_tag
{
    trend_var_info_t trend_var_info;     /* trend var information */
    trend_group_info_t trend_group_info; /* photon trend group information */
} trend_info_t;

typedef struct vars_parse_des
{
    char_t ch_name[VAR_NAME_SIZE_MAX]; /* name */
    u32_t u32_data_id;                 /* data_id */
    u32_t u32_offset;                  /* offset */
    u32_t u32_data_type;               /* data_type */
    char_t ch_unit[VAR_UNIT_SIZE_MAX]; /* var unit */
    char_t ch_note[VAR_NOTE_SIZE_MAX]; /* var note */
} vars_parse_des_t;

typedef struct vars_info_tag
{
    u32_t u32_vars_parse_lines;                             /* vars parse lines */
    vars_parse_des_t vars_parse_buf[DOWNLOAD_VAR_SIZE_MAX]; /* vars parse buffer */
} vars_info_t;

extern firm_bool_t file_read(u32_t u32_read_buf, u32_t u32_size, char_t *p_ch_file_name);
extern u32_t parse_download_var_csv(trend_var_file_head_t *ptrend_var_file_head, trend_info_t *ptrend_info, vars_info_t *pvars_info, char_t *pch_recv_array);

extern u32_t force_var(var_hash_t *s_p_st_var_hash, char_t *p_ch_var_name, char_t *p_ch_var_val, char_t *p_ch_var_sig);
#endif /* FILE_H */
