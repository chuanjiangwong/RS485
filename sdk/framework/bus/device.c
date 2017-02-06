
/*
 * =====================================================================================
 *
 *       Filename:  device.c
 *
 *    Description:  device class
 *
 *        Version:  1.0
 *        Created:  Sep 29, 2016 2:39:53 PM
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
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include <wlog.h>
#include <wbus.h>
#include <wlist.h>

#include "private.h"


#define wlog_e(...)         wlog_error("dev", ##__VA_ARGS__)
#define wlog_d(...)         wlog_debug("dev", ##__VA_ARGS__)
#define wlog_i(...)         wlog_info("dev", ##__VA_ARGS__)
#define wlog_w(...)         wlog_warning("dev", ##__VA_ARGS__)



inline void *dev_get_drvdata(struct device *dev)
{
	return dev->driver_data;
}

inline void dev_set_drvdata(struct device *dev, void *data)
{
	dev->driver_data = data;
}

inline bool device_is_registered(struct device *dev)
{
	return dev->is_driver_register;
}


static void device_initialize(struct device *dev)
{

}


static int dev_set_name(struct device *dev, const char *fmt, ...)
{
	va_list vargs;

	va_start(vargs, fmt);
	vsnprintf(dev->bus_id, sizeof(dev->bus_id), fmt, vargs);
	va_end(vargs);
	return 0;
}


static int device_add(struct device *dev)
{
	int error = 0;

    if(dev == NULL)
        return -EINVAL;

	if (!dev)
		goto done;

	if (dev->init_name)
		dev_set_name(dev, "%s", dev->init_name);

	if (!strlen(dev->bus_id))
		goto done;

	wlog_i("device: '%s' add", dev->bus_id);

    error = bus_add_device(dev);
    if(error)
        goto done;

	bus_attach_device(dev);


done:
	return error;
}


int device_register(struct device *dev)
{
	device_initialize(dev);
	return device_add(dev);
}



static void device_del(struct device *dev)
{
	bus_remove_device(dev);

	devres_release_all(dev);

	dev->release(dev);
}


void device_unregister(struct device *dev)
{
	wlog_i("device: '%s' unregister ", dev->bus_id);
	device_del(dev);
}





