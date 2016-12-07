/*
 * =====================================================================================
 *
 *          @file:  DTA116A621.c
 *
 *    Description:  The daikin air condition interface Shang Hai dta116a621
 *
 *        Version:  1.0
 *        Created:  Apr 19, 2016 5:19:30 PM
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

#include "enum.h"
#include "adapter.h"
#include "device/airCondition/daikin/DTA116A621.h"
#include "protocol/modbus/modbus.h"
#include "syslog/log.h"


/* -----------------------------------------*/
/**
 * @brief define the HA adapter enum
 */
/* -----------------------------------------*/
enum DAIKIN_HA_ADAPTER_STATUS_ENUM
{
    HA_ADAPTER_STATUS_WAIT_INIT,
    HA_ADAPTER_STATUS_WAIT_CONNECT,
    HA_ADAPTER_STATUS_NORMAL,
    HA_ADAPTER_STATUS_ERROR,
};

#define DAIKIN_INPUT_REGISTER_START                         (30001)
#define DAIKIN_INPUT_REGISTER_DEVICE_CONNECT_STATUS_START   (30002)
#define DAIKIN_INPUT_REGISTER_DEVICE_CONNECT_STATUS_STOP    (30005)
#define DAIKIN_INPUT_REGISTER_DEVICE_COMM_STATUS_START      (30006)
#define DAIKIN_INPUT_REGISTER_DEVICE_COMM_STATUS_STOP       (30009)
#define DAIKIN_INPUT_REGISTER_DEVICE_FUNCTION_STATUS_START  (31001)
#define DAIKIN_INPUT_REGISTER_DEVICE_FUNCTION_STATUS_STOP   (31192)
#define DAIKIN_INPUT_REGISTER_DEVICE_STATUS_START           (32001)
#define DAIKIN_INPUT_REGISTER_DEVICE_STATUS_STOP            (32384)

#define DAIKIN_HOLD_REGISTER_START                          (40001)
#define DAIKIN_HOLD_REGISTER_DEVICE_CONTROL_START           (42001)
#define DAIKIN_HOLD_REGISTER_DEVICE_CONTROL_STOP            (42192)

#define DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET (0)
#define DAIKIN_HOLD_REGISTER_DEVICE_MODE_OFFSET             (1)
#define DAIKIN_HOLD_REGISTER_DEVICE_TEMPERATURE_OFFSET      (2)

#define DAIKIN_HA_ADAPTER_DEFAULT_ADDRESS                   (0x02)

