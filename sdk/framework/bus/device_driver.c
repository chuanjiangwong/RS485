/*
 * =====================================================================================
 *
 *       Filename:  device_driver.c
 *
 *    Description:  driver class
 *
 *        Version:  1.0
 *        Created:  Sep 29, 2016 2:41:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), wangchuanjiang1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#include <errno.h>

#include <string.h>

/* SDK include */
#include <wlog.h>
#include <wbus.h>
#include "private.h"



#define wlog_e(...)         wlog_error("dri", ##__VA_ARGS__)
#define wlog_d(...)         wlog_debug("dri", ##__VA_ARGS__)
#define wlog_i(...)         wlog_info("dri", ##__VA_ARGS__)
#define wlog_w(...)         wlog_warning("dri", ##__VA_ARGS__)



static void driver_bound(struct device *dev)
{
	if (wlist_node_attached(&dev->wnode_driver))
	{
		wlog_w("device %s already bound", dev->init_name);
		return;
	}

	wlog_i("driver: '%s': bound to device '%s'",
            dev->bus_id,dev->driver->name);

	/*if (dev->bus)*/

	wlist_add_tail(&dev->wnode_driver, &dev->driver->p->wlist_devices);
}


int device_bind_driver(struct device *dev)
{
	driver_bound(dev);

	return 0;
}



int devres_release_all(struct device *dev)
{
	return 0;
}




static int really_probe(struct device *dev, struct device_driver *drv)
{
	int ret = 0;

	wlog_d("bus: '%s': probing driver %s with device %s",
					drv->bus->name, drv->name, dev->bus_id);

	dev->driver = drv;

	if (dev->bus->probe)
	{
		ret = dev->bus->probe(dev);
		if (ret)
			goto probe_failed;
	}
	else if (drv->probe)
	{
		ret = drv->probe(dev);
		if (ret)
			goto probe_failed;
	}

	driver_bound(dev);
	ret = 1;
	wlog_i("bus:'%s': bound device %s to driver %s",
					drv->bus->name, dev->bus_id, drv->name);
	goto done;


probe_failed:
    devres_release_all(dev);
    dev->driver = NULL;

    if (ret != -ENODEV && ret != -ENXIO)
	{
		/* driver matched but the probe failed */
		wlog_w("%s: probe of %s failed with error %d",
                drv->name, dev->bus_id, ret);
	}
	ret = 0;

done:
    return ret;
}


int driver_probe_done(void)
{
	return 0;
}


int driver_probe_device(struct device_driver *drv, struct device *dev)
{
	int ret = 0;

	if (device_is_registered(dev))
		return -ENODEV;

	if (drv->bus->match && !drv->bus->match(dev, drv))
		goto done;

	wlog_i("bus: '%s':matched device %s with driver %s",
					drv->bus->name, dev->bus_id, drv->name);
	ret = really_probe(dev, drv);

done:
    return ret;
}

static int __device_attach(struct device_driver *drv, void *data)
{
	struct device *dev = data;
	return driver_probe_device(drv, dev);
}


int device_attach(struct device *dev)
{
	int ret = 0;

	if (dev->driver)
	{
		ret = device_bind_driver(dev);
		if (ret == 0)
			ret = 1;
		else
		{
			dev->driver = NULL;
			ret = 0;
		}
	}
    else
		ret = bus_for_each_drv(dev->bus, NULL, dev, __device_attach);

	return ret;
}

static int __driver_attach(struct device *dev, void *data)
{
	struct device_driver *drv = data;

	if (!dev->driver)
		driver_probe_device(drv, dev);

	return 0;
}


int driver_attach(struct device_driver *drv)
{
	return bus_for_each_dev(drv->bus, NULL, drv, __driver_attach);
}


static void __device_release_driver(struct device *dev)
{
	struct device_driver *drv;

	drv = dev->driver;
	if (drv)
	{
		if (dev->bus && dev->bus->remove)
			dev->bus->remove(dev);
		else if (drv->remove)
			drv->remove(dev);
		devres_release_all(dev);
		dev->driver = NULL;
		wlist_remove(&dev->wnode_driver);
	}
}


void device_release_driver(struct device *dev)
{
	__device_release_driver(dev);
}

void driver_detach(struct device_driver *drv)
{
	struct device *dev;

	for (;;)
	{
		if (list_empty(&drv->p->wlist_devices.w_list))
			break;
		dev = list_entry(drv->p->wlist_devices.w_list.prev, struct device,
		                wnode_driver.n_node);
		get_device(dev);

		if (dev->driver == drv)
			__device_release_driver(dev);

		put_device(dev);
	}
}





static struct device *next_device(struct wlist_iter *i)
{
	struct wlist_node *n = wlist_next(i);
	return n ? container_of(n, struct device, wnode_driver) : NULL;
}

int driver_for_each_device(struct device_driver *drv, struct device *start,
                void *data, int (*fn)(struct device *, void *))
{
	struct wlist_iter i;
	struct device *dev;
	int error = 0;

	if (!drv)
		return -EINVAL;

	wlist_iter_init_node(&drv->p->wlist_devices, &i,
	                start ? &start->wnode_driver : NULL);
	while ((dev = next_device(&i)) && !error)
		error = fn(dev, data);
	wlist_iter_exit(&i);
	return error;
}

struct device *driver_find_device(struct device_driver *drv,
                struct device *start, void *data,
                int (*match)(struct device *dev, void *data))
{
	struct wlist_iter i;
	struct device *dev;

	if (!drv)
		return NULL;

	wlist_iter_init_node(&drv->p->wlist_devices, &i,
	                (start ? &start->wnode_driver : NULL));
	while ((dev = next_device(&i)))
		if (match(dev, data) && get_device(dev))
			break;
	wlist_iter_exit(&i);
	return dev;
}


static int driver_match_name(struct device_driver* driver, void* data)
{
    const char* name = data;

    if(strcmp(name, driver->name) == 0)
        return 1;

    return 0;
}

static int driver_find(const char *name, struct bus_type *bus)
{
    return bus_for_each_drv(bus, NULL, (void*)name, driver_match_name);
}



int driver_register(struct device_driver *drv)
{
	if((drv->bus->probe && drv->probe) || (drv->bus->remove && drv->remove))
		wlog_w("Driver '%s' needs updating - please use bus_type methods",
                drv->name);

	if(driver_find(drv->name, drv->bus))
	{
		wlog_e("Driver '%s' is already registered,  aborting...", drv->name);
		return -EEXIST;
	}

	return bus_add_driver(drv);
}

void driver_unregister(struct device_driver *drv)
{
	bus_remove_driver(drv);
}




