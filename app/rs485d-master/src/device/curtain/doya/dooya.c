
/*
 * =====================================================================================
 *
 *       Filename:  dooya.c
 *
 *    Description:  dooya curtian
 *
 *        Version:  1.0
 *        Created:  Jan 20, 2017 1:55:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#include <errno.h>

#include <string.h>

#include <core.h>
#include <register.h>


static int dooya_get(void* buffer, int len, const void* arg, bool* broadcast)
{
    return 0;
}

static int dooya_put(const void* buffer, int len, const void* arg, void* profile)
{
    return 0;
}

static int dooya_probe(struct rs485_device* device, const struct rs485_device_info* info)
{
    return 0;
}

static int dooya_remove(struct rs485_device* device)
{
    return 0;
}





static const struct rs485_driver_data dooya_driver_data =
{
    .retransmission = 3,
    .receive_timeout_ms = 300,
    .broadcast = false,
    .broadcast_mask = 0x00,
};

static struct rs485_driver dooya_driver =
{
    .name       = "curtain_custom_dooya",
    .match_bus  = "custom",

    .probe      = dooya_probe,
    .remove     = dooya_remove,

    .get        = dooya_get,
    .put        = dooya_put,

    .data       = &dooya_driver_data,
};

static int rs485_dooya_driver_init(const char* name)
{
    if(!name)
        return -EINVAL;

    if(strncmp(dooya_driver.match_bus, name,
    				strlen(dooya_driver.match_bus)))
        return 1;

    return rs485_driver_register(&dooya_driver, name);
}


RS485_DRIVER_INIT(rs485_dooya_driver_init);



