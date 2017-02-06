
/*
 * =====================================================================================
 *
 *       Filename:  client.h
 *
 *    Description:  rs485 device client
 *
 *        Version:  1.0
 *        Created:  Jan 23, 2017 9:36:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#ifndef APP_RS485D_MASTER_INCLUDE_CLIENT_H_
#define APP_RS485D_MASTER_INCLUDE_CLIENT_H_

#include <enum.h>
#include <core.h>


extern int get_client(struct rs485_device_info * info,
        const char* bus_port_name,
        rs485_factory_name_enum device_type,
        const unsigned char mac[4]);

#endif /* APP_RS485D_MASTER_INCLUDE_CLIENT_H_ */
