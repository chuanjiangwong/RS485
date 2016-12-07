/*
 * =====================================================================================
 *
 *          @file:  vrv_manager.c
 *
 *    Description:  hitachi vrv manager module
 *
 *        Version:  1.0
 *        Created:  Jun 28, 2016 10:35:06 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <errno.h>

#include "enum.h"
#include "adapter.h"
#include "syslog/log.h"
#include "device/airCondition/hitachi/vrv_manager.h"
#include "protocol/modbus/modbus.h"

#ifndef HITACHI_VRV_MANAGER_DEFAULT_ADDRESS
#define HITACHI_VRV_MANAGER_DEFAULT_ADDRESS         (0x02)
#endif


#define STATUS_POWER_OFF                            (0x00)
#define STATUS_POWER_ON                             (0x01)
#define STATUS_RUN_OK                               (0x00)
#define STATUS_RUN_ERROR                            (0x01)
#define STATUS_MODE_COOLING                         (0x01)
#define STATUS_MODE_DRYING                          (0x02)
#define STATUS_MODE_FANING                          (0x04)
#define STATUS_MODE_HEATING                         (0x08)
#define STATUS_FANG_HIGH                            (0x01)
#define STATUS_FANG_MIDDLE                          (0x02)
#define STATUS_FANG_LOW                             (0x04)


#define SET_POWER_OFF                               (0x00)
#define SET_POWER_ON                                (0x01)
#define SET_MODE_COOLING                            (0x01)
#define SET_MODE_DRYING                             (0x02)
#define SET_MODE_FANING                             (0x04)
#define SET_MODE_HEATING                            (0x08)
#define SET_FAN_HIGH                                (0x01)
#define SET_FAN_MIDDLE                              (0x02)
#define SET_FAN_LOW                                 (0x04)

#define SET_POWER_OFFSET                            (0)
#define SET_TEMPERATURE_OFFSET                      (1)
#define SET_MODE_OFFSET                             (2)
#define SET_FANG_OFFSET                             (3)

#define HOLD_REGISTER_START                         (4000)

#define AIR_CONDITION_INFO_REGISTER_LENGTH          (6)

int hitachi_vrv_manager_send_package_handle(volatile void* arg)
{
    modbus_port_handle_t                *handle = (modbus_port_handle_t*)arg;
    uint16_t                            hold_register = 0, hold_register_value = 0;

    if(handle == NULL)
        return -EINVAL;


    /* full the function code */
    handle->code = MODBUS_FUNCTION_CODE_WRITE_SIGNLE_REGISTER;

    if(handle->device_addr > 127)
    {
        syslog_warning("[hitachi]: The device addr:%u inval, except less than 127", handle->device_addr);
        return -EINVAL;
    }

    /* every air condition have a 4 hold register */
    hold_register = HOLD_REGISTER_START + handle->device_addr * 4;
    switch(handle->method)
    {
        case RS485_AIR_SET_TEMP_18:
            /* becasue The hitachi air condition The low value is 19 */
#if 1
            hold_register_value = 19;
            handle->register_addr = hold_register + SET_TEMPERATURE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
#else
            handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
#endif
            break;
        case RS485_AIR_SET_TEMP_19:
            hold_register_value = 19;
            handle->register_addr = hold_register + SET_TEMPERATURE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SET_TEMP_20:
            hold_register_value = 20;
            handle->register_addr = hold_register + SET_TEMPERATURE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SET_TEMP_21:
            hold_register_value = 21;
            handle->register_addr = hold_register + SET_TEMPERATURE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SET_TEMP_22:
            hold_register_value = 22;
            handle->register_addr = hold_register + SET_TEMPERATURE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SET_TEMP_23:
            hold_register_value = 23;
            handle->register_addr = hold_register + SET_TEMPERATURE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SET_TEMP_24:
            hold_register_value = 24;
            handle->register_addr = hold_register + SET_TEMPERATURE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SET_TEMP_25:
            hold_register_value = 25;
            handle->register_addr = hold_register + SET_TEMPERATURE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SET_TEMP_26:
            hold_register_value = 26;
            handle->register_addr = hold_register + SET_TEMPERATURE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SET_TEMP_27:
            hold_register_value = 27;
            handle->register_addr = hold_register + SET_TEMPERATURE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SET_TEMP_28:
            hold_register_value = 28;
            handle->register_addr = hold_register + SET_TEMPERATURE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SET_TEMP_29:
            hold_register_value = 29;
            handle->register_addr = hold_register + SET_TEMPERATURE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SET_TEMP_30:
            hold_register_value = 30;
            handle->register_addr = hold_register + SET_TEMPERATURE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SWING_AUTO:
            handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
            break;
        case RS485_AIR_SWING_UP_DOWN:
            handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
            break;
        case RS485_AIR_SWING_LEFT_RIGHT:
            handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
            break;
        case RS485_AIR_FAN_AUTO:
            hold_register_value = SET_FAN_MIDDLE;
            handle->register_addr = hold_register + SET_FANG_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_FAN_HIGH:
            hold_register_value = SET_FAN_HIGH;
            handle->register_addr = hold_register + SET_FANG_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_FAN_MIDDLE:
            hold_register_value = SET_FAN_MIDDLE;
            handle->register_addr = hold_register + SET_FANG_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_FAN_LOW:
            hold_register_value = SET_FAN_LOW;
            handle->register_addr = hold_register + SET_FANG_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_MODE_FANING:
            hold_register_value = SET_MODE_FANING;
            handle->register_addr = hold_register + SET_MODE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_MODE_HEATING:
            hold_register_value = SET_MODE_HEATING;
            handle->register_addr = hold_register + SET_MODE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_MODE_COOLING:
            hold_register_value = SET_MODE_COOLING;
            handle->register_addr = hold_register + SET_MODE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_MODE_DRYING:
            hold_register_value = SET_MODE_DRYING;
            handle->register_addr = hold_register + SET_MODE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_MODE_AUTOING:
            hold_register_value = SET_MODE_DRYING;
            handle->register_addr = hold_register + SET_MODE_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_OFF:
            hold_register_value = SET_POWER_OFF;
            handle->register_addr = hold_register + SET_POWER_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_ON:
            hold_register_value = SET_POWER_ON;
            handle->register_addr = hold_register + SET_POWER_OFFSET;
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;

        default:
            handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
            return 0;
    }

    handle->device_addr = HITACHI_VRV_MANAGER_DEFAULT_ADDRESS;

    /* return the frame length is 1 */
    return 1;
}

int hitachi_vrv_manager_get_device_info_send(volatile void* arg)
{
    modbus_port_handle_t        *handle = (modbus_port_handle_t*)arg;

    if(handle == NULL)
        return -1;

    /* The every air condition have a 6 hold register */
    handle->register_addr = handle->device_addr * AIR_CONDITION_INFO_REGISTER_LENGTH;
    handle->code = MODBUS_FUNCTION_CODE_READ_MULTIPLE_REGISTERS;

    handle->device_addr = HITACHI_VRV_MANAGER_DEFAULT_ADDRESS;

    return AIR_CONDITION_INFO_REGISTER_LENGTH;
}

int hitachi_vrv_manager_get_device_info_handle(volatile void* arg)
{
    modbus_port_handle_t        *handle = (modbus_port_handle_t*)arg;
    uint16_t                    *value = (uint16_t*)handle->buffer;

    if(handle == NULL && (value == NULL))
        return -1;

    /* TODO: process data */

    return 0;
}
