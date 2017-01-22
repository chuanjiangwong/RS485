
/*
 * =====================================================================================
 *
 *       Filename:  custom.c
 *
 *    Description:  RS485 custom bus protocol
 *
 *        Version:  1.0
 *        Created:  Jan 17, 2017 10:06:23 AM
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
#include "register.h"


#define wlog_e(...)         wlog_error("cus", ##__VA_ARGS__)
#define wlog_d(...)         wlog_debug("cus", ##__VA_ARGS__)
#define wlog_i(...)         wlog_info("cus", ##__VA_ARGS__)
#define wlog_w(...)         wlog_warning("cus", ##__VA_ARGS__)


#define CUSTOM_STACK_BUFFER_SIZE   (256)
struct rs485_custom_stack
{
    /* bus handle list */
    struct list_head            list;
    sem_t                       sem;

    /* tty fd */
    int                         fd;

    /* stack buffer */
    unsigned char               buffer[CUSTOM_STACK_BUFFER_SIZE];
    unsigned int                data_len;
};


struct custom_stack_data
{
    write_device_t              data;
    struct list_head            entry;
};


static int custom_bus_probe(struct rs485_bus_type * bus)
{
    int     retval = 0;
    struct rs485_custom_stack * stack = NULL;

    if(!bus)
        return -EINVAL;

    /* step 1. allocate a stack */
    stack = calloc(1, sizeof(struct rs485_custom_stack));
    if(!stack)
        return -ENOMEM;

    /* step2. list head init */
    INIT_LIST_HEAD(&stack->list);
    stack->fd = -1;

    /* step3. initialize the semaphore */
    retval = sem_init(&stack->sem, 0, 0);
    if(retval)
        goto sem_init_fail;

    /* step4. initialize the interface */
    retval = interface_initialize(&stack->fd, &bus->interface);
    if(retval)
        goto interface_init_fail;

    /* step4. set bus data */
    bus->priv_data = stack;

    return 0;

interface_init_fail:
    sem_destroy(&stack->sem);
sem_init_fail:
    free(stack);
    return retval;
}

static void custom_bus_remove(struct rs485_bus_type * bus)
{
    struct rs485_custom_stack *stack = bus->priv_data;

    if(!stack)
        return;

    interface_cleanup(stack->fd);
    sem_destroy(&stack->sem);
    free(stack);
}


static int custom_bus_write(void* bus_data, void * buffer, int len)
{
    int retval = 0;
    struct rs485_custom_stack *stack = bus_data;
    struct custom_stack_data *node = NULL;

    if(!(stack && buffer))
        return -EINVAL;

    if(len != sizeof(write_device_t))
        return -EPERM;

    node = calloc(1, sizeof(struct custom_stack_data));
    if(!node)
        return -ENOMEM;

    memcpy(&node->data, buffer, len);

    /* add the data to list */
    list_add(&node->entry, &stack->list);

    retval = sem_post(&stack->sem);
    if(retval)
        goto sem_post_fail;

    return 0;

sem_post_fail:
    list_del(&node->entry);
    free(node);
    return retval;
}


static int custom_bus_read(void* message, void* buffer, int len)
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


#define CUSTOM_DATA_RETRANSMISSION_MAX  (3)
#define CUSTOM_DATA_TIMEOUT_MAX         (1000) // 1s
static void do_bus_message(struct rs485_custom_stack* stack,
        const struct custom_stack_data* node)
{
    int                         retval = 0;
    unsigned int                retransmission = 0;
    unsigned int                timeout = 0;
    struct rs485_device         *device = NULL;
    bool                        broadcast = false;

    if(!(stack && node))
        return;

    /* get device ptr */
    device = get_device_by_id(node->data.device_id);
    if(!device)
        return;

    /* get device driver */
    if(!device->driver)
        return;

    /* set device retransmission */
    retransmission = device->driver->data->retransmission;
    if(retransmission > CUSTOM_DATA_RETRANSMISSION_MAX)
        retransmission = CUSTOM_DATA_RETRANSMISSION_MAX;

    /* clean the buffer */
    memset(stack->buffer, 0, sizeof(stack->buffer));
    do
    {
        if(device->driver->get)
        {
            retval = device->driver->get(stack->buffer,
                    sizeof(stack->buffer), &node->data, &broadcast);
            if(retval < 0 || retval > (int)sizeof(stack->buffer))
            {
                wlog_w("get device:%s, command:%d, fail:%d",
                        device->dev.bus_id, node->data.method_value, retval);
                return;
            }

            /* send data to bus */
            retval = interface_send_a_frame(stack->fd,
                    stack->buffer, retval);
            if(retval)
                return;
        }

        if(device->driver->data->broadcast)
            return;

        if(broadcast)
            return;

        timeout = device->driver->data->receive_timeout_ms;
        if(timeout > CUSTOM_DATA_TIMEOUT_MAX)
            timeout = CUSTOM_DATA_TIMEOUT_MAX;

        /* receive device reply data */
        retval = interface_recv_a_frame(stack->fd,
                stack->buffer,
                sizeof(stack->buffer),
                timeout);
        if(retval > 0)
        {
            /* handle data */
            if(device->driver->put)
                retval = device->driver->put(stack->buffer,
                        retval, &node->data, device->device_data);
            else
                return;
        }
        else
        {
            /* set bus have error, sleep 2000 */
            usleep(5000);
        }

    }while(--retransmission && retval);
}


static void* custom_stack_handle(void* bus_data)
{
    struct rs485_custom_stack   *stack = bus_data;
    struct custom_stack_data    *node = NULL;
    struct list_head            *pos = NULL, *target = NULL;

    if(!stack)
        return stack;

    /* pthread handle */
    while(1)
    {
        /* step1. wait sem */
        sem_wait(&stack->sem);

        /* step2. get data from queue */
        list_for_each(pos, &stack->list)
        {
            target = pos;
        }
        node = list_entry(target, struct custom_stack_data, entry);
        if(node == NULL)
            continue;

        /* step3. handle message */
        do_bus_message(stack, node);

        /* step4. free message */
        list_del(target);
        free(node);
        node = NULL;
        target = NULL;
    }

    return stack;
}





static const struct rs485_bus_type custom =
{
    .name 			= "custom",
    .pthread_func 	= custom_stack_handle,
    .probe 			= custom_bus_probe,
    .remove 		= custom_bus_remove,
    .write 			= custom_bus_write,
    .read			= custom_bus_read,
};

static int custom_init(const struct interface_profile * interface,
        const char* match_bus)
{
    struct rs485_bus_type *new_bus = NULL;
    int retval = 0;

    if(!(interface && match_bus))
        return -EINVAL;

    /* match bus name */
    if(strcmp(match_bus, custom.name))
        return 1;

    /* match custom bus */
    new_bus = calloc(1, sizeof(struct rs485_bus_type));
    if(!new_bus)
        return -ENOMEM;

    memcpy(new_bus, &custom, sizeof(struct rs485_bus_type));
    memcpy(&new_bus->interface, interface, sizeof(new_bus->interface));

    retval = rs485_bus_register(new_bus);
    if(retval)
        goto bus_register_fail;

    return 0;

bus_register_fail:
    free(new_bus);
    new_bus = NULL;
    return retval;
}


static int custom_exit(struct rs485_bus_type *bus)
{
    if(!bus)
        return -EINVAL;

    if(strcmp(custom.name, bus->name))
        return 1;

    rs485_bus_unregister(bus);

    free(bus);
    bus = NULL;

    return 0;
}



RS485_BUS_INIT(custom_init);
RS485_BUS_EXIT(custom_exit);

