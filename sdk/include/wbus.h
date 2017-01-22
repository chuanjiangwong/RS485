
/*
 * =====================================================================================
 *
 *       Filename:  wbus.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Oct 8, 2016 2:46:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#ifndef SDK_INCLUDE_WBUS_H_
#define SDK_INCLUDE_WBUS_H_



/* refer to Linux kernel */

#include <wlist.h>



#define BUS_ID_SIZE		20

struct device;
struct device_driver;
struct driver_private;
struct bus_type;
struct bus_type_private;



struct bus_type
{
    const char		        *name;

    int (*match)(struct device *dev, struct device_driver *drv);
    int (*probe)(struct device *dev);
    int (*remove)(struct device *dev);

    int (*suspend)(struct device *dev);
    int (*resume)(struct device *dev);

    struct bus_type_private *p;
};


struct device
{
	struct wlist_node	wnode_driver;
	struct wlist_node	wnode_bus;

	char	            bus_id[BUS_ID_SIZE];	/* position on parent bus */
	const char		    *init_name; /* initial name of the device */
    unsigned            is_driver_register:1;

	struct bus_type	    *bus;		/* type of bus device is on */
	struct device_driver *driver;	/* which driver has allocated this device */
	void		        *driver_data;	/* data private to the driver */

	void	(*release)(struct device *dev);
};


struct device_driver
{
	const char		    *name;
	struct bus_type		*bus;

	int (*probe) (struct device *dev);
	int (*remove) (struct device *dev);
	int (*suspend) (struct device *dev);
	int (*resume) (struct device *dev);

	struct driver_private *p;
};


extern int bus_register(struct bus_type *bus);
extern void bus_unregister(struct bus_type *bus);
extern int bus_add_device(struct device *dev);
extern void bus_attach_device(struct device *dev);
extern void bus_remove_device(struct device *dev);
extern int bus_add_driver(struct device_driver *drv);
extern void bus_remove_driver(struct device_driver *drv);
extern int bus_for_each_drv(struct bus_type *bus, struct device_driver *start,
                void *data, int (*fn)(struct device_driver *, void *));
extern int bus_for_each_dev(struct bus_type *bus, struct device *start, void *data,
		     int (*fn)(struct device *dev, void *data));


extern int driver_register(struct device_driver *drv);
extern void driver_unregister(struct device_driver *drv);
extern int driver_attach(struct device_driver *drv);
extern void driver_detach(struct device_driver *drv);
extern int driver_probe_device(struct device_driver *drv, struct device *dev);


extern int device_register(struct device *dev);
extern void device_unregister(struct device *dev);
extern void device_release_driver(struct device *dev);
extern int device_attach(struct device *dev);

extern inline void *dev_get_drvdata(struct device *dev);
extern inline void dev_set_drvdata(struct device *dev, void *data);



#endif /* SDK_INCLUDE_WBUS_H_ */
