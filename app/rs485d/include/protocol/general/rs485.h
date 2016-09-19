/*
 * =====================================================================================
 *
 *          @file:  rs485.h
 *
 *    Description:  The rs485 driver interface
 *
 *        Version:  1.0
 *        Created:  03/28/2016 01:50:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */
#ifndef INCLUDE_PROTOCOL_GENERAL_RS485_H_
#define INCLUDE_PROTOCOL_GENERAL_RS485_H_


#include <stdint.h>
#include <stdbool.h>
#include "protocol/general/general.h"

/* ----------------------------------------- */
/** @addtogroup general_protocol_rs485      General RS485
 *  @ingroup general
 *
 *  define the rs485 physics driver interface
 *
 *
 *  @{
 */
/* -----------------------------------------*/


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* -----------------------------------------*/
/**
* @brief RS485_Set_Interface
*  rs485 interface name
*
* @param[in] ifname : The rs485 name , just like "rs4851"
*/
/* -----------------------------------------*/
extern void rs485_set_interface(char *ifname);


/* -----------------------------------------*/
/**
 * @brief RS485_Get_Interface
 *  get the rs485 interface name
 *
 * @return  The rs485 interface name
 */
/* -----------------------------------------*/
extern const char *rs485_get_interface(void);


/* -----------------------------------------*/
/**
 * @brief RS485_Initialize
 *
 * initiallize a rs485 interface
 */
/* -----------------------------------------*/
extern void rs485_initialize(void);

/* -----------------------------------------*/
/**
 * @brief rs485_send_handle_frame
 *  rs485 bus package a send frame, and send the package to bus.
 *
 * @param[in] mstp_port : The mstp_port struct, and consist of some send frame information
 *
 * @return  0 is success, and others is fail.
 */
/* -----------------------------------------*/
extern int rs485_send_handle_frame(volatile struct mstp_port_handle* mstp_port);


/* -----------------------------------------*/
/**
 * @brief rs485_recv_handle_frame
 *  rs485 bus receive a frame, and call process these data.
 *
 * @param[in] mstp_port : The mstp_port struct, and consist of some frame infromation
 *
 * @return  0 is success, and others is fail.
 */
/* -----------------------------------------*/
extern int rs485_recv_handle_frame(volatile struct mstp_port_handle* mstp_port);

/* -----------------------------------------*/
/**
 * @brief RS485_Set_Baud_Rate
 *  set the rs485 buad rate.
 *
 * @param[in] baud : The rs485 UART buad, like "B9600" "B38400"
 *
 * @return  True is set ok, and others is set fail.
 */
/* -----------------------------------------*/
extern bool rs485_set_baud_rate(uint32_t baud);


/* -----------------------------------------*/
/**
 * @brief RS485_Cleanup
 *  The rs485 initaialize fail, have clean
 *
 */
/* -----------------------------------------*/
extern void rs485_cleanup(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} */



#endif /* INCLUDE_PROTOCOL_GENERAL_RS485_H_ */
