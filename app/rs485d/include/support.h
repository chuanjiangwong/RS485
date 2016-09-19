/*
 * =====================================================================================
 *
 *          @file:  support.h
 *
 *    Description:  The rs485 device support register
 *
 *        Version:  1.0
 *        Created:  03/25/2016 06:47:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_SUPPORT_H_
#define INCLUDE_SUPPORT_H_

#include "enum.h"
#include "adapter.h"

#include <stdbool.h>


/* ----------------------------------------- */
/** @addtogroup support     Device register management
 *  @ingroup management
 *
 *  Functions to device register on rs485 service.
 *
 *
 *  @{
 */



/* -----------------------------------------*/
/**
 * @brief int
 *  you have full the context pointer.
 *
 * @note
 * <pre>
 *      if the interface is general, The context = mstp_port_handle_t* handle;
 *      if the interface is bacnet, The context = bacnet_port_handle_t* handle;
 *      if the interface is modbus, The context = modbus_port_handle_t* handle;
 * </pre>
 *
 * @param context , The *_port_handle_t pointer.
 *
 * @return The send byte numbers, or, return nagetive value
 */
/* -----------------------------------------*/
typedef int (*method_send)(volatile void* context);
/* -----------------------------------------*/
/**
 * @brief int
 *  you have full the context pointer.
 *
 * @note
 * <pre>
 *      if the interface is general, The context = mstp_port_handle_t* handle;
 *      if the interface is bacnet, The context = bacnet_port_handle_t* handle;
 *      if the interface is modbus, The context = modbus_port_handle_t* handle;
 * </pre>
 *
 * @param context , The *_port_handle_t pointer.
 *
 * @return The send byte numbers, or, return nagetive value
 */
/* -----------------------------------------*/
typedef int (*method_recv)(volatile void* context);


/* -----------------------------------------*/
/**
 * @brief device_profile
 *  device process method
 */
/* -----------------------------------------*/
struct device_profile
{
    /* The device address length */
    int                         addr_real_len;
    /* The device support command , The command define on enum.h */
    int                         method;
    /* The device command send callback function */
    method_send                 send;
    /* The device command receive callback function */
    method_recv                 recv;
};


/* -----------------------------------------*/
/**
 * @brief check_device_support
 *  check the device have supported by rs485 service
 *
 * @param[in] adatper : The adapter struct, It's define by adapther.h
 *
 * @return  If the rs485 service have support this device return true, or return false.
 */
/* -----------------------------------------*/
extern bool check_device_support(const adapter_t* adatper);

/* -----------------------------------------*/
/**
 * @brief get_support_device_profile
 *  Get the device profile, The struct device_profile
 *
 * @param[in] name :    The device factory name enum, It's define by enum.h
 *
 * @return  return The device profile pointer, or return NULL.
 */
/* -----------------------------------------*/
extern struct device_profile* get_support_device_profile(rs485_factory_name_enum name);

/* -----------------------------------------*/
/**
 * @brief get_support_device_profile_numbers
 *  Get the device profile have support how many command.
 *
 * @param[in] name :    The device factory name enum, It's define by enum.h
 *
 * @return  return the numbers about of device support command, or return negative value.
 */
/* -----------------------------------------*/
extern int get_support_device_profile_numbers(rs485_factory_name_enum name);

/* -----------------------------------------*/
/**
 * @brief get_device_send_package_function
 *  Get the device profile send package callback function
 *
 * @param[in] profile :         The device profile pointer.
 * @param[in] profile_numbers : The device profile have support command numbers.
 * @param[in] command :         The which command you have chose.
 *
 * @return  return the device method send package callback function pointer, or return NULL.
 */
/* -----------------------------------------*/
extern method_send inline get_device_send_package_function(const struct device_profile* profile, int profile_numbers, int command);

/* -----------------------------------------*/
/**
 * @brief get_device_recv_package_function
 *  Get the device profile receive package callback function
 *
 * @param[in] profile :         The device profile pointer.
 * @param[in] profile_numbers : The device profile have support command numbers.
 * @param[in] command :         The which command you have chose.
 *
 * @return  return the device method recv package callback function pointer, or return NULL.
 */
/* -----------------------------------------*/
extern method_recv inline get_device_recv_package_function(const struct device_profile* profile, int profile_numbers, int command);


/** @} */
#endif   /* INCLUDE_SUPPORT_H_ */
