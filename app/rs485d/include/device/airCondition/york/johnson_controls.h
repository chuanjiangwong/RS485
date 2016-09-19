/*
 * =====================================================================================
 *
 *          @file:  johnson_controls.h
 *
 *    Description:  offer the johnson controls air conditioner device interface
 *
 *        Version:  1.0
 *        Created:  Jul 4, 2016 10:48:39 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_DEVICE_AIRCONDITION_YORK_JOHNSON_CONTROLS_H_
#define INCLUDE_DEVICE_AIRCONDITION_YORK_JOHNSON_CONTROLS_H_

/* ----------------------------------------- */
/** @addtogroup johnson      Johnson Controls WuXi
 *  @ingroup air_condition
 *
 *  Functions to Johnson air condition interface.
 *
 *
 *  @{
 */

/* -----------------------------------------*/


/* -----------------------------------------*/
/**
 * @brief johnson_controls_vrv_send_package_handle
 *  The johnson controls air condition device send package handle
 *
 * @param[in,out] arg ,     The modbus_port_interface_t struct
 *
 * @return  Send buffer frame length
 */
/* -----------------------------------------*/
extern int johnson_controls_vrv_send_package_handle(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief johnson_controls_vrv_get_device_info_send
 *  The johnson controls air condition get device information send handle
 *
 * @param[in,out] arg ,     The modbus_port_interface_t struct
 *
 * @return  Send buffer frame length, if have a error, return negative value.
 */
/* -----------------------------------------*/
extern int johnson_controls_vrv_get_device_info_send(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief johnson_controls_vrv_get_device_info_handle
 *  The johnson device information handle
 *
 * @param[in,out]  arg ,    The modbus_port_interface_t struct
 *
 * @return process the device information to device
 */
/* -----------------------------------------*/
extern int johnson_controls_vrv_get_device_info_handle(volatile void* arg);


/** @} */

#endif /* INCLUDE_DEVICE_AIRCONDITION_YORK_JOHNSON_CONTROLS_H_ */
