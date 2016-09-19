/*
 * =====================================================================================
 *
 *          @file:  object.c
 *
 *    Description:  The RS485 object management
 *
 *        Version:  1.0
 *        Created:  03/25/2016 02:05:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <semaphore.h>

#include "read_config.h"
#include "service.h"
#include "syslog/log.h"
#include "adapter.h"
#include "enum.h"
#include "object.h"
#include "protocol/general/general.h"
#include "protocol/bacnet/bacnet.h"
#include "protocol/bacnet/handle_property.h"
#include "protocol/modbus/modbus.h"
#include "device.h"
#include "enumtxt.h"


#ifndef RS485_OBJECT_MAX_NUMBERS
#define RS485_OBJECT_MAX_NUMBERS                            (5)
#endif

#ifndef RS485_WORK_QUEUE_DEPTH
#define RS485_WORK_QUEUE_DEPTH                              (256)
#endif


/** define the RS485 object management table */
static object_management_t                                  *glb_object_manage[RS485_OBJECT_MAX_NUMBERS] = { 0 };

/** define the RS485 object ring buffer */
static struct ring_buffer_t                                 glb_work_queue[RS485_OBJECT_MAX_NUMBERS];


/* -----------------------------------------*/
/**
 * @brief check_object_id
 *  check the object is legal
 *
 * @ingroup object
 *
 * @param[in] object_id : The need to check object id.
 *
 * @return  if object id is legal return true, and return false.
 */
/* -----------------------------------------*/
inline bool check_object_id(int object_id)
{
    if(object_id < 0 || object_id >= RS485_OBJECT_MAX_NUMBERS)
        return false;

    if(glb_object_manage[object_id] == NULL)
        return false;
    else
        return true;
}


/* -----------------------------------------*/
/**
 * @brief find_available_object_id
 *  Find a available object id from object table
 *
 * @ingroup object
 *
 * @return  return the available object id, or return negative
 */
/* -----------------------------------------*/
static inline int find_available_object_id(void)
{
    int id = -1;
    int i;

    /* have no use the glb_object_manage[0] */
    for(i=1; i<RS485_OBJECT_MAX_NUMBERS; i++)
    {
        if(glb_object_manage[i] == NULL)
        {
            id = i;
            break;
        }
    }

    return id;
}


/* -----------------------------------------*/
/**
 * @brief object_is_used
 *  To determine whether the object id has been used
 *
 * @ingroup object
 *
 * @param adapter : The adapter struct.
 *
 * @return  if have used return false, and return ture;
 */
/* -----------------------------------------*/
static inline bool object_is_used(const adapter_t* adapter)
{
    char interface[16];
    int i;

    strncpy(interface, adapter->message_content.new_object.port.interface_name, sizeof(interface));
    for(i=1; i<RS485_OBJECT_MAX_NUMBERS; i++)
    {
        if(glb_object_manage[i])
        {
            if(strncmp(glb_object_manage[i]->port.interface_name, interface, sizeof(interface)) == 0)
                return false;
        }
    }

    return true;
}


/* -----------------------------------------*/
/**
 * @brief work_thread_create
 *  create work thread
 *
 * @ingroup object
 *
 * @param[in,out] object :  The object struct information
 *
 * @return  0 is create success, others is fail.
 */
