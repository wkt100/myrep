#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <memory.h>
#include "mq.h"
#include "error.h"
#include "mem.h"

PHOTON_MQ_BUFFER PH_MQ_BUF = {{0}}; // Display send & recv string buffer
mq_handle_t PH_MQ_HANDLE[PHOTON_MQ_TYPES] = {/* display message queue handle buffer */
    {INVALID_MQD, PH_MQ_BUF.ch_ctrl_run_data, MQ_DISPLAY_SEND_SIZE, MQ_DISPLAY_SEND_NUM, -1, MQ_DISPLAY_SEND_NAME},
    {INVALID_MQD, PH_MQ_BUF.ch_disp_run_data, MQ_DISPLAY_RECV_SIZE, MQ_DISPLAY_RECV_NUM, -1, MQ_DISPLAY_RECV_NAME}};

static u32_t s_u32_ack_tick = 0U;

static firm_bool_t get_timet(u32_t *pu32_time, u32_t *pu32_ms);
static u32_t mq_make_app_display_data(char_t *pch_app_data_buf, char_t *pch_recv_data_buf, u32_t *pu32_app_size);
static u32_t mq_make_display_data(char_t *pch_send_buf, char_t *pch_recv_data_buf, u32_t u32_tick);
static u32_t mq_parse_frame_display_data(char_t *pch_src_buf, u32_t *pu32_msg_cmd, u32_t *pu32_var_num);
static u32_t mq_make_head(char_t *pch_send_buf, u32_t u32_app_data_size, u32_t u32_tick, u32_t u32_frame_type);
static u32_t mq_parse_display_data_req(char_t *pch_recv_buf, u32_t *pu32_var_num);
static u32_t mq_parse_head(char_t *pch_recv_buf, u32_t *pu32_msg_cmd);

/*******************************************************************************
 * Function: mqueue_create
 * Identifier: SSD-PDT-031 (Trace to: PDT-036¡¢PDT-037)
 * Description: create message queue
 * Input: pmq_handle---message queue handle
 * Output: pmq_handle---message queue handle
 * Return: OK---create OK, error code---create message queue error
 * Call: memset(), strcpy(), mq_open(), wlog(), mq_setattr(), mq_getattr()
 * Others: None
 * Log: 2017/10/12
 *******************************************************************************/
u32_t mqueue_create(mq_handle_t *pmq_handle)
{
    u32_t u32_err = OK;      /* function return value */
    i32_t i32_err_ret = 0;   /* function return value */
    struct mq_attr attr;     /* message queue attribute */
    struct mq_attr old_attr; /* save message queue attribute */
    struct mq_attr setattr;  /* message queue attribute */
                             // extern int errno;                               /* error number to be explained */

    if (P_NULL == pmq_handle)
    {
        u32_err = ERR_FAILED;
    }

    if (OK == u32_err)
    {
        /* init vars */
        //    	memset( pmq_handle, INT_0, sizeof( mq_handle_t ) );
        //    	strcpy( pmq_handle->ch_mqname, pmq_name );
        //    	pmq_handle->pch_queue_buf = pch_buf;
        //    	pmq_handle->u32_size = u32_size;
        //    	pmq_handle->u32_msg_num = u32_msg_num;
        memset(&setattr, INT_0, sizeof(setattr));
        setattr.mq_maxmsg = pmq_handle->u32_msg_num;
        setattr.mq_msgsize = (long)pmq_handle->u32_size;
        setattr.mq_flags = O_NONBLOCK;
        memset(&old_attr, 	INT_0, sizeof(old_attr));
        memset(&attr, 		INT_0, sizeof(attr));

        /* open message queue*/
        pmq_handle->mqd = mq_open(pmq_handle->ch_mqname, O_RDWR | O_NONBLOCK | O_CREAT | O_EXCL, 0644, &setattr);
        if ((pmq_handle->mqd < INT_0) && (errno != EEXIST))
        {
            u32_err = ERR_OPEN_MQ;
        }
    }

    if (OK == u32_err)
    {
        if ((pmq_handle->mqd < INT_0) && (errno == EEXIST)) // ��Ϣ�����Ѿ�����
        {
            pmq_handle->mqd = mq_open(pmq_handle->ch_mqname, O_RDWR | O_NONBLOCK); // ���Ѵ��ڵ���Ϣ����
            if (pmq_handle->mqd < INT_0)
            {
                u32_err = ERR_OPEN_MQ;
            }
        }
    }

    /* set attributes */
    if (OK == u32_err)
    {
        i32_err_ret = mq_setattr(pmq_handle->mqd, &setattr, &old_attr);
        if (i32_err_ret < INT_0)
        {
            u32_err = ERR_SETATTR_MQ;
        }
    }

    /* get attributes */
    if (OK == u32_err)
    {
        i32_err_ret = mq_getattr(pmq_handle->mqd, &attr);
        if (i32_err_ret < INT_0)
        {
            u32_err = ERR_GETATTR_MQ;
        }
    }

    return u32_err;
}

