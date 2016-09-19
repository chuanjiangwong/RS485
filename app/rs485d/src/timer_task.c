/**
 * www.enno.com
 *
 * @file :	timer_task.c
 *
 * @date : 	Mar 14, 2016
 *
 * @author: chuanjiang.wong
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

#include "timer_task.h"
#include "syslog/log.h"
#include "enum.h"
#include "device.h"
#include "adapter.h"
#include "service.h"

#include "list.h"

#ifndef SYSTEM_TIMER_TICK_SECOND
#define SYSTEM_TIMER_TICK_SECOND            (10)
#endif


/** define the timer managemnt list */
static list_t                               *timer_task_list = NULL;
/** define the list iterate */
static list_iterator_t                      *timer_task_list_iterator = NULL;
/** define the timer thread status */
static int                                  timer_task_thread_status = TIMER_TASK_THREAD_STATUS_START;
/** define a mutex to used to add and delete list data */
static pthread_mutex_t                      timer_task_lock;


/* -----------------------------------------*/
/**
 * @brief timer_task_init
 *  timer task initial
 *
 * @ingroup timer_task
 *
 * @return , 0 is success, others is fail.
 */
/* -----------------------------------------*/
static int timer_task_init(void)
{
    int error = 0;

    timer_task_thread_status = TIMER_TASK_THREAD_STATUS_INIT;
    /* create a list */
    timer_task_list = list_new();
    if(timer_task_list == NULL)
    {
        syslog_error("%s", "create timer task list fail");
        goto error;
    }

    /* initialization a thread mutex */
    error = pthread_mutex_init(&timer_task_lock, NULL);
    if(error)
    {
        syslog_error("%s", "timer task mutex initialization fail");
        goto lock_fail;
    }

    return 0;

lock_fail:
    list_destroy(timer_task_list);

error:
    return -EPERM;
}



void* timer_task_thread_function(void* UNUSED(arg))
{
    list_node_t     *timer = NULL;
    timer_task_t    *task = NULL;

    /* set the timer task status */
    timer_task_thread_status = TIMER_TASK_THREAD_STATUS_INIT;

    if(timer_task_init())
    {
        syslog_error("%s", "timer task initial fail.");
        return NULL;
    }

    /* initail finish */
    timer_task_thread_status = TIMER_TASK_THREAD_STATUS_RUNNING;

    syslog_info("[timer]:%s", "timer thread initial success...");
    while(1)
    {
        /* sleep the tick */
        sleep(SYSTEM_TIMER_TICK_SECOND);

        /* create a list iterator*/
        timer_task_list_iterator = list_iterator_new(timer_task_list, LIST_HEAD);
        if(timer_task_list_iterator == NULL)
        	continue;

        /* for each list */
        while(1)
        {
            /* for each list node */
            timer = list_iterator_next(timer_task_list_iterator);
            if(timer == NULL)
                break;

            /* get the node value */
            task = timer->val;
            if(task == NULL)
                continue;

            /* process the timer task */
            task->tick += SYSTEM_TIMER_TICK_SECOND;
//            syslog_debug("[timer]:tick:%u, timeout:%u", task->tick, task->timeout);
            if(task->tick >= task->timeout)
            {
                task->tick = 0;

                if(task->function)
                	task->function(task->device_id, task->command);
            }
        } //end the for each list

        /* delete the iterator */
        list_iterator_destroy(timer_task_list_iterator);
    }

    timer_task_thread_status = TIMER_TASK_THREAD_STATUS_STOP;

    pthread_detach(pthread_self());
    return NULL;
}


int create_device_timer_task(timer_task_t* task)
{
    list_node_t *node = NULL;
    list_node_t *value = NULL;

    if(task == NULL)
        return -EINVAL;

    /* The thread synchronous */
    if(timer_task_thread_status == TIMER_TASK_THREAD_STATUS_INIT)
    {
        syslog_warning("%s", "timer task having initial");
        return -EPERM;
    }

    /* new a list node */
    node = list_node_new(task);
    if(node == NULL)
    {
        syslog_warning("%s", "crate list node fail");
        return -ENOMEM;
    }

    /* push The node to list */
    pthread_mutex_lock(&timer_task_lock);
    value = list_rpush(timer_task_list, node);
    pthread_mutex_unlock(&timer_task_lock);
    if(value == NULL)
    {
        syslog_warning("%s", "push list fail");
        goto error;
    }

    node = NULL;

    return 0;

error:
    free(node);
    return -EPERM;
}


