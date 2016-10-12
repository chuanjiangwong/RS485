
/*
 * =====================================================================================
 *
 *       Filename:  core.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Oct 8, 2016 4:33:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#include <wlog.h>

#include "core.h"


static int rs485_device_probe(struct device* dev)
{
    int status = 0;
    struct rs485_device *device = to_rs485_device(dev);
    struct rs485_driver *driver = to_rs485_driver(dev->driver);

    if(!driver->probe)
        return -ENODEV;

    wlog_debug("probe: %s", dev->name);

    device->driver = driver;
    status = driver->probe(device, driver->driver_data);
    if(status)
        device->driver = NULL;

    return status;
}

static int rs485_device_remove(struct device* dev)
{
    struct rs485_device *device = to_rs485_device(dev);
    struct rs485_driver *driver = NULL;
    int status = 0;

    if(!dev->driver)
        return 0;

    driver = to_rs485_driver(dev->driver);
    if(driver->remove)
    {
        wlog_debug("remove: dev:%s", dev->name);
        status = driver->remove(device);
    }
    else
    {
        dev->driver = NULL;
        status = 0;
    }

    if(status == 0)
        device->driver = NULL;

    return status;
}

static int rs485_device_suspend(struct device* dev)
{
    struct rs485_driver * driver = NULL;

    if(!dev->driver)
        return 0;

    driver = to_rs485_driver(dev->driver);
    if(!driver->suspend)
        return 0;

    return driver->suspend(to_rs485_device(dev));
}

static int rs485_device_resume(struct device * dev)
{
    struct rs485_driver* driver;

    if(!dev->driver)
        return 0;

    driver = to_rs485_driver(dev->driver);
    if(!driver->resume)
        return 0;

    return driver->resume(to_rs485_device(dev));
}

static int rs485_device_match(struct device* dev, struct device_driver* drv)
{
    struct rs485_device *device = to_rs485_device(dev);
    struct rs485_driver *driver = to_rs485_driver(drv);

    if(!(device->info.match_driver && driver->name))
        return -EINVAL;

    if(strcmp(device->info.match_driver, driver->name))
        return -EPERM;

    return 0;
}

int rs485_bus_register(struct rs485_bus_type* bus)
{
    int status = 0;

    if(!(bus && bus->name))
        return -EINVAL;

    if(!(bus->interface.port_name))
        return -EINVAL;

    /* TODO:set id */

    /* step1. set bus name , and register system bus*/
    bus->bus.name = bus->interface.port_name;
    bus->bus.probe = rs485_device_probe;
    bus->bus.remove = rs485_device_remove;
    bus->bus.suspend = rs485_device_suspend;
    bus->bus.resume = rs485_device_resume;
    bus->bus.match = rs485_device_match;
    status = bus_register(&bus->bus);
    if(status)
        goto sysbus_register_fail;

    /* step2. init the pthread */
    status = pthread_create(&bus->pthread, NULL, bus->pthread_function, bus->bus_data);
    if(status)
        goto pthread_init_fail;

    return 0;

pthread_init_fail:
    bus_unregister(&bus->bus);
sysbus_register_fail:
    return status;
}

void rs485_bus_unregister(struct rs485_bus_type* bus)
{
    if(!bus)
        return;

    /* step1. cancel the work pthread */
    pthread_cancel(bus->pthread);

    /* step2. unregister system bus */
    bus_unregister(&bus->bus);
}


static int rs485_driver_probe(struct device *dev)
{
    const struct rs485_driver *driver = to_rs485_driver(dev->driver);
    const struct rs485_device *device = to_rs485_device(dev);

    return driver->probe(to_rs485_device(dev), &device->info);
}

static int rs485_driver_remove(struct device *dev)
{
    const struct rs485_driver *driver = to_rs485_driver(dev->driver);

    return driver->remove(to_rs485_device(dev));
}

static int rs485_driver_suspend(struct device *dev)
{
    const struct rs485_driver *driver = to_rs485_driver(dev->driver);

    return driver->suspend(to_rs485_device(dev));
}

static int rs485_driver_resume(struct device *dev)
{
    const struct rs485_driver *driver = to_rs485_driver(dev->driver);

    return driver->resume(to_rs485_device(dev));
}


int rs485_driver_register(struct rs485_driver *driver)
{
    if(!(&driver->match_bus->bus))
        return -EINVAL;

    driver->driver.bus = &driver->match_bus->bus;

    if(driver->probe)
        driver->driver.probe = rs485_driver_probe;
    if(driver->remove)
        driver->driver.remove = rs485_driver_remove;
    if(driver->suspend)
        driver->driver.suspend = rs485_driver_suspend;
    if(driver->resume)
        driver->driver.resume = rs485_driver_resume;

    return driver_register(&driver->driver);
}




static void rs485_device_create_release(struct device *dev)
{
    struct rs485_device *device = to_rs485_device(dev);

    wlog_info("device: '%s' : %s ", device->dev.bus_id, __func__);
    if(device)
        free(device);
}

static void* rs485_bus_match_by_name(const char* name)
{
    return NULL;
}


int rs485_device_create(struct rs485_device_info const *info)
{
    struct rs485_device *device = NULL;
    struct rs485_bus_type *match_bus = NULL;
    int retval = -ENODEV;

    /* TODO: get device id */

    /* step2. match bus */
    match_bus = rs485_bus_match_by_name(info->match_bus);
    if(!match_bus)
    {
        wlog_debug("bus: '%s' have not exist");
        goto error;
    }

    /* step3. alloc device */
    device = malloc(sizeof(*device));
    if(!device)
    {
        retval = -ENOMEM;
        goto error;
    }
    memset(device, 0, sizeof(*device));

    /* step4. full device info */
    memcpy(&device->info, info, sizeof(struct rs485_device_info));

    /* step5. full dev */
    device->dev.bus = &match_bus->bus;
    device->dev.release = rs485_device_create_release;
    dev_set_drvdata(&device->dev, &device->info);
    snprintf(device->dev.bus_id, BUS_ID_SIZE, "%s-%02x%02x%02x%02x",
            device->info.name,
            device->info.mac[0],
            device->info.mac[1],
            device->info.mac[2],
            device->info.mac[3]);

    retval = device_register(&device->dev);
    if(retval)
        goto sysdevice_register_fail;

    return 0;

sysdevice_register_fail:
    free(device);
error:
    return retval;
}


void rs485_device_destroy(struct rs485_device *device)
{
    if(!device)
        return;

    device_unregister(&device->dev);
}