/* -----------------------------------------*/
static int work_thread_create(object_management_t* object)
{
    int error = 0;

    object->work_queue = &glb_work_queue[object->object_id];

    switch(object->object_type)
    {
        case RS485_PROTOCOL_TYPE_BACNET:
            /* create bacnet work queue used to work */
            object->queue_depth = glb_config_bacnet_work_queue_depth;
            object->work_queue_buffer = (uint8_t*)malloc(object->queue_depth * sizeof(bacnet_port_handle_t));
            if(object->work_queue_buffer == NULL)
            {
                syslog_warning("%s", "malloc fail");
                return -ENOMEM;
            }

            Ringbuf_Init(object->work_queue, (uint8_t*)object->work_queue_buffer,
                    sizeof(bacnet_port_handle_t), object->queue_depth);

            /* create the work_queue sem */
            error = sem_init(&(object->queue_sem), 0, 0);
            if(error)
            {
                syslog_error("work queue sem initialize fail:%d", error);
                goto after_fail_work_queue_buffer;
            }

            /* initial the bacnet obect */
            error = bacnet_service_init(object);
            if(error)
            {
                syslog_error("%s", "bacnet mstp initialize fail");
                goto after_fail_sem_init;
            }

            /* create bacnet work thread */
            error = pthread_create(&(object->object_thread), NULL, bacnet_work_thread_function, object);
            if(error)
            {
                syslog_error("%s", "create bacnet work thread fail");
                goto after_fail_sem_init;
            }
            break;

        case RS485_PROTOCOL_TYPE_MODBUS:
            /* create modbus work queue used to work */
            object->queue_depth = glb_config_modbus_work_queue_depth;
            object->work_queue_buffer = malloc(object->queue_depth * sizeof(modbus_port_handle_t));
            if(object->work_queue_buffer == NULL)
            {
                syslog_warning("%s", "malloc fail");
                return -ENOMEM;
            }

            Ringbuf_Init(object->work_queue, (uint8_t*)object->work_queue_buffer,
                    sizeof(modbus_port_handle_t), object->queue_depth);

            /* create the work_queue sem */
            error = sem_init(&(object->queue_sem), 0, 0);
            if(error)
            {
                syslog_error("work queue sem initialize fail:%d", error);
                goto after_fail_work_queue_buffer;
            }

            /*initial the modbus service */
            error = modbus_service_init(object);
            if(error)
            {
                syslog_error("[object]:modbus service initial fail:%d", error);
                goto after_fail_work_queue_buffer;
            }

            /* create modbus work thread */
            error = pthread_create(&(object->object_thread), NULL, modbus_work_thread_function, object);
            if(error)
            {
                syslog_error("%s", "create modbus work thread fail");
                goto after_fail_sem_init;
            }
            break;
        case RS485_PROTOCOL_TYPE_GENERAL:
            /* create general work queue used to work */
            object->queue_depth = glb_config_general_work_queue_depth;
            object->work_queue_buffer = malloc(object->queue_depth * sizeof(mstp_port_handle_t));
            if(object->work_queue_buffer == NULL)
            {
                syslog_warning("%s", "malloc fail");
                return -ENOMEM;
            }

            Ringbuf_Init(object->work_queue, (uint8_t*)object->work_queue_buffer,
                    sizeof(mstp_port_handle_t), object->queue_depth);

            /* create the work_queue sem */
            error = sem_init(&(object->queue_sem), 0, 0);
            if(error)
            {
                syslog_error("work queue sem initialize fail:%d", error);
                goto after_fail_work_queue_buffer;
            }

            /* initialize the general object */
            error = general_service_init(object);
            if(error)
            {
                syslog_error("general service init fail:%d", error);
                goto after_fail_sem_init;
            }

            /* create general work thread */
            error = pthread_create(&(object->object_thread), NULL, general_work_thread_function, object);
            if(error)
            {
                syslog_error("%s", "create general work thread fail");
                goto after_fail_sem_init;
            }
            break;

        default:
            syslog_error("%s", "unknown protocol type");
            return -EPERM;
    }

    return 0;

after_fail_sem_init:
    sem_destroy(&object->queue_sem);

after_fail_work_queue_buffer:
    free(object->work_queue_buffer);
    return error;
}


/* -----------------------------------------*/
/**
 * @brief work_thread_clean
 *  clean the work thread
 *
 * @ingroup object
 *
 * @param[in] object :  The object device_management_t struct.
 */
/* -----------------------------------------*/
static void work_thread_clean(object_management_t* object)
{
    /* cancel the work thread */
    pthread_cancel(object->object_thread);
    /* free the work queue sem */
    sem_destroy(&object->queue_sem);
    /* free the work queue buffer */
    free(object->work_queue_buffer);
}

static bool check_object_is_support(rs485_protocol_type_enum object_type)
{
    switch(object_type)
    {
        case RS485_PROTOCOL_TYPE_BACNET:
        case RS485_PROTOCOL_TYPE_MODBUS:
        case RS485_PROTOCOL_TYPE_GENERAL:
            return true;

        default:
            return false;
    }
}


