/*
 * =====================================================================================
 *
 *          @file:  bacnet.h
 *
 *    Description:  offer the BACnet interface
 *
 *        Version:  1.0
 *        Created:  03/28/2016 08:57:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_PROTOCOL_BACNET_BACNET_H_
#define INCLUDE_PROTOCOL_BACNET_BACNET_H_

#include "enum.h"
#include "protocol/bacnet/handle_property.h"
#include "support.h"

/* ----------------------------------------- */
/** @addtogroup bacnet_interface     BACnet interface
 *  @ingroup bacnet
 *
 *  define the bacnet interface
 *
 *
 *  @{
 */
/* -----------------------------------------*/

/* -----------------------------------------*/
/**
 * @brief bacnet
 *  bacnet interface struct
 */
/* -----------------------------------------*/
typedef struct bacnet
{
    char                                mac[4];
    bacnet_function_code_enum           code;
    bacnet_write_args_t                 write_arg;
    bacnet_read_args_t                  read_arg;
    method_send                         send_handle;
    method_recv                         recv_handle;
    int                                 method;
    int                                 value;
}bacnet_port_handle_t;


/* -----------------------------------------*/
/**
 * @brief bacnet_work_thread_function
 *  The bacnet work thread
 *
 * @param[in] arg   :   The object management pointer
 *
 * @return  have no return.
 */
/* -----------------------------------------*/
extern void* bacnet_work_thread_function(void* arg);

/* -----------------------------------------*/
/**
 * @brief bacnet_service_init
 *  bacnet physics initialze.
 *
 * @param[in] adapter       : bacnet work thread initial.
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int bacnet_service_init(object_management_t* adapter);

/** @} */

#endif /* INCLUDE_PROTOCOL_BACNET_BACCNET_H_ */
