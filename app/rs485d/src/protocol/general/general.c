/*
 * =====================================================================================
 *
 *          @file:  general.c
 *
 *    Description:  offer the rs485 general protocol interface
 *
 *        Version:  1.0
 *        Created:  03/28/2016 11:07:29 AM
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

#include "service.h"
#include "syslog/log.h"
#include "adapter.h"
#include "enum.h"
#include "ringbuf.h"
#include "protocol/general/general.h"
#include "protocol/general/rs485.h"
#include "object.h"
#include "read_config.h"

#ifndef BUS_MAX_RETRANSMISSION
#define BUS_MAX_RETRANSMISSION                      (3)
#endif


int general_service_init(object_management_t* object)
{
    if(object == NULL)
    {
        syslog_error("%s", "[general]:argument error");
        return -EINVAL;
    }

    if(object->port.interface_name)
    {
       rs485_set_interface(object->port.interface_name);
    }

    rs485_set_baud_rate(object->port.baud_rate);

    rs485_initialize();

    return 0;
}


void* general_work_thread_function(void* arg)
{
    int                     error = 0;
    object_management_t     *object = (object_management_t*)arg;
    RING_BUFFER             *work_queue = object->work_queue;
    struct mstp_port_handle *handle = NULL;
    unsigned char           *package_buffer = NULL;

    if(object == NULL)
    {
        syslog_error("%s", "[general]:thread argument error");
        return NULL;
    }

    if(work_queue == NULL)
    {
        syslog_error("%s", "[general]:work queue inval");
        return NULL;
    }

    package_buffer = malloc(glb_config_general_work_package_mtu);
    if(package_buffer == NULL)
    {
        syslog_error("%s", "[general]:package buffer malloc fail");
        return NULL;
    }

    while(1)
    {
        /* wait the message queue have a data*/
        sem_wait(&(object->queue_sem));

        /* check the work queue*/
        if(Ringbuf_Empty(work_queue) == true)
        {
            continue;
        }

        /* get work from work queue */
        handle = (struct mstp_port_handle*)Ringbuf_Peek(work_queue);
        if(handle == NULL)
        {
            continue;
        }

        /* TODO: process the handle */
        if(handle->retransmission > BUS_MAX_RETRANSMISSION)
            handle->retransmission = BUS_MAX_RETRANSMISSION;

        do
        {
            /* set the package buffer */
            handle->package_buffer = package_buffer;
            handle->package_buffer_len = glb_config_general_work_package_mtu;

            /* make a send package */
            error = rs485_send_handle_frame(handle);
            if(error)
            {
                syslog_warning("[general]:rs485 send error:%d", error);
                goto free_message;
            }

            /* have except reply */
            if(handle->except_reply)
            {
                handle->package_buffer = package_buffer;
                handle->package_buffer_len = glb_config_general_work_package_mtu;
                error = rs485_recv_handle_frame(handle);
                if(error)
                {
                    /* send a data, and wait 20ms, for trainsmit */
                    /* if have a error, since 20ms */
                    usleep(20000);

                    if(error != -EIO)
                        syslog_warning("[general]:rs485 receive error:%d", error);
                }
            }

        }while(-- handle->retransmission && error);

free_message:
        /* free the message */
        Ringbuf_Pop(work_queue, (uint8_t*)handle);
    }

    /** add the thread detach attr */
    pthread_detach(pthread_self());
    return NULL;
}
