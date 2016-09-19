/**
 * www.enno.com
 *
 * @file :	device.h
 *
 * @date : 	Mar 24, 2016
 *
 * @author: wong
 *
 *
 */

#ifndef INCLUDE_DEVICE_H_
#define INCLUDE_DEVICE_H_

#include "enum.h"
#include "timer_task.h"
#include "adapter.h"
#include "support.h"

/* ----------------------------------------- */
/** @addtogroup device_management  Device management
 *  @ingroup management
 *
 *  Functions to rs485 device create , delete , management.
 *
 *
 *  @{
 */


/* -----------------------------------------*/
/**
 * @brief device
 *  define the device management struct
 */
/* -----------------------------------------*/
typedef struct device_management
{
    /** The device pointer self */
    struct device_management*   device;
    /** The device name */
    char*                       device_name;
    /** The device factory name */
    rs485_factory_name_enum     factory_name;
    /** The device belong to RS485 object */
    int                         object_id;
    /** The device protocol, It's define by enum.h too */
    rs485_protocol_type_enum    object_type;
    /** The device id, It's a key */
    int                         device_id;
    /** The device type */
    rs485_device_type_enum      device_type;
    /** The device address len */
    int                         device_addr_len;
    /** The device address, The used length is 4, just for struct have align */
    unsigned char               device_addr[4];
    /** The rs485 device have use the bus time, The max time is 1s */
    unsigned int                time_out;
    /** The rs485 device timer task cyc period */
    unsigned int                device_status_period;
    /** The rs485 device send to fail, and retransmission count */
    unsigned int                retransmission;
    /** The device have support reply */
    bool                        support_reply;
    /** The device information */
    read_device_return_t*       device_info;
    /** The timer task, every device have create a timer task */
    timer_task_t*               timer;
    /** The device have a private profile */
    struct device_profile*      device_private;
    /** The device private profile numbers */
    int                         device_private_numbers;
}device_management_t;


/* -----------------------------------------*/
/**
 * @brief create_device
 *  create a rs485 device, mount the device to protocol
 *
 * @param[in] adapter :        The adapter message service type
 *
 * @return  return the device id. if the device id have a negative value, you have create device fail.
 */
/* -----------------------------------------*/
extern int create_device(adapter_t* adapter);

/* -----------------------------------------*/
/**
 * @brief delete_device
 *  delete a device form device management table.
 *
 * @param[in] device_id :       The device id .
 * @param[in] object_id :       The object id .
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int delete_device(int object_id, int device_id);

/* -----------------------------------------*/
/**
 * @brief get_device_name
 *  get a device name from device database.
 *
 * @param[in,out] out   :       The device name have write it.
 * @param[in] out_len   :       The devide name buffer length.
 * @param[in] device_id :       The device id.
 *
 * @return  0 is success, and others is fail.
 */
/* -----------------------------------------*/
extern int get_device_name(char* out, int out_len, int device_id);

/* -----------------------------------------*/
/**
 * @brief get_device_type
 *  get a device type from device database, just like air condition, fresh air.....
 *
 * @param[in] device_id :       The device id.
 *
 * @return  0 is success, and others is fail.
 */
/* -----------------------------------------*/
extern int get_device_type(int device_id);

/* -----------------------------------------*/
/**
 * @brief get_device_protocol
 *  get a device protocol from device database, just like BACnet, MODUBS...
 *
 * @param[in] device_id :       The device id.
 *
 * @return  0 is success, and others is fail.
 */
/* -----------------------------------------*/
extern int get_device_protocol(int device_id);

/* -----------------------------------------*/
/**
 * @brief get_device_addr
 *  get a rs485 device addr, you maybe have no address for some device.
 *
 * @param[in,out] addr  :       The device address pointer
 * @param[in] addr_len  :       The device address buffer length.
 * @param[in] device_id :       The device id.
 *
 * @return  0 is success, and others is fail.
 */
/* -----------------------------------------*/
extern int get_device_addr(unsigned char* addr, unsigned int addr_len, int device_id);