/** The 64 indoor machine device status */
static uint16_t glb_daikin_hold_register_value[64][3] =
{
    /* on_off_swing_fan */ /* mode */ /* temperature */
    /* off / auto / auto*/ /* auto */ /*  25 */
    { 0x3761,                 0x0103,     250 },                // 1
    { 0x3761,                 0x0103,     250 },                // 2
    { 0x3761,                 0x0103,     250 },                // 3
    { 0x3761,                 0x0103,     250 },                // 4
    { 0x3761,                 0x0103,     250 },                // 5
    { 0x3761,                 0x0103,     250 },                // 6
    { 0x3761,                 0x0103,     250 },                // 7
    { 0x3761,                 0x0103,     250 },                // 8
    { 0x3761,                 0x0103,     250 },                // 9
    { 0x3761,                 0x0103,     250 },                // 11
    { 0x3761,                 0x0103,     250 },                // 12
    { 0x3761,                 0x0103,     250 },                // 13
    { 0x3761,                 0x0103,     250 },                // 14
    { 0x3761,                 0x0103,     250 },                // 15
    { 0x3761,                 0x0103,     250 },                // 16
    { 0x3761,                 0x0103,     250 },                // 17
    { 0x3761,                 0x0103,     250 },                // 18
    { 0x3761,                 0x0103,     250 },                // 19
    { 0x3761,                 0x0103,     250 },                // 20
    { 0x3761,                 0x0103,     250 },                // 21
    { 0x3761,                 0x0103,     250 },                // 22
    { 0x3761,                 0x0103,     250 },                // 23
    { 0x3761,                 0x0103,     250 },                // 24
    { 0x3761,                 0x0103,     250 },                // 25
    { 0x3761,                 0x0103,     250 },                // 26
    { 0x3761,                 0x0103,     250 },                // 27
    { 0x3761,                 0x0103,     250 },                // 28
    { 0x3761,                 0x0103,     250 },                // 29
    { 0x3761,                 0x0103,     250 },                // 30
    { 0x3761,                 0x0103,     250 },                // 31
    { 0x3761,                 0x0103,     250 },                // 32
    { 0x3761,                 0x0103,     250 },                // 33
    { 0x3761,                 0x0103,     250 },                // 34
    { 0x3761,                 0x0103,     250 },                // 35
    { 0x3761,                 0x0103,     250 },                // 36
    { 0x3761,                 0x0103,     250 },                // 37
    { 0x3761,                 0x0103,     250 },                // 38
    { 0x3761,                 0x0103,     250 },                // 39
    { 0x3761,                 0x0103,     250 },                // 40
    { 0x3761,                 0x0103,     250 },                // 41
    { 0x3761,                 0x0103,     250 },                // 42
    { 0x3761,                 0x0103,     250 },                // 43
    { 0x3761,                 0x0103,     250 },                // 44
    { 0x3761,                 0x0103,     250 },                // 45
    { 0x3761,                 0x0103,     250 },                // 46
    { 0x3761,                 0x0103,     250 },                // 47
    { 0x3761,                 0x0103,     250 },                // 48
    { 0x3761,                 0x0103,     250 },                // 49
    { 0x3761,                 0x0103,     250 },                // 50
    { 0x3761,                 0x0103,     250 },                // 51
    { 0x3761,                 0x0103,     250 },                // 52
    { 0x3761,                 0x0103,     250 },                // 53
    { 0x3761,                 0x0103,     250 },                // 54
    { 0x3761,                 0x0103,     250 },                // 55
    { 0x3761,                 0x0103,     250 },                // 56
    { 0x3761,                 0x0103,     250 },                // 57
    { 0x3761,                 0x0103,     250 },                // 58
    { 0x3761,                 0x0103,     250 },                // 59
    { 0x3761,                 0x0103,     250 },                // 60
    { 0x3761,                 0x0103,     250 },                // 61
    { 0x3761,                 0x0103,     250 },                // 62
    { 0x3761,                 0x0103,     250 },                // 63
    { 0x3761,                 0x0103,     250 },                // 64
};


/** The HA machine max have connect 64 indoor machine */
static bool indoor_machine_connect_status[64] = {false};

/** wait the HA adapter machine initialize finish */
static bool ha_adapter_initialize_status = false;

/** used to jument the ha status */
static enum DAIKIN_HA_ADAPTER_STATUS_ENUM glb_ha_status = HA_ADAPTER_STATUS_WAIT_INIT;
/* -----------------------------------------*/
/**
 * @brief set_indoor_machine_connect_status
 *  set the indoor machine connect status
 *
 * @param[in] machine   : The machine number, should have less than 64
 * @param[in] status    : The machine status
 */
/* -----------------------------------------*/
void inline set_indoor_machine_connect_status(int machine, bool status)
{
    if(machine < 0 && machine > 64)
        return ;

    indoor_machine_connect_status[machine] = status;
}

/* -----------------------------------------*/
/**
 * @brief set_ha_adapter_initialize_status
 *  set the ha system initialize status, while initialize finish, have set it
 *
 * @param[in] status    : The initialize finish status
 */
/* -----------------------------------------*/
void inline set_ha_adapter_initialize_status(bool status)
{
    ha_adapter_initialize_status = status;
}

/* -----------------------------------------*/
/**
 * @brief get_daikin_write_register_addr
 *
 * @param[in] device_addr : 0~63
 *
 * @return  The daikin register address
 */
