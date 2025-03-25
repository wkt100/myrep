
/* WinBase.c */
i32_t WinBaseSetup ( PtWidget_t *p_st_link_instance , ApInfo_t *p_st_apinfo , PtCallbackInfo_t *p_st_cbinfo );
i32_t BtnForceActivate ( PtWidget_t *p_st_widget , ApInfo_t *p_st_apinfo , PtCallbackInfo_t *P_st_cbinfo );
i32_t TimerRefreshActivate ( PtWidget_t *p_st_widget , ApInfo_t *p_st_apinfo , PtCallbackInfo_t *p_st_cbinfo );

/* abmain.c */

/* crc32.c */
firm_bool_t crc32_calculate ( byte_t a_u8_crc_cal_data [], u32_t u32_crc_cal_len , u32_t *p_u32_crc_cal_sum );
firm_bool_t crc32_check ( byte_t a_u8_chk_data [], u32_t u32_chk_len , u32_t u32_chk_sum );
firm_bool_t crc32_append ( byte_t a_u8_apd_data [], u32_t u32_apd_len );

/* file.c */

/* mem.c */

/* mq.c */
u32_t mqueue_create ( mq_handle_t *pmq_handle );
u32_t mqueue_close ( mq_handle_t *pmq_handle );
u32_t mqueue_send ( mq_handle_t *pmq_handle );
u32_t mqueue_receive ( mq_handle_t *pmq_handle );
u32_t mqueue_init ( void );
u32_t ack_display_data ( void );
