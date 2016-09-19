/*
 * =====================================================================================
 *
 *          @file:  loreley.h
 *
 *    Description:  The loreley fresh air interface
 *
 *        Version:  1.0
 *        Created:  Apr 12, 2016 4:28:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_DEVICE_FRESHAIR_LORELEY_LORELEY_H_
#define INCLUDE_DEVICE_FRESHAIR_LORELEY_LORELEY_H_


/* ----------------------------------------- */
/** @addtogroup loreley     Loreley ShenZhen
 *  @ingroup fresh_air
 *
 *  Functions to loreley fresh air interface.
 *
 *
 *  @{
 */
/* -----------------------------------------*/


/* -----------------------------------------*/
/**
 * @brief loreley_send_package_handle
 *  loreley fresh air package send a buffer
 *
 * @param[in,out] arg : The struct "mstp_port_handle_t" pointer.
 *
 * @return  The send buffer package length, if have a error ,return negative value.
 */
/* -----------------------------------------*/
extern int loreley_send_package_handle(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief loreley_recv_package_handle
 *  loreley fresh air process the receive data.
 *
 * @param[in] arg   : The struct "mstp_port_handle_t" pointer.
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int loreley_recv_package_handle(volatile void* arg);


/** @} */
#endif /* INCLUDE_DEVICE_FRESHAIR_LORELEY_LORELEY_H_ */
