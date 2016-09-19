#ifndef _PANEL_S_APPLICATION_H_
#define _PANEL_S_APPLICATION_H_

/* Includes ------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

#define SYS_APP_PROGRAM_MAX         0X2000
#define SCENE_DATA_OFFSET     0x51
#define GROUP_DATA_OFFSET     0x3f
#define DEVICE_CONF_START    0x56
#define CURTAIN_ADDR_START1     0x8e
#define CURTAIN_ADDR_START2     0xb2
#define CURTAIN_TYPE_START      0x9e
#define AIR_CONDITIONER_1_ENABLE_START   0x8b
#define AIR_CONDITIONER_2_ENABLE_START   0x8c
#define AIR_CONDITIONER_3_ENABLE_START   0x8d
#define DEVICE_NAME_START       0x1d7b

#define SCENE_ENABLE_BASEADDR     0x00
#define AIR_CONDITIONER_1_ADDR_BASEADDR   0xbc
#define AIR_CONDITIONER_2_ADDR_BASEADDR   0xbd
#define AIR_CONDITIONER_3_ADDR_BASEADDR   0x8a
#define PANNO_X_CONF_START    0xa8
#define PANNO_X_SCENE_BASEADDR    0x03
#define PANNO_X_FUNC_BASEADDR    0x2c

#define SCENE_BLOCK_START      0x100
#define GROUP_BLOCK_1_START  0xa80
#define GROUP_BLOCK_2_START  0x1460

//-------------------------------------------start----------------------------------------------////
//change log by zlt 2016.05.23 for latest svn 751.
//adding threads status watching, avoid threads dead locking.
//Adding a watching thread to watch the status, each thread will change the status in 30s, if the status are not change in 30s, 
//the watch thread will restart the application and report the log to server. 
typedef struct
{
    unsigned int gui_watching;
    unsigned int enocean_watching;
    unsigned int wifi_recv_watching;
    unsigned int wifi_send_watching;
    unsigned int key_watching;
    unsigned int taskpoll_watching;
    unsigned int sensors_watching;
    unsigned int delay_fuc_watching;
    unsigned int blight_watching;
    unsigned int dry_curtain1_watching;
    unsigned int dry_curtain2_watching;
    unsigned int dim_channel1_watching;
    unsigned int dim_channel2_watching;
    unsigned int dim_channel3_watching;
    unsigned int dim_channel4_watching;
    unsigned int dim_channel5_watching;
    unsigned int dim_channel6_watching;
    unsigned int dim_channel7_watching;
    unsigned int dim_channel8_watching;
    unsigned int enocean_uart_watching;
    unsigned int knx_L2_recv_watching;
    unsigned int knx_L2_send_watching;
    unsigned int knx_L7_watching;
    unsigned int knx_app_watching;
    unsigned int knx_ail_watching;
    unsigned int knx5120_uart_watching;
    unsigned int rs485_knx_map_watching;
    unsigned int wifi_uart_watching;
}THREAD_WATCHING_CONF;

typedef enum
{
    T_GUI = 0,
    T_ENOCEAN,
    T_WIFI_RECV,
    T_WIFI_SEND,
    T_KEY,
    T_TASKPOLL,
    T_SENSORS,
    T_DELAY_FUN,
    T_BLIGHT,
    T_DRY_CURTAIN1,
    T_DRY_CURTAIN2,
    T_DIM1,
    T_DIM2,
    T_DIM3,
    T_DIM4,
    T_DIM5,
    T_DIM6,
    T_DIM7,
    T_DIM8,
    T_ENOCEAN_UART,
    T_KNX_L2_RECV,
    T_KNX_L2_SEND,
    T_KNX_L7,
    T_KNX_APP,
    T_KNX_AIL,
    T_KNX_UART,
    T_RS485_KNX_MAP,
    T_WIFI_UART,
    T_END
}THREAD_ID_CONF;

THREAD_WATCHING_CONF thread_status;
//---------------------------------------------end----------------------------------------------////

//-------------------------------------------start----------------------------------------------////
//change log by zlt 2016.06.1 for latest svn 768.
//adding init error, if an error occured in KNX init, the init_flag will not change. there is a dead loop.
//the changed code avoid dead loop occure, when the error occures the system will restart. 
typedef enum
{
    E_INIT = 1,
    E_THREAD_CREAT,
    E_END
}ERROR_ID_CONF;
//---------------------------------------------end----------------------------------------------////

extern pthread_mutex_t configuration_mutex;
extern pthread_mutex_t obj_update_mutex;
extern unsigned char sys_app_program_cfg[SYS_APP_PROGRAM_MAX];
extern int temperature_int;
extern int temperature_dec;
extern int humidity_int;
extern int humidity_dec;
extern int sensor_brightness;
extern char device_id[30];
extern unsigned char init_flag;

extern int save_newconfig(void);
extern int save_device_id(void);
extern int load_scene_name(void);
extern int save_scene_name(void);
extern int panel_s_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _PANEL_S_APPLICATION_H_ */
/* --------------------------------- End Of File ------------------------------ */

