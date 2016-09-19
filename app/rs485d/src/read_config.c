/*
 * =====================================================================================
 *
 *          @file:  read_config.c
 *
 *    Description:  read configure for RS485
 *
 *        Version:  1.0
 *        Created:  03/29/2016 09:49:17 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#include "read_config.h"



/* -----------------------------------------*/
/**
 * @brief about the general configure
 */
/* -----------------------------------------*/
/**
 * The general work thread work queue depth, if the queue is ful,
 *  the new message have not go to the work queue
 */
int glb_config_general_work_queue_depth = 256;

/** The general work thread have a buffer to send or receive the bus data */
int glb_config_general_work_package_mtu = 512;





/* -----------------------------------------*/
/**
 * @brief about the bacnet configure
 */
/* -----------------------------------------*/
/**
 * The bacnet work thread work queue depth
 */
int glb_config_bacnet_work_queue_depth = 128;

/** every bacnet device have a device instance,
 * This object intacne is pannoS device_id*/
int glb_config_bacnet_object_instance = 10086;




/* -----------------------------------------*/
/**
 * @brief about the modbus configure
 */
/* -----------------------------------------*/
/** The modbus work queue depth */
int glb_config_modbus_work_queue_depth = 256;





/* -----------------------------------------*/
/**
 * @brief about the system arch configure
 */
/* -----------------------------------------*/
/** The adapter queue depth, The queue is process the client data */
int glb_config_adapter_message_queue_depth = 32;

/** The server have support the client max connect numbers */
int glb_config_client_max_numbers = 10;






