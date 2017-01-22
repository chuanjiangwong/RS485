/*
 * =====================================================================================
 *
 *          @file:  bacnet.c
 *
 *    Description:  offer the BACnet interface
 *
 *        Version:  1.0
 *        Created:  03/28/2016 08:54:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
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
#include "protocol/bacnet/bacnet.h"
#include "protocol/bacnet/handle_property.h"
#include "protocol/bacnet/read_property.h"
#include "protocol/bacnet/write_property.h"
#include "syslog/log.h"
#include "ringbuf.h"
/* The device include */

#if 0

void* bacnet_work_thread_function(void* arg)
{
    object_management_t     *object = (object_management_t*)arg;
    RING_BUFFER             *work_queue = object->work_queue;
    struct bacnet           *handle = NULL;
    int                     error = 0;

    if(object == NULL)
        return NULL;

    if(work_queue == NULL)
        return NULL;

    syslog_info("[bacnet]:bacnet stack initial success, object:0x%08x", (unsigned int)object);

    while(1)
    {
        /* after have change it use sem */
        error = sem_wait(&(object->queue_sem));
        if(error)
        {
            syslog_warning("wait work queue sem :%d", error);
        }

        if(Ringbuf_Empty(work_queue) == true)
        {
            continue;
        }

        handle = (struct bacnet*)Ringbuf_Peek(work_queue);
        if(handle == NULL)
        {
            continue;
        }

        /* TODO: process the message */
        switch(handle->device_type)
        {
            /* process the air condition */
            case RS485_DEVICE_TYPE_AIR_CONDITION:
                switch(handle->factory_name)
                {
                    /* york air condition */
                    case RS485_FACTORY_YORK:
                        error = york_air_condition_handle(handle);
                        if(error)
                        {
                            goto free_message;
                        }

                        syslog_info("[bacnet]:process york air condition:%d", handle->command);
                        break;


                    default:
                        syslog_warning("unknown factory:%d", handle->factory_name);
                        goto free_message;
                }
                break;

            case RS485_DEVICE_TYPE_CURTAIN:
                break;

            case RS485_DEVICE_TYPE_FRESH_AIR:
                break;

            default:
                syslog_warning("unknown device type:%d", handle->device_type);
                goto free_message;
        }

free_message:
        /* free the message */
        Ringbuf_Pop(work_queue, (uint8_t*)handle);
    }

    return NULL;
}

#endif


void* bacnet_work_thread_function(void* arg)
{
    object_management_t     *object = (object_management_t*)arg;
    RING_BUFFER             *work_queue = object->work_queue;
    struct bacnet           *handle = NULL;
    int                     error = 0;
    int                     nbyte = 0;

    if(object == NULL)
        return NULL;

    if(work_queue == NULL)
        return NULL;

    syslog_info("[bacnet]:bacnet stack initial success, object:0x%08x", (unsigned int)object);

    while(1)
    {
        /* after have change it use sem */
        error = sem_wait(&(object->queue_sem));
        if(error)
        {
            syslog_warning("wait work queue sem :%d", error);
        }

        if(Ringbuf_Empty(work_queue) == true)
        {
            continue;
        }

        handle = (struct bacnet*)Ringbuf_Peek(work_queue);
        if(handle == NULL)
        {
            continue;
        }


        /* make the bacnet backage*/
        if(handle->send_handle == NULL)
        {
            syslog_warning("[BACnet]: The send method is NULL");
            goto free_message;
        }
        nbyte = handle->send_handle(handle);
        if(nbyte  < 0)
        {
            syslog_warning("[BACnet]: make a send arguments fail.");
            goto free_message;
        }

        /* TODO: process the message */
        switch(handle->code)
        {
            /* process the air condition */
            case BACNET_FUNCTION_CODE_WRITE_PROPERTY:
                bacnet_write_property(&(handle->write_arg));
                break;

            case BACNET_FUNCTION_CODE_READ_PROPERTY:
                bacnet_read_property(&(handle->read_arg));
                break;

            case BACNET_FUNCTION_CODE_WRITE_MULTIPLE_PROPERTY:
                break;

            case BACNET_FUNCTION_CODE_READ_MULTIPLE_PROPERTY:
                bacnet_read_property(&(handle->read_arg));
                break;

            case BACNET_FUNCTION_CODE_DO_NOTHING:
                break;

            default:
                syslog_warning("unknown BACnet function:%d", handle->code);
                goto free_message;
        }

free_message:
        /* free the message */
        Ringbuf_Pop(work_queue, (uint8_t*)handle);
    }

    /** add the thread detach attr */
    pthread_detach(pthread_self());
    return NULL;
}