/* -----------------------------------------*/
static inline uint16_t get_daikin_write_register_addr(unsigned char device_addr)
{
    return device_addr * 3 +
        (DAIKIN_HOLD_REGISTER_DEVICE_CONTROL_START - DAIKIN_HOLD_REGISTER_START);
}


/* -----------------------------------------*/
/**
 * @brief get_daikin_hold_register_value
 *  get the daikon air condition control value
 *
 * @param[in] method        : The rs485 device method
 * @param[in] value         : The rs485 device method vlaue
 * @param[in] device_addr   : The rs485 device device address
 *
 * @return  a 16bit hold register value, if have a error, return -1
 */
/* -----------------------------------------*/
static uint16_t get_daikin_hold_register_value(rs485_device_method_enum method, int value, unsigned char device_addr)
{
    uint16_t    hold_register_value_new = -1;
    uint16_t    hold_register_value_old = 0;
    switch(method)
    {
        case RS485_AIR_SET_TEMP:
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
            hold_register_value_new = value * 10;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_TEMPERATURE_OFFSET] = hold_register_value_new;
            break;
        case RS485_AIR_SWING_AUTO:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET];
            /* clear the swing bit */
            hold_register_value_old &= 0xf0ff;
            /* get the swing value */
            /* set the dir is PO */
            hold_register_value_new = 0x0200;
            /* set the new value */
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET] = hold_register_value_new;

            break;
        case RS485_AIR_SWING_UP_DOWN:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET];
            /* clear the swing bit */
            hold_register_value_old &= 0xf0ff;
            /* get the swing value */
            /* set the dir is P4 */
            hold_register_value_new = 0x0400;
            /* set the new value */
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET] = hold_register_value_new;

            break;
        case RS485_AIR_SWING_LEFT_RIGHT:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET];
            /* clear the swing bit */
            hold_register_value_old &= 0xf0ff;
            /* get the swing value */
            /* set the dir is P0 */
            hold_register_value_new = 0x0000;
            /* set the new value */
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET] = hold_register_value_new;

            break;
        case RS485_AIR_SWING_UP_DOWN_LEFT_RIGHT:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET];
            /* clear the swing bit */
            hold_register_value_old &= 0xf0ff;
            /* get the swing value */
            /* set the dir is P7 */
            hold_register_value_new = 0x0700;
            /* set the new value */
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET] = hold_register_value_new;
            break;

        case RS485_AIR_FAN_AUTO:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET];
            /* clear the fan bit */
            hold_register_value_old &= 0x0fff;
            /* get the fan value */
            /* set the fan is middle*/
            hold_register_value_new = 0x3000;
            /* set the new value */
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET] = hold_register_value_new;
            break;

        case RS485_AIR_FAN_HIGH:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET];
            /* clear the fan bit */
            hold_register_value_old &= 0x0fff;
            /* get the fan value */
            /* set the fan is high*/
            hold_register_value_new = 0x5000;
            /* set the new value */
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET] = hold_register_value_new;
            break;
        case RS485_AIR_FAN_MIDDLE:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET];
            /* clear the fan bit */
            hold_register_value_old &= 0x0fff;
            /* get the fan value */
            /* set the fan is middle*/
            hold_register_value_new = 0x3000;
            /* set the new value */
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET] = hold_register_value_new;
            break;
        case RS485_AIR_FAN_LOW:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET];
            /* clear the fan bit */
            hold_register_value_old &= 0x0fff;
            /* get the fan value */
            /* set the fan is low*/
            hold_register_value_new = 0x1000;
            /* set the new value */
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET] = hold_register_value_new;
            break;

        case RS485_AIR_MODE_FANING:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_MODE_OFFSET];
            /* clear the mode bit */
            hold_register_value_old &= 0xfff0;
            /* get the mode value */
            hold_register_value_new = 0x0000;
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_MODE_OFFSET] = hold_register_value_new;
            break;
        case RS485_AIR_MODE_HEATING:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_MODE_OFFSET];
            /* clear the mode bit */
            hold_register_value_old &= 0xfff0;
            /* get the mode value */
            hold_register_value_new = 0x0001;
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_MODE_OFFSET] = hold_register_value_new;
            break;
        case RS485_AIR_MODE_COOLING:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_MODE_OFFSET];
            /* clear the mode bit */
            hold_register_value_old &= 0xfff0;
            /* get the mode value */
            hold_register_value_new = 0x0002;
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_MODE_OFFSET] = hold_register_value_new;
            break;
        case RS485_AIR_MODE_DRYING:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_MODE_OFFSET];
            /* clear the mode bit */
            hold_register_value_old &= 0xfff0;
            /* get the mode value */
            hold_register_value_new = 0x0007;
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_MODE_OFFSET] = hold_register_value_new;
            break;
        case RS485_AIR_MODE_AUTOING:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_MODE_OFFSET];
            /* clear the mode bit */
            hold_register_value_old &= 0xfff0;
            /* get the mode value */
            hold_register_value_new = 0x0103;
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_MODE_OFFSET] = hold_register_value_new;
            break;

        case RS485_AIR_OFF:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET];
            /* clear the swing bit */
            hold_register_value_old &= 0xfff0;
            /* get the swing value */
            /* set the dir is PO */
            hold_register_value_new = 0x0000;
            /* set the new value */
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET] = hold_register_value_new;
            break;
        case RS485_AIR_ON:
            hold_register_value_old = glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET];
            /* clear the swing bit */
            hold_register_value_old &= 0xfff0;
            /* get the swing value */
            /* set the dir is PO */
            hold_register_value_new = 0x0001;
            /* set the new value */
            hold_register_value_new |= hold_register_value_old;
            glb_daikin_hold_register_value[device_addr][DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET] = hold_register_value_new;
            break;

        default:
            return -1;

    }

    return hold_register_value_new;
}