/*******************************************************************************
 * Function: mqueue_close
 * Identifier: COD-1GPU-NM- (Trace to: SPD-1GPU-NM-)
 * Description: close message queue
 * Input: pmq_handle---message queue handle
 * Output: None
 * Return: OK---close OK, error code---close message queue error
 * Call: memset(), mq_close(), wlog()
 * Others: None
 * Log: 2017/10/12
 *******************************************************************************/
u32_t mqueue_close(mq_handle_t *pmq_handle)
{
    u32_t u32_err = OK;        /* function return value */
    i32_t i32_err_ret = INT_0; /* function return value */

    if (P_NULL == pmq_handle)
    {
        u32_err = ERR_FAILED;
    }

    if (OK == u32_err)
    {
        i32_err_ret = mq_close(pmq_handle->mqd);
        if (i32_err_ret < INT_0)
        {
            u32_err = ERR_CLOSE_MQ;
        }
    }

    if (OK == u32_err)
    {
        memset(pmq_handle, INT_0, sizeof(mq_handle_t));
    }

    return u32_err;
}

/*******************************************************************************
 * Function: mqueue_send
 * Identifier: COD-1GPU-NM- (Trace to: SPD-1GPU-NM-)
 * Description: message queue send data
 * Input: pmq_handle---message queue handle
 * Output: None
 * Return: OK---send data OK, error code---send data error
 * Call: memset(), mq_getattr(), wlog(), mq_send()
 * Others: None
 * Log: 2017/10/12
 *******************************************************************************/
u32_t mqueue_send(mq_handle_t *pmq_handle)
{
    u32_t u32_err = OK;        /* function return value */
    i32_t i32_err_ret = INT_0; /* function return value */
    u32_t u32_prio = UINT_0;   /* mq prio max */
    struct mq_attr attr;       /* message queue attributes*/
                               //    struct mq_attr old_attr;                        /* save message queue attributes */

    if (P_NULL == pmq_handle)
    {
        u32_err = ERR_FAILED;
    }

    if (OK == u32_err)
    {
        /* init vars */
        memset(&attr, INT_0, sizeof(attr));

        i32_err_ret = mq_getattr(pmq_handle->mqd, &attr);
        if (i32_err_ret < INT_0)
        {
            u32_err = ERR_GETATTR_MQ;
        }
    }

    if (OK == u32_err)
    {
        /*
//    	if(attr.mq_curmsgs == pmq_handle->u32_msg_num)
        {
            attr.mq_flags = O_NONBLOCK;
            mq_setattr(pmq_handle->mqd, &attr, &old_attr);
        }
        */

        i32_err_ret = mq_send(pmq_handle->mqd, pmq_handle->pch_queue_buf, pmq_handle->u32_size, u32_prio);

        //    	attr = old_attr;
        //    	mq_setattr(pmq_handle->mqd, &attr, &old_attr);
        if (i32_err_ret < INT_0)
        {
            u32_err = ERR_SEND_MQ;
        }
    }

    return u32_err;
}

/*******************************************************************************
 * Function: mqueue_receive
 * Identifier: COD-1GPU-NM- (Trace to: SPD-1GPU-NM-)
 * Description: message queue receive data
 * Input: pmq_handle---message queue handle
 * Output: pmq_handle->i32_receive_len---receive length
 * Return: OK---receive data OK, error code---receive data error
 * Call: memset(), mq_getattr(), wlog(), mq_receive()
 * Others: None
 * Log: 2017/10/12
 *******************************************************************************/
