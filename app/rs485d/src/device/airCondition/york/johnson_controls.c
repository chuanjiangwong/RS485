/*
 * =====================================================================================
 *
 *          @file:  johnson_controls.c
 *
 *    Description:  Johnson control for YORK air conditoner interface, It's
 *                  Modbus protocol
 *
 *        Version:  1.0
 *        Created:  Jul 4, 2016 10:48:19 AM
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
#include "protocol/modbus/modbus.h"
#include "device/airCondition/york/johnson_controls.h"

#ifndef JOHNSON_CONTROLS_VRV_DEFAULT_ADDRESS
#define JOHNSON_CONTROLS_VRV_DEFAULT_ADDRESS        (0x02)
#endif


#define SET_POWER_OFF                               (0x00)
#define SET_POWER_ON                                (0x02)

#define SET_MODE_COOLING                            (0x01)
#define SET_MODE_DRYING                             (0x02)
#define SET_MODE_FANING                             (0x03)
#define SET_MODE_HEATING                            (0x04)

#define SET_SWING_ON                                (0x02)
#define SET_SWING_OFF                               (0x00)

#define SET_FAN_OFF                                 (0x00)
#define SET_FAN_LOW                                 (0x01)
#define SET_FAN_MIDDLE                              (0x02)
#define SET_FAN_HIGH                                (0x03)
#define SET_FAN_AUTO                                (0x04)


#define HOLD_REGISTER_START                         (3000)

#define AIR_CONDITION_INFO_REGISTER_LENGTH          (16)
#define AIR_CONDITION_DATA_REGISTER_START           (500)


/** The indoor status , all of 64  */
static uint16_t indoor_status[64][2] =
{
    /** have a defualt setting
     * off, swing on, middle speed, 25*/
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
    {0x0002, 0x0419},
};