int daikin_dta116a621_set_temperature(volatile void* arg)
{
    modbus_port_handle_t            *handle = (modbus_port_handle_t*)arg;
    uint16_t                        hold_register = 0, hold_register_value = 0;

    if(handle == NULL)
        return -1;

    if(glb_ha_status == false)
    {
        handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
        return 0;
    }

    /* full the function code */
    handle->code = MODBUS_FUNCTION_CODE_WRITE_SIGNLE_REGISTER;
    /* ful the hold_register */
    if(handle->device_addr > 63)
    {
        syslog_warning("[daikin]:The device addr:%u inval, except less than 64", handle->device_addr);
        return -1;
    }
    hold_register = get_daikin_write_register_addr(handle->device_addr);
    handle->register_addr = hold_register + DAIKIN_HOLD_REGISTER_DEVICE_TEMPERATURE_OFFSET;
    /*full the value */
    hold_register_value = get_daikin_hold_register_value(handle->method, handle->value, handle->device_addr);
    if(hold_register_value < 0)
    {
        syslog_warning("[daikin]The hold register value:%d fail", hold_register_value);
        return -1;
    }
    /* high byte value */
    handle->buffer[0] = hold_register_value >> 8;
    /* low  byte value */
    handle->buffer[1] = hold_register_value & 0xff;

    /* FIXME: change the device address */
    handle->device_addr = DAIKIN_HA_ADAPTER_DEFAULT_ADDRESS;

    /* return the send frame length */
    return 1;
}

