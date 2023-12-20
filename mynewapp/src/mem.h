#ifndef MEM_H
#define MEM_H

#include "typedef.h"

#define AREA_ID_MAX (101U) /* area id max */

/* area id */
#define AREA_GLOBAL (0U)         /* area global */
#define AREA_SEND (3U)           /* area send */
#define AREA_REC (4U)            /* area rec */
#define AREA_PARAM (5U)          /* area param */
#define AREA_DSS (6U)            /* area dss */
#define AREA_DSR (7U)            /* area dsr */
#define AREA_VIDIO (9U)          /* area vidio */
#define AREA_SYSTEM (10U)        /* area system */
#define AREA_TEMP (11U)          /* area temp */
#define AREA_CMDI (13U)          /* area command in */
#define AREA_CMDO (14U)          /* area command out */
#define AREA_CMD (24U)           /* area command */
#define AREA_TIME (25U)          /* area time */
#define AREA_EXTERNALINPUT (26U) /* area external input */
#define AREA_FAULTLOG (27U)      /* area fault log */
#define AREA_OPTLOG (28U)        /* area opt log */
#define AREA_SYSLOG (29U)        /* area sys log */
#define AREA_TRENDDATA (30U)     /* area trend data */
#define AREA_VARFM (31U)         /* area var fm */
#define AREA_GPIOIN (32U)        /* area var GPIO input */
#define AREA_GPIOOUT (33U)       /* area var GPIO output */

/*******************************************************************************
 * share RAM Address for QNX
 *******************************************************************************/
#define SHRAM_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00000000U) /* share ram region offset address */
#define SHRAM_REGION_SIZE (0x00E80000U)                         /* share ram region size 14.75MB */

#define SHRAM_APP_GLOBAL_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00000000U) /* share ram app software global offset address */
#define SHRAM_APP_GLOBAL_REGION_SIZE (0x00A00000U)                         /* share ram app software global size 10MB */

#define SHRAM_APP_SEND_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00A00000U) /* share ram app software send offset address */
#define SHRAM_APP_SEND_REGION_SIZE (0x00040000U)                         /* share ram app software send size 256KB */

#define SHRAM_APP_REC_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00A40000U) /* share ram app software rec offset address */
#define SHRAM_APP_REC_REGION_SIZE (0x00040000U)                         /* share ram app software rec size 256KB */

#define SHRAM_APP_PARAM_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00A80000U) /* share ram app software param offset address */
#define SHRAM_APP_PARAM_REGION_SIZE (0x00040000U)                         /* share ram app software param size 256KB */

#define SHRAM_APP_DSS_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00AC0000U) /* share ram app software dss offset address */
#define SHRAM_APP_DSS_REGION_SIZE (0x00040000U)                         /* share ram app software dss size 256KB */

#define SHRAM_APP_DSR_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00B00000U) /* share ram app software dsr offset address */
#define SHRAM_APP_DSR_REGION_SIZE (0x00040000U)                         /* share ram app software dsr size 256KB */

#define SHRAM_APP_VIDEO_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00B40000U) /* share ram app software video offset address */
#define SHRAM_APP_VIDEO_REGION_SIZE (0x00040000U)                         /* share ram app software video size 256KB */

#define SHRAM_APP_SYSTEM_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00B80000U) /* share ram app software system offset address */
#define SHRAM_APP_SYSTEM_REGION_SIZE (0x00040000U)                         /* share ram app software system size 256KB */

#define SHRAM_APP_TEMP_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00BC0000U) /* share ram app software temp offset address */
#define SHRAM_APP_TEMP_REGION_SIZE (0x00040000U)                         /* share ram app software temp size 256KB */

#define SHRAM_APP_CMDIN_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00C00000U) /* share ram command in offset address */
#define SHRAM_APP_CMDIN_REGION_SIZE (0x00040000U)                         /* share ram command in size 256KB */

#define SHRAM_APP_CMDOUT_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00C40000U) /* share ram command out offset address */
#define SHRAM_APP_CMDOUT_REGION_SIZE (0x00040000U)                         /* share ram command out size 256KB */

#define SHRAM_TIME_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00C80000U) /* share ram time offset address */
#define SHRAM_TIME_REGION_SIZE (0x00040000U)                         /* share ram time size 256KB */

#define SHRAM_EXINPUT_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00CC0000U) /* share ram extenral input offset address */
#define SHRAM_EXINPUT_REGION_SIZE (0x00040000U)                         /* share ram extenral input size 256KB */

#define SHRAM_FLOG_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00D00000U) /* share ram fault log offset address */
#define SHRAM_FLOG_REGION_SIZE (0x00040000U)                         /* share ram fault log size 256KB */

#define SHRAM_OPTLOG_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00D40000U) /* share ram log offset address */
#define SHRAM_OPTLOG_REGION_SIZE (0x00040000U)                         /* share ram log size 256KB */

#define SHRAM_SYSLOG_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00D80000U) /* share ram log offset address */
#define SHRAM_SYSLOG_REGION_SIZE (0x00040000U)                         /* share ram log size 256KB */

#define SHRAM_TREND_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00DC0000U) /* share ram trend data offset address */
#define SHRAM_TREND_REGION_SIZE (0x00040000U)                         /* share ram trend data size 256KB */

#define SHRAM_VARFM_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00E00000U) /* share ram varfm data offset address */
#define SHRAM_VARFM_REGION_SIZE (0x00040000U)                         /* share ram varfm data size 256KB */

#define SHRAM_GPIOIN_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00E40000U) /* share ram GPIO input data offset address */
#define SHRAM_GPIOIN_REGION_SIZE (0x00040000U)                         /* share ram GPIO input data size 256KB */

#define SHRAM_GPIOOUT_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00E80000U) /* share ram GPIO output data offset address */
#define SHRAM_GPIOOUT_REGION_SIZE (0x00040000U)                         /* share ram GPIO output data size 256KB */

/*******************************************************************************
 * share COMMANDIN Address
 *******************************************************************************/
#define SHAME_CMDIN_CYC_COUNTER_OFFSET (32U) /* share ram commandin cyc counter address offset */

/*******************************************************************************
 * private memory Address
 *******************************************************************************/
#define PRIVATE_DEBUGLOG_REGION_OFFSET(addr) ((u32_t)(addr) + 0x00000000U) /* private ram debug log address offset */
#define PRIVATE_DEBUGLOG_REGION_SIZE (0x00040000U)                         /* share ram log size 256KB */

extern u32_t get_area_address(u32_t u32_area_id, u32_t *pu32_addr);

#endif /* MEM_H */
