/*
 * =====================================================================================
 *
 *          @file:  support.c
 *
 *    Description:  The rs485 device support register
 *
 *        Version:  1.0
 *        Created:  03/28/2016 09:02:49 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "enum.h"
#include "adapter.h"
#include "support.h"
#include "syslog/log.h"
#include "device/airCondition/panasonnic/panasonnic.h"
#include "device/curtain/doya/doya.h"
#include "device/curtain/aoke/aoke.h"
#include "device/freshAir/loreley/loreley.h"
#include "device/airCondition/daikin/DTA116A621.h"
#include "device/airCondition/york/york.h"
#include "device/airCondition/hitachi/vrv_manager.h"
#include "device/airCondition/york/johnson_controls.h"
#include "device/airCondition/mistsubishi/mac_ccs_01m.h"


/* -----------------------------------------*/
/**
 * @brief device_profile
 *  The panasonnic air condition device profile
 */
/* -----------------------------------------*/
static struct device_profile air_condition_panasonnic[] =
{
    /* address len */ /* device method */ /* method send package */ /* method recv package */
    {1,     RS485_AIR_SET_TEMP_18,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SET_TEMP_19,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SET_TEMP_20,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SET_TEMP_21,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SET_TEMP_22,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SET_TEMP_23,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SET_TEMP_24,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SET_TEMP_25,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SET_TEMP_26,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SET_TEMP_27,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SET_TEMP_28,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SET_TEMP_29,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SET_TEMP_30,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},

    {1,     RS485_AIR_SWING_AUTO,               panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SWING_UP_DOWN,            panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SWING_LEFT_RIGHT,         panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_SWING_UP_DOWN_LEFT_RIGHT, panasonnic_send_package_handle,     panasonnic_recv_package_handle},

    {1,     RS485_AIR_FAN_AUTO,    	    panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_FAN_HIGH,    	    panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_FAN_MIDDLE,        panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_FAN_LOW,    	    panasonnic_send_package_handle,     panasonnic_recv_package_handle},

    {1,     RS485_AIR_MODE_FANING,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_MODE_HEATING,      panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_MODE_COOLING,      panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_MODE_DRYING,       panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_MODE_AUTOING,      panasonnic_send_package_handle,     panasonnic_recv_package_handle},

    {1,     RS485_AIR_OFF,    		    panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_ON,    		    panasonnic_send_package_handle,     panasonnic_recv_package_handle},
    {1,     RS485_AIR_GET_DEVICE_INFO,   panasonnic_send_package_handle,     panasonnic_recv_package_handle},
};

/** dooya curtain */
static struct device_profile curtain_doya[] =
{
    {2,     RS485_CURTAIN_OPEN,                doya_send_package_handle,           doya_recv_package_handle},
    {2,     RS485_CURTAIN_CLOSE,               doya_send_package_handle,           doya_recv_package_handle},
    {2,     RS485_CURTAIN_SET_PERCENT,         doya_send_package_handle,           doya_recv_package_handle},
    {2,     RS485_CURTAIN_GET_DEVICE_INFO,     doya_send_package_handle,           doya_recv_package_handle},
};

/** aoke curtain */
static struct device_profile curtain_aoke[] =
{
	{3,     RS485_CURTAIN_OPEN,                     aoke_send_package_handle,           aoke_recv_package_handle},
	{3,     RS485_CURTAIN_CLOSE,                    aoke_send_package_handle,           aoke_recv_package_handle},
	{3,     RS485_CURTAIN_SET_PERCENT,              aoke_send_package_handle,           aoke_recv_package_handle},
	{3,     RS485_CURTAIN_GET_DEVICE_INFO,          aoke_send_package_handle,           aoke_recv_package_handle},
};

/* loreley fresh air */
static struct device_profile fresh_air_loreley[] =
{
	{1,     RS485_FRESH_AIR_AUTO_ON,                loreley_send_package_handle,           loreley_recv_package_handle},
	{1,     RS485_FRESH_AIR_AUTO_OFF,               loreley_send_package_handle,           loreley_recv_package_handle},
	{1,     RS485_FRESH_AIR_GET_DEVICE_INFO,        loreley_send_package_handle,           loreley_recv_package_handle},
};

