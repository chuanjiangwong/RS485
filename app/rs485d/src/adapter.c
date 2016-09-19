/**
 * www.enno.com
 *
 * @file :	adapter.c
 *
 * @date : 	Mar 14, 2016
 *
 * @author:	chuanjiang.wong
 *
 *
 */


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Linux includes */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sched.h>
#include <linux/serial.h>       /* for struct serial_struct */
#include <sys/ioctl.h>
#include <errno.h>

/* Local includes */
#include "enum.h"
#include "adapter.h"
#include "message_queue.h"
#include "syslog/log.h"
#include "device.h"
#include "object.h"
#include "service.h"
#include "protocol/bacnet/bacnet.h"
#include "protocol/modbus/modbus.h"
#include "protocol/general/general.h"
#include "ringbuf.h"

#ifndef ADAPTER_MESSAGE_QUEUE_MAX_DEPTH
#define ADAPTER_MESSAGE_QUEUE_MAX_DEPTH             (48)
#endif


/** define the thread status */
static adapter_thread_status_enum                   adapter_thread_status = ADAPTER_THREAD_STATUS_START;
/** define the message queue */
static adapter_t                                    reply_client;

/* -----------------------------------------*/
/**
 * @brief adapter_thread_init
 *  initialze the adapter thread , and mesesage queue initial.
 *
 * @ingroup adapter
 *
 * @return  0 is initialize success, and others is fail.
 */
/* -----------------------------------------*/
static int adapter_thread_init(void)
{
    int error = 0;

    /* TODO:  add some others initialize things*/
    error = device_managemnt_init();

    return error;
}

/** The bacnet interface  */
static bacnet_port_handle_t        bacnet;
/** The modbus interface */
static modbus_port_handle_t        modbus;
/** The general interface */
static mstp_port_handle_t          general;

/* -----------------------------------------*/
/**
 * @brief process_write_value_service
 *  process the client write value to device service
 *
 * @ingroup adapter
 *
 * @param[in] adapter :    The adapter struct information
 *
 * @return  0 is success, and others is fail.
 */
