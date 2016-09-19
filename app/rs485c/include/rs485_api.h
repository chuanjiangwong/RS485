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
    unsigned char                                   address;

    /* add :@date Sun 18 Sep, 2016 */
    /* The device factory type, defind by enum.h */
    rs485_factory_name_enum                         factory;
}rs485_init_t;


/*device function*/
extern int rs485_device_init(struct rs485_init device[], int numbers, rs485_protocol_type_enum protocol);

extern void rs485_device_deinit(void);

extern int rs485_send_to_device(int knx_id, int value);

extern int rs485_get_device_info(read_device_return_t* out,int knx_id);

extern int rs485_server_stop(void);


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