/** daikin dta116a621 air conditoner */
static struct device_profile air_condition_daikin_dta116a621[] =
{
    /* address len */ /* device method */ /* method send package */ /* method recv package */
    {1,     RS485_AIR_SET_TEMP_18,                  daikin_dta116a621_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_19,                  daikin_dta116a621_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_20,                  daikin_dta116a621_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_21,                  daikin_dta116a621_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_22,                  daikin_dta116a621_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_23,                  daikin_dta116a621_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_24,                  daikin_dta116a621_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_25,                  daikin_dta116a621_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_26,                  daikin_dta116a621_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_27,                  daikin_dta116a621_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_28,                  daikin_dta116a621_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_29,                  daikin_dta116a621_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_30,                  daikin_dta116a621_set_temperature,      NULL},

    {1,     RS485_AIR_SWING_AUTO,                   daikin_dta116a621_set_swing,            NULL},
    {1,     RS485_AIR_SWING_UP_DOWN,                daikin_dta116a621_set_swing,            NULL},
    {1,     RS485_AIR_SWING_LEFT_RIGHT,             daikin_dta116a621_set_swing,            NULL},
    {1,     RS485_AIR_SWING_UP_DOWN_LEFT_RIGHT,     daikin_dta116a621_set_swing,            NULL},

    {1,     RS485_AIR_FAN_AUTO,    	                daikin_dta116a621_set_fan,              NULL},
    {1,     RS485_AIR_FAN_HIGH,    	                daikin_dta116a621_set_fan,              NULL},
    {1,     RS485_AIR_FAN_MIDDLE,                   daikin_dta116a621_set_fan,              NULL},
    {1,     RS485_AIR_FAN_LOW,    	                daikin_dta116a621_set_fan,              NULL},

    {1,     RS485_AIR_MODE_FANING,                  daikin_dta116a621_set_mode,             NULL},
    {1,     RS485_AIR_MODE_HEATING,                 daikin_dta116a621_set_mode,             NULL},
    {1,     RS485_AIR_MODE_COOLING,                 daikin_dta116a621_set_mode,             NULL},
    {1,     RS485_AIR_MODE_DRYING,                  daikin_dta116a621_set_mode,             NULL},
    {1,     RS485_AIR_MODE_AUTOING,                 daikin_dta116a621_set_mode,             NULL},

    {1,     RS485_AIR_OFF,    		                daikin_dta116a621_set_switch,           NULL},
    {1,     RS485_AIR_ON,    		                daikin_dta116a621_set_switch,           NULL},
    {1,     RS485_AIR_GET_DEVICE_INFO,              daikin_dta116a621_get_device_info_send, daikin_dta116a621_get_device_info_handle},
};

/** hitachi vrv manager air conditoner */
static struct device_profile air_condition_hitachi_vrv_manager[] =
{
    /* address len */ /* device method */ /* method send package */ /* method recv package */
    {1,     RS485_AIR_SET_TEMP_18,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_19,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_20,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_21,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_22,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_23,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_24,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_25,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_26,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_27,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_28,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_29,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_30,                  hitachi_vrv_manager_send_package_handle,        NULL},

    {1,     RS485_AIR_SWING_AUTO,                   hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SWING_UP_DOWN,                hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SWING_LEFT_RIGHT,             hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_SWING_UP_DOWN_LEFT_RIGHT,     hitachi_vrv_manager_send_package_handle,        NULL},

    {1,     RS485_AIR_FAN_AUTO,    	                hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_FAN_HIGH,    	                hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_FAN_MIDDLE,                   hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_FAN_LOW,    	                hitachi_vrv_manager_send_package_handle,        NULL},

    {1,     RS485_AIR_MODE_FANING,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_MODE_HEATING,                 hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_MODE_COOLING,                 hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_MODE_DRYING,                  hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_MODE_AUTOING,                 hitachi_vrv_manager_send_package_handle,        NULL},

    {1,     RS485_AIR_OFF,    		                hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_ON,    		                hitachi_vrv_manager_send_package_handle,        NULL},
    {1,     RS485_AIR_GET_DEVICE_INFO,              hitachi_vrv_manager_get_device_info_send,       hitachi_vrv_manager_get_device_info_handle},
};

/** hitachi vrv manager air conditoner */
static struct device_profile air_condition_mistsubishi_mac_ccs_01m[] =
{
    /* address len */ /* device method */ /* method send package */ /* method recv package */
    {1,     RS485_AIR_SET_TEMP_18,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_19,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_20,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_21,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_22,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_23,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_24,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_25,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_26,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_27,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_28,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_29,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_30,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},

