
/*
 * =====================================================================================
 *
 *       Filename:  mac_ccs_01m.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Dec 7, 2016 10:41:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#ifndef RS485_INCLUDE_DEVICE_AIRCONDITION_MISTSUBISHI_MAC_CCS_01M_H_
#define RS485_INCLUDE_DEVICE_AIRCONDITION_MISTSUBISHI_MAC_CCS_01M_H_


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
extern int mistsubishi_mac_ccs_01m_send_package_handle(volatile void* arg);

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
extern int mistsubishi_mac_ccs_01m_get_device_info_send(volatile void* arg);

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
extern int mistsubishi_mac_ccs_01m_get_device_info_handle(volatile void* arg);


/** @} */


#endif /* RS485_INCLUDE_DEVICE_AIRCONDITION_MISTSUBISHI_MAC_CCS_01M_H_ */
