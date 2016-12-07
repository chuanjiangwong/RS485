
/*
 * =====================================================================================
 *
 *       Filename:  mac_ccs_01m.c
 *
 *    Description:  mistsubishi eclectric coproration mac-ccs-01m
 *
 *        Version:  1.0
 *        Created:  Dec 7, 2016 9:39:36 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
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
#include "protocol/modbus/modbus.h"

#ifndef MISTSUBISHI_ELECTRIC_CORPORATION_DEFAULT_ADDRESS
#define MISTSUBISHI_ELECTRIC_CORPORATION_DEFAULT_ADDRESS         (0xdc)
#endif


#define STATUS_POWER_OFF                            (0x00)
#define STATUS_POWER_ON                             (0x01)

#define STATUS_RUN_OK                               (0x00)
#define STATUS_RUN_ERROR                            (0x01)

#define STATUS_MODE_AUTOING                         (0x00)
#define STATUS_MODE_COOLING                         (0x01)
#define STATUS_MODE_FANING                          (0x02)
#define STATUS_MODE_DRYING                          (0x03)
#define STATUS_MODE_HEATING                         (0x04)

#define STATUS_FANG_AUTO                            (0x00)
#define STATUS_FANG_HIGH                            (0x01)
#define STATUS_FANG_MIDDLE                          (0x02)
#define STATUS_FANG_LOW                             (0x04)


#define SET_POWER_OFF                               (0x00)
#define SET_POWER_ON                                (0x01)

#define SET_MODE_AUTOING                            (0x00)
#define SET_MODE_COOLING                            (0x01)
#define SET_MODE_FANING                             (0x02)
#define SET_MODE_DRYING                             (0x03)
#define SET_MODE_HEATING                            (0x04)

#define SET_FAN_AUTO                                (0x00)
#define SET_FAN_HIGH                                (0x06)
#define SET_FAN_MIDDLE                              (0x03)
#define SET_FAN_LOW                                 (0x02)

#define SET_SWING_AUTO                              (0x00)
#define SET_SWING_UP_DOWN                           (0x03)
#define SET_SWING_LEFT_RIGHT                        (0x07)

#define SET_POWER_OFFSET                            (0)
#define SET_MODE_OFFSET                             (1)
#define SET_FANG_OFFSET                             (2)
#define SET_TEMPERATURE_OFFSET                      (3)
#define SET_SWING_OFFSET                            (5)

#define GET_ROOM_TEMPERATURE_OFFSET                 (4)
#define GET_FAULT_CODE                              (6)


#define AIR_CONDITION_INFO_REGISTER_LENGTH          (7)

int mistsubishi_mac_ccs_01m_send_package_handle(volatile void* arg)
{
    modbus_port_handle_t                *handle = (modbus_port_handle_t*)arg;
    uint16_t                            hold_register = 0, hold_register_value = 0;

    if(handle == NULL)
        return -EINVAL;


    /* full the function code */
    handle->code = MODBUS_FUNCTION_CODE_WRITE_SIGNLE_REGISTER;

    if(handle->device_addr > 32)
    {
        syslog_warning("[hitachi]: The device addr:%u inval, except less than 32", handle->device_addr);
        return -EINVAL;
    }

    /* every air condition have a 4 hold register */
    hold_register = handle->device_addr;
    switch(handle->method)
    {
        case RS485_AIR_SET_TEMP_18:
        case RS485_AIR_SET_TEMP_19:
        case RS485_AIR_SET_TEMP_20:
        case RS485_AIR_SET_TEMP_21:
        case RS485_AIR_SET_TEMP_22:
        case RS485_AIR_SET_TEMP_23:
        case RS485_AIR_SET_TEMP_24:
        case RS485_AIR_SET_TEMP_25:
        case RS485_AIR_SET_TEMP_26:
        case RS485_AIR_SET_TEMP_27:
        case RS485_AIR_SET_TEMP_28:
        case RS485_AIR_SET_TEMP_29:
        case RS485_AIR_SET_TEMP_30:
            hold_register_value = handle->method * 10;
            handle->register_addr = hold_register << 8 | (SET_TEMPERATURE_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SWING_AUTO:
            hold_register_value = SET_SWING_AUTO;
            handle->register_addr = hold_register << 8 | (SET_SWING_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SWING_UP_DOWN:
            hold_register_value = SET_SWING_UP_DOWN;
            handle->register_addr = hold_register << 8 | (SET_SWING_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_SWING_LEFT_RIGHT:
            hold_register_value = SET_SWING_LEFT_RIGHT;
            handle->register_addr = hold_register << 8 | (SET_SWING_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_FAN_AUTO:
            hold_register_value = SET_FAN_AUTO;
            handle->register_addr = hold_register << 8 | (SET_FANG_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_FAN_HIGH:
            hold_register_value = SET_FAN_HIGH;
            handle->register_addr = hold_register << 8 | (SET_FANG_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_FAN_MIDDLE:
            hold_register_value = SET_FAN_MIDDLE;
            handle->register_addr = hold_register << 8 | (SET_FANG_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_FAN_LOW:
            hold_register_value = SET_FAN_LOW;
            handle->register_addr = hold_register << 8 | (SET_FANG_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_MODE_FANING:
            hold_register_value = SET_MODE_FANING;
            handle->register_addr = hold_register << 8 | (SET_MODE_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_MODE_HEATING:
            hold_register_value = SET_MODE_HEATING;
            handle->register_addr = hold_register << 8 | (SET_MODE_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_MODE_COOLING:
            hold_register_value = SET_MODE_COOLING;
            handle->register_addr = hold_register << 8 | (SET_MODE_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_MODE_DRYING:
            hold_register_value = SET_MODE_DRYING;
            handle->register_addr = hold_register << 8 | (SET_MODE_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_MODE_AUTOING:
            hold_register_value = SET_MODE_AUTOING;
            handle->register_addr = hold_register << 8 | (SET_MODE_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_OFF:
            hold_register_value = SET_POWER_OFF;
            handle->register_addr = hold_register << 8 | (SET_POWER_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;
        case RS485_AIR_ON:
            hold_register_value = SET_POWER_ON;
            handle->register_addr = hold_register << 8 | (SET_POWER_OFFSET & 0xff);
            handle->buffer[0] = hold_register_value >> 8;
            handle->buffer[1] = hold_register_value & 0xff;
            break;

        default:
            handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
            return 0;
    }

    handle->device_addr = MISTSUBISHI_ELECTRIC_CORPORATION_DEFAULT_ADDRESS;

    /* return the frame length is 1 */
    return 1;
}

int mistsubishi_mac_ccs_01m_get_device_info_send(volatile void* arg)
{
    modbus_port_handle_t        *handle = (modbus_port_handle_t*)arg;

    if(handle == NULL)
        return -1;

    /* The every air condition have a 6 hold register */
    handle->register_addr = handle->device_addr << 8;
    handle->code = MODBUS_FUNCTION_CODE_READ_MULTIPLE_REGISTERS;

    handle->device_addr = MISTSUBISHI_ELECTRIC_CORPORATION_DEFAULT_ADDRESS;

    return AIR_CONDITION_INFO_REGISTER_LENGTH;
}

int mistsubishi_mac_ccs_01m_get_device_info_handle(volatile void* arg)
{
    modbus_port_handle_t        *handle = (modbus_port_handle_t*)arg;
    uint16_t                    *value = (uint16_t*)handle->buffer;

    if(handle == NULL && (value == NULL))
        return -1;

    /* TODO: process data */

    return 0;
}

