/*
 * =====================================================================================
 *
 *          @file:  panasonnic.h
 *
 *    Description:  The panasonnic air conditoner interface
 *
 *        Version:  1.0
 *        Created:  04/01/2016 04:57:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_DEVICE_AIRCONDITION_PANASONNIC_PANASONNIC_H_
#define INCLUDE_DEVICE_AIRCONDITION_PANASONNIC_PANASONNIC_H_


/* ----------------------------------------- */
/** @addtogroup panasonnic      Panasonnic GuangZhou
 *  @ingroup air_condition
 *
 *  Functions to Panasonnic air condition interface.
 *
 *
 *  @{
 */

/* -----------------------------------------*/



/* -----------------------------------------*/
/**
 * @brief panasonnic_send_package_handle
 *  The panasonnic package a send buffer interface.
 *
 * @param[in,out] arg   : The struct "mstp_port_handle_t" defined by general.h
 *
 * @return      Return The send package length, if have a error , return a negative value.
 */
/* -----------------------------------------*/
extern int panasonnic_send_package_handle(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief panasonnic_send_package_handle
 *  The panasonnic package a receive buffer processs interface.
 *
 * @param[in] arg   : The struct "mstp_port_handle_t" defined by general.h
 *
 * @return      0 is success , others is fail.
 */
/* -----------------------------------------*/
extern int panasonnic_recv_package_handle(volatile void* arg);


/** @} */

#endif // AIR_CONDITIONER_H
