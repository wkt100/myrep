#ifndef ERROR_H
#define ERROR_H

#define OK (0U)
#define ERROR (-1)
#define ERR_FAILED (1U)
#define ERR_AREA_ID (228U) /* share memory area id error */
/* message queue error */
#define ERR_REC_LOG_MQ (136U)  /* message queue get record log error */
#define ERR_DISP_LOG_MQ (137U) /* message queue get display log error */

/* message queue protocol error */
#define ERR_MAKE_HEAD_MQ (141U)       /* message queue frame head error */
#define ERR_MAKE_TREND_MQ (143U)      /* message queue make trend data error */
#define ERR_PARSE_HEAD_MQ (144U)      /* message queue parse receive head error */
#define ERR_PARSE_APP_MQ (155U)       /* message queue parse receive app data error */
#define ERR_PARSE_PSM_MQ (156U)       /* message queue parse PSM error */
#define ERR_PARSE_PARAM_ACK_MQ (158U) /* message queue parse param ack error */
#define ERR_PARSE_FILE_ACK_MQ (159U)  /* message queue parse file ack error */

#define ERR_PSM_DISPLAY (160U) /* message queue display PSM error */
#define ERR_PSM_RECORD (161U)  /* message queue record PSM error */

#define ERR_OPEN_MQ (2U)    /* message queue open error */
#define ERR_SETATTR_MQ (3U) /* set attributes error */
#define ERR_GETATTR_MQ (4U) /* get attributes error */
#define ERR_CLOSE_MQ (5U)   /* message queue close error */
#define ERR_SEND_MQ (6U)    /* message queue send error */
#define ERR_RECV_MQ (7U)    /* message queue receive error*/
#define ERR_INIT_MQ (8U)    /* message queue init error */

#define ERR_CHECK_TREND_VAR             (10U)

#endif /* ERROR_H */
