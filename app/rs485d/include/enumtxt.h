/*
 * =====================================================================================
 *
 *          @file:  enumtxt.h
 *
 *    Description:  offer the enum text
 *
 *        Version:  1.0
 *        Created:  Apr 8, 2016 1:57:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_ENUMTXT_H_
#define INCLUDE_ENUMTXT_H_

#include <stdbool.h>

#include "enum.h"

/* ----------------------------------------- */
/** @addtogroup enum    String management
 *  @ingroup management
 *
 *  Functions to rs485 service enum to string.
 *
 *
 *  @{
 */


/* -----------------------------------------*/
/**
 * @brief get_enum_txt_service
 *  get enum rs485 service message type
 *
 * @param[in] type : rs485 service type , define on enum.h
 *
 * @return  a string about the message
 */
/* -----------------------------------------*/
extern char* get_enum_txt_service(rs485_service_type_enum type);

/* -----------------------------------------*/
/**
 * @brief get_enum_txt_rs485_device_type
 *  get enum rs485 device type
 *
 * @param[in] type : rs485 device type, define on enum.h
 *
 * @return  a string about the device type
 */
/* -----------------------------------------*/
extern char* get_enum_txt_rs485_device_type(rs485_device_type_enum type);

/* -----------------------------------------*/
/**
 * @brief get_enum_txt_rs485_protocol_type
 *  get enum rs485 protocol type
 *
 * @param[in] type : rs485 protocol type, define on enum.h
 *
 * @return  a string about the rs485 protocol type
 */
/* -----------------------------------------*/
extern char* get_enum_txt_rs485_protocol_type(rs485_protocol_type_enum type);

/* -----------------------------------------*/
/**
 * @brief get_enum_txt_device_method
 *  get enum device method(command)
 *
 * @param[in] type : rs485 device method type , define on enum.h
 *
 * @return  a string about the device command
 */
/* -----------------------------------------*/
extern char* get_enum_txt_device_method(rs485_device_method_enum type);

/* -----------------------------------------*/
/**
 * @brief get_enum_txt_device_factory
 *  get enum device factory name
 *
 * @param[in] name : The rs485 device factory name , define on enum.h
 *
 * @return  a string about the device factory name
 */
/* -----------------------------------------*/
extern char* get_enum_txt_device_factory(rs485_factory_name_enum name);

/* -----------------------------------------*/
/**
 * @brief get_enum_txt_bool
 *  get the string about bool value
 *
 * @param[in] status : The bool status
 *
 * @return   a string about the true and false value.
 */
/* -----------------------------------------*/
extern char* get_enum_txt_bool(bool status);

/** @} */

#endif /* INCLUDE_ENUMTXT_H_ */
