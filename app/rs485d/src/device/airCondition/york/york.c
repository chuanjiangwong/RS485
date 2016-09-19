/*
 * york.c
 *
 *  Created on: Nov 3, 2015
 *      Author: wong
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>       /* for time */
#include <string.h>

#include "protocol/bacnet/bacnet.h"
#include "enum.h"
#include "syslog/log.h"


enum york_air_conditioner_object
{
	AI_OUTDOOR_TEMPERATURE,
	AI_INDOOR_TEMPERATURE,
	AI_SET_TEMPERATURE,
	AI_INDOOR_HUMIDITY,
	AI_SET_HUMIDITY,
	AI_COMPRESSOR_FREQUENCY,
	AI_NET_CLEAR_TIME_LEFT,

	BI_ON_OFF_STATUS,
	BI_ERROR_RESET_STATUS,
	BI_NET_CLEAR_RESET_STATUS,
	BI_SLEEP_STATUS,
	BI_ELECTRICAL_HEAT_STATUS,
	BI_HEALTH_AIR_STATUS,
	BI_HOT_WATER_STATUS,
	BI_NEW_AIR_STATUS,
	BI_FIX_RUN_STATUS,
	BI_SAVING_STATUS,
	BI_DEFROST,
	BI_COMPRESSOR_RUNNING_STATUS,
	BI_COOL_ONLY_STATUS,
	BI_CENTRAL_CONONLY_STATUS,

	MI_RESERVED,
	MI_MODE,
	MI_FAN,
	MI_SWING,
	MI_VENTILATION,
	MI_LOCAL_SET,
	MI_ERROR,
	MI_COMMUNICATION_STATUS,
	MI_INDOOR_STYLE,

	AV_SET_TEMPERATURE,
	AV_SET_HUMIDUTY,
};


static const int york_air_condition_object[][7] =
{
//  {object type, instance, property, priority, index, value type, value}
//    {2,  0,  85, 16, -1, 4, 16},
//    {2,  0,  85, 16, -1, 4, 17},
    {2,  0,  85, 16, -1, 4, 18},                                        // 0
    {2,  0,  85, 16, -1, 4, 19},                                        // 1
    {2,  0,  85, 16, -1, 4, 20},                                        // 2
    {2,  0,  85, 16, -1, 4, 21},                                        // 3
    {2,  0,  85, 16, -1, 4, 22},                                        // 4
    {2,  0,  85, 16, -1, 4, 23},                                        // 5
    {2,  0,  85, 16, -1, 4, 24},                                        // 6
    {2,  0,  85, 16, -1, 4, 25},                                        // 7
    {2,  0,  85, 16, -1, 4, 26},                                        // 8
    {2,  0,  85, 16, -1, 4, 27},                                        // 9
    {2,  0,  85, 16, -1, 4, 28},                                        // 10
    {2,  0,  85, 16, -1, 4, 29},                                        // 11
    {2,  0,  85, 16, -1, 4, 30},                                        // 12
    {2,  0,  85, 16, -1, 4, 31},                                        // 13
    {2,  0,  85, 16, -1, 4, 32},                                        // 14

    {2,  1,  85, 16, -1, 4, 60},    //set humidity 60%                  // 15

    {14,  2,  85, 16, -1, 2, 0},	//set swing auto                    // 16
    {14,  2,  85, 16, -1, 2, 1},	//set swing up down                 // 17
    {14,  2,  85, 16, -1, 2, 2},	//set swing	left right              // 18
    {14,  2,  85, 16, -1, 2, 3},	//set swing up down left right      // 19

    {14,  1,  85, 16, -1, 2, 0},	//set fan auto                      // 20
    {14,  1,  85, 16, -1, 2, 1},	//set fan high                      // 21
    {14,  1,  85, 16, -1, 2, 3},	//set fan middle                    // 22
    {14,  1,  85, 16, -1, 2, 2},	//set fan low                       // 23

    {14,  0,  85, 16, -1, 2, 0},	//set mode fan                      // 24
    {14,  0,  85, 16, -1, 2, 1},	//set mode heat                     // 25
    {14,  0,  85, 16, -1, 2, 2},	//set mode cool                     // 26
    {14,  0,  85, 16, -1, 2, 3},	//set mode dry                      // 27                        //24
    {14,  0,  85, 16, -1, 2, 4},	//set mode auto                     // 28

    {5,  0,  85, 16, -1, 9, 0},		//OFF                               // 29
    {5,  0,  85, 16, -1, 9, 1},		//ON                                // 30

    {5,  1,  85, 16, -1, 9, 1},		//error reset yes
    {5,  1,  85, 16, -1, 9, 0},		//error reset no

    {5,  2,  85, 16, -1, 9, 1},		//net reset yes
    {5,  2,  85, 16, -1, 9, 0},		//net reset no

    {5,  3,  85, 16, -1, 9, 1},		//sleep yes
    {5,  3,  85, 16, -1, 9, 0},		//sleep no

    {5,  4,  85, 16, -1, 9, 1},		//electrical heat yes
    {5,  4,  85, 16, -1, 9, 0},		//electrical heat no

    {5,  5,  85, 16, -1, 9, 1},		//health air yes
    {5,  5,  85, 16, -1, 9, 0},		//health air no

    {5,  6,  85, 16, -1, 9, 1},		//hot water yes
    {5,  6,  85, 16, -1, 9, 0},		//hot water no

    {5,  7,  85, 16, -1, 9, 1},		//home left yes
    {5,  7,  85, 16, -1, 9, 0},		//home left no

    {5,  8,  85, 16, -1, 9, 1},		//fix run yes
    {5,  8,  85, 16, -1, 9, 0},		//fix run no

    {5,  9,  85, 16, -1, 9, 1},		//saving yes
    {5,  9,  85, 16, -1, 9, 0},		//saving no

    {5,  10,  85, 16, -1, 9, 1},	//defrost yes
    {5,  10,  85, 16, -1, 9, 0},	//defrost no

    {5,  11,  85, 16, -1, 9, 1},	//cool only yes
    {5,  11,  85, 16, -1, 9, 0},	//cool only no

    {5,  12,  85, 16, -1, 9, 1},	//central control only yes
    {5,  12,  85, 16, -1, 9, 0},	//central control only no
};

