
/*
 * =====================================================================================
 *
 *       Filename:  private.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Sep 29, 2016 11:01:27 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), wangchuanjiang1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#ifndef APP_FRAME_INCLUDE_PRIVATE_H_
#define APP_FRAME_INCLUDE_PRIVATE_H_

#include <wlist.h>
#include <stdbool.h>

struct bus_type_private
{
	struct wlist wlist_devices;
	struct wlist wlist_drivers;
	unsigned int drivers_autoprobe:1;
	struct bus_type *bus;
};



struct driver_private
{
	struct wlist wlist_devices;
	struct wlist_node wnode_bus;
	struct device_driver *driver;
};



extern struct device *get_device(struct device *dev);
extern void put_device(struct device *dev);
extern int devres_release_all(struct device *dev);

extern inline void *dev_get_drvdata(struct device *dev);

extern inline void dev_set_drvdata(struct device *dev, void *data);

extern inline bool device_is_registered(struct device *dev);


#endif /* APP_FRAME_INCLUDE_PRIVATE_H_ */
