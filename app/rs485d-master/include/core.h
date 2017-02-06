
/*
 * =====================================================================================
 *
 *       Filename:  core.h
 *
 *    Description:  The rs485 class
 *
 *        Version:  1.0
 *        Created:  Oct 8, 2016 4:33:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#ifndef APP_CORE_INCLUDE_CORE_H_
#define APP_CORE_INCLUDE_CORE_H_

#include <stdbool.h>

#include <pthread.h>
#include <wbus.h>
#include <klist.h>



struct rs485_device;
struct rs485_driver;
struct rs485_bus_type;



enum interface_data_bits
{
    INTERFACE_DATA_BITS_5 = 5,
    INTERFACE_DATA_BITS_6,
    INTERFACE_DATA_BITS_7,
    INTERFACE_DATA_BITS_8,
};

enum interface_parodd
{
    INTERFACE_PARITY_NULL,
    INTERFACE_PARITY_ODD,
    INTERFACE_PARITY_EVEN,
};

enum interface_stop_bits
{
    INTERFACE_STOP_BITS_0,
    INTERFACE_STOP_BITS_1,
    INTERFACE_STOP_BITS_2,
};

struct interface_profile
{
    const char*                 port_name;
    unsigned int                baud_rate;
    enum interface_data_bits    data_bits;
    enum interface_parodd       parity;
    enum interface_stop_bits    stop_bits;
};


#define RS485_NAME_SIZE (20)
/* ------------------------------------------------------------------*/
/**
 * @brief rs485 bus type
 */
/* ------------------------------------------------------------------*/
struct rs485_bus_type
{
    const char*                 name;
    char                        init_name[RS485_NAME_SIZE];
    struct interface_profile    interface;

    /* The rs485 bus protocol thread */
    pthread_t                   pthread;
    void                        *priv_data; /* bus private data */
    void*   (*pthread_func)	    (void* 	priv_data);

    /* bus init and clean function */
    int     (*probe)            (struct rs485_bus_type * bus);
    void 	(*remove)			(struct rs485_bus_type * bus);

    /* read write data to bus stack */
    int     (*write)            (void* priv_data, void* buffer, int len);
    int     (*read)             (void* priv_data, void* buffer, int len);

    /* all rs485 bus list */
    struct list_head            entry;

    /* system bus type */
    struct bus_type             bus;
};



struct rs485_device_info
{
    char                        match_bus[RS485_NAME_SIZE];
    const char*                 match_driver;
    unsigned char               mac[4];
};


/* ------------------------------------------------------------------*/
/**
 * @brief rs485 device interface
 */
/* ------------------------------------------------------------------*/
struct rs485_device
{
    unsigned int                id;
    /* device information */
    struct rs485_device_info    info;

    /* device read information */
    void*                       device_data;

    /* list node */
    struct list_head            entry;

    /* parent class bus */
    struct rs485_driver*        driver;
    struct device               dev;
};
#define to_rs485_device(d) container_of(d, struct rs485_device, dev)





/* ------------------------------------------------------------------*/
/**
 * @brief rs485 driver interface
 */
/* ------------------------------------------------------------------*/
struct rs485_driver_data
{
    unsigned int                retransmission;
    unsigned int                receive_timeout_ms;
    unsigned int                broadcast:1;
    unsigned int                broadcast_mask;
};
struct rs485_driver
{
    const char*                 name;
    const char*                 match_bus;

    int (*probe)(struct rs485_device*, const struct rs485_device_info*);
    int (*remove)(struct rs485_device*);

    int (*suspend)(struct rs485_device*);
    int (*resume)(struct rs485_device*);

    int (*command)(struct rs485_device* device, unsigned int cmd, void* arg);

    /* custom device driver */
    int (*get)(void* buffer, int len, const void* arg, bool* broadcast);
    int (*put)(const void* buffer, int len, const void* arg, void* profile);

    /* client information */
    const struct rs485_driver_data   *data;

    /* parent class -> driver */
    struct device_driver        driver;
};

#define to_rs485_driver(d) container_of(d, struct rs485_driver, driver)






/* for rs485 bus function */
extern int rs485_bus_register(struct rs485_bus_type* bus);
extern void rs485_bus_unregister(struct rs485_bus_type* bus);


/* for rs485 driver device function */
extern int rs485_driver_register(struct rs485_driver *driver, const char* bus_name);
extern void rs485_driver_unregister(struct rs485_driver* driver);


/* for rs485 device function */
extern int rs485_device_create(struct rs485_device_info const *info, int *id);
extern void rs485_device_destroy(struct rs485_device *device);
extern inline int set_device_data(struct rs485_device* dev, void* data);
extern inline void* get_device_data(struct rs485_device const * dev);


/* The device id map */
extern int get_device_id(struct rs485_device* device);
extern void free_device_id(int id);
extern struct rs485_device* get_device_by_id(int id);


#endif /* APP_CORE_INCLUDE_CORE_H_ */