    {1,     RS485_AIR_SWING_AUTO,                   mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SWING_UP_DOWN,                mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SWING_LEFT_RIGHT,             mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_SWING_UP_DOWN_LEFT_RIGHT,     mistsubishi_mac_ccs_01m_send_package_handle,        NULL},

    {1,     RS485_AIR_FAN_AUTO,    	                mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_FAN_HIGH,    	                mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_FAN_MIDDLE,                   mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_FAN_LOW,    	                mistsubishi_mac_ccs_01m_send_package_handle,        NULL},

    {1,     RS485_AIR_MODE_FANING,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_MODE_HEATING,                 mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_MODE_COOLING,                 mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_MODE_DRYING,                  mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_MODE_AUTOING,                 mistsubishi_mac_ccs_01m_send_package_handle,        NULL},

    {1,     RS485_AIR_OFF,    		                mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_ON,    		                mistsubishi_mac_ccs_01m_send_package_handle,        NULL},
    {1,     RS485_AIR_GET_DEVICE_INFO,              mistsubishi_mac_ccs_01m_get_device_info_send,       mistsubishi_mac_ccs_01m_get_device_info_handle},
};



/** johnson controls air conditoner */
static struct device_profile air_condition_johnson_controls[] =
{
    /* address len */ /* device method */ /* method send package */ /* method recv package */
    {1,     RS485_AIR_SET_TEMP_18,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_19,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_20,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_21,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_22,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_23,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_24,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_25,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_26,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_27,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_28,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_29,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SET_TEMP_30,                  johnson_controls_vrv_send_package_handle,        NULL},

    {1,     RS485_AIR_SWING_AUTO,                   johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SWING_UP_DOWN,                johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SWING_LEFT_RIGHT,             johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_SWING_UP_DOWN_LEFT_RIGHT,     johnson_controls_vrv_send_package_handle,        NULL},

    {1,     RS485_AIR_FAN_AUTO,    	                johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_FAN_HIGH,    	                johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_FAN_MIDDLE,                   johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_FAN_LOW,    	                johnson_controls_vrv_send_package_handle,        NULL},

    {1,     RS485_AIR_MODE_FANING,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_MODE_HEATING,                 johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_MODE_COOLING,                 johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_MODE_DRYING,                  johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_MODE_AUTOING,                 johnson_controls_vrv_send_package_handle,        NULL},

    {1,     RS485_AIR_OFF,    		                johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_ON,    		                johnson_controls_vrv_send_package_handle,        NULL},
    {1,     RS485_AIR_GET_DEVICE_INFO,              johnson_controls_vrv_get_device_info_send,       johnson_controls_vrv_get_device_info_handle},
};

/** the york kelong air conditoner */
static struct device_profile air_condition_york_kelong[] =
{
    /* address len */ /* device method */ /* method send package */ /* method recv package */
    {1,     RS485_AIR_SET_TEMP_18,                  york_kelong_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_19,                  york_kelong_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_20,                  york_kelong_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_21,                  york_kelong_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_22,                  york_kelong_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_23,                  york_kelong_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_24,                  york_kelong_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_25,                  york_kelong_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_26,                  york_kelong_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_27,                  york_kelong_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_28,                  york_kelong_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_29,                  york_kelong_set_temperature,      NULL},
    {1,     RS485_AIR_SET_TEMP_30,                  york_kelong_set_temperature,      NULL},

    {1,     RS485_AIR_SWING_AUTO,                   york_kelong_set_swing,            NULL},
    {1,     RS485_AIR_SWING_UP_DOWN,                york_kelong_set_swing,            NULL},
    {1,     RS485_AIR_SWING_LEFT_RIGHT,             york_kelong_set_swing,            NULL},
    {1,     RS485_AIR_SWING_UP_DOWN_LEFT_RIGHT,     york_kelong_set_swing,            NULL},

    {1,     RS485_AIR_FAN_AUTO,    	                york_kelong_set_fan,              NULL},
    {1,     RS485_AIR_FAN_HIGH,    	                york_kelong_set_fan,              NULL},
    {1,     RS485_AIR_FAN_MIDDLE,                   york_kelong_set_fan,              NULL},
    {1,     RS485_AIR_FAN_LOW,    	                york_kelong_set_fan,              NULL},

    {1,     RS485_AIR_MODE_FANING,                  york_kelong_set_mode,             NULL},
    {1,     RS485_AIR_MODE_HEATING,                 york_kelong_set_mode,             NULL},
    {1,     RS485_AIR_MODE_COOLING,                 york_kelong_set_mode,             NULL},
    {1,     RS485_AIR_MODE_DRYING,                  york_kelong_set_mode,             NULL},
    {1,     RS485_AIR_MODE_AUTOING,                 york_kelong_set_mode,             NULL},

