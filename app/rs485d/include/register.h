
/*
 * =====================================================================================
 *
 *       Filename:  register.h
 *
 *    Description:  device and bus init section
 *
 *        Version:  1.0
 *        Created:  Jan 17, 2017 9:35:35 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#ifndef APP_RS485D_MASTER_INCLUDE_REGISTER_H_
#define APP_RS485D_MASTER_INCLUDE_REGISTER_H_

/* extern the bus arg */
struct interface_profile;
struct rs485_bus_type;


/* define the bus init type */
typedef int (*bus_register_t)(const struct interface_profile* interface,
        const char* bus_name);
typedef int (*bus_unregister_t)(struct rs485_bus_type* bus);

/* bus init function link section */
extern bus_register_t _register_bus_start[], _register_bus_end[];
extern bus_unregister_t _unregister_bus_start[], _unregister_bus_end[];

#define __define_bus_register(fn) \
    bus_register_t __bus_register_##fn \
    __attribute__((__section__(".register.bus"))) = fn

#define RS485_BUS_INIT(fn)      __define_bus_register(fn)

#define __define_bus_unregister(fn) \
    bus_unregister_t __bus_unregister_##fn \
    __attribute__((__section__(".unregister.bus"))) = fn

#define RS485_BUS_EXIT(fn)      __define_bus_unregister(fn)



extern int bus_initcalls(const struct interface_profile * interface,
        const char* name);
extern int bus_exitcalls(struct rs485_bus_type * bus);


/* ------------------------------------------------------------------*/
/**
 * @brief driver init register function
 *
 * @param[in] bus_name  : The bus name need to mount
 *
 * @returns 0 is success, others is fail.
 */
/* ------------------------------------------------------------------*/
typedef int (*driver_register_t)(const char* bus_name);

extern driver_register_t _register_driver_start[], _register_driver_end[];

#define __define_driver_register(fn) \
    driver_register_t __driver_register_##fn \
    __attribute__((__section__(".register.driver"))) = fn

#define RS485_DRIVER_INIT(fn)      __define_driver_register(fn)



extern void driver_initcalls(const char* bus_name);





#endif /* APP_RS485D_MASTER_INCLUDE_REGISTER_H_ */
