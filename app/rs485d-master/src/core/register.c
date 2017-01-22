
/*
 * =====================================================================================
 *
 *       Filename:  register.c
 *
 *    Description:  bus and driver register
 *
 *        Version:  1.0
 *        Created:  Jan 20, 2017 4:43:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#include <errno.h>

#include <register.h>



int bus_initcalls(const struct interface_profile * interface,
        const char* name)
{
    int retval = 0;
    bus_register_t *call_ptr = _register_bus_start;

    while(call_ptr < _register_bus_end)
    {
        retval = (*call_ptr)(interface, name);
        if(retval <= 0)
            return retval;

        ++call_ptr;
    }

    return -EPERM;
}

int bus_exitcalls(struct rs485_bus_type * bus)
{
    int retval = 0;
    bus_unregister_t *call_ptr = _unregister_bus_start;

    while(call_ptr < _unregister_bus_end)
    {
        retval = (*call_ptr)(bus);
        if(retval <= 0)
            return retval;

        ++call_ptr;
    }

    return -EPERM;
}

void driver_initcalls(const char* bus_name)
{
    driver_register_t *call_ptr = _register_driver_start;

    while(call_ptr < _register_driver_end)
    {
        (*call_ptr)(bus_name);

        ++call_ptr;
    }
}
