#ifndef MQ_H
#define MQ_H

#include <mqueue.h>
#include "typedef.h"
#include "file.h"

/*******************************************************************************
 * Macro Definition
 *******************************************************************************/
/******************************/
/*  mqueue type definition    */
/******************************/
enum MSG_QUEUE_TYPE
{
    CTRL_RUN_DATA,
    DISP_RUN_DATA,
    MSG_QUEUE_TYPE_MAX, /* display message queue types */
};

/* message queue type size definition */
#define PHOTON_MQ_TYPES MSG_QUEUE_TYPE_MAX /* msg queue types */
#define INVALID_MQD (-1)
#define INT_0 (0)
#define UINT_0 (0U)

/* message queue size definition */
#define MSG_QUEUE_MAX_SIZE (10U * 1024U * 1024U + 1024U)
#define MSG_QUEUE_SIZE_8M (8U * 1024U * 1024U)
#define MSG_QUEUE_SIZE_4M (4U * 1024U * 1024U)
#define MSG_QUEUE_SIZE_2M (2U * 1024U * 1024U)
#define MSG_QUEUE_SIZE_1M (1U * 1024U * 1024U)
#define MSG_QUEUE_SIZE_512K (512U * 1024U)
#define MSG_QUEUE_SIZE_256K (256U * 1024U)
#define MSG_QUEUE_SIZE_128K (128U * 1024U)
#define MSG_QUEUE_SIZE_64K (64U * 1024U)
#define MSG_QUEUE_SIZE_32K (32U * 1024U)
#define MSG_QUEUE_SIZE_16K (16U * 1024U)
#define MSG_QUEUE_SIZE_8K (8U * 1024U)
#define MSG_QUEUE_SIZE_1K (1U * 1024U)

#define MSG_QUEUE_1K_MAX_NUM (1000U)
#define MSG_QUEUE_8K_MAX_NUM (500U)
#define MSG_QUEUE_16K_MAX_NUM (400U)
#define MSG_QUEUE_32K_MAX_NUM (100U)
#define MSG_QUEUE_128K_MAX_NUM (50U)
#define MSG_QUEUE_256K_MAX_NUM (20U)
#define MSG_QUEUE_1M_MAX_NUM (100U)
#define MSG_QUEUE_2M_MAX_NUM (100U)
#define MSG_QUEUE_4M_MAX_NUM (16U)
#define MSG_QUEUE_8M_MAX_NUM (10U)
#define MSG_QUEUE_10M_MAX_NUM (10U)

/* message queue definition(name & size)*/
#define MQ_DISPLAY_SEND_NAME "/ctrl_run_data"
#define MQ_DISPLAY_SEND_SIZE MSG_QUEUE_SIZE_1M
#define MQ_DISPLAY_SEND_NUM MSG_QUEUE_1M_MAX_NUM
#define MQ_DISPLAY_RECV_NAME "/disp_run_data"
#define MQ_DISPLAY_RECV_SIZE MSG_QUEUE_SIZE_2M
#define MQ_DISPLAY_RECV_NUM MSG_QUEUE_2M_MAX_NUM

#define MQ_DISPLAY_DATA_ACK_OK (0x00U)         /* Display data variables ack ok */
#define MQ_DISPLAY_DATA_ACK_VAR_OLOW (0x10U)   /* Display data ack variables number overflow */
#define MQ_DISPLAY_DATA_ACK_AREAID_ERR (0x11U) /* Display data ack variable area id error */
#define MQ_DISPLAY_DATA_ACK_TYPE_ERR (0x12U)   /* Display data ack variable type error */
#define MQ_DISPLAY_DATA_ACK_OFFSET_ERR (0x13U) /* Display data ack variable offset error */

#define MQ_FROM_DISPLAY_DATA_REQ (0xA090U) /* message queue from display data request */
#define MQ_TO_DISPLAY_DATA_ACK (0xB190U)   /* message queue send ACK data to display */
#define MQ_DISPLAY_DATA_SIZE_MAX (65536U)  /* app var data size max */

/* message queue handle structure */
typedef struct mq__handle
{
    mqd_t mqd;             /* the message queue descriptor */
    char_t *pch_queue_buf; /* message send/receive string */
    u32_t u32_size;        /* the message queue size */
    u32_t u32_msg_num;     /* message queue max num */
    i32_t i32_receive_len; /* message queue receive length */
    char_t ch_mqname[64U]; /* message queue name */
} mq_handle_t;

/* message queue send&recv buffer structure */
typedef struct photon_mq_buffer
{
    char_t ch_ctrl_run_data[MQ_DISPLAY_SEND_SIZE];
    char_t ch_disp_run_data[MQ_DISPLAY_RECV_SIZE];
} PHOTON_MQ_BUFFER;

