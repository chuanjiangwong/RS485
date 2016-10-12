
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

#include <pthread.h>
#include <semaphore.h>

#include <wbus.h>
#include <ringbuf.h>


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
    INTERFACE_PARODD_NULL,
    INTERFACE_PARODD_ODD,
    INTERFACE_PARODD_EVEN,
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
    enum interface_parodd       parodd;
    enum interface_stop_bits    stop_bots;
};


/* ------------------------------------------------------------------*/
/**
 * @brief rs485 bus type
 */
/* ------------------------------------------------------------------*/
struct rs485_bus_type
{
    const char*                 name;
    unsigned int                id;
    struct interface_profile    interface;

    /* The rs485 bus protocol pthread */
    pthread_t                   pthread;
    void* (*pthread_function)(void* arg);

    /* The rs485 bus protocol stack function */
    int (*write_data_to_stack)(void const * data, int len);
    int (*read_data_to_stack)(void* data, int len);

    struct bus_type             bus;
    void*                       bus_data;
};



struct rs485_device_info
{
    const char*                 name;
    const char*                 match_bus;
    const char*                 match_driver;

    unsigned char               mac[4];
    unsigned int                retransmission;
    unsigned int                receive_timeout;
    unsigned int                broadcast:1;
    unsigned int                broadcast_mask;

    /* timer task */
    int (*timer_task)(void* arg);
    void*                       arg;
    unsigned int                period_ms;
};


/* ------------------------------------------------------------------*/
/**
 * @brief rs485 device interface
 */
/* ------------------------------------------------------------------*/
struct rs485_device
{
    unsigned int                id;

    struct rs485_device_info    info;

    struct rs485_driver*        driver;

    struct device               dev;
    void*                       device_data;
};
#define to_rs485_device(d) container_of(d, struct rs485_device, dev)





/* ------------------------------------------------------------------*/
/**
 * @brief rs485 driver interface
 */
/* ------------------------------------------------------------------*/
struct rs485_driver
{
    const char*                 name;

    struct rs485_bus_type       *match_bus;

    int (*probe)(struct rs485_device*, const struct rs485_device_info*);
    int (*remove)(struct rs485_device*);

    int (*suspend)(struct rs485_device*);
    int (*resume)(struct rs485_device*);

    int (*command)(struct rs485_device* device, unsigned int cmd, void* arg);
    int (*get_buffer)(unsigned char* buffer, struct rs485_device const * device, void* bus_stack_data);
    int (*put_buffer)(unsigned char* buffer, struct rs485_device const * device, void* bus_stack_data);

    struct device_driver        driver;
    void*                       driver_data;

    struct list_head            devices;
};

#define to_rs485_driver(d) container_of(d, struct rs485_driver, driver)






/* for rs485 bus function */
extern int rs485_bus_register(struct rs485_bus_type* bus);
extern void rs485_bus_unregister(struct rs485_bus_type* bus);


/* for rs485 device function */
extern int rs485_device_register(struct rs485_device* dev);
extern void rs485_device_unregister(struct rs485_device* dev);

extern inline int set_device_data(struct rs485_device* dev, void* data);
extern inline void* get_device_data(struct rs485_device const * dev);


/* for rs485 driver device function */
extern int rs485_driver_register(struct rs485_driver *driver);
extern void rs485_driver_unregister(struct rs485_driver* driver);


#endif /* APP_CORE_INCLUDE_CORE_H_ */
