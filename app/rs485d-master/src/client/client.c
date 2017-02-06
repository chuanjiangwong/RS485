
/*
 * =====================================================================================
 *
 *       Filename:  client.c
 *
 *    Description:  device instance
 *
 *        Version:  1.0
 *        Created:  Jan 22, 2017 5:08:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#include <errno.h>
#include <stdio.h>

#include <string.h>
#include <stdlib.h>

#include <register.h>
#include <core.h>
#include <adapter.h>


struct rs485_driver_info_table
{
    rs485_factory_name_enum     device_type;
    const char*                 bus_name;
    const char*                 driver_name;
};

/* bus name  */
static const char custom_bus_str[] = "custom";
static const char bacnet_bus_str[] = "bacnet";
static const char modbus_bus_str[] = "modbus";


/* device client table */
static const struct rs485_driver_info_table driver_table[] =
{
    {RS485_CURTAIN_CUSTOMIZE_DOOYA_ALL,         custom_bus_str,    "curtain_custom_dooya"},

};



int get_client(struct rs485_device_info * info,
        const char* bus_port_name,
        rs485_factory_name_enum device_type,
        const unsigned char mac[4])
{
    unsigned int i = 0;
    const struct rs485_driver_info_table * driver = NULL;

    if(!info)
        return -EINVAL;

    for(i=0; i<sizeof(driver_table)/sizeof(struct rs485_driver_info_table); i++)
    {
        if(driver_table[i].device_type == device_type)
            driver = &driver_table[i];
    }
    if(driver == NULL)
        return -ENODEV;

    /* bus name format ---> 'custom-/dev/ttyS1' */
    info->match_driver = driver->driver_name;
    snprintf(info->match_bus, sizeof(info->match_bus),
            "%s-%s", driver->bus_name, bus_port_name);
    memcpy(info->mac, mac, sizeof(info->mac));

    return 0;
}


