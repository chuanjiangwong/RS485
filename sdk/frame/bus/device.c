
/*
 * =====================================================================================
 *
 *       Filename:  device.c
 *
 *    Description:  
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
#include <private.h>
#include <wbus.h>
#include <wlist.h>





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



const char *dev_driver_string(const struct device *dev)
{
	return dev->driver ? dev->driver->name :
			(dev->bus ? dev->bus->name : "");

}


void device_initialize(struct device *dev)
{

}


int dev_set_name(struct device *dev, const char *fmt, ...)
{
	va_list vargs;

	va_start(vargs, fmt);
	vsnprintf(dev->bus_id, sizeof(dev->bus_id), fmt, vargs);
	va_end(vargs);
	return 0;
}


int device_add(struct device *dev)
{
	int error = 0;

    if(dev == NULL)
        return -EINVAL;

	dev = get_device(dev);
	if (!dev)
		goto done;

	if (dev->init_name)
		dev_set_name(dev, "%s", dev->init_name);

	if (!strlen(dev->bus_id))
		goto done;

	wlog_info("device: '%s': %s\n", dev->bus_id, __func__);

    error = bus_add_device(dev);
    if(error)
        goto done;

	bus_attach_device(dev);


done:
	put_device(dev);
	return error;
}


int device_register(struct device *dev)
{
	device_initialize(dev);
	return device_add(dev);
}


struct device *get_device(struct device *dev)
{
    return dev;
}


void put_device(struct device *dev)
{
}


void device_del(struct device *dev)
{
	bus_remove_device(dev);

	devres_release_all(dev);
}


void device_unregister(struct device *dev)
{
	wlog_info("device: '%s': %s\n", dev->bus_id, __func__);
	device_del(dev);
	put_device(dev);
}



static void device_create_release(struct device *dev)
{
	wlog_info("device: '%s': %s\n", dev->bus_id, __func__);
	free(dev);
}


struct device *device_create_vargs(void *drvdata,
				   const char *fmt,
				   va_list args)
{
	struct device *dev = NULL;
	int retval = -ENODEV;

	dev = malloc(sizeof(*dev));
	if (!dev) {
		retval = -ENOMEM;
		goto error;
	}
    memset(dev, 0, sizeof(*dev));

	dev->release = device_create_release;
	dev_set_drvdata(dev, drvdata);

	vsnprintf(dev->bus_id, BUS_ID_SIZE, fmt, args);
	retval = device_register(dev);
	if (retval)
		goto error;

	return dev;

error:
	return  NULL;
}


struct device *device_create(void *drvdata, const char *fmt, ...)
{
	va_list vargs;
	struct device *dev;

	va_start(vargs, fmt);
	dev = device_create_vargs(drvdata, fmt, vargs);
	va_end(vargs);
	return dev;
}


void device_destroy(struct device *dev)
{
	if (dev)
	{
		put_device(dev);
		device_unregister(dev);
	}
}



