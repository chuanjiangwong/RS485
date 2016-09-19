#ifndef _PANEL_S_DEV_CONTROL_H_
#define _PANEL_S_DEV_CONTROL_H_

/* Includes ------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

#define DEVICE_TOTAL_NUM                91

typedef enum
{
    AIRC_OPEN=70,
    AIRC_CLOSE
    
}AIRC_MODE;

typedef enum
{
    AIR_SUPPLY = 43,
    HEATING,
    COLDING,
    DEHUMIDIFICATION,
    AUTO_MODE
}HEATING_COOLING_MODE;

typedef enum
{
    LEVEL_WIND_AUTO = 37,					//wind rate auto
    LEVEL_HIGH,						//wind rate	high
    LEVEL_MIDDLE,						//wind rate middle
    LEVEL_LOW,						//wind rate low
    LEVEL_WIND_RATE_MOST,				//wind rate most
    LEVEL_WIND_RATE_MUTE				//wind rate mute
}FAN_SPEED_MODE;

typedef enum
{
    DIRECTION_AUTO = 31,				//wind direction auto
    DIRECTION_HAND5,						//wind handle 5
    DIRECTION_HAND4,
    DIRECTION_HAND3,
    DIRECTION_HAND2,
    DIRECTION_HAND1
}FAN_DIRECTION_MODE;


typedef enum
{
    Local_Dimming_Light = 0,
    Remote_Dimming_Light,
    RS485_Curtain,
    Door_Magnetic_Switch,
    Floor_Heating,
    Fresh_Air,
    Motion_Sensor,
    Light_Switch,
    Enocean_Curtain,
    Dry_Circuit_Contact_Curtain,
    General_Switch,
    Air_Conditioner
}DEVICE_TYPE_DEF;

typedef struct
{
    unsigned char index;
    unsigned short knx_obj_id;
    unsigned char device_type;
    char device_name[9];
    unsigned char device_enable;
    unsigned char device_address;
    unsigned short *device_value;
    unsigned char scene_enable[16];
    unsigned char scene_value[16];
    unsigned char group_num;
    unsigned char group_id[20];
    void (*update)();
}DEVICE_CONF;

typedef struct
{
    unsigned char j_type;
    unsigned char j_number;
    int j_knx_id[255];
}JSON_DEVICE_CONF;

typedef struct
{
    unsigned short knx_obj_id;
    unsigned char scene_enable[16];
    unsigned char scene_value[16];
}JSON_SCENE_CONF;

typedef struct
{
    DEVICE_CONF *pointer;
    DEVICE_CONF table;
}DEVICE_CONF_TABLE;

typedef struct
{
    unsigned char scene_id;
    unsigned char scene_enable;
    unsigned char name[255];
    unsigned short knx_obj_id[DEVICE_TOTAL_NUM];
    unsigned char scene_value[DEVICE_TOTAL_NUM];
    unsigned short *current_value[DEVICE_TOTAL_NUM];
    void (*control)();
}SCENE_CONF_TABLE;

typedef struct
{
    JSON_DEVICE_CONF j_device_conf[10];
    unsigned char j_group_conf[255][DEVICE_TOTAL_NUM];
    JSON_SCENE_CONF j_scene_conf[DEVICE_TOTAL_NUM];
}DEVICE_DATABASE_TABLE;

typedef struct
{
    DEVICE_CONF_TABLE device_table[DEVICE_TOTAL_NUM];
    SCENE_CONF_TABLE scene_table[16];
    pthread_mutex_t device_table_mutex;
}TABLE_MANAGEMENT_DEF;

extern DEVICE_DATABASE_TABLE device_configuration;
extern TABLE_MANAGEMENT_DEF table_management;
extern unsigned short scene_enable;
extern unsigned char floorheat_switch;

extern void init_management_table(void);
extern void load_device_configuration(unsigned char *ptable);
extern void save_device_configuration(unsigned char *ptable, unsigned char *scene_enable);
extern void load_group_configuration(unsigned char *ptable);
extern int save_group_configuration(unsigned char *ptable, char *pentry);
extern void load_scene_configuration(unsigned char *ptable, unsigned char scene_id);
extern int save_scene_configuration(unsigned char *ptable, char *pentry);
extern void scene_control(unsigned char scene_id, unsigned char control);
extern void group_control_by_id(unsigned char group_id, unsigned char *value);
extern void group_control_by_device(unsigned short knx_obj_id, unsigned char *value);
extern void knx_local_object_update(unsigned short knx_obj_id, unsigned char *value, unsigned char from);
extern void device_update(unsigned short knx_obj_id, unsigned char *value, unsigned char from);
extern void update_device_status_fuc(unsigned char device_type, unsigned short knx_obj_id, unsigned char* value);
extern void enocean_device_add(void);
extern void enocean_exit_device_add(void);
extern void enocean_device_removeall(void);
extern void enocean_curtain_add(unsigned short knx_obj_id);

    
#ifdef __cplusplus
}
#endif

#endif /* _PANEL_S_DEV_CONTROL_H_ */
/* --------------------------------- End Of File ------------------------------ */