int create_object(const adapter_t* adapter)
{
    int                 error = 0, i = 0;
    int                 object_id = 0;
    object_management_t *new_object = NULL;
    int                 object_name_len = 0;

    /* find a available object id */
    object_id = find_available_object_id();
    if(object_id < 0)
        return object_id;

    /* find the object is have used? */
    if(object_is_used(adapter) == false)
        return -EPERM;

    /* read to create object */
    new_object = (object_management_t*)malloc(sizeof(object_management_t));
    if(new_object == NULL)
    {
        syslog_warning("%s", "malloc fail");
        return -ENOMEM;
    }

    /* check the object type is support */
    if(check_object_is_support(adapter->message_content.new_object.object_type) == false)
    {
        syslog_warning("%s", "hane no support object type");
        return -EPERM;
    }

    /* full the object type */
    new_object->object_type = adapter->message_content.new_object.object_type;

    /* full the objcet id */
    new_object->object_id = object_id;

    /* full the object name */
    object_name_len = strlen(adapter->message_content.new_object.object_name);
    new_object->object_name = malloc(object_name_len + 1 + 4 - (object_name_len + 1)%4);
    if(new_object->object_name == NULL)
    {
        syslog_warning("%s", "malloc fail");
        goto after_fail_object;
    }
    strcpy(new_object->object_name, adapter->message_content.new_object.object_name);

    /* full the object address */
    memcpy(new_object->address, adapter->message_content.new_object.address, sizeof(new_object->address));

    /* full the object mount max device numbers */
    new_object->mount_device_max = adapter->message_content.new_object.mount_device_max;

    /* malloc the mount device id */
    new_object->mount_device = malloc(sizeof(int) * new_object->mount_device_max);
    if(new_object->mount_device == NULL)
    {
        syslog_warning("%s", "malloc fial");
        goto after_fail_object_name;
    }
    /* initail the mount device id */
    for(i=0; i<new_object->mount_device_max; i++)
    {
        new_object->mount_device[i] = -1;
    }

    /* full the object prot information */
    memcpy(&(new_object->port), &(adapter->message_content.new_object.port), sizeof(rs485_port_t));

    /* create object thread */
    error = work_thread_create(new_object);
    if(error)
        goto after_fail_mount_device;

    /* full the object management table */
    glb_object_manage[object_id] = new_object;
    syslog_info("[object]:create a rs485 object success, object type:%s, object id:%d",
    	get_enum_txt_rs485_protocol_type(glb_object_manage[object_id]->object_type),
        glb_object_manage[object_id]->object_id);
    new_object = NULL;

    return object_id;

after_fail_mount_device:
    free(new_object->mount_device);
after_fail_object_name:
    free(new_object->object_name);
after_fail_object:
    free(new_object);

    return -EPERM;
}



int delete_object(int object_id)
{
    int buffer[32];
    int i, n;

    object_management_t* object = NULL;

    if(check_object_id(object_id) == false)
    {
        syslog_error("The object id :%d error or object is NULL", object_id);
        return -EPERM;
    }

    /* get the object */
    object = glb_object_manage[object_id];

    /* clear the work thread */
    work_thread_clean(object);

    /* delete the device on mount this object*/
    /* TODO: */
    n = get_devices_from_object(object_id, buffer, sizeof(buffer));
    for(i=0; i<n; i++)
        delete_device(object_id, buffer[i]);

    /* free the mount device */
    free(object->mount_device);

    /* free the device name */
    free(object->object_name);

    /* free the all object */
    free(glb_object_manage[object_id]);

    glb_object_manage[object_id] = NULL;

    return 0;
}


int get_object_type(int object_id)
{
    object_management_t* object = NULL;

    if(check_object_id(object_id) == false)
        return -EPERM;

    object = glb_object_manage[object_id];

    return object->object_type;
}

int get_devices_from_object(int object_id, int* out_id, int out_id_len)
{
    object_management_t* object = NULL;
    int i = 0, j = 0;

    if(check_object_id(object_id) == false)
        return -EPERM;

    /* get the object */
    object = glb_object_manage[object_id];

    /* check the buffer is length */
    if(out_id_len < object->mount_device_max)
        return -EPERM;

    for(i=0; i<object->mount_device_max; i++)
    {
        if(object->mount_device[i] > 0)
        {
            out_id[j++] = object->mount_device[i];
        }
    }

    return j;
}

int mount_a_device_to_object(int object_id, int device_id)
{
    object_management_t* object = NULL;
    int i = 0;

    if(check_object_id(object_id) == false)
    {
        syslog_error("The object id :%d is inval", object_id);
        return -EPERM;
    }

    /* get the object */
    object = glb_object_manage[object_id];
    if(object == NULL)
    {
        syslog_error("The object id :%d is NULL", object_id);
        return -EPERM;
    }

    for(i=0; i<object->mount_device_max; i++)
    {
        if(object->mount_device[i] < 0)
        {
            object->mount_device[i] = device_id;
            return 0;
        }
    }

    return -EPERM;
}

int unmount_a_device_from_object(int object_id, int device_id)
{
    object_management_t* object = NULL;
    int i = 0;

    if(check_object_id(object_id) == false)
        return -EPERM;

    /* get the object */
    object = glb_object_manage[object_id];

    for(i=0; i<object->mount_device_max; i++)
    {
        if(object->mount_device[i] == device_id)
        {
            object->mount_device[i] = -1;
            break;
        }
    }

    return 0;
}

bool check_object_numbers_have_idle(int object_id)
{
    object_management_t* object = NULL;
    int i = 0;

    if(check_object_id(object_id) == false)
        return false;

    /* get the object */
    object = glb_object_manage[object_id];

    for(i=0; i<object->mount_device_max; i++)
        if(object->mount_device[i] < 0)
            return true;

    return false;
}


void* get_object_work_queue(int object_id)
{
    if(check_object_id(object_id) == false)
        return NULL;

    return &glb_work_queue[object_id];
}



void* get_object_queue_sem(int object_id)
{
    object_management_t     *object = NULL;

    if(check_object_id(object_id) == false)
        return NULL;

    object = glb_object_manage[object_id];

    return &(object->queue_sem);
}