int daikin_dta116a621_set_mode(volatile void* arg)
{
    modbus_port_handle_t            *handle = (modbus_port_handle_t*)arg;
    uint16_t                        hold_register = 0, hold_register_value = 0;

    if(handle == NULL)
        return -1;

    if(glb_ha_status == false)
    {
        handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
        return 0;
    }

    if(handle->method == RS485_AIR_FAN_AUTO)
    {
    	/*The daiking air condition havn't support auto mode*/
    	handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
    	return 0;
    }

    /* full the function code */
    handle->code = MODBUS_FUNCTION_CODE_WRITE_SIGNLE_REGISTER;
    /* ful the hold_register */
    if(handle->device_addr > 63)
    {
        syslog_warning("[daikin]:The device addr:%u inval, except less than 64", handle->device_addr);
        return -1;
    }
    hold_register = get_daikin_write_register_addr(handle->device_addr);
    handle->register_addr = hold_register + DAIKIN_HOLD_REGISTER_DEVICE_MODE_OFFSET;
    /*full the value */
    hold_register_value = get_daikin_hold_register_value(handle->method, handle->value, handle->device_addr);
    if(hold_register_value < 0)
    {
        syslog_warning("[daikin]The hold register value:%d fail", hold_register_value);
        return -1;
    }
    /* high byte value */
    handle->buffer[0] = hold_register_value >> 8;
    /* low  byte value */
    handle->buffer[1] = hold_register_value & 0xff;

    /* FIXME: change the device address */
    handle->device_addr = DAIKIN_HA_ADAPTER_DEFAULT_ADDRESS;

    /* return the send frame length */
    return 1;
}

int daikin_dta116a621_set_swing(volatile void* arg)
{
    modbus_port_handle_t            *handle = (modbus_port_handle_t*)arg;
    uint16_t                        hold_register = 0, hold_register_value = 0;

    if(handle == NULL)
        return -1;

    if(glb_ha_status == false)
    {
        handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
        return 0;
    }

    /* full the function code */
    handle->code = MODBUS_FUNCTION_CODE_WRITE_SIGNLE_REGISTER;
    /* ful the hold_register */
    if(handle->device_addr > 63)
    {
        syslog_warning("[daikin]:The device addr:%u inval, except less than 64", handle->device_addr);
        return -1;
    }
    hold_register = get_daikin_write_register_addr(handle->device_addr);
    handle->register_addr = hold_register + DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET;
    /*full the value */
    hold_register_value = get_daikin_hold_register_value(handle->method, handle->value, handle->device_addr);
    if(hold_register_value < 0)
    {
        syslog_warning("[daikin]The hold register value:%d fail", hold_register_value);
        return -1;
    }
    /* high byte value */
    handle->buffer[0] = hold_register_value >> 8;
    /* low  byte value */
    handle->buffer[1] = hold_register_value & 0xff;

    /* FIXME: change the device address */
    handle->device_addr = DAIKIN_HA_ADAPTER_DEFAULT_ADDRESS;

    /* return the send frame length */
    return 1;
}

int daikin_dta116a621_set_fan(volatile void* arg)
{
    modbus_port_handle_t            *handle = (modbus_port_handle_t*)arg;
    uint16_t                        hold_register = 0, hold_register_value = 0;

    if(handle == NULL)
        return -1;

    if(glb_ha_status == false)
    {
        handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
        return 0;
    }

    /* full the function code */
    handle->code = MODBUS_FUNCTION_CODE_WRITE_SIGNLE_REGISTER;
    /* ful the hold_register */
    if(handle->device_addr > 63)
    {
        syslog_warning("[daikin]:The device addr:%u inval, except less than 64", handle->device_addr);
        return -1;
    }
    hold_register = get_daikin_write_register_addr(handle->device_addr);
    handle->register_addr = hold_register + DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET;
    /*full the value */
    hold_register_value = get_daikin_hold_register_value(handle->method, handle->value, handle->device_addr);
    if(hold_register_value < 0)
    {
        syslog_warning("[daikin]The hold register value:%d fail", hold_register_value);
        return -1;
    }
    /* high byte value */
    handle->buffer[0] = hold_register_value >> 8;
    /* low  byte value */
    handle->buffer[1] = hold_register_value & 0xff;

    /* FIXME: change the device address */
    handle->device_addr = DAIKIN_HA_ADAPTER_DEFAULT_ADDRESS;

    /* return the send frame length */
    return 1;
}

