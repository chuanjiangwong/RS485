/*
 * =====================================================================================
 *
 *          @file:  doya.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Apr 1, 2016 7:56:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_DEVICE_CURTAIN_DOYA_DOYA_H_
#define INCLUDE_DEVICE_CURTAIN_DOYA_DOYA_H_

#include <stdbool.h>
#include "adapter.h"

/* ----------------------------------------- */
/** @addtogroup dooya   Dooya HangZhou
 *  @ingroup curtain
 *
 *  Functions to dooya curtain interface.
 *
 *
 *  @{
 */
/* -----------------------------------------*/

/* -----------------------------------------*/
/**
 * @brief doya_send_package_handle
 *  The dooya curtain package a send buffer
 *
 * @param[in,out] arg   : The struct "mstp_port_handle_t" pointer
 *
 * @return  The send buffer package length, if have a error, return a negative value.
 */
/* -----------------------------------------*/
extern int doya_send_package_handle(volatile void* arg);
/* -----------------------------------------*/
/**
 * @brief doya_recv_package_handle
 *  The dooya curtain process the receive data.
 *
 * @param[in] arg       : The struct "mstp_port_handle_t" pointer
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int doya_recv_package_handle(volatile void* arg);

/** @} */

#endif /* INCLUDE_DEVICE_CURTAIN_DOYA_DOYA_H_ */
