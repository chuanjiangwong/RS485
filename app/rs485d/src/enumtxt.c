/*
 * =====================================================================================
 *
 *          @file:  enumtxt.c
 *
 *    Description:  offer the enum text string
 *
 *        Version:  1.0
 *        Created:  Apr 8, 2016 1:56:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#include <unistd.h>

#include <stdbool.h>

#include "enumtxt.h"
#include "enum.h"




char* get_enum_txt_service(rs485_service_type_enum type)
{
    switch(type)
    {
        case SERVICE_CREATE_RS485_OBJECT:
            return "service:[create rs485 object]";
        case SERVICE_DELETE_RS485_OBJECT:
            return "service:[delete rs485 object]";
        case SERVICE_MOUNT_DEVICE_TO_OBJECT:
            return "service:[mount device to object]";
        case SERVICE_UNMOUNT_DEVICE_FROM_OBJECT:
            return "service:[unmount device from object]";
        case SERVICE_WRITE_VALUE_TO_DEVICE:
            return "service:[write value to device]";
        case SERVICE_READ_VALUE_FROM_DEVICE:
            return "service:[read value from device]";
        case SERVICE_UNKNOWN:
            return "service:[unknown]";

        default:
            return NULL;
    }
}




char* get_enum_txt_rs485_device_type(rs485_device_type_enum type)
{
    switch(type)
    {
        case RS485_DEVICE_TYPE_AIR_CONDITION:
            return "air conditioner";
        case RS485_DEVICE_TYPE_CURTAIN:
            return "curtain";
        case RS485_DEVICE_TYPE_FRESH_AIR:
            return "fresh air";
        case RS485_DEVICE_TYPE_UNKNOWN:
            return "unknown";

        default:
            return NULL;
    }
}



char* get_enum_txt_rs485_protocol_type(rs485_protocol_type_enum type)
{
    switch(type)
    {
        case RS485_PROTOCOL_TYPE_BACNET:
            return "BACnet";
        case RS485_PROTOCOL_TYPE_MODBUS:
            return "MODBUS";
        case RS485_PROTOCOL_TYPE_GENERAL:
            return "GENERAL";
        case RS485_PROTOCOL_TYPE_UNKNOWN:
            return "unknown";

        default:
            return NULL;
    }
}


char* get_enum_txt_device_method(rs485_device_method_enum type)
{
    switch(type)
    {
        /* air conddition */
        case RS485_AIR_SET_TEMP:
            return "set air condition temperature";
        case RS485_AIR_SET_TEMP_18:
            return "set air condition temperature 18";
        case RS485_AIR_SET_TEMP_19:
            return "set air condition temperature 19";
        case RS485_AIR_SET_TEMP_20:
            return "set air condition temperature 20";
        case RS485_AIR_SET_TEMP_21:
            return "set air condition temperature 21";
        case RS485_AIR_SET_TEMP_22:
            return "set air condition temperature 22";
        case RS485_AIR_SET_TEMP_23:
            return "set air condition temperature 23";
        case RS485_AIR_SET_TEMP_24:
            return "set air condition temperature 24";
        case RS485_AIR_SET_TEMP_25:
            return "set air condition temperature 25";
        case RS485_AIR_SET_TEMP_26:
            return "set air condition temperature 26";
        case RS485_AIR_SET_TEMP_27:
            return "set air condition temperature 27";
        case RS485_AIR_SET_TEMP_28:
            return "set air condition temperature 28";
        case RS485_AIR_SET_TEMP_29:
            return "set air condition temperature 29";
        case RS485_AIR_SET_TEMP_30:
            return "set air condition temperature 30";

        case RS485_AIR_SWING:
            return "set air condition swing";
        case RS485_AIR_SWING_AUTO:
            return "set air condition swing auto";
        case RS485_AIR_SWING_UP_DOWN:
            return "set air condition swing up and down";
        case RS485_AIR_SWING_LEFT_RIGHT:
            return "set air condition swing left and right";
        case RS485_AIR_SWING_UP_DOWN_LEFT_RIGHT:
            return "set air condition swing all";

        case RS485_AIR_FAN:
            return "set air condition fan";
        case RS485_AIR_FAN_AUTO:
            return "set air condition fan auto";
        case RS485_AIR_FAN_HIGH:
            return "set air condition fan high";
        case RS485_AIR_FAN_MIDDLE:
            return "set air condition fan middle";
        case RS485_AIR_FAN_LOW:
            return "set air condition fan high";

        case RS485_AIR_MODE:
            return "set air conditon mode";
        case RS485_AIR_MODE_FANING:
            return "set air conditon mode faning";
        case RS485_AIR_MODE_HEATING:
            return "set air conditon mode heating";
        case RS485_AIR_MODE_COOLING:
            return "set air conditon mode cooling";
        case RS485_AIR_MODE_DRYING:
            return "set air conditon mode drying";
        case RS485_AIR_MODE_AUTOING:
            return "set air conditon mode autoing";

        case RS485_AIR_SWITCH:
            return "set air conditon switch";
        case RS485_AIR_OFF:
            return "set air conditon OFF";
        case RS485_AIR_ON:
            return "set air conditon ON";
        case RS485_AIR_RESTART:
            return "restart conditon";

        case RS485_AIR_GET_DEVICE_INFO:
            return "get air conditon information";


        /* curtain */
        case RS485_CURTAIN:
            return "set curtain";
        case RS485_CURTAIN_OPEN:
            return "set curtain open";
        case RS485_CURTAIN_CLOSE:
            return "set curtain close";
        case RS485_CURTAIN_SET_PERCENT:
            return "set curtain percent";
        case RS485_CURTAIN_RESET:
            return "restart curtain";

        case RS485_CURTAIN_GET_DEVICE_INFO:
            return "get curtain information";

        /*fresh air*/
        case RS485_FRESH_AIR:
            return "set fresh air";
        case RS485_FRESH_AIR_AUTO_ON:
            return "set fresh air auto mode ON";
        case RS485_FRESH_AIR_AUTO_OFF:
            return "set fresh air auto mode OFF";
        case RS485_FRESH_AIR_RESET:
            return "restart fresh air";
        case RS485_FRESH_AIR_GET_DEVICE_INFO:
            return "get fresh air information";

        default:
            return NULL;
    }
}


char* get_enum_txt_device_factory(rs485_factory_name_enum name)
{
    switch(name)
    {
        case RS485_AIRCONDITION_BACNET_YORK_KELONG_QINGDAO_BOX:
            return "YORK";
        case RS485_AIRCONDITION_CUSTOMIZE_PANASONNIC_UNKNOWN_SHENZHEN:
            return "PANASONNIC";
        case RS485_AIRCONDITION_MODBUS_DAIKIN_DTA116A621_BOX:
            return "DAIKIN";
        case RS485_CURTAIN_CUSTOMIZE_DOOYA_ALL:
            return "DOOYA";
        case RS485_CURTAIN_CUSTOMIZE_AOKE_ALL:
            return "AOKE";
        case RS485_FRESHAIR_CUSTOMIZE_LORELEY_UNKNOWN_GUANGZHOU:
            return "LORELEY";
        case RS485_AIRCONDITION_MODBUS_HITACHI_VRV_ALL_BOX:
            return "HITACHI";
        case RS485_AIRCONDITION_MODBUS_YORK_JOHNSON_CONTROLS_BOX:
            return "JOHNSON";

        default:
            return "unknown";
    }
}


char* get_enum_txt_bool(bool status)
{
    if(status)
        return "true";
    else
        return "false";
}