u32_t mqueue_receive(mq_handle_t *pmq_handle)
{
    u32_t u32_err = OK;        /* function return value */
    i32_t i32_err_ret = INT_0; /* function return value */
    u32_t u32_prio = UINT_0;   /* mq prio max */
    struct mq_attr attr;       /* mesage queue attributes */
                               //   struct mq_attr old_attr;                        /* save message queue attributes */

    if (P_NULL == pmq_handle)
    {
        u32_err = ERR_FAILED;
    }

    if (OK == u32_err)
    {
        i32_err_ret = mq_getattr(pmq_handle->mqd, &attr);
        if (i32_err_ret < INT_0)
        {
            u32_err = ERR_GETATTR_MQ;
        }
    }

    if (OK == u32_err)
    {
        /*
//    	if(attr.mq_curmsgs == UINT_0)
        {
            attr.mq_flags = O_NONBLOCK;
            mq_setattr(pmq_handle->mqd, &attr, &old_attr);
        }
        */

        pmq_handle->i32_receive_len = mq_receive(pmq_handle->mqd, pmq_handle->pch_queue_buf, pmq_handle->u32_size, &u32_prio);
        //    	attr = old_attr;
        //    	mq_setattr(pmq_handle->mqd, &attr, &old_attr);

        if (pmq_handle->i32_receive_len < INT_0)
        {
            u32_err = ERR_RECV_MQ;
        }
    }
    return u32_err;
}

/*******************************************************************************
 * Function: mqueue_init
 * Identifier: COD-1GPU-NM- (Trace to: SPD-1GPU-NM-)
 * Description: message queue receive data
 * Input:pmq_handle---message queue handle array poninter
 * Output:None
 * Return: OK---receive data OK, error code---receive data error
 * Call: memset(), mq_getattr(), wlog(), mq_receive()
 * Others: None
 * Log: 2017/10/12
 *******************************************************************************/
u32_t mqueue_init(void)
{
    u32_t u32_err = OK;
    u32_t u32_mq_type_index = UINT_0;

    if (OK == u32_err)
    {
        for (u32_mq_type_index = UINT_0; u32_mq_type_index < PHOTON_MQ_TYPES; u32_mq_type_index++)
        {
            if (OK == u32_err)
            {
                u32_err = mqueue_create(&PH_MQ_HANDLE[u32_mq_type_index]);
            }
            else
            {
                u32_err = ERR_INIT_MQ;
                break;
            }
        }
    }

    return u32_err;
}

u32_t ack_display_data(void)
{
    u32_t u32_err = 0U;         // 错误码
    i32_t i32_ret = INT_0;      // 函数返回值
    u32_t u32_ret = 0U;         // 函数返回值
    u32_t u32_i = 0U;           // 循环计数
    u32_t u32_msg_cmd = UINT_0; /* message command */
    u32_t u32_var_num = UINT_0; /* display data var number */

    if (OK == u32_err)
    {
        i32_ret = mqueue_receive(&PH_MQ_HANDLE[CTRL_RUN_DATA]);//delay(10000);
        if (OK == i32_ret)	// change >UINT_0 to OK==, ken
        {
            u32_err = mq_parse_frame_display_data(PH_MQ_HANDLE[CTRL_RUN_DATA].pch_queue_buf, &u32_msg_cmd, &u32_var_num);
        }
    }

    if ((OK == u32_err) && (MQ_FROM_DISPLAY_DATA_REQ == u32_msg_cmd) && (u32_var_num < MQ_DISPLAY_DATA_SIZE_MAX))
    {
        /* send display data ack */
        u32_err = mq_make_display_data(PH_MQ_HANDLE[DISP_RUN_DATA].pch_queue_buf, PH_MQ_HANDLE[CTRL_RUN_DATA].pch_queue_buf, s_u32_ack_tick);
        if (OK == u32_err)
        {
            /* send message queue ack to Display */
            u32_ret = mqueue_send(&PH_MQ_HANDLE[DISP_RUN_DATA]);
        }
    }

    return u32_err;
}

static u32_t mq_parse_frame_display_data(char_t *pch_src_buf, u32_t *pu32_msg_cmd, u32_t *pu32_var_num)
{
    u32_t u32_err = OK; /* function return value */

    if ((P_NULL == pch_src_buf) || (P_NULL == pu32_msg_cmd) || (P_NULL == pu32_var_num))
    {
        u32_err = ERR_FAILED;
    }

    if (OK == u32_err)
    {
        u32_err = mq_parse_head(pch_src_buf, pu32_msg_cmd);
    }

    if (OK == u32_err)
    {
        u32_err = mq_parse_display_data_req(pch_src_buf, pu32_var_num);
    }

    return u32_err;
}

