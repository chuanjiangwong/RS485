/*
 * =====================================================================================
 *
 *          @file:  DTA116A621.h
 *
 *    Description:  he daikin air condition interface Shang Hai dta116a621
 *
 *        Version:  1.0
 *        Created:  Apr 19, 2016 5:21:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_DEVICE_AIRCONDITION_DAIKIN_DTA116A621_H_
#define INCLUDE_DEVICE_AIRCONDITION_DAIKIN_DTA116A621_H_

/* ----------------------------------------- */
/** @addtogroup daikin_dta116a621   DaiKin DTA116A621
 *  @ingroup air_condition
 *
 *  Functions to DaiKin DTA116A621 interface.
 *
 *
 *  @{
 */

/* -----------------------------------------*/
/**
 * @brief daikin_dta116a621_set_temperature
 *  set daikin air condition temperature send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  Return the set temperature send package length, if have a error ,return a negative value.
 */
/* -----------------------------------------*/
extern int daikin_dta116a621_set_temperature(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief daikin_dta116a621_set_mode
 *  set daikin air conditon mode send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  Return the set mode send package length, if have a error , return a negative value.
 */
/* -----------------------------------------*/
extern int daikin_dta116a621_set_mode(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief daikin_dta116a621_set_swing
 *  set daikin air conditon swing send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  Return the set swing send package length, if have a error , return a negative value.
 */
/* -----------------------------------------*/
extern int daikin_dta116a621_set_swing(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief daikin_dta116a621_set_fan
 *  set daikin air conditon fan send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  Return the set fan send package length, if have a error , return a negative value.
 */
/* -----------------------------------------*/
extern int daikin_dta116a621_set_fan(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief daikin_dta116a621_set_switch
 *  set daikin air conditon switch send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  Return the set switch send package length, if have a error , return a negative value.
 */
/* -----------------------------------------*/
extern int daikin_dta116a621_set_switch(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief daikin_dta116a621_get_device_info_send
 *  set daikin air conditon device information send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  Return the set device information send package length, if have a error , return a negative value.
 */
/* -----------------------------------------*/
extern int daikin_dta116a621_get_device_info_send(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief daikin_dta116a621_get_device_info_handle
 *  process daikin air conditon get device information send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int daikin_dta116a621_get_device_info_handle(volatile void* arg);

/** @} */

#endif /* INCLUDE_DEVICE_AIRCONDITION_DAIKIN_DTA116A621_H_ */
