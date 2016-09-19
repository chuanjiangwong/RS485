/*
 * =====================================================================================
 *
 *          @file:  object.h
 *
 *    Description:  The RS485 object management
 *
 *        Version:  1.0
 *        Created:  03/25/2016 02:00:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_OBJECT_H_
#define INCLUDE_OBJECT_H_


#include "enum.h"
#include "adapter.h"
#include "ringbuf.h"

#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>


/* ----------------------------------------- */
/** @addtogroup object  Object management
 *  @ingroup management
 *  Functions to rs485 Object create ,delete, management.
 *
 *  <pre>
 *      The object It's consist of the rs485 protocol .
 *      The Modbus is a object,
 *      The BACnet is a object,
 *
 *      every object have create a work thread to process the work.
 *
 *  </pre>
 *
 *  @{
 */


/* -----------------------------------------*/
/**
 * @brief object_management
 *  define the object management struct
 */
/* -----------------------------------------*/
typedef struct object_management
{
    /** The linux thread descriptor , it's be used to save the work thread*/
    pthread_t                       object_thread;
    /** The work thread queue depth */
    int                             queue_depth;
    /** The object ID */
    int                             object_id;
    /** The object name , It's malloc */
    char*                           object_name;
    /** The rs485 protocol type, It's defined by enum.h */
    rs485_protocol_type_enum        object_type;
    /** The rs485 object have mount max device numbers */
    int                             mount_device_max;
    /** The rs485 object UART physics information, The port struct have define by adapter.h */
    rs485_port_t                    port;
    /** The rs485 object MAC address, It's have 1 byte  */
    unsigned char                   address[4];
    /** The rs485 object address length */
    int                             address_len;
    /** The buffer have save the device id, It's malloc */
    int                             *mount_device;
    /** The work thread have use a queue */
    struct ring_buffer_t            *work_queue;
    /** The work queue buffer, It's malloc */
    uint8_t                        	*work_queue_buffer;
    /** The work queue semaphore , it's be used to save the work queue semaphore*/
    sem_t                           queue_sem;
    /** The pointer have save the device private profile */
    void*                           object_private;
}object_management_t;


/* -----------------------------------------*/
/**
 * @brief create_object
 *  create a object by the adapter message
 *
 * @param[in] adapter : The adapter message
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int create_object(const adapter_t* adapter);


/* -----------------------------------------*/
/**
 * @brief delete_object
 *  delete a rs485 object by object id
 *
 * @param[in] object_id : The you want to delete object id.
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int delete_object(int object_id);


/* -----------------------------------------*/
/**
 * @brief check_object_id
 *  check the object is legal
 *
 * @param[in] object_id : The need to check object id.
 *
 * @return  if object id is legal return true, and return false.
 */
/* -----------------------------------------*/
extern inline bool check_object_id(int object_id);

/* -----------------------------------------*/
/**
 * @brief get_object_type
 *  get the object protocol type
 *
 * @param[in] object_id : The object id
 *
 * @return    return the protocol type, return a negative value is error
 */
/* -----------------------------------------*/
extern int get_object_type(int object_id);


/* -----------------------------------------*/
/**
 * @brief get_object_mount_device
 *  get the object mount device
 *
 * @param[in] object_id     :   The object id
 * @param[out] out_id       :   Out the device id on this object
 * @param[in] out_id_len    :   The out buffer length.
 *
 * @return   return the mount device numbers ,negative value is error
 */
/* -----------------------------------------*/
extern int get_devices_from_object(int object_id, int* out_id, int out_id_len);


/* -----------------------------------------*/
/**
 * @brief check_object_numbers_have_idle
 *  check object mount device is full ?
 *
 * @param[in] object_id     :   The object id
 *
 * @return  If the object have mount device have not full return true, or return false.
 */
/* -----------------------------------------*/
extern bool check_object_numbers_have_idle(int object_id);

/* -----------------------------------------*/
/**
 * @brief object_mount_device_id
 *  add a device to his object
 *
 * @param[in] object_id     :   The object id
 * @param[in] device_id     :   The device id
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int mount_a_device_to_object(int object_id, int device_id);

/* -----------------------------------------*/
/**
 * @brief object_unmount_device_id
 *  delete a device form his object
 *
 * @param[in] object_id     :   The object id
 * @param[in] device_id     :   The device id
 */
/* -----------------------------------------*/
extern int unmount_a_device_from_object(int object_id, int device_id);

/* -----------------------------------------*/
/**
 * @brief get_object_work_queue
 *  get the object of work queue
 *
 * @param[in] object_id     :   The object id
 *
 * @return  The object of work queue pointer, or return NULL.
 */
/* -----------------------------------------*/
extern void* get_object_work_queue(int object_id);

/* -----------------------------------------*/
/**
 * @brief get_object_queue_sem
 *  get the object of work queue semphore
 *
 * @param[in] object_id     :   The object id
 *
 * @return  The object of work queue semphore pointer, or return NULL.
 */
/* -----------------------------------------*/
extern void* get_object_queue_sem(int object_id);

/** @} */

#endif /* INCLUDE_OBJECT_H */