static u32_t mq_make_display_data(char_t *pch_send_buf, char_t *pch_recv_data_buf, u32_t u32_tick)
{
    u32_t u32_err = OK;                   /* function return value */
    u32_t u32_app_data_seg_addr = UINT_0; /* app data segment address */
    u32_t u32_app_data_size = UINT_0;     /* app data bytes size */

    if ((P_NULL == pch_send_buf) || (P_NULL == pch_recv_data_buf))
    {
        u32_err = ERR_FAILED;
    }

    /* make frame app data */
    if (OK == u32_err)
    {
        u32_app_data_seg_addr = (u32_t)pch_send_buf + sizeof(mq_frame_head_t);
        u32_err = mq_make_app_display_data((char_t *)u32_app_data_seg_addr, pch_recv_data_buf, &u32_app_data_size);
    }

    /* make frame head */
    if (OK == u32_err)
    {
        u32_err = mq_make_head(pch_send_buf, u32_app_data_size, u32_tick, MQ_TO_DISPLAY_DATA_ACK);
    }

    return u32_err;
}

static u32_t mq_make_app_display_data(char_t *pch_app_data_buf, char_t *pch_recv_data_buf, u32_t *pu32_app_size)
{
    u32_t u32_err = OK;                   /* function return value */
    firm_bool_t b_err = FAILURE;          /* function return value */
    u32_t *pu32_ret = P_NULL;             /* function return value */
    mq_req_ddata_t *pt_req_data = P_NULL; /* request display data pointer */
    mq_ack_ddata_t *pt_ack_data = P_NULL; /* ACK display data pointer */
    u32_t u32_index = UINT_0;             /* var index */
    u32_t u32_area_addr = UINT_0;         /* area address */
    mq_frame_head_t *pt_mfh = P_NULL;     /* app head pointer */

    if ((P_NULL == pch_app_data_buf) || (P_NULL == pch_recv_data_buf))
    {
        u32_err = ERR_FAILED;
    }

    if (OK == u32_err)
    {
        pt_mfh = (mq_frame_head_t *)pch_recv_data_buf;
        pt_req_data = (mq_req_ddata_t *)((u32_t)pch_recv_data_buf + sizeof(mq_frame_head_t));
        pt_ack_data = (mq_ack_ddata_t *)pch_app_data_buf;
        pt_ack_data->u32_data_ack = MQ_DISPLAY_DATA_ACK_OK;
        if (pt_req_data->u32_num > 64U * 1024U)
        {
            pt_ack_data->u32_data_ack = MQ_DISPLAY_DATA_ACK_VAR_OLOW;
            pt_ack_data->u32_num = UINT_0;
        }
    }

    if (OK == u32_err)
    {
        b_err = get_timet(&pt_ack_data->u32_time, &pt_ack_data->u32_ms);
        if (FAILURE == b_err)
        {
            u32_err = MQ_DISPLAY_DATA_ACK_TYPE_ERR;
        }
    }

    if ((OK == u32_err) && (MQ_DISPLAY_DATA_ACK_OK == pt_ack_data->u32_data_ack))
    {
        /* make ack display data packet */
        pt_ack_data->u32_tick = pt_mfh->u32_tick; /* ACK display data frame same reuquest display data */
        pt_ack_data->u32_num = pt_req_data->u32_num;

        /* tick(4bytes) + data ack(4bytes) + var num(4bytes) + var info(20bytes * var num) */
        *pu32_app_size = 12U + sizeof(var_ack_des_t) * pt_ack_data->u32_num;
        for (u32_index = UINT_0; u32_index < pt_req_data->u32_num; u32_index++)
        {
            pt_ack_data->u32_data_ack = get_area_address(pt_req_data->var_buf[u32_index].u32_area_id, &u32_area_addr);
            if (MQ_DISPLAY_DATA_ACK_OK == pt_ack_data->u32_data_ack)
            {
                pt_ack_data->var_ack_buf[u32_index].t_var = pt_req_data->var_buf[u32_index];
                switch (pt_req_data->var_buf[u32_index].u32_type)
                {
                case VAR_TYPE_REAL_SIGNAL:
                    pu32_ret = (u32_t *)memcpy((u8_t *)pt_ack_data->var_ack_buf[u32_index].u8_val,
                                               (u8_t *)(u32_area_addr + pt_req_data->var_buf[u32_index].u32_offset),
                                               sizeof(real_signal_t));
                    if (P_NULL == pu32_ret)
                    {
                        u32_err = MQ_DISPLAY_DATA_ACK_TYPE_ERR;
                    }
                    break;
                case VAR_TYPE_INT_SIGNAL:
                    pu32_ret = (u32_t *)memcpy((u8_t *)pt_ack_data->var_ack_buf[u32_index].u8_val,
                                               (u8_t *)(u32_area_addr + pt_req_data->var_buf[u32_index].u32_offset),
                                               sizeof(int_signal_t));
                    if (P_NULL == pu32_ret)
                    {
                        u32_err = MQ_DISPLAY_DATA_ACK_TYPE_ERR;
                    }
                    break;
                case VAR_TYPE_REAL:
                    pu32_ret = (u32_t *)memcpy((u8_t *)pt_ack_data->var_ack_buf[u32_index].u8_val,
                                               (u8_t *)(u32_area_addr + pt_req_data->var_buf[u32_index].u32_offset),
                                               sizeof(real_net_t));
                    if (P_NULL == pu32_ret)
                    {
                        u32_err = MQ_DISPLAY_DATA_ACK_TYPE_ERR;
                    }
                    break;
                case VAR_TYPE_BOOL_SIGNAL:
                    pu32_ret = (u32_t *)memcpy((u8_t *)pt_ack_data->var_ack_buf[u32_index].u8_val,
                                               (u8_t *)(u32_area_addr + pt_req_data->var_buf[u32_index].u32_offset),
                                               sizeof(bool_signal_t));
                    if (P_NULL == pu32_ret)
                    {
                        u32_err = MQ_DISPLAY_DATA_ACK_TYPE_ERR;
                    }
                    break;
                case VAR_TYPE_INT:
                    pu32_ret = (u32_t *)memcpy((u8_t *)pt_ack_data->var_ack_buf[u32_index].u8_val,
                                               (u8_t *)(u32_area_addr + pt_req_data->var_buf[u32_index].u32_offset),
                                               sizeof(int_net_t));
                    if (P_NULL == pu32_ret)
                    {
                        u32_err = MQ_DISPLAY_DATA_ACK_TYPE_ERR;
                    }
                    break;
                case VAR_TYPE_BOOL:
                    pu32_ret = (u32_t *)memcpy((u8_t *)pt_ack_data->var_ack_buf[u32_index].u8_val,
                                               (u8_t *)(u32_area_addr + pt_req_data->var_buf[u32_index].u32_offset),
                                               sizeof(bool_net_t));
                    if (P_NULL == pu32_ret)
                    {
                        u32_err = MQ_DISPLAY_DATA_ACK_TYPE_ERR;
                    }
                    break;
                case VAR_TYPE_DEVICE_SIGNAL:
                    pu32_ret = (u32_t *)memcpy((u8_t *)pt_ack_data->var_ack_buf[u32_index].u8_val,
                                               (u8_t *)(u32_area_addr + pt_req_data->var_buf[u32_index].u32_offset),
                                               sizeof(dev_signal_t));
                    if (P_NULL == pu32_ret)
                    {
                        u32_err = MQ_DISPLAY_DATA_ACK_TYPE_ERR;
                    }
                    break;
                default:
                    pt_ack_data->u32_data_ack = MQ_DISPLAY_DATA_ACK_TYPE_ERR;
                    break;
                }
            }
        }
    }

    return u32_err;
}

