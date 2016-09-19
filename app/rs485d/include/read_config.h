/*
 * =====================================================================================
 *
 *          @file:  read_config.h
 *
 *    Description:  read configure for rs485 
 *
 *        Version:  1.0
 *        Created:  03/29/2016 09:51:36 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_READ_CONFIG_H_
#define INCLUDE_READ_CONFIG_H_


extern int glb_config_general_work_queue_depth;


extern int glb_config_bacnet_work_queue_depth;


extern int glb_config_modbus_work_queue_depth;


extern int glb_config_adapter_message_queue_depth;


extern int glb_config_general_work_package_mtu;


#endif