static const int york_air_condition_read_property[][3]=
{
//	{"enno"},{"id"},
//	{"0"},{"0"},{"85"},
//	{"0"},{"1"},{"85"},
//	{"0"},{"2"},{"85"},
//	{"0"},{"3"},{"85"},
//	{"0"},{"4"},{"85"},
//	{"0"},{"5"},{"85"},
//	{"3"},{"1"},{"85"},

	{0, 0, 85},
	{0, 1, 85},
	{0, 2, 85},
	{0, 3, 85},
	{0, 4, 85},
	{0, 5, 85},
	{3, 1, 85},
};


static int get_air_york_write_args(bacnet_write_args_t* args, unsigned int device_id,
        int command, char* value)
{
	if(args == NULL)
		return -1;

	if(command < RS485_AIR_SET_TEMP_18 &&
	  (command > RS485_AIR_GET_DEVICE_INFO))
		return -1;

    switch(command)
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
            args->device_id 				= device_id;
            args->object_type 				= york_air_condition_object[command - RS485_AIR_SET_TEMP_18][0];
            args->object_instance 			= york_air_condition_object[command - RS485_AIR_SET_TEMP_18][1];
            args->object_property 			= york_air_condition_object[command - RS485_AIR_SET_TEMP_18][2];
            args->object_property_priority 	= york_air_condition_object[command - RS485_AIR_SET_TEMP_18][3];
            args->object_property_value_type= york_air_condition_object[command - RS485_AIR_SET_TEMP_18][5];

            if(york_air_condition_object[command - RS485_AIR_SET_TEMP_18][4] == -1)
                args->object_property_index = 0xFFFFFFFFU;

            sprintf(args->object_property_value, "%d",
                    york_air_condition_object[command - RS485_AIR_SET_TEMP_18][6]);
            break;

        case RS485_AIR_SWING_AUTO:
        case RS485_AIR_SWING_UP_DOWN:
        case RS485_AIR_SWING_LEFT_RIGHT:
        case RS485_AIR_SWING_UP_DOWN_LEFT_RIGHT:
            args->device_id 				= device_id;
            args->object_type 				= york_air_condition_object[command - RS485_AIR_SWING_AUTO + 16][0];
            args->object_instance 			= york_air_condition_object[command - RS485_AIR_SWING_AUTO + 16][1];
            args->object_property 			= york_air_condition_object[command - RS485_AIR_SWING_AUTO + 16][2];
            args->object_property_priority 	= york_air_condition_object[command - RS485_AIR_SWING_AUTO + 16][3];
            args->object_property_value_type= york_air_condition_object[command - RS485_AIR_SWING_AUTO + 16][5];

            if(york_air_condition_object[command - RS485_AIR_SWING_AUTO + 16][4] == -1)
                args->object_property_index = 0xFFFFFFFFU;

            sprintf(args->object_property_value, "%d",
                    york_air_condition_object[command - RS485_AIR_SWING_AUTO + 16][6]);
            break;

        case RS485_AIR_FAN_AUTO:
        case RS485_AIR_FAN_HIGH:
        case RS485_AIR_FAN_MIDDLE:
        case RS485_AIR_FAN_LOW:
            args->device_id 				= device_id;
            args->object_type 				= york_air_condition_object[command - RS485_AIR_FAN_AUTO + 20][0];
            args->object_instance 			= york_air_condition_object[command - RS485_AIR_FAN_AUTO + 20][1];
            args->object_property 			= york_air_condition_object[command - RS485_AIR_FAN_AUTO + 20][2];
            args->object_property_priority 	= york_air_condition_object[command - RS485_AIR_FAN_AUTO + 20][3];
            args->object_property_value_type= york_air_condition_object[command - RS485_AIR_FAN_AUTO + 20][5];

            if(york_air_condition_object[command - RS485_AIR_FAN_AUTO + 20][4] == -1)
                args->object_property_index = 0xFFFFFFFFU;

            sprintf(args->object_property_value, "%d",
                    york_air_condition_object[command - RS485_AIR_FAN_AUTO + 20][6]);
            break;

        case RS485_AIR_MODE_COOLING:
        case RS485_AIR_MODE_DRYING:
        case RS485_AIR_MODE_AUTOING:
        case RS485_AIR_MODE_HEATING:
        case RS485_AIR_MODE_FANING:
            args->device_id 				= device_id;
            args->object_type 				= york_air_condition_object[command - RS485_AIR_MODE_FANING+ 24][0];
            args->object_instance 			= york_air_condition_object[command - RS485_AIR_MODE_FANING+ 24][1];
            args->object_property 			= york_air_condition_object[command - RS485_AIR_MODE_FANING+ 24][2];
            args->object_property_priority 	= york_air_condition_object[command - RS485_AIR_MODE_FANING+ 24][3];
            args->object_property_value_type= york_air_condition_object[command - RS485_AIR_MODE_FANING+ 24][5];

            if(york_air_condition_object[command - RS485_AIR_FAN_AUTO + 24][4] == -1)
                args->object_property_index = 0xFFFFFFFFU;

            sprintf(args->object_property_value, "%d",
                    york_air_condition_object[command - RS485_AIR_FAN_AUTO + 24][6]);
            break;

        case RS485_AIR_OFF:
        case RS485_AIR_ON:
            args->device_id 				= device_id;
            args->object_type 				= york_air_condition_object[command - RS485_AIR_OFF+ 29][0];
            args->object_instance 			= york_air_condition_object[command - RS485_AIR_OFF+ 29][1];
            args->object_property 			= york_air_condition_object[command - RS485_AIR_OFF+ 29][2];
            args->object_property_priority 	= york_air_condition_object[command - RS485_AIR_OFF+ 29][3];
            args->object_property_value_type= york_air_condition_object[command - RS485_AIR_OFF+ 29][5];

            if(york_air_condition_object[command - RS485_AIR_OFF+ 29][4] == -1)
                args->object_property_index = 0xFFFFFFFFU;

            sprintf(args->object_property_value, "%d",
                    york_air_condition_object[command - RS485_AIR_OFF+ 29][6]);
            break;

        default:
            syslog_warning("[york]:have no support command:%d", command);
            return -EPERM;
            /* TODO: */
    }

	return 0;
}