typedef struct mq__frame__head
{
    u32_t u32_tick;         /* tick */
    u32_t u32_command;      /* command */
    u32_t u32_data_len;     /* app data length */
    u32_t u32_data_crc;     /* app data segment CRC */
    u8_t u32_res[12U];      /* reserved field */
    u32_t u32_app_head_crc; /* app head CRC (0 ~ 27)bytes */
} mq_frame_head_t;

typedef struct mq__req__ddata
{
    u32_t u32_num;                  /* request var number */
    var_des_t var_buf[64U * 1024U]; /* var buffer */
} mq_req_ddata_t;

typedef struct var__ack__des
{
    var_des_t t_var; /* variable */
    u8_t u8_val[8U]; /* variable value */
} var_ack_des_t;

/* ACK dispaly data segment descriptor structure */
typedef struct mq__ack__ddata
{
    u32_t u32_tick;                         /* tick */
    u32_t u32_data_ack;                     /* display data ack 0x00:OK, 0x10:var number out of range */
    u32_t u32_time;                         /* time_t data */
    u32_t u32_ms;                           /* millisecond */
    u32_t u32_num;                          /* request var number */
    var_ack_des_t var_ack_buf[64U * 1024U]; /* ack var buffer */
} mq_ack_ddata_t;

/* message queue handle buffer */
extern PHOTON_MQ_BUFFER PH_MQ_BUF;                /* display send & receive string buffer */
extern mq_handle_t PH_MQ_HANDLE[PHOTON_MQ_TYPES]; /* display message queu handle buffer */

/*******************************************************************************
 * Function: mqueue_create
 * Identifier: SSD-PDT- (Trace to: PDT-)
 * Description: create message queue
 * Input: pmq_handle---message queue handle
 *        pmq_name---message queue name
 *        pch_buf---message queue send/receive buffer
 *        u32_size---the depth of the message queue (bytes) u32_size > 0 and u32_size <= 10MB
 *        u32_msg_num---message queue num
 * Output: pmq_handle---message queue handle
 * Return: OK---create OK, error code---create message queue error
 * Call: memset(), strcpy(), mq_open(), wlog(), mq_setattr(), mq_getattr()
 * Others: None
 * Log: 2017/10/12
 *******************************************************************************/
extern u32_t mqueue_create(mq_handle_t *pmq_handle /*, char_t *pmq_name, char_t *pch_buf, u32_t u32_size, u32_t u32_msg_num*/);

/*******************************************************************************
 * Function: mqueue_close
 * Identifier: SSD-PDT- (Trace to: PDT-)
 * Description: close message queue
 * Input: pmq_handle---message queue handle
 * Output: None
 * Return: OK---close OK, error code---close message queue error
 * Call: memset(), mq_close(), wlog()
 * Others: None
 * Log: 2017/10/12
 *******************************************************************************/
extern u32_t mqueue_close(mq_handle_t *pmq_handle);

/*******************************************************************************
 * Function: mqueue_send
 * Identifier: SSD-PDT- (Trace to: PDT-)
 * Description: message queue send data
 * Input: pmq_handle---message queue handle
 * Output: None
 * Return: OK---send data OK, error code---send data error
 * Call: memset(), mq_getattr(), wlog(), mq_send()
 * Others: None
 * Log: 2017/10/12
 *******************************************************************************/
extern u32_t mqueue_send(mq_handle_t *pmq_handle);

/*******************************************************************************
 * Function: mqueue_receive
 * Identifier: SSD-PDT- (Trace to: PDT-)
 * Description: message queue receive data
 * Input: pmq_handle---message queue handle
 * Output: pmq_handle->i32_receive_len---receive length
 * Return: OK---receive data OK, error code---receive data error
 * Call: memset(), mq_getattr(), wlog(), mq_receive()
 * Others: None
 * Log: 2017/10/12
 *******************************************************************************/
extern u32_t mqueue_receive(mq_handle_t *pmq_handle);

/*******************************************************************************
 * Function: mqueue_init
 * Identifier: SSD-PDT- (Trace to: PDT-)
 * Description: message queue receive data
 * Input:pmq_handle---message queue handle array poninter
 * Output:None
 * Return: OK---receive data OK, error code---receive data error
 * Call: memset(), mq_getattr(), wlog(), mq_receive()
 * Others: None
 * Log: 2017/10/12
 *******************************************************************************/
extern u32_t mqueue_init(void);

extern u32_t ack_display_data(void);

#endif /* MQ_H */
