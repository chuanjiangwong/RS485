
/*
 * =====================================================================================
 *
 *       Filename:  bus.c
 *
 *    Description:  device bus class
 *
 *        Version:  1.0
 *        Created:  Sep 29, 2016 9:56:37 AM
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
#include <stdlib.h>

#include <wlog.h>
#include <wbus.h>
#include "private.h"


#define wlog_e(...)         wlog_error("bus", ##__VA_ARGS__)
#define wlog_d(...)         wlog_debug("bus", ##__VA_ARGS__)
#define wlog_i(...)         wlog_info("bus", ##__VA_ARGS__)
#define wlog_w(...)         wlog_warning("bus", ##__VA_ARGS__)



static struct bus_type *bus_get(struct bus_type *bus)
{
	return bus;
}

static void bus_put(struct bus_type *bus)
{

}


int bus_add_device(struct device *dev)
{
	struct bus_type *bus = bus_get(dev->bus);

	if (bus)
	{
		wlog_i("bus: '%s': add device %s", bus->name, dev->bus_id);
	}

	return 0;
}


static struct device_driver *next_driver(struct wlist_iter *i)
{
	struct wlist_node *n = wlist_next(i);
	struct driver_private *drv_priv;

	if (n)
	{
		drv_priv = container_of(n, struct driver_private, wnode_bus);
		return drv_priv->driver;
	}
	return NULL;
}


int bus_for_each_drv(struct bus_type *bus, struct device_driver *start,
                void *data, int (*fn)(struct device_driver *, void *))
{
	struct wlist_iter i;
	struct device_driver *drv;
	int error = 0;

	if (!bus)
		return -EINVAL;

	wlist_iter_init_node(&bus->p->wlist_drivers, &i,
	                start ? &start->p->wnode_bus : NULL);
	while ((drv = next_driver(&i)) && !error)
		error = fn(drv, data);
	wlist_iter_exit(&i);
	return error;
}





static struct device *next_device(struct wlist_iter *i)
{
	struct wlist_node *n = wlist_next(i);
	return n ? container_of(n, struct device, wnode_bus) : NULL;
}


int bus_for_each_dev(struct bus_type *bus, struct device *start,
		     void *data, int (*fn)(struct device *, void *))
{
	struct wlist_iter i;
	struct device *dev;
	int error = 0;

	if (!bus)
		return -EINVAL;

	wlist_iter_init_node(&bus->p->wlist_devices, &i,
			     (start ? &start->wnode_bus : NULL));
	while ((dev = next_device(&i)) && !error)
		error = fn(dev, data);
	wlist_iter_exit(&i);
	return error;
}


struct device *bus_find_device(struct bus_type *bus, struct device *start,
                void *data, int (*match)(struct device *dev, void *data))
{
	struct wlist_iter i;
	struct device *dev;

	if (!bus)
		return NULL;

	wlist_iter_init_node(&bus->p->wlist_devices, &i,
	                (start ? &start->wnode_bus : NULL));
	while ((dev = next_device(&i)))
		if (match(dev, data) && get_device(dev))
			break;
	wlist_iter_exit(&i);
	return dev;
}


static int match_name(struct device *dev, void *data)
{
	const char *name = data;

	if (strcmp(name, dev->bus_id) == 0)
		return 1;
	return 0;
}


struct device *bus_find_device_by_name(struct bus_type *bus,
        struct device *start, const char *name)
{
	return bus_find_device(bus, start, (void*)name, match_name);
}


void bus_attach_device(struct device *dev)
{
	struct bus_type *bus = dev->bus;
	int ret = 0;

	if (bus)
	{
		if (bus->p->drivers_autoprobe)
			ret = device_attach(dev);
		if (ret >= 0)
			wlist_add_tail(&dev->wnode_bus, &bus->p->wlist_devices);
		else
		{
            wlog_i("bus attach device fail: %d", ret);
		}
	}
}


void bus_remove_device(struct device *dev)
{
	if (dev->bus)
	{
		if (wlist_node_attached(&dev->wnode_bus))
			wlist_del(&dev->wnode_bus);

		wlog_i("bus: '%s': remove device %s",
						dev->bus->name, dev->bus_id);
		device_release_driver(dev);
		bus_put(dev->bus);
	}
}


int bus_add_driver(struct device_driver *drv)
{
	struct bus_type *bus;
	struct driver_private *priv;
	int error = 0;

	bus = bus_get(drv->bus);
	if (!bus)
		return -EINVAL;

	priv = malloc(sizeof(*priv));
	if (!priv)
	{
		error = -ENOMEM;
		goto out_put_bus;
	}
	memset(priv, 0, sizeof(*priv));
	wlist_init(&priv->wlist_devices, NULL, NULL);
	priv->driver = drv;
	drv->p = priv;

	if (drv->bus->p->drivers_autoprobe)
	{
		error = driver_attach(drv);
		if (error)
			goto out_unregister;
	}

	wlist_add_tail(&priv->wnode_bus, &bus->p->wlist_drivers);

	wlog_i("bus: '%s': add driver '%s'", bus->name, drv->name);

    return 0;

out_unregister:
    free(priv);
out_put_bus:
    bus_put(bus);
    return error;
}


void bus_remove_driver(struct device_driver *drv)
{
	if (!drv->bus)
		return;

	wlist_remove(&drv->p->wnode_bus);
	wlog_i("bus: '%s': remove driver %s", drv->bus->name, drv->name);
	driver_detach(drv);
	free(drv->p);
	bus_put(drv->bus);
}



/* Helper for bus_rescan_devices's iter */
static int bus_rescan_devices_helper(struct device *dev, void *data)
{
	int ret = 0;

	if (!dev->driver)
	{
		ret = device_attach(dev);
	}
	return ret < 0 ? ret : 0;
}

int bus_rescan_devices(struct bus_type *bus)
{
	return bus_for_each_dev(bus, NULL, NULL, bus_rescan_devices_helper);
}


int device_reprobe(struct device *dev)
{
	if (dev->driver)
	{
		device_release_driver(dev);
	}
	return bus_rescan_devices_helper(dev, NULL);
}




static void wlist_devices_get(struct wlist_node *n)
{
	struct device *dev = container_of(n, struct device, wnode_bus);

	get_device(dev);
}


static void wlist_devices_put(struct wlist_node *n)
{
	struct device *dev = container_of(n, struct device, wnode_bus);

	put_device(dev);
}


int bus_register(struct bus_type *bus)
{
	struct bus_type_private *priv;

	priv = malloc(sizeof(struct bus_type_private));
	if (!priv)
		return -ENOMEM;
	memset(priv, 0, sizeof(struct bus_type_private));

	priv->bus = bus;
	bus->p = priv;

	priv->drivers_autoprobe = 1;

	wlist_init(&priv->wlist_devices, wlist_devices_get, wlist_devices_put);
	wlist_init(&priv->wlist_drivers, NULL, NULL);

	wlog_i("bus: '%s': registereing", bus->name);
	return 0;
}


void bus_unregister(struct bus_type *bus)
{
	wlog_i("bus: '%s': unregistering", bus->name);
	free(bus->p);
}


struct wlist *bus_get_device_wlist(struct bus_type *bus)
{
	return &bus->p->wlist_devices;
}