int delete_device_timer_task(timer_task_t* task)
{
    list_node_t *node = NULL;
    if(task == NULL)
        return -EINVAL;

    if(timer_task_thread_status == TIMER_TASK_THREAD_STATUS_INIT)
    {
        syslog_warning("%s", "timer task having initial");
        return -EPERM;
    }

    node = list_find(timer_task_list, task);
    if(node == NULL)
    {
        syslog_warning("%s", "timer task inval");
        return -EPERM;
    }

    pthread_mutex_lock(&timer_task_lock);
    list_remove(timer_task_list, node);
    pthread_mutex_unlock(&timer_task_lock);

    return 0;
}

int device_timer_task_handle_demo(int device_id, int command)
{
    adapter_t adapter;

    if(check_device_id(device_id) == false)
    {
        return -EPERM;
    }

    memset(&adapter, 0x00, sizeof(adapter));

    adapter.message_type = SERVICE_WRITE_VALUE_TO_DEVICE;
    adapter.message_length = sizeof(adapter_t);
    adapter.socket_fd = -1;
    adapter.message_content.write.device_id = device_id;
    adapter.message_content.write.device_method = command;

    return send_msg_to_adapter(&adapter);
}


int device_timer_task_handle_curtain_init(int device_id, int UNUSED(command))
{
    adapter_t adapter;
    static int count = 0;

    /* have finish */
    if(count >= 2)
        return 0;

    if(check_device_id(device_id) == false)
        return -EPERM;

    memset(&adapter, 0x00, sizeof(adapter));

    adapter.message_type = SERVICE_WRITE_VALUE_TO_DEVICE;
    adapter.message_length = sizeof(adapter_t);
    adapter.socket_fd = -1;
    adapter.message_content.write.device_id = device_id;
    adapter.message_content.write.broadcast = true;
    if(count == 0)
        adapter.message_content.write.device_method = RS485_CURTAIN_OPEN;
    else if(count == 1)
        adapter.message_content.write.device_method = RS485_CURTAIN_CLOSE;

    count ++;

    return send_msg_to_adapter(&adapter);
}


int device_timer_task_handle_curtain_aoke_init(int device_id, int UNUSED(command))
{
    adapter_t adapter;
    static int count = 0;

    /* have finish */
    if(count >= 2)
        return 0;

    if(check_device_id(device_id) == false)
        return -EPERM;

    memset(&adapter, 0x00, sizeof(adapter));

    adapter.message_type = SERVICE_WRITE_VALUE_TO_DEVICE;
    adapter.message_length = sizeof(adapter_t);
    adapter.socket_fd = -1;
    adapter.message_content.write.device_id = device_id;
    adapter.message_content.write.broadcast = true;
    if(count == 0)
        adapter.message_content.write.device_method = RS485_CURTAIN_OPEN;
    else if(count == 1)
        adapter.message_content.write.device_method = RS485_CURTAIN_CLOSE;

    count ++;

    return send_msg_to_adapter(&adapter);
}


int device_timer_task_handle_curtain_doya_init(int device_id, int UNUSED(command))
{
    adapter_t adapter;
    static int count = 0;

    /* have finish */
    if(count >= 2)
        return 0;

    if(check_device_id(device_id) == false)
        return -EPERM;

    memset(&adapter, 0x00, sizeof(adapter));

    adapter.message_type = SERVICE_WRITE_VALUE_TO_DEVICE;
    adapter.message_length = sizeof(adapter_t);
    adapter.socket_fd = -1;
    adapter.message_content.write.device_id = device_id;
    adapter.message_content.write.broadcast = true;
    if(count == 0)
        adapter.message_content.write.device_method = RS485_CURTAIN_OPEN;
    else if(count == 1)
        adapter.message_content.write.device_method = RS485_CURTAIN_CLOSE;

    count ++;

    return send_msg_to_adapter(&adapter);
}