#define SEND_PACKAGE_LENGTH     (6)
int johnson_controls_vrv_send_package_handle(volatile void* arg)
{
    modbus_port_handle_t                *handle = (modbus_port_handle_t*)arg;
    uint16_t                            *value = (uint16_t*)handle->buffer;

    if(handle == NULL || value == NULL)
        return -EINVAL;


    /* full the function code */
    handle->code = MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_REGISTERS;

    /* set the address registers */
    if(handle->device_addr > 64 || (handle->device_addr == 0))
    {
        syslog_warning("[hitachi]: The device addr:%u inval, except less than 127", handle->device_addr);
        return -EINVAL;
    }
    else
    {
        /* bit0 is address 1 */
        handle->device_addr -= 1;
    }

    if(handle->buffer_len < (SEND_PACKAGE_LENGTH * 2))
        return -ENOMEM;

    /* bzero the buffer */
    memset(value, 0x00, SEND_PACKAGE_LENGTH * 2 );
    if(48 <= handle->device_addr && handle->device_addr < 64)
    {
        *(value) |= 0x1 << (handle->device_addr % 16);
    }
    else if(32 <= handle->device_addr && handle->device_addr < 48)
    {
        *(value + 1) |= 0x1 << (handle->device_addr % 16);
    }
    else if(16 <= handle->device_addr && handle->device_addr < 32)
    {
        *(value + 2) |= 0x1 << (handle->device_addr % 16);
    }
    else
    {
        *(value + 3) |= 0x1 << (handle->device_addr % 16);
    }


    handle->register_addr = HOLD_REGISTER_START;
    switch(handle->method)
    {
        case RS485_AIR_SET_TEMP_18:
        case RS485_AIR_SET_TEMP_19:
        case RS485_AIR_SET_TEMP_20:
        case RS485_AIR_SET_TEMP_22:
        case RS485_AIR_SET_TEMP_23:
        case RS485_AIR_SET_TEMP_24:
        case RS485_AIR_SET_TEMP_25:
        case RS485_AIR_SET_TEMP_26:
        case RS485_AIR_SET_TEMP_27:
        case RS485_AIR_SET_TEMP_28:
        case RS485_AIR_SET_TEMP_29:
        case RS485_AIR_SET_TEMP_30:
            *(value + 4) = indoor_status[handle->device_addr][0];
            /* low 8 bit, mask: 1111 1111, 0000 0000 */
            indoor_status[handle->device_addr][1] &= 0xff00;
            indoor_status[handle->device_addr][1] |= handle->method;
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;

        case RS485_AIR_SWING_AUTO:
        case RS485_AIR_SWING_UP_DOWN:
        case RS485_AIR_SWING_LEFT_RIGHT:
            /* low 8 bit, mask: 1111 1111, 1111, 1101 */
            indoor_status[handle->device_addr][0] &= 0xfffd;
            indoor_status[handle->device_addr][0] |= SET_SWING_ON;
            *(value + 4) = indoor_status[handle->device_addr][0];
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;
        case RS485_AIR_SWING_UP_DOWN_LEFT_RIGHT:
            indoor_status[handle->device_addr][0] &= 0xfffd;
            indoor_status[handle->device_addr][0] |= SET_SWING_OFF;
            *(value + 4) = indoor_status[handle->device_addr][0];
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;

        case RS485_AIR_FAN_AUTO:
            *(value + 4) = indoor_status[handle->device_addr][0];
            /* high 8 bit, mask: 0000 0000, 1111 1111 */
            indoor_status[handle->device_addr][1] &= 0x00ff;
            indoor_status[handle->device_addr][1] |= (SET_FAN_AUTO << 8);
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;
        case RS485_AIR_FAN_HIGH:
            *(value + 4) = indoor_status[handle->device_addr][0];
            indoor_status[handle->device_addr][1] &= 0x00ff;
            indoor_status[handle->device_addr][1] |= (SET_FAN_HIGH << 8);
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;
        case RS485_AIR_FAN_MIDDLE:
            *(value + 4) = indoor_status[handle->device_addr][0];
            indoor_status[handle->device_addr][1] &= 0x00ff;
            indoor_status[handle->device_addr][1] |= (SET_FAN_MIDDLE << 8);
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;
        case RS485_AIR_FAN_LOW:
            *(value + 4) = indoor_status[handle->device_addr][0];
            indoor_status[handle->device_addr][1] &= 0x00ff;
            indoor_status[handle->device_addr][1] |= (SET_FAN_LOW << 8);
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;

        case RS485_AIR_MODE_FANING:
            indoor_status[handle->device_addr][0] &= 0x00ff;
            indoor_status[handle->device_addr][0] |= (SET_MODE_FANING << 8);
            *(value + 4) = indoor_status[handle->device_addr][0];
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;
        case RS485_AIR_MODE_HEATING:
            indoor_status[handle->device_addr][0] &= 0x00ff;
            indoor_status[handle->device_addr][0] |= (SET_MODE_HEATING << 8);
            *(value + 4) = indoor_status[handle->device_addr][0];
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;
        case RS485_AIR_MODE_COOLING:
            indoor_status[handle->device_addr][0] &= 0x00ff;
            indoor_status[handle->device_addr][0] |= (SET_MODE_COOLING << 8);
            *(value + 4) = indoor_status[handle->device_addr][0];
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;
        case RS485_AIR_MODE_DRYING:
            indoor_status[handle->device_addr][0] &= 0x00ff;
            indoor_status[handle->device_addr][0] |= (SET_MODE_DRYING << 8);
            *(value + 4) = indoor_status[handle->device_addr][0];
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;
        case RS485_AIR_MODE_AUTOING:
            indoor_status[handle->device_addr][0] &= 0x00ff;
            indoor_status[handle->device_addr][0] |= (SET_MODE_DRYING << 8);
            *(value + 4) = indoor_status[handle->device_addr][0];
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;

        case RS485_AIR_OFF:
            indoor_status[handle->device_addr][0] &= 0x00ff;
            indoor_status[handle->device_addr][0] |= (SET_POWER_OFF << 8);
            *(value + 4) = indoor_status[handle->device_addr][0];
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;
        case RS485_AIR_ON:
            indoor_status[handle->device_addr][0] &= 0x00ff;
            indoor_status[handle->device_addr][0] |= (SET_POWER_ON << 8);
            *(value + 4) = indoor_status[handle->device_addr][0];
            *(value + 5) = indoor_status[handle->device_addr][1];
            break;

        default:
            handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
            return 0;
    }

    handle->device_addr = JOHNSON_CONTROLS_VRV_DEFAULT_ADDRESS;

    /* return the frame length is 6 */
    return 6;
}

int johnson_controls_vrv_get_device_info_send(volatile void* arg)
{
    modbus_port_handle_t        *handle = (modbus_port_handle_t*)arg;

    if(handle == NULL)
        return -EINVAL;

    handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
#if 0
    handle->code = MODBUS_FUNCTION_CODE_READ_MULTIPLE_REGISTERS;

    /* The every air condition have a 6 hold register */
    handle->register_addr = handle->device_addr * AIR_CONDITION_INFO_REGISTER_LENGTH +
        AIR_CONDITION_DATA_REGISTER_START;

    handle->device_addr = JOHNSON_CONTROLS_VRV_DEFAULT_ADDRESS;
#endif

    return AIR_CONDITION_INFO_REGISTER_LENGTH;
}

int johnson_controls_vrv_get_device_info_handle(volatile void* arg)
{
    modbus_port_handle_t        *handle = (modbus_port_handle_t*)arg;
    uint16_t                    *value = (uint16_t*)handle->buffer;

    if(handle == NULL && (value == NULL))
        return -EINVAL;

    /* TODO: process data */

    return 0;
}