/* -----------------------------------------*/
static int process_write_value_service(const adapter_t* adapter)
{
    int                         device_id = 0;
    int                         object_id = 0;
    int                         error = 0;
    RING_BUFFER                 *work_queue = NULL;
    sem_t                       *queue_sem = NULL;
    device_management_t         *device;


    if(adapter == NULL)
        return -EPERM;

    device_id = adapter->message_content.write.device_id;
    if(check_device_id(device_id) == false)
    {
        syslog_warning("device id:[%d] inval", device_id);
        return -EPERM;
    }

    object_id = get_device_object_id(device_id);
    if(check_object_id(object_id) == false)
    {
        syslog_warning("object id:[%d] inval", object_id);
        return -EPERM;
    }

    work_queue = get_object_work_queue(object_id);
    if(work_queue == NULL)
    {
        syslog_warning("%s", "work queue is inval");
        return -EPERM;
    }

    queue_sem = get_object_queue_sem(object_id);
    if(queue_sem == NULL)
    {
        syslog_warning("%s", "work queue sem is inval");
        return -EPERM;
    }

    device = get_device_management(device_id);
    if(device == NULL)
    {
        syslog_warning("%s", "get device management error");
        return -EPERM;
    }

    switch(get_device_protocol(device_id))
    {
        case RS485_PROTOCOL_TYPE_GENERAL:
            memset(&general, 0x00, sizeof(general));
            general.device_id = adapter->message_content.write.device_id;
            general.broadcast = adapter->message_content.write.broadcast;
            general.method = adapter->message_content.write.device_method;
            general.value = adapter->message_content.write.method_value;
            general.arg = NULL;
            if(general.broadcast == true)
                general.except_reply = false;
            else
                general.except_reply = true;

            if(device->support_reply == false)
                general.except_reply = false;

            general.send_handle = get_device_send_package_function(device->device_private, device->device_private_numbers, general.method);
            general.recv_handle = get_device_recv_package_function(device->device_private, device->device_private_numbers, general.method);
            general.address_len = device->device_addr_len;
            memcpy(general.address, device->device_addr, device->device_addr_len);
            general.retransmission = device->retransmission;
            general.timeout_ms = device->time_out;

            /* check the queue status */
            if(Ringbuf_Full(work_queue) == true)
            {
                syslog_warning("%s", "The work queue have full");
                return -EPERM;
            }

            /* push to work queue */
            if(Ringbuf_Put(work_queue, (uint8_t*)&general) == false)
            {
                syslog_warning("%s", "push work queue fail");
                return -EPERM;
            }

            /* send the sem to read queue */
            error = sem_post(queue_sem);
            if(error)
            {
                syslog_error("sem post error:%d", error);
            }

            syslog_debug("[adapter]:general write address:0x%02x, command:%d, value:%d",
                    general.address[0], general.method, general.value);

            break;

        case RS485_PROTOCOL_TYPE_BACNET:
            memset(&bacnet, 0x00, sizeof(bacnet));
            bacnet.method = adapter->message_content.write.device_method;
            bacnet.value = adapter->message_content.write.method_value;

            bacnet.mac[0] = device->device_addr[0];
            bacnet.send_handle = get_device_send_package_function(device->device_private, device->device_private_numbers, bacnet.method);
            bacnet.recv_handle = get_device_recv_package_function(device->device_private, device->device_private_numbers, bacnet.method);

            /* check the queue status */
            if(Ringbuf_Full(work_queue) == true)
            {
                syslog_warning("%s", "The work queue have full");
                return -EPERM;
            }

            /* push to work queue */
            if(Ringbuf_Put(work_queue, (uint8_t*)&bacnet) == false)
            {
                syslog_warning("%s", "push work queue fail");
                return -EPERM;
            }

            /* send the sem to read queue */
            error = sem_post(queue_sem);
            if(error)
            {
                syslog_error("sem post error:%d", error);
            }

            syslog_debug("[adapter]:mac:0x%02x, command:%d, value:%d",
                    bacnet.mac[0], bacnet.method, bacnet.value);

            break;

        case RS485_PROTOCOL_TYPE_MODBUS:

            /* TODO: add the message */
            memset(&modbus, 0x00, sizeof(modbus));
            modbus.device_id = adapter->message_content.write.device_id;
            modbus.method = adapter->message_content.write.device_method;
            modbus.value = adapter->message_content.write.method_value;
            modbus.broadcast = adapter->message_content.write.broadcast;

            modbus.retransmission = device->retransmission;
            modbus.device_addr = device->device_addr[0];
            modbus.send_handle = get_device_send_package_function(device->device_private, device->device_private_numbers, modbus.method);
            modbus.recv_handle = get_device_recv_package_function(device->device_private, device->device_private_numbers, modbus.method);


            /* check the queue status */
            if(Ringbuf_Full(work_queue) == true)
            {
                syslog_warning("%s", "The work queue have full");
                return -EPERM;
            }

            /* push to work queue */
            if(Ringbuf_Put(work_queue, (uint8_t*)&modbus) == false)
            {
                syslog_warning("%s", "push work queue fail");
                return -EPERM;
            }

            error = sem_post(queue_sem);
            if(error)
            {
                syslog_error("sem post error:%d", error);
            }
            break;

        default:
            syslog_warning("%s", "unknown protocol");
            return -EPERM;
    }

    return 0;
}


/* -----------------------------------------*/
/**
 * @brief process_read_value_service
 *  process the client read value from device service
 *
 * @ingroup adapter
 *
 * @param adapter :     The adapter struct information
 *
 * @return    0 is success, and others is fail.
 */
/* -----------------------------------------*/
static int process_read_value_service(adapter_t* adapter)
{
    int                         device_id;

    device_id = adapter->message_content.read.device_id;
    if(check_device_id(device_id) == false)
    {
        syslog_warning("device id:[%d]inval", device_id);
        return -EPERM;
    }

    return get_read_device_information(&(adapter->message_content.read_return), device_id);
}