static firm_bool_t get_timet(u32_t *pu32_time, u32_t *pu32_ms)
{
    firm_bool_t b_err = SUCCESS; /* function status */
    i32_t i32_ret = INT_0;       /* function return value */
    struct timeval timeval_now;  /* get time of day */
    time_t lcurtime = 0U;        /* current system time */
    i32_t i32_usec = INT_0;      /* tv_usec */

    if ((P_NULL == pu32_time) || (P_NULL == pu32_ms))
    {
        b_err = FAILURE;
    }

    if (SUCCESS == b_err)
    {
        i32_ret = gettimeofday(&timeval_now, P_NULL);
        if (ERROR != i32_ret)
        {
            lcurtime = timeval_now.tv_sec; /* determine the current calendar time */
            *pu32_time = (u32_t)lcurtime;
            i32_usec = (timeval_now.tv_usec / 1000);
            *pu32_ms = (u32_t)i32_usec;
        }
    }

    return b_err;
}

static u32_t mq_make_head(char_t *pch_send_buf, u32_t u32_app_data_size, u32_t u32_tick, u32_t u32_frame_type)
{
    u32_t u32_err = OK;                             /* function return value */
    u32_t *pu32_ret = P_NULL;                       /* function return value */
    firm_bool_t b_err = FAILURE;                    /* function return value */
    mq_frame_head_t *pt_mfh = P_NULL;               /* app head pointer */
    u32_t u32_app_data_seg_addr = UINT_0;           /* app data segment address */

    if ((P_NULL == pch_send_buf) || (u32_app_data_size > UINT_10MB))
    {
        u32_err = ERR_FAILED;
    }

    if (OK == u32_err)
    {
        /* make frame head */
        pt_mfh = (mq_frame_head_t *)pch_send_buf;
        pu32_ret = memset(pt_mfh, INT_0, sizeof(mq_frame_head_t));
        if (P_NULL == pu32_ret)
        {
            u32_err = ERR_MAKE_HEAD_MQ;
        }
    }

    if (OK == u32_err)
    {
        pt_mfh->u32_tick = u32_tick;
        pt_mfh->u32_command = u32_frame_type;
        pt_mfh->u32_data_len = u32_app_data_size;
        u32_app_data_seg_addr = (u32_t)pch_send_buf + sizeof(mq_frame_head_t);

        /* calc app data segment CRC */
        b_err = crc32_calculate((byte_t *)u32_app_data_seg_addr, u32_app_data_size, &pt_mfh->u32_data_crc);
        if (FAILURE == b_err)
        {
            u32_err = ERR_MAKE_HEAD_MQ;
        }
    }

    if (OK == u32_err)
    {
        /* calc frame head CRC */
        b_err = crc32_calculate((byte_t *)pch_send_buf, (sizeof(mq_frame_head_t) - UINT_4), &pt_mfh->u32_app_head_crc);
        if (FAILURE == b_err)
        {
            u32_err = ERR_MAKE_HEAD_MQ;
        }
    }

    return u32_err;
}

