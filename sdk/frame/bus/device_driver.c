/*
 * =====================================================================================
 *
 *       Filename:  device_driver.c
 *
 *    Description:  
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

/* SDK include */
#include <wlog.h>
#include <private.h>
#include <wbus.h>




static void driver_bound(struct device *dev)
{
	if (wlist_node_attached(&dev->wnode_driver))
	{
		wlog_warning("%s: device %s already bound\n",
						__func__, dev->init_name);
		return;
	}

	wlog_info("driver: '%s': %s: bound to device '%s'\n", dev->bus_id,
					__func__, dev->driver->name);

	/*if (dev->bus)*/

	wlist_add_tail(&dev->wnode_driver, &dev->driver->p->wlist_devices);
}


int device_bind_driver(struct device *dev)
{
	int ret = 0;

	driver_bound(dev);

	return ret;
}




int devres_release_all(struct device *dev)
{
	return 0;
}




static int really_probe(struct device *dev, struct device_driver *drv)
{
	int ret = 0;

	wlog_info("bus: '%s': %s: probing driver %s with device %s\n",
					drv->bus->name, __func__, drv->name, dev->bus_id);

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
	wlog_info("bus: '%s': %s: bound device %s to driver %s\n",
					drv->bus->name, __func__, dev->bus_id, drv->name);
	goto done;

	probe_failed:
        devres_release_all(dev);
        dev->driver = NULL;

	if (ret != -ENODEV && ret != -ENXIO)
	{
		/* driver matched but the probe failed */
		wlog_warning(
						"%s: probe of %s failed with error %d\n",
						drv->name, dev->bus_id, ret);
	}

	ret = 0;
	done: return ret;
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

	wlog_info("bus: '%s': %s: matched device %s with driver %s\n",
					drv->bus->name, __func__, dev->bus_id, drv->name);

	ret = really_probe(dev, drv);

	done: return ret;
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
	} else
	{
		ret = bus_for_each_drv(dev->bus, NULL, dev, __device_attach);
	}

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

struct device_driver *get_driver(struct device_driver *drv)
{
	return drv;
}

void put_driver(struct device_driver *drv)
{
}


struct device_driver *driver_find(const char *name, struct bus_type *bus)
{

    /* FIXME: This function have to find drvier private,
     * but now have now used it*/
/*
	struct driver_private *priv;

	if (bus && name)
	{
		priv = bus->p;
		return priv->driver;
	}
*/

	return NULL;
}



int driver_register(struct device_driver *drv)
{
	int ret;
	struct device_driver *other;

	if ((drv->bus->probe && drv->probe) || (drv->bus->remove && drv->remove))
		wlog_warning("Driver '%s' needs updating - please use "
						"bus_type methods\n", drv->name);

	other = driver_find(drv->name, drv->bus);
	if (other)
	{
		put_driver(other);
		wlog_error("Error: Driver '%s' is already registered, "
						"aborting...\n", drv->name);
		return -EEXIST;
	}

	ret = bus_add_driver(drv);
	if (ret)
		return ret;

	return ret;
}

void driver_unregister(struct device_driver *drv)
{
	bus_remove_driver(drv);
}