static int get_air_york_read_args(bacnet_read_args_t* args, unsigned int device_id)
{
	int i;

	if(args == NULL)
		return -1;

	args->device_id = device_id;
	args->object_numbers = 7;

	for(i=0; i<args->object_numbers; i++)
	{
		args->object_type[i] 		= york_air_condition_read_property[i][0];
		args->object_instance[i] 	= york_air_condition_read_property[i][1];
		args->object_property[i] 	= york_air_condition_read_property[i][2];
	}

	return 0;
}


static int get_air_york_instance(unsigned char mac)
{
    int instance = 0;

    if(mac >= 127)
        return -EPERM;

    switch(mac)
    {
        case 0x11:
            instance = 10001;
            break;

        case 0x12:
            instance = 10002;
            break;

        case 0x13:
            instance = 10003;
            break;

        case 0x14:
            instance = 10004;
            break;

        case 0x15:
            instance = 10005;
            break;

        case 0x16:
            instance = 10006;
            break;

        case 0x17:
            instance = 10007;
            break;

        case 0x18:
            instance = 10008;
            break;

        case 0x19:
            instance = 10009;
            break;

        case 0x1a:
            instance = 10010;
            break;

        case 0x1b:
            instance = 10011;
            break;

        case 0x1c:
            instance = 10012;
            break;

        case 0x1d:
            instance = 10013;
            break;

        case 0x1e:
            instance = 10014;
            break;

        case 0x1f:
            instance = 10015;
            break;

        default:
            syslog_warning("%s", "instance error");
            return -EPERM;
    }

    return instance;

}