int daikin_dta116a621_set_switch(volatile void* arg)
{
    modbus_port_handle_t            *handle = (modbus_port_handle_t*)arg;
    uint16_t                        hold_register = 0, hold_register_value = 0;

    if(handle == NULL)
        return -1;

    if(glb_ha_status == false)
    {
        handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
        return 0;
    }

    /* full the function code */
    handle->code = MODBUS_FUNCTION_CODE_WRITE_SIGNLE_REGISTER;
    /* ful the hold_register */
    if(handle->device_addr > 63)
    {
        syslog_warning("[daikin]:The device addr:%u inval, except less than 64", handle->device_addr);
        return -1;
    }
    hold_register = get_daikin_write_register_addr(handle->device_addr);
    handle->register_addr = hold_register + DAIKIN_HOLD_REGISTER_DEVICE_ON_OFF_SWING_FAN_OFFSET;
    /*full the value */
    hold_register_value = get_daikin_hold_register_value(handle->method, handle->value, handle->device_addr);
    if(hold_register_value < 0)
    {
        syslog_warning("[daikin]The hold register value:%d fail", hold_register_value);
        return -1;
    }
    /* high byte value */
    handle->buffer[0] = hold_register_value >> 8;
    /* low  byte value */
    handle->buffer[1] = hold_register_value & 0xff;

    /* FIXME: change the device address */
    handle->device_addr = DAIKIN_HA_ADAPTER_DEFAULT_ADDRESS;

    /* return the send frame length, have a 16bits */
    return 1;
}


int daikin_dta116a621_get_device_info_send(volatile void* arg)
{
    modbus_port_handle_t    *handle = (modbus_port_handle_t*)arg;
    int                     send_frame_len = 0;

    if(handle == NULL)
        return -1;

    switch(glb_ha_status)
    {
        case HA_ADAPTER_STATUS_WAIT_INIT:
            handle->code = MODBUS_FUNCTION_CODE_READ_INPUT_REGISTER;
            handle->register_addr = DAIKIN_INPUT_REGISTER_START - DAIKIN_INPUT_REGISTER_START;
            send_frame_len = 1;
            break;

        case HA_ADAPTER_STATUS_WAIT_CONNECT:
            handle->code = MODBUS_FUNCTION_CODE_READ_MULTIPLE_REGISTERS;
            handle->register_addr = DAIKIN_INPUT_REGISTER_DEVICE_CONNECT_STATUS_START - DAIKIN_INPUT_REGISTER_START;
            send_frame_len = 4;
            break;

        case HA_ADAPTER_STATUS_NORMAL:
            handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
            break;

        case HA_ADAPTER_STATUS_ERROR:
            handle->code = MODBUS_FUNCTION_CODE_DO_NOTHING;
            break;

        default:
            break;
    }


    /* FIXME: change the device address */
    handle->device_addr = DAIKIN_HA_ADAPTER_DEFAULT_ADDRESS;

    return send_frame_len;
}

int daikin_dta116a621_get_device_info_handle(volatile void* arg)
{
    modbus_port_handle_t    *handle = (modbus_port_handle_t*)arg;
    uint16_t                *value = (uint16_t*)handle->buffer;
    int                     i, j;
    bool                    status;

    if(handle == NULL && (value == NULL))
        return -1;

    switch(glb_ha_status)
    {
        case HA_ADAPTER_STATUS_WAIT_INIT:
            if(*value == true)
                glb_ha_status = HA_ADAPTER_STATUS_WAIT_CONNECT;
            break;

        case HA_ADAPTER_STATUS_WAIT_CONNECT:
            for(i=0; i<4; i++)
            {
                for(j=0; j<8; j++)
                {
                    status = (*(value + i) >> j) && 0x01;
                    set_indoor_machine_connect_status(i*j , status);
                }
            }
            glb_ha_status = HA_ADAPTER_STATUS_NORMAL;
            break;

        case HA_ADAPTER_STATUS_NORMAL:
            break;

        case HA_ADAPTER_STATUS_ERROR:
            break;

        default:
            break;
    }

    return 0;
}

