/*
 * =====================================================================================
 *
 *          @file:  modbus.h
 *
 *    Description:  offer the modbus interface
 *
 *        Version:  1.0
 *        Created:  03/29/2016 09:30:46 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_PROTOCOL_MODBUS_MODBUS_H_
#define INCLUDE_PROTOCOL_MODBUS_MODBUS_H_

#include <stdint.h>

#include "enum.h"
#include "object.h"
#include "support.h"

/* ----------------------------------------- */
/** @addtogroup modbus_protocol_rs485      Modbus RS485
 *  @ingroup modbus
 *
 *  define the modbus interface
 *
 *
 *  @{
 */
/* -----------------------------------------*/


/* -----------------------------------------*/
/**
 * @brief The modbus port interface
 */
/* -----------------------------------------*/
typedef struct
{
    /** device id */
    int                                 device_id;
    /** The message data is brodadcase ? */
    bool                                broadcast;
    /** The device have send fail. retransmission count */
    unsigned int                        retransmission;
    /** The modbus function code, define by enum.h */
    modbus_function_code_enum           code;
    /** The device method,(command) define by enum.h */
    int                                 method;
    /** The method have a value. */
    int                                 value;
    /** temp save the data */
    unsigned char*                      buffer;
    /** The buffer length */
    unsigned int                        buffer_len;
    /** The device address */
    unsigned int                        device_addr;
    /** The you need to operator device register address */
    unsigned int                        register_addr;
    /** The send package callback function */
    method_send                         send_handle;
    /** The receive package callback function */
    method_recv                         recv_handle;
    /** The modbus interface have do something is finish status */
    bool                                done;
}modbus_port_handle_t;


/* -----------------------------------------*/
/**
 * @brief modbus_work_thread_function
 *  The modbus work thread
 *
 * @param[in] arg   : The thread argument is object management pointer.
 *
 * @return  Have no return.
 */
/* -----------------------------------------*/
extern void* modbus_work_thread_function(void* arg);

/* -----------------------------------------*/
/**
 * @brief modbus_service_init
 *  The modbus interface intialize.
 *
 * @param[in] object    :  The object port information
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int modbus_service_init(object_management_t* object);

/* -----------------------------------------*/
/**
 * @brief modbus_service_deinit
 *  clean the modbus service, The haved called by thread have exit.
 *
 * @param[in] object    : The object information
 */
/* -----------------------------------------*/
extern void modbus_service_deinit(object_management_t* object);

/** @} */

#endif  /* INCLUDE_PROTOCOL_MODBUS_MODBUS_H_ */