/* -----------------------------------------*/
/**
 * @brief get_device_timer
 *  get a device timer task.
 *
 * @param[in] device_id :       The devcie id.
 *
 * @return  0 is success, and others is fail.
 *
 * FIXME: the timer pointer is have memcopy a buffer, so, the struct have used?
 */
/* -----------------------------------------*/
extern inline timer_task_t* get_device_timer(int device_id);

/* -----------------------------------------*/
/**
 * @brief get_device_private
 *  get a device private profile
 *
 * @param[in] device_id :       The device id.
 *
 * @return  The private ppointer, if error ,and return NULL.
 * FIXME: the private pointer is have memcopy a buffer, so, the struct have used?
 */
/* -----------------------------------------*/
extern inline struct device_profile* get_device_private(int device_id);

/* -----------------------------------------*/
/**
 * @brief get_device_private_numbers
 *
 * @param[in] device_id :       The device id
 *
 * @return  The private profile numbers, if error, and return negative value.
 */
/* -----------------------------------------*/
extern inline int get_device_private_numbers(int device_id);

/* -----------------------------------------*/
/**
 * @brief check_object_id
 *  check the object is legal
 *
 * @param[in] device_id :       The need to check device id.
 *
 * @return  if object id is legal return true, and return false.
 */
/* -----------------------------------------*/
extern inline bool check_device_id(int device_id);

/* -----------------------------------------*/
/**
 * @brief get_device_object_id
 *  get the object id by device id
 *
 * @param[in] device_id :       The device id.
 *
 * @return  The object id, if have a error return negative value.
 */
/* -----------------------------------------*/
extern inline int get_device_object_id(int device_id);

/* -----------------------------------------*/
/**
 * @brief get_device_factory_name
 *  Get the device factory name
 *
 * @param[in] device_id :       The device id.
 *
 * @return  The device factory name numbers define by enum.h
 */
/* -----------------------------------------*/
extern inline int get_device_factory_name(int device_id);

/* -----------------------------------------*/
/**
 * @brief get_device_retransmission
 *  Get the device retransmission count on bus
 *
 * @param[in] device_id :       The device id.
 *
 * @return  The device retransmission numbers, if have a error return negative value.
 */
/* -----------------------------------------*/
extern inline int get_device_retransmission(int device_id);

/* -----------------------------------------*/
/**
 * @brief get_device_timeout_ms
 *  Get The device timeout (ms), The bus have send a package have wait timeout count.
 *
 * @param[in] device_id :       The device id.
 *
 * @return  The device timeout, if have a error return negative value.
 */
/* -----------------------------------------*/
extern inline int get_device_timeout_ms(int device_id);

/* -----------------------------------------*/
/**
 * @brief get_device_address_len
 *  Get the device address length.
 *
 * @param[in] device_id :       The device id.
 *
 * @return  The device address len, if have a error return negative value.
 */
/* -----------------------------------------*/
extern inline int get_device_address_len(int device_id);

/* -----------------------------------------*/
/**
 * @brief get_device_management
 *  get the device management pointer
 *
 * @param device_id :           The device id
 *
 * @return  The device management pointer, if error, and return NULL.
 */
/* -----------------------------------------*/
extern device_management_t* get_device_management(int device_id);

/* -----------------------------------------*/
/**
 * @brief device_managemnt_init
 *  The device management modele have a initialize
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int device_managemnt_init(void);

/* -----------------------------------------*/
/**
 * @brief set_read_device_information
 *  bus have get a device information have wirte it.
 *
 * @param[in] info      : The device private information
 * @param[in] device_id : The device id.
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int set_read_device_information(const read_device_return_t* info, int device_id);

/* -----------------------------------------*/
/**
 * @brief get_read_device_information
 *  It's read a device information called by adapter layer.
 *
 * @param[out] out      : The device private information
 * @param[in] device_id : The device id.
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int get_read_device_information(read_device_return_t* out, int device_id);

/** @} */

#endif /* INCLUDE_DEVICE_H_ */
