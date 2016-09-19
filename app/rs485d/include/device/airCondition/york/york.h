/*
 * york.h
 *
 *  Created on: Nov 3, 2015
 *      Author: wong
 */

#ifndef DEMO_ENNO_BACNET_YORK_AIR_CONDITIONER_YORK_H_
#define DEMO_ENNO_BACNET_YORK_AIR_CONDITIONER_YORK_H_

#include "protocol/bacnet/handle_property.h"

/* ----------------------------------------- */
/** @addtogroup york_guangzhou  YORK GuangZhou KeLong
 *  @ingroup air_condition
 *
 *  Functions to york air condition interface.
 *
 *
 *  @{
 */

#if 0
/* -----------------------------------------*/

/* -----------------------------------------*/
/**
 * @brief get_air_york_write_args
 *  The york air condition bacnet interface
 *
 * @param[in,out] args  :   The bacnet write struct, so need to full it.
 * @param[in] device_id :   The bacnet device id.
 * @param[in] command   :   The device command, method too.
 * @param[in] value     :   The value mayto is ununsed.
 *
 * @return    0 is success, and others is fail.
 */
/* -----------------------------------------*/
extern int get_air_york_write_args(bacnet_write_args_t* args, unsigned int device_id,
        int command, int value);


/* -----------------------------------------*/
/**
 * @brief get_air_york_read_args
 *  The york air confition bacnet read interface
 *
 * @param[in,out] args  :   The bacnet read struct, so need to full it.
 * @param[in] device_id :   The bacnet device id.
 *
 * @return   0 is success, and others is fail.
 */
/* -----------------------------------------*/
extern int get_air_york_read_args(bacnet_read_args_t* args, unsigned int device_id);


/* -----------------------------------------*/
/**
 * @brief get_air_york_instance
 *  get the youk bacnet instance.
 *
 * @param[in] mac :     The device MAC address
 *
 * @return  return the instance, if return negative value is error
 */
/* -----------------------------------------*/
extern int get_air_york_instance(unsigned char mac);
#endif




/* -----------------------------------------*/
/**
 * @brief york_kelong_set_temperature
 *  set kelong air condition temperature send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  Return the set temperature send package length, if have a error ,return a negative value.
 */
/* -----------------------------------------*/
extern int york_kelong_set_temperature(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief york_kelong_set_mode
 *  set kelong air conditon mode send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  Return the set mode send package length, if have a error , return a negative value.
 */
/* -----------------------------------------*/
extern int york_kelong_set_mode(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief york_kelong_set_swing
 *  set kelong air conditon swing send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  Return the set swing send package length, if have a error , return a negative value.
 */
/* -----------------------------------------*/
extern int york_kelong_set_swing(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief york_kelong_set_fan
 *  set kelong air conditon fan send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  Return the set fan send package length, if have a error , return a negative value.
 */
/* -----------------------------------------*/
extern int york_kelong_set_fan(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief york_kelong_set_switch
 *  set kelong air conditon switch send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  Return the set switch send package length, if have a error , return a negative value.
 */
/* -----------------------------------------*/
extern int york_kelong_set_switch(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief york_kelong_get_device_info_send
 *  set kelong air conditon device information send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  Return the set device information send package length, if have a error , return a negative value.
 */
/* -----------------------------------------*/
extern int york_kelong_get_device_info_send(volatile void* arg);

/* -----------------------------------------*/
/**
 * @brief york_kelong_get_device_info_handle
 *  process kelong air conditon get device information send package to "modbus_port_handle_t"
 *
 * @param[in, out] arg : The struct (modbus_port_handle_t) pointer
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int york_kelong_get_device_info_handle(volatile void* arg);

/** @} */

#endif /* DEMO_ENNO_BACNET_YORK_AIR_CONDITIONER_YORK_H_ */
