/*
 * =====================================================================================
 *
 *          @file:  vrv_manager.h
 *
 *    Description:  offter the hitachi air conditoner interface
 *
 *        Version:  1.0
 *        Created:  Jun 28, 2016 10:35:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_DEVICE_AIRCONDITION_HITACHI_VRV_MANAGER_H_
#define INCLUDE_DEVICE_AIRCONDITION_HITACHI_VRV_MANAGER_H_


/* ----------------------------------------- */
/** @addtogroup hitachi      VRV_MANAGER QingDao
 *  @ingroup air_condition
 *
 *  Functions to HITACHI VRV MANAGER air condition interface.
 *
 *
 *  @{
 */

/* -----------------------------------------*/


/* -----------------------------------------*/
/**
 * @brief hitachi_vrv_manager_send_package_handle
 *  The hitachi vrv manager device send modbus handle
 *
 * @param[in,out] arg :  The modbus_port_handle_t struct.
 *
 * @return  The send buffer frame length
 */
/* -----------------------------------------*/
int hitachi_vrv_manager_send_package_handle(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief hitachi_vrv_manager_get_device_info_send
 *  The hitachi vrv air condition send package
 *
 * @param[in, out] arg : The modbus_port_handle_t struct
 *
 * @return  The send buffer frame length
 */
/* -----------------------------------------*/
int hitachi_vrv_manager_get_device_info_send(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief hitachi_vrv_manager_get_device_info_handle
 *  The air condition information process
 *
 * @param[in] arg : The modubs_port_handle_t struct
 *
 * @return  The process the modbus result.
 */
/* -----------------------------------------*/
int hitachi_vrv_manager_get_device_info_handle(volatile void* arg);


/** @} */

#endif /* INCLUDE_DEVICE_AIRCONDITION_HITACHI_VRV_MANAGER_H_ */
