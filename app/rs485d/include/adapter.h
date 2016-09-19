/**
 * www.enno.com
 *
 * @file :	adapter.h
 *
 * @date : 	Mar 15, 2016
 *
 * @author: wong
 *
 *
 */

#ifndef INCLUDE_ADAPTER_H_
#define INCLUDE_ADAPTER_H_

#include "enum.h"

#include <stdbool.h>


/* ----------------------------------------- */
/** @addtogroup adapter     Adapter management
 *  @ingroup management
 *
 *  Functions to rs485 device create , delete , management.
 *
 *
 *  @{
 */




/* -----------------------------------------*/
/**
 * @brief The rs485 port physical
 */
/* -----------------------------------------*/
typedef struct
{
    /** The rs485 baud rate, like 9600, 115200 ... */
    unsigned int                    baud_rate;
    /** The rs485 port profile, like /dev/ttyS1, /dev/usbS0....*/
    char                            interface_name[16];
    /** The rs485 parity profile */
    rs485_port_parity_type_enum     parity;
}rs485_port_t;

/* -----------------------------------------*/
/**
 * @brief message create a rs485 object
 */
/* -----------------------------------------*/
typedef struct
{
    /** The rs485 object name */
    char                            object_name[36];
    /** The rs485 protocol type , every protocol type represent a object */
    rs485_protocol_type_enum        object_type;
    /** The rs485 object mount max device numbers */
    int                             mount_device_max;
    /** The rs485 port message */
    rs485_port_t                    port;
    /** The rs485 address, just master device address */
    unsigned char                   address[4];
    /** The address length */
    int                             address_len;
}create_object_t;


/* -----------------------------------------*/
/**
 * @brief message create a rs485 object return
 */
/* -----------------------------------------*/
typedef struct
{
    /** The object ID, the id have created by server */
    int                             object_id;
}create_object_return_t;


/* -----------------------------------------*/
/**
 * @brief message delete a rs485 object
 */
/* -----------------------------------------*/
typedef struct
{
    /** The object id, your want to delete it. */
    int                             object_id;
}delete_object_t;

/* -----------------------------------------*/
/**
 * @brief message delete a rs485 object return
 */
/* -----------------------------------------*/
typedef struct
{
    int                             delete_status;
}delete_object_return_t;

/* -----------------------------------------*/
/**
 * @brief message mount a device to rs485 object
 */
/* -----------------------------------------*/
typedef struct
{
    /** The device name  */
    char                            device_name[36];
    /** The device factory */
    rs485_factory_name_enum         factory_name;
    /** The device mount the which object,so ,you must have crate a object frist */
    int                             object_id;
    /** The rs485 protocol type, The object type, we need to check it */
    rs485_protocol_type_enum        object_type;
    /** The rs485 device address, and the address maybe to NULL */
    char                            device_addr[4];
    /** The rs485 device address length */
    unsigned int                    device_addr_len;
    /** The rs485 device type, reference enum.h */
    rs485_device_type_enum          device_type;
    /** The rs485 device have use the bus time, The max time is 1s */
    unsigned int                    time_out;
    /** The rs485 device have wiat the device reply */
    unsigned int                    support_reply;
    /** The rs485 device timer task cyc period */
    unsigned int                    device_status_period;
    /** The rs485 device send to fail, and retransmission count */
    unsigned int                    retransmission;
}mount_devcie_to_object_t;

/* -----------------------------------------*/
/**
 * @brief message mount a device to rs485 object return
 */
/* -----------------------------------------*/
typedef struct
{
    /** return the device id , if the device have a negative value, It's mount fail */
    int                             device_id;
}mount_device_to_object_return_t;

/* -----------------------------------------*/
/**
 * @brief message unmount a device form rs485 object
 */
/* -----------------------------------------*/
typedef struct
{
    /** The device id what you want to unmount */
    int                             device_id;
    /** The object id that the device have mounted*/
    int                             object_id;
}unmount_device_from_object_t;

/* -----------------------------------------*/
/**
 * @brief message unmount a device from rs485 ojbect return
 */
