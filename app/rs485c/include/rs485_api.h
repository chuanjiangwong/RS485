/*
 * rs485_api.h
 *
 *  Created on: Sep 28, 2015
 *      Author: wong
 */

#ifndef RS485_API_H_
#define RS485_API_H_

#include <stdbool.h>

#include "enum.h"
#include "adapter.h"

#ifdef __cplusplus
extern "C" {
#endif



enum RS485_PORT_BAUD_RATE
{
    RS485_PORT_BAUD_RATE_2400 = 0,
    RS485_PORT_BAUD_RATE_4800,
    RS485_PORT_BAUD_RATE_9600,
    RS485_PORT_BAUD_RATE_14400,
    RS485_PORT_BAUD_RATE_19200,
    RS485_PORT_BAUD_RATE_38400,
    RS485_PORT_BAUD_RATE_57600,
    RS485_PORT_BADU_RATE_115200,
};

/* -----------------------------------------*/
/**
 * @brief rs485_init device initialize define
 */
/* -----------------------------------------*/
typedef struct rs485_init
{
    /* The panel_s_dev_control.h have define the device type */
    int                                             device_type;
    /* The pannel_s define the knx id */
    int                                             knx_id;
    /* The device mac address */
    unsigned char                                   address[4];

    /* add :@date Sun 18 Sep, 2016 */
    /* The device factory type, defind by enum.h */
    rs485_factory_name_enum                         factory;
}rs485_init_t;

struct rs485_bus_init
{
    /* bus type */
    rs485_protocol_type_enum                        protocol;

    /* RS485 UART information, define by adapter.h */
    /* @note: typedef struct
     * {
     *  unsigned int    baud_rate;
     *  char            interface_name[16];
     *  rs485_port_parity_type_enum     parity;
     * };
     *
     * The must full -> baud_rate;
     *                  parity;
     * */
    rs485_port_t                                    interface;
};


/*device function*/
extern int rs485_device_init(struct rs485_init device[], int numbers,
        const struct rs485_bus_init * bus);

extern void rs485_device_deinit(void);

extern int rs485_send_to_device(int knx_id, int value);

extern int rs485_get_device_info(read_device_return_t* out,int knx_id);

extern int rs485_server_stop(void);

extern void rs485_daemon_restart(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * @defgroup rs485_device RS485 device
 *
 *
 * @defgroup rs485_app RS485 application interface
 */


#endif /* RS485_API_H_ */