int york_kelong_set_temperature(volatile void* arg)
{
    bacnet_port_handle_t            *handle = (bacnet_port_handle_t*)arg;

    if(handle == NULL)
        return -1;

    handle->code = BACNET_FUNCTION_CODE_WRITE_PROPERTY;
    handle->write_arg.device_id = get_air_york_instance(handle->mac[0]);

    return get_air_york_write_args(&(handle->write_arg), handle->write_arg.device_id, handle->method , NULL);
}


int york_kelong_set_mode(volatile void* arg)
{
    bacnet_port_handle_t            *handle = (bacnet_port_handle_t*)arg;

    if(handle == NULL)
        return -1;

    handle->code = BACNET_FUNCTION_CODE_WRITE_PROPERTY;
    handle->write_arg.device_id = get_air_york_instance(handle->mac[0]);

    return get_air_york_write_args(&(handle->write_arg), handle->write_arg.device_id, handle->method , NULL);
}



int york_kelong_set_swing(volatile void* arg)
{
    bacnet_port_handle_t            *handle = (bacnet_port_handle_t*)arg;

    if(handle == NULL)
        return -1;

    handle->code = BACNET_FUNCTION_CODE_WRITE_PROPERTY;
    handle->write_arg.device_id = get_air_york_instance(handle->mac[0]);

    return get_air_york_write_args(&(handle->write_arg), handle->write_arg.device_id, handle->method , NULL);
}


int york_kelong_set_fan(volatile void* arg)
{
    bacnet_port_handle_t            *handle = (bacnet_port_handle_t*)arg;

    if(handle == NULL)
        return -1;

    handle->code = BACNET_FUNCTION_CODE_WRITE_PROPERTY;
    handle->write_arg.device_id = get_air_york_instance(handle->mac[0]);

    return get_air_york_write_args(&(handle->write_arg), handle->write_arg.device_id, handle->method , NULL);
}


int york_kelong_set_switch(volatile void* arg)
{
    bacnet_port_handle_t            *handle = (bacnet_port_handle_t*)arg;

    if(handle == NULL)
        return -1;

    handle->code = BACNET_FUNCTION_CODE_WRITE_PROPERTY;
    handle->write_arg.device_id = get_air_york_instance(handle->mac[0]);

    return get_air_york_write_args(&(handle->write_arg), handle->write_arg.device_id, handle->method , NULL);
}


int york_kelong_get_device_info_send(volatile void* arg)
{
    bacnet_port_handle_t            *handle = (bacnet_port_handle_t*)arg;

    if(handle == NULL)
        return -1;

    handle->code = BACNET_FUNCTION_CODE_READ_PROPERTY;
    handle->write_arg.device_id = get_air_york_instance(handle->mac[0]);

    return get_air_york_read_args(&(handle->read_arg), handle->write_arg.device_id);
}


int york_kelong_get_device_info_handle(volatile void* arg)
{
    bacnet_port_handle_t            *handle = (bacnet_port_handle_t*)arg;

    if(handle == NULL)
        return -1;

    return 0;
}