/* -----------------------------------------*/
typedef struct
{
    /** The device unmount status */
    int                             unmount_status;
}unmount_device_from_object_return_t;

/* -----------------------------------------*/
/**
 * @brief message write value to device
 */
/* -----------------------------------------*/
typedef struct
{
    /** The device id, you must create a device, you can used it */
    int                             device_id;
    /** The write message is broadcast */
    bool                            broadcast;
    /** The device method, you can reference enum.h */
    unsigned int                    device_method;
    /** The device method value, just like set the air condition 24 */
    int                             method_value;
    /** The reserve value, you can't used it. */
    int                             value_reserve;
}write_device_t;

/* -----------------------------------------*/
/**
 * @brief message write value to device return
 */
/* -----------------------------------------*/
typedef struct
{
    /** The write value return status, just to wirte to work thread have return the struct */
    int                             write_status;
}write_device_return_t;

/* -----------------------------------------*/
/**
 * @brief message read value from device
 */
/* -----------------------------------------*/
typedef struct
{
    /** The device id what you want to read device value */
    int                             device_id;
}read_device_t;



/* -----------------------------------------*/
/**
 * @brief The air conditon profile
 */
/* -----------------------------------------*/
typedef struct
{
    int                             room_temperature;
    int                             outdoor_temperature;
    int                             pipe_temperature;
    int                             current_mode;
    int                             current_swing;
    int                             current_fan;
    int                             current_set_temperature;
}air_condition_profile_t;

/* -----------------------------------------*/
/**
 * @brief The curtain profile
 */
/* -----------------------------------------*/
typedef struct
{
    int                             current_percent;

}curtain_profile_t;

/* -----------------------------------------*/
/**
 * @brief The fresh profile
 */
/* -----------------------------------------*/
typedef struct
{
    int                             room_temperature;
    int                             room_humidity;
    int                             pm2_5;
    int                             fresh_level;
}fresh_air_profile_t;

/* -----------------------------------------*/
/**
 * @brief rs485 device profile
 */
/* -----------------------------------------*/
union rs485_device_profile
{
    /** The air conditioner profile */
    air_condition_profile_t         air_condition;
    /** The curtain conditioner profile */
    curtain_profile_t               curtain;
    /** The fresh air profile */
    fresh_air_profile_t             fresh_air;
};

/* -----------------------------------------*/
/**
 * @brief message read value from device return
 */
/* -----------------------------------------*/
typedef struct
{
    /** The read request status */
    bool                            read_status;
    /** the read device status */
    bool                            runing;
    /** the device have a error status */
    bool                            error;
    /** the device profile , have fill it */
    union rs485_device_profile      profile;
}read_device_return_t;


/* -----------------------------------------*/
/**
 * @brief define the receive the message type
 */
/* -----------------------------------------*/
typedef union
{
    /* have receive message from client */
    create_object_t                     new_object;
    delete_object_t                     delete_object;
    mount_devcie_to_object_t            mount_device;
    unmount_device_from_object_t        unmount_device;
    write_device_t                      write;
    read_device_t                       read;

    /* have send result message to client */
    create_object_return_t              new_object_return;
    delete_object_return_t              delete_object_return;
    mount_device_to_object_return_t     mount_device_return;
    unmount_device_from_object_return_t unmount_device_return;
    write_device_return_t               write_return;
    read_device_return_t                read_return;
}message_service_t;

/* -----------------------------------------*/
/**
 * @brief define the adapter struct
 */
/* -----------------------------------------*/
typedef struct
{
    /** The message service type */
    rs485_service_type_enum         message_type;
    /** The message length, just like "sizeof(struct adapter_t)", It's used to check the pakcage imperfections  */
    unsigned int                    message_length;
    /** The message retvl, just message process retvl, the retvl, have used to client to check the service */
    int                             message_retvl;
    /** The message socket id */
    int                             socket_fd;
    /** The message content */
    message_service_t               message_content;
}adapter_t;

/** @} */


#endif /* INCLUDE_ADAPTER_H_ */
