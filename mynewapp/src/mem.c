#include "mem.h"
#include "error.h"

static char_t glch_shm_buf[16U * 1024U * 1024U] = {0};

extern u32_t get_area_address(u32_t u32_area_id, u32_t *pu32_addr)
{
    u32_t u32_ret = OK; /* function return value */

    if (u32_area_id > AREA_ID_MAX)
    {
        u32_ret = ERR_AREA_ID;
    }

    if ((OK == u32_ret) && (P_NULL == pu32_addr))
    {
        u32_ret = ERR_AREA_ID;
    }

    if (OK == u32_ret)
    {
        /* get area address */
        switch (u32_area_id)
        {
        case AREA_GLOBAL:
            *pu32_addr = SHRAM_APP_GLOBAL_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_SEND:
            *pu32_addr = SHRAM_APP_SEND_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_REC:
            *pu32_addr = SHRAM_APP_REC_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_PARAM:
            *pu32_addr = SHRAM_APP_PARAM_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_DSS:
            *pu32_addr = SHRAM_APP_DSS_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_DSR:
            *pu32_addr = SHRAM_APP_DSR_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_VIDIO:
            *pu32_addr = SHRAM_APP_VIDEO_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_SYSTEM:
            *pu32_addr = SHRAM_APP_SYSTEM_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_TEMP:
            *pu32_addr = SHRAM_APP_TEMP_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_CMDI:
            *pu32_addr = SHRAM_APP_CMDIN_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_CMDO:
            *pu32_addr = SHRAM_APP_CMDOUT_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_TIME:
            *pu32_addr = SHRAM_TIME_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_EXTERNALINPUT:
            *pu32_addr = SHRAM_EXINPUT_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_FAULTLOG:
            *pu32_addr = SHRAM_FLOG_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_OPTLOG:
            *pu32_addr = SHRAM_OPTLOG_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_SYSLOG:
            *pu32_addr = SHRAM_SYSLOG_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_TRENDDATA:
            *pu32_addr = SHRAM_TREND_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_GPIOIN:
            *pu32_addr = SHRAM_GPIOIN_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_GPIOOUT:
            *pu32_addr = SHRAM_GPIOOUT_REGION_OFFSET(glch_shm_buf);
            break;
        case AREA_VARFM:
            *pu32_addr = SHRAM_VARFM_REGION_OFFSET(glch_shm_buf);
            break;
        default:
            *pu32_addr = 0xFFFFFFFFU;
            u32_ret = ERR_AREA_ID;
            break;
        }
    }

    return u32_ret;
}
