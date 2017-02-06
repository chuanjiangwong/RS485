
/*
 * =====================================================================================
 *
 *       Filename:  mdev.c
 *
 *    Description:  device and bus mangage
 *
 *        Version:  1.0
 *        Created:  Dec 16, 2016 2:37:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#include <stdbool.h>

#include <errno.h>

#include "core.h"



#define RS485_DEVICE_MAX_NUMBERS  (1024)



static struct rs485_device *rs485_device_table[RS485_DEVICE_MAX_NUMBERS] = { NULL };


static bool check_id(int id)
{
    if(id >= RS485_DEVICE_MAX_NUMBERS)
        return false;
    else
        return true;
}



static int find_next_device_id(void)
{
    unsigned int i = 0;

    for(i=1; i<sizeof(rs485_device_table)/sizeof(struct rs485_device*); i++)
    {
        if(!rs485_device_table[i])
            return i;
    }

    return -EPERM;
}

int get_device_id(struct rs485_device* device)
{
    int id = find_next_device_id();

    if(!device)
        return -EPERM;

    /* set */
    rs485_device_table[id] = device;

    return id;
}

void free_device_id(int id)
{
    if(check_id(id))
        rs485_device_table[id] = NULL;
}

struct rs485_device* get_device_by_id(int id)
{
    struct rs485_device* device = NULL;

    if(check_id(id))
        device = rs485_device_table[id];

    return device;
}








