/*
 * =====================================================================================
 *
 *          @file:  general.h
 *
 *    Description:  offer the rs485 general protocol interface
 *
 *        Version:  1.0
 *        Created:  03/28/2016 11:10:40 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_PROTOCOL_GENERAL_GENERAL_H_
#define INCLUDE_PROTOCOL_GENERAL_GENERAL_H_

#include "support.h"                /* define the method_send */
#include "object.h"                 /* define the method receive */

/* ----------------------------------------- */
/** @addtogroup general_protocol_interface     General interface
 *  @ingroup general
 *
 *  define the general interface interface
 *
 *
 *  @{
 */
/* -----------------------------------------*/


/* -----------------------------------------*/
/**
 * @brief mstp_port_handle
 *  general protocol(user defined)
 */
/* -----------------------------------------*/
typedef struct mstp_port_handle
{
    /** The send buffer data to bus */
    unsigned char*              package_buffer;
    /** The send buffer length */
    unsigned int                package_buffer_len;
    /** Is wait device reply data? */
    bool                        except_reply;
    /** send the data to device have fail, you can retransmission count */
    unsigned int                retransmission;
    /** wait the device reply timeout (ms) */
    unsigned int                timeout_ms;
    /** The device addresss  */
    unsigned char               address[4];
    /** The device address len, should less than 4 byte */
    unsigned int                address_len;
    /** The send package is a broadcast ? */
    bool                        broadcast;
    /** package a send data callback function */
    method_send                 send_handle;
    /** The device method  */
    int                         method;
    /** The device method include value */
    int                         value;
    /** process the receive buffer */
    method_recv                 recv_handle;
    /** The device id. */
    int                         device_id;
    /** resaved argument */
    void*                       arg;
}mstp_port_handle_t;


/* -----------------------------------------*/
/**
 * @brief general_service_init
 *  The general protocol(user defined) initilize
 *
 * @param[in] object    : The object information.
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int general_service_init(object_management_t* object);

/* -----------------------------------------*/
/**
 * @brief general_work_thread_function
 *  The general work thread function
 *
 * @param[in] arg       : The thread argument , This arg uesd "The general object, mstp_port_handle_t"
 *
 * @return  Thread have error have a return.
 */
/* -----------------------------------------*/
extern void* general_work_thread_function(void* arg);


/** @} */
#endif  /* INCLUDE_PROTOCOL_GENERAL_RS485_H */