    {1,     RS485_AIR_OFF,    		                york_kelong_set_switch,           NULL},
    {1,     RS485_AIR_ON,    		                york_kelong_set_switch,           NULL},
    {1,     RS485_AIR_GET_DEVICE_INFO,              york_kelong_get_device_info_send, york_kelong_get_device_info_handle},
};



bool check_device_support(const adapter_t* adatper)
{
    return true;
}


struct device_profile* get_support_device_profile(rs485_factory_name_enum name)
{
    switch(name)
    {
        case RS485_CURTAIN_CUSTOMIZE_AOKE_ALL:
            return curtain_aoke;

        case RS485_CURTAIN_CUSTOMIZE_DOOYA_ALL:
            return curtain_doya;

        case RS485_AIRCONDITION_CUSTOMIZE_PANASONNIC_UNKNOWN_SHENZHEN:
            return air_condition_panasonnic;

        case RS485_FRESHAIR_CUSTOMIZE_LORELEY_UNKNOWN_GUANGZHOU:
            return fresh_air_loreley;

        case RS485_AIRCONDITION_MODBUS_DAIKIN_DTA116A621_BOX:
            return air_condition_daikin_dta116a621;

        case RS485_AIRCONDITION_BACNET_YORK_KELONG_QINGDAO_BOX:
            return air_condition_york_kelong;

        case RS485_AIRCONDITION_MODBUS_HITACHI_VRV_ALL_BOX:
            return air_condition_hitachi_vrv_manager;

        case RS485_AIRCONDITION_MODBUS_YORK_JOHNSON_CONTROLS_BOX:
            return air_condition_johnson_controls;

        case RS485_AIRCONDITION_MODBUS_MISTSUBISHI_MAC_CCS_01M:
        	return air_condition_mistsubishi_mac_ccs_01m;

        default:
            return NULL;
    }
}

int get_support_device_profile_numbers(rs485_factory_name_enum name)
{
    switch(name)
    {
        case RS485_CURTAIN_CUSTOMIZE_AOKE_ALL:
            return sizeof(curtain_aoke)/sizeof(struct device_profile);

        case RS485_CURTAIN_CUSTOMIZE_DOOYA_ALL:
            return sizeof(curtain_doya)/sizeof(struct device_profile);

        case RS485_AIRCONDITION_CUSTOMIZE_PANASONNIC_UNKNOWN_SHENZHEN:
            return sizeof(air_condition_panasonnic)/sizeof(struct device_profile);

        case RS485_FRESHAIR_CUSTOMIZE_LORELEY_UNKNOWN_GUANGZHOU:
            return sizeof(fresh_air_loreley)/sizeof(struct device_profile);

        case RS485_AIRCONDITION_MODBUS_DAIKIN_DTA116A621_BOX:
            return sizeof(air_condition_daikin_dta116a621)/sizeof(struct device_profile);

        case RS485_AIRCONDITION_BACNET_YORK_KELONG_QINGDAO_BOX:
            return sizeof(air_condition_york_kelong)/sizeof(struct device_profile);

        case RS485_AIRCONDITION_MODBUS_HITACHI_VRV_ALL_BOX:
            return sizeof(air_condition_hitachi_vrv_manager)/sizeof(struct device_profile);

        case RS485_AIRCONDITION_MODBUS_YORK_JOHNSON_CONTROLS_BOX:
            return sizeof(air_condition_johnson_controls)/sizeof(struct device_profile);

        case RS485_AIRCONDITION_MODBUS_MISTSUBISHI_MAC_CCS_01M:
        	return sizeof(air_condition_mistsubishi_mac_ccs_01m)/sizeof(struct device_profile);

        default:
            return 0;
    }
}

method_send inline get_device_send_package_function(const struct device_profile* profile, int profile_numbers, int command)
{
    int i;

    if(profile == NULL)
        return NULL;

    for(i=0; i<profile_numbers; i++)
    {
        if(command == profile[i].method)
            return profile[i].send;
    }

    return NULL;
}

method_recv inline get_device_recv_package_function(const struct device_profile* profile, int profile_numbers, int command)
{
    int i;

    if(profile == NULL)
        return NULL;

    for(i=0; i<profile_numbers; i++)
    {
        if(command == profile[i].method)
            return profile[i].recv;
    }

    return NULL;
}