void* adapter_thread_function(void* arg)
{
    struct message_queue    *adapter_message_queue = (struct message_queue*)arg;
    int                     error = 0;
    adapter_t               *adapter = NULL;
    int                     id = 0, status = 0;


    adapter_thread_status = ADAPTER_THREAD_STATUS_INIT;
    if(adapter_message_queue == NULL)
    {
        syslog_warning("%s", "thread 'arg' error");
        return NULL;
    }

    /* adapter initialize */
    error = adapter_thread_init();
    if(error)
    {
        syslog_error("%s", "adapter thread initialize fail");
        return NULL;
    }

    /* process the message */
    adapter_thread_status = ADAPTER_THREAD_STATUS_RUNNING;
    syslog_info("[adapter]:%s", "adapter thread initial success...");
    while(1)
    {
        /* read message from queue, and no message have block */
        adapter = message_queue_read(adapter_message_queue);

        /* process the message */
        memset(&reply_client, 0x00, sizeof(adapter_t));
        reply_client.socket_fd = adapter->socket_fd;
        switch(adapter->message_type)
        {
            case SERVICE_CREATE_RS485_OBJECT:
                id = create_object(adapter);

                /* full the create object return information*/
                reply_client.message_type = adapter->message_type;
                reply_client.message_retvl = 0;
                reply_client.message_length = sizeof(adapter_t);
                reply_client.message_content.new_object_return.object_id = id;

                break;

            case SERVICE_DELETE_RS485_OBJECT:
                status = delete_object(adapter->message_content.delete_object.object_id);

                /* full the delete status information */
                reply_client.message_type = adapter->message_type;
                reply_client.message_retvl = 0;
                reply_client.message_length = sizeof(adapter_t);
                reply_client.message_content.delete_object_return.delete_status = status;

                break;

            case SERVICE_MOUNT_DEVICE_TO_OBJECT:
                id = create_device(adapter);

                /* full the mount device to object */
                reply_client.message_type = adapter->message_type;
                reply_client.message_retvl = 0;
                reply_client.message_length = sizeof(adapter_t);
                reply_client.message_content.mount_device_return.device_id = id;

                break;

            case SERVICE_UNMOUNT_DEVICE_FROM_OBJECT:
                status = delete_device( adapter->message_content.unmount_device.object_id,
                        adapter->message_content.unmount_device.device_id);

                /* full the mount device to object */
                reply_client.message_type = adapter->message_type;
                reply_client.message_retvl = 0;
                reply_client.message_length = sizeof(adapter_t);
                reply_client.message_content.unmount_device_return.unmount_status = status;

                break;

            case SERVICE_WRITE_VALUE_TO_DEVICE:
                status = process_write_value_service(adapter);
                /* full the write value to device */
                reply_client.message_type = adapter->message_type;
                reply_client.message_retvl = 0;
                reply_client.message_length = sizeof(adapter_t);
                reply_client.message_content.write_return.write_status = status;

                break;

            case SERVICE_READ_VALUE_FROM_DEVICE:
                status = process_read_value_service(adapter);

                /* full the read value from device */
                reply_client.message_type = adapter->message_type;
                reply_client.message_retvl = 0;
                reply_client.message_length = sizeof(adapter_t);
                memcpy(&(reply_client.message_content.read_return), &(adapter->message_content.read_return), sizeof(read_device_return_t));
                if(status < 0)
                    reply_client.message_content.read_return.read_status = false;
                else
                    reply_client.message_content.read_return.read_status = true;
                break;

            case SERVICE_SYSTEM_UPDATE_START:
                syslog_info("[adapter]:I have recev exit message, exit....");
                /* close the work thread wait system update */
                exit(-1);
                break;

            default:
                syslog_warning("%s", "The message have unknown");
                reply_client.message_retvl = -EPERM;

        }

        /* send the message to client */
        if(reply_client.socket_fd > 0)
        	error = rs485_send_msg_to_client(reply_client.socket_fd, &reply_client, sizeof(adapter_t));

        /* free the message */
        message_queue_message_free(adapter_message_queue, adapter);
    } /* end the while */

    /** add the thread detach attr */
    pthread_detach(pthread_self());
    return NULL;
}

