
/*
 * =====================================================================================
 *
 *       Filename:  custom_stack.c
 *
 *    Description:  customize rs485 protocol
 *
 *        Version:  1.0
 *        Created:  Oct 13, 2016 11:51:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include <wlog.h>
#include <ringbuf.h>
#include <wbitmap.h>
#include "core.h"
#include "interface.h"
#include "adapter.h"


#define CUSTOM_STACK_ELEMENT_SIZE           (sizeof(write_device_t))


#ifndef CUSTOM_STACK_ELEMENT_SIZE
#define CUSTOM_STACK_ELEMENT_SIZE           (32)
#endif

#ifndef CUSTOM_STACK_DEEP
#define CUSTOM_STACK_DEEP                   (256)
#endif

#ifndef CUSTOM_STACK_BUFFER_LEN
#define CUSTOM_STACK_BUFFER_LEN             (128)
#endif


#define custom_error(...)        wlog_error("[custom]: ", ##__VA_ARGS__)
#define custom_debug(...)        wlog_debug("[custom]: ", ##__VA_ARGS__)
#define custom_info(...)         wlog_info("[custom]: ", ##__VA_ARGS__)
#define custom_warning(...)      wlog_warning("[custom]: ", ##__VA_ARGS__)
#define custom_printf(...)       wlog_printf("[custom]: ", ##__VA_ARGS__)



struct custom_stack
{
    struct ring_buffer_t        queue;
    uint8_t*                    queue_buffer;
    unsigned int                element_size;
    unsigned int                deep;

    sem_t                       sem;
    int                         fd;

    uint8_t*                    stack;
    unsigned int                stack_len;
};



static int custom_bus_data_init(void ** bus_data, struct interface_profile const * interface)
{
    struct custom_stack *stack = NULL;
    int     retval = 0;

    if(bus_data)
    {
        retval = -EINVAL;
        goto done;
    }

    /* step 1. allocate a stack */
    stack = calloc(1, sizeof(struct custom_stack));
    if(!stack)
    {
        retval = -ENOMEM;
        goto done;
    }

    /* step2. allocate a stack buffer */
    stack->stack_len = CUSTOM_STACK_BUFFER_LEN;
    stack->stack = malloc(stack->stack_len);
    if(!stack->stack)
    {
        retval = -ENOMEM;
        goto alloc_stack_buffer_fail;
    }

    /* step2. initialize stack buffer */
    stack->element_size = CUSTOM_STACK_ELEMENT_SIZE;
    stack->deep = CUSTOM_STACK_DEEP;
    stack->queue_buffer = calloc(stack->deep, stack->element_size);
    if(!stack->queue_buffer)
    {
        retval = -ENOMEM;
        goto alloc_queue_buffer_fail;
    }
    ringbuf_init(&stack->queue, stack->queue_buffer, stack->element_size, stack->deep);

    /* step3. initialize the semaphore */
    retval = sem_init(&stack->sem, 0, 0);
    if(retval)
        goto sem_init_fail;

    /* step4. initialize the interface */
    retval = interface_initialize(&stack->fd, interface);
    if(retval)
        goto interface_init_fail;

    /* step4. set bus data */
    *bus_data = stack;

    return 0;

interface_init_fail:
    sem_destroy(&stack->sem);
sem_init_fail:
    free(stack->queue_buffer);
alloc_queue_buffer_fail:
    free(stack->stack);
alloc_stack_buffer_fail:
    free(stack);
done:
    *bus_data = NULL;

    return retval;
}

static void custom_bus_data_clean(void* bus_data)
{
    struct custom_stack *stack = bus_data;

    if(!stack)
        return;
    interface_cleanup(stack->fd);
    sem_destroy(&stack->sem);
    free(stack->queue_buffer);
    free(stack->stack);
    free(stack);
}


static int custom_write(void* bus_data, void const* buffer, int len)
{
    struct custom_stack *stack = bus_data;

    if(!(stack && buffer))
        return -EINVAL;

    if(ringbuf_full(&stack->queue))
        return -EPERM;

    if((int)stack->element_size < len)
        return -EPERM;

    ringbuf_put(&stack->queue, (uint8_t*)buffer);

    return sem_post(&stack->sem);
}


static int custom_read(void* message, void* buffer, int len)
{
    read_device_t * msg = message;

    if(!(msg && buffer))
    {
        return -EINVAL;
    }

    if(len != sizeof(read_device_t))
        return -EINVAL;

    /* get device message */


    return 0;
}


static void do_bus_message(void* bus_data, void* message)
{
    write_device_t      *msg = (write_device_t*)message;
    struct custom_stack *stack = bus_data;
    struct rs485_device *device = NULL;
    int                 retval = 0;
    unsigned int        retransmission = 0;

    if(!msg)
    {
        errno = EINVAL;
        return;
    }

    device = get_device_by_id(msg->device_id);
    if(!device)
    {
        errno = ENODEV;
        return;
    }
    custom_debug("do message: id: %d, command: %d, value: %d", msg->device_id,
            msg->device_method,
            msg->value);

    if(!device->driver)
    {
        errno = EPERM;
        return;
    }


    /* set device retransmission */
    if(retransmission > device->info.retransmission)
        retransmission = device->info.retransmission;

    memset(stack->stack, 0, stack->stack_len);
    do
    {
        if(device->driver->get_buffer)
        {
            retval = device->driver->get_buffer(stack->stack, stack->stack_len, &device->info, msg);
            if(retval)
                return;

            interface_send_a_frame(stack->fd, stack->stack, retval);
        }

        if(device->info.broadcast)
        {
            return;
        }

        if(msg->broadcast)
        {
            return;
        }

        /* receive device reply data */
        retval = interface_recv_a_frame(stack->fd,
                stack->stack,
                stack->stack_len,
                device->info.receive_timeout);
        if(retval > 0)
        {
            /* handle data */
            if(device->driver->put_buffer)
            {
                retval = device->driver->put_buffer(stack->stack, retval, &device->info, msg);
            }
        }
        else
        {
            /* set bus have error, sleep 2000 */
            usleep(20000);
        }

    }while(--retransmission && retval);
}


static void* custom_stack_handle(void* bus_data)
{
    struct custom_stack     *stack = bus_data;
    void                    *message;

    if(!stack)
    {
        errno = EINVAL;
        goto done;
    }

    while(1)
    {
        /* step1. wait sem */
        sem_wait(&stack->sem);

        /* step2. get data from queue */
        message = (void*)ringbuf_peek(&stack->queue);
        if(!message)
            continue;

        /* step3. handle message */
        do_bus_message(stack, message);

        /* step4. free message */
        ringbuf_pop(&stack->queue, message);
    }

done:
    custom_bus_data_clean(stack);
    return stack;
}





struct rs485_bus_type custom =
{
    .name 			= "custom",
    .stack_handle 	= custom_stack_handle,
    .init 			= custom_bus_data_init,
    .clean 			= custom_bus_data_clean,
    .write 			= custom_write,
    .read			= custom_read,
    .stack 			= NULL,
};