static u32_t mq_parse_display_data_req(char_t *pch_recv_buf, u32_t *pu32_var_num)
{
    u32_t u32_err = OK;                             /* function return value */
    mq_req_ddata_t *pt_req_ddata = P_NULL;          /* receive frame request display data pointer */
    u32_t u32_app_data_seg_addr = UINT_0;           /* app data segment address */

    if ((P_NULL == pch_recv_buf) || (P_NULL == pu32_var_num))
    {
        u32_err = ERR_FAILED;
    }

    if (OK == u32_err)
    {
        /* parse frame app data */
        u32_app_data_seg_addr = (u32_t)pch_recv_buf + sizeof(mq_frame_head_t);
        pt_req_ddata = (mq_req_ddata_t *)u32_app_data_seg_addr;
        *pu32_var_num = pt_req_ddata->u32_num;
    }

    return u32_err;
}

static u32_t mq_parse_head(char_t *pch_recv_buf, u32_t *pu32_msg_cmd)
{
    u32_t u32_err = OK;                             /* function return value */
    mq_frame_head_t *pt_mfh = P_NULL;               /* app head pointer */
    firm_bool_t b_ret = FAILURE;                    /* function return value */
    u32_t u32_app_data_seg_addr = UINT_0;           /* app data segment address */
    u32_t u32_len = UINT_0;                         /* head length */

    if ((P_NULL == pch_recv_buf) || (P_NULL == pu32_msg_cmd))
    {
        u32_err = ERR_FAILED;
    }

    if (OK == u32_err)
    {
        /* check head segment CRC */
        u32_len = sizeof(mq_frame_head_t) - UINT_4;
        pt_mfh = (mq_frame_head_t *)pch_recv_buf;
        b_ret = crc32_check((byte_t *)pch_recv_buf, u32_len, pt_mfh->u32_app_head_crc);
        if (FAILURE == b_ret)
        {
            u32_err = ERR_PARSE_HEAD_MQ;
        }
    }

    if (OK == u32_err)
    {
        u32_app_data_seg_addr = (u32_t)pch_recv_buf + sizeof(mq_frame_head_t);

        /* check app data segment CRC */
        b_ret = crc32_check((byte_t *)u32_app_data_seg_addr, pt_mfh->u32_data_len, pt_mfh->u32_data_crc);
        if (FAILURE == b_ret)
        {
            u32_err = ERR_PARSE_HEAD_MQ;
        }
    }

    if (OK == u32_err)
    {
        *pu32_msg_cmd = pt_mfh->u32_command;
    }

    return u32_err;
}
