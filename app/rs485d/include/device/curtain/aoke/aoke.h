/*
 * =====================================================================================
 *
 *          @file:  aoke.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Apr 1, 2016 7:56:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_DEVICE_CURTAIN_AOKE_AOKE_H_
#define INCLUDE_DEVICE_CURTAIN_AOKE_AOKE_H_

#include <stdbool.h>


/* ----------------------------------------- */
/** @addtogroup aoke    Aoke GuangZhou
 *  @ingroup curtain
 *
 *  Functions to aoke curtain interface.
 *
 *
 *  @{
 */

/* -----------------------------------------*/

/* -----------------------------------------*/
/**
 * @brief aoke_send_package_handle
 *  aoke curtian package a send buffer
 *
 * @param[in,out] arg   : The struct "mstp_port_handle_t" pointer
 *
 * @return  The send package length, if have a error return a negative value.
 */
/* -----------------------------------------*/
extern int aoke_send_package_handle(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief aoke_recv_package_handle
 *  aoke curtain process the receive package
 *
 * @param[in] arg       : The struct "mstp_port_handle_t" pointer
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int aoke_recv_package_handle(volatile void* arg);

/** @} */

#endif /* INCLUDE_DEVICE_CURTAIN_AOKE_AOKE_H_ */
