
/*
 * =====================================================================================
 *
 *       Filename:  core.c
 *
 *    Description:  RS485 BUS / DEVICE / DRIVER CORE
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
#include <stdbool.h>

#include <errno.h>

#include <wlog.h>
#include <wbitmap.h>

#include <core.h>



#define wlog_e(...)         wlog_error("core", ##__VA_ARGS__)
#define wlog_d(...)         wlog_debug("core", ##__VA_ARGS__)
#define wlog_i(...)         wlog_info("core", ##__VA_ARGS__)
#define wlog_w(...)         wlog_warning("core", ##__VA_ARGS__)


#if 0
static int rs485_device_probe(struct device* dev)
{
    int status = 0;
    struct rs485_device *device = to_rs485_device(dev);
    struct rs485_driver *driver = to_rs485_driver(dev->driver);

    if(!driver->probe)
        return -ENODEV;

    wlog_d("probe: %s", dev->bus_id);

    device->driver = driver;
    status = driver->probe(device, &device->info);
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
        wlog_d("remove: dev:%s", dev->bus_id);
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
#endif

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

    if(strcmp(device->info.match_driver, driver->name) == 0)
        return 0;
    else
        return -ENODEV;
}



/* RS485 bus list */
static struct list_head rs485_bus_list = LIST_HEAD_INIT(rs485_bus_list);


static bool is_rs485_bus_used(const char* name)
{
    struct list_head *p = NULL;
    struct rs485_bus_type *bus = NULL;

    list_for_each(p, &rs485_bus_list)
    {
        bus = (struct rs485_bus_type*)list_entry(p, struct rs485_bus_type, entry);
        if(strcmp(bus->interface.port_name, name) == 0)
            return true;
    }

    return false;
}


int rs485_bus_register(struct rs485_bus_type* bus)
{
    int status = 0;

    if(!(bus && bus->name))
        return -EINVAL;

    if(!(bus->interface.port_name))
        return -EINVAL;

    /* step1. check bus port is had used */
    snprintf(bus->init_name, sizeof(bus->init_name), "%s-%s",
            bus->name, bus->interface.port_name);
    if(is_rs485_bus_used(bus->interface.port_name))
    {
        wlog_w("The bus '%s' have registered", bus->init_name);
        return -EPERM;
    }

    /* step2. init the bus*/
    if(bus->probe)
    {
        status = bus->probe(bus);
        if(status)
            goto bus_init_fail;
    }

    /* step3. set bus name , and register system bus*/
    bus->bus.name = bus->init_name;
#if 0
    bus->bus.probe = rs485_device_probe;
    bus->bus.remove = rs485_device_remove;
#endif
    bus->bus.suspend = rs485_device_suspend;
    bus->bus.resume = rs485_device_resume;
    bus->bus.match = rs485_device_match;
    status = bus_register(&bus->bus);
    if(status)
        goto sysbus_register_fail;

    /* step4. init the bus protocol stack thread */
    if(!bus->pthread_func)
        goto pthread_init_fail;
    status = pthread_create(&bus->pthread, NULL, bus->pthread_func, bus->priv_data);
    if(status)
        goto pthread_init_fail;

    /* step5. add the bus to rs485 bus list */
    list_add(&bus->entry, &rs485_bus_list);

    wlog_i("rs485 bus: '%s', register success", bus->init_name);

    return 0;

pthread_init_fail:
    bus_unregister(&bus->bus);
sysbus_register_fail:
    if(bus->remove)
        bus->remove(bus);
bus_init_fail:
    return status;
}

void rs485_bus_unregister(struct rs485_bus_type* bus)
{
    if(!bus)
        return;

    /* step1. unregister bus from rs485_bus */
    list_del(&bus->entry);

    /* step2. cancel the protocol stack thread */
    pthread_cancel(bus->pthread);

    /* step3. unregister system bus */
    bus_unregister(&bus->bus);

    /* step4. unregister bus from rs485_bus */
    if(bus->remove)
        bus->remove(bus);
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

static void* rs485_driver_match_bus(const char* bus_name)
{
    struct list_head *p = NULL;
    struct rs485_bus_type *bus = NULL;

    list_for_each(p, &rs485_bus_list)
    {
        bus = (struct rs485_bus_type*)list_entry(p, struct rs485_bus_type, entry);
        if(strcmp(bus->init_name, bus_name) == 0)
            return bus;
    }

    return NULL;
}

int rs485_driver_register(struct rs485_driver *driver, const char* bus_name)
{
    struct rs485_bus_type *bus = NULL;

    bus = rs485_driver_match_bus(bus_name);
    if(!bus)
        return -ENODEV;

    driver->driver.bus = &bus->bus;
    driver->driver.name = driver->name;

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

void rs485_driver_unregister(struct rs485_driver *driver)
{
    if(!driver)
        return;

    driver_unregister(&driver->driver);
}







static void rs485_device_create_release(struct device *dev)
{
    struct rs485_device *device = to_rs485_device(dev);

    wlog_i("device: '%s' free : %s ", device->dev.bus_id, __func__);
    if(device)
        free(device);
}

static void* rs485_bus_match_by_name(const char* name)
{
    struct list_head *p = NULL;
    struct rs485_bus_type *bus = NULL;

    list_for_each(p, &rs485_bus_list)
    {
        bus = (struct rs485_bus_type*)list_entry(p, struct rs485_bus_type, entry);
        if(strcmp(bus->init_name, name) == 0)
            return bus;
    }

    return NULL;
}

/* RS485 bus list */
static struct list_head rs485_device_list = LIST_HEAD_INIT(rs485_device_list);

int rs485_device_create(struct rs485_device_info const *info, int *id)
{
    struct rs485_device *device = NULL;
    struct rs485_bus_type *bus = NULL;
    int retval = -ENODEV;

    /* step1. match bus */
    bus = rs485_bus_match_by_name(info->match_bus);
    if(!bus)
    {
        wlog_e("bus: '%s' have not exist", info->match_bus);
        goto error;
    }

    /* step2. alloc device */
    device = calloc(1, sizeof(struct rs485_device));
    if(!device)
    {
        retval = -ENOMEM;
        goto error;
    }

    /* FIXME: set device id to id table */
    *id = get_device_id(device);
    if(*id < 0)
    {
        retval = -ENODEV;
        wlog_d("id is full");
        goto get_device_id_fail;
    }

    /* step3. full device info */
    device->id = *id;
    memcpy(&device->info, info, sizeof(struct rs485_device_info));

    /* step4. full dev */
    device->dev.bus = &bus->bus;
    device->dev.release = rs485_device_create_release;
    dev_set_drvdata(&device->dev, &device->info);
    snprintf(device->dev.bus_id, BUS_ID_SIZE, "%02x%02x%02x%02x-%s",
            device->info.mac[0],
            device->info.mac[1],
            device->info.mac[2],
            device->info.mac[3],
            device->info.match_driver);

    retval = device_register(&device->dev);
    if(retval)
        goto sysdevice_register_fail;

    /* step5. add device to list */
    list_add(&device->entry, &rs485_device_list);

    return 0;

sysdevice_register_fail:
    free_device_id(device->id);
get_device_id_fail:
    free(device);
error:
    return retval;
}


void rs485_device_destroy(struct rs485_device *device)
{
    if(!device)
        return;

    /* step1. delete device from device list */
    list_del(&device->entry);

    /* step2. unregister system device */
    device_unregister(&device->dev);

    /* step3. free device id */
    free_device_id(device->id);

    /* step3. free rs485 device */
    free(device);
}



