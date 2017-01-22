/*
 * =====================================================================================
 *
 *          @file:  modbus.c
 *
 *    Description:  offer the modbus interface
 *
 *        Version:  1.0
 *        Created:  03/29/2016 09:29:13 AM
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
#include <stdint.h>

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
#include "protocol/modbus/modbus.h"  //define by self
#include "modbus.h" //define by libmodbus
#include "object.h"


/** define the modbus MTU */
#ifndef RS485_MODBUS_MTU
#define RS485_MODBUS_MTU                                (512)
#endif

/** define the a modbus do things max times */
#ifndef DO_SOMETHINGS_MAX_COUNTS
#define DO_SOMETHINGS_MAX_COUNTS                        (3)
#endif


/* -----------------------------------------*/
/**
 * @brief message_process
 *  process the modubs message from work queue
 *
 * @param[in] message       :   The modbus interface message
 * @param[in] lib_modbus_fd :   The modbus descriptor
 */
/* -----------------------------------------*/
static void message_process(modbus_port_handle_t* message, modbus_t* lib_modbus_fd)
{
    int                     retval = 0;
    int                     value = 0;
    int                     send_frame_len = 0;

    if(message->send_handle == NULL)
    {
        syslog_warning("[modbus]:package send function is NULL");
        goto done;
    }

    /* This function have full the "code", and "message->buffer" content */
    send_frame_len = message->send_handle(message);
    if(send_frame_len < 0)
    {
        syslog_warning("[modbus]:package a send handle fail:%d", send_frame_len);
        goto done;
    }

    /* FIXME: set the slave address, but some device have change the address
     * from "mesesage->handle" function */
    modbus_set_slave(lib_modbus_fd, message->device_addr);

    switch(message->code)
    {
        case MODBUS_FUNCTION_CODE_WRITE_SIGNLE_COIL:
            do
            {
                retval = modbus_write_bit(lib_modbus_fd, message->register_addr, message->buffer[0]);
            }while(-- message->retransmission && (retval != 1));

            break;

        case MODBUS_FUNCTION_CODE_READ_SIGNLE_COIL:
            do
            {
                retval = modbus_read_bits(lib_modbus_fd, message->register_addr, 1, message->buffer);
            }while(-- message->retransmission && (retval != 1));

            if(message->broadcast == false)
            {
                if(message->recv_handle)
                {
                    message->buffer_len = 1;
                    message->recv_handle(message);
                }
            }
            break;

        case MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_COILS:
            do
            {
                retval = modbus_write_bits(lib_modbus_fd, message->register_addr, send_frame_len, message->buffer);
            }while(-- message->retransmission && (retval != send_frame_len));
            break;

        case MODBUS_FUNCTION_CODE_READ_MULTIPLE_COILS:
            do
            {
                retval = modbus_read_bits(lib_modbus_fd, message->register_addr, send_frame_len, message->buffer);
            }while(-- message->retransmission && (retval != send_frame_len));

            if(message->broadcast == false)
            {
                if(message->recv_handle)
                {
                    message->buffer_len = retval;
                    message->recv_handle(message);
                }
            }
            break;

        case MODBUS_FUNCTION_CODE_READ_INPUT_BIT:
            do
            {
                retval = modbus_read_input_bits(lib_modbus_fd, message->register_addr, 1, message->buffer);
            }while(-- message->retransmission && (retval != 1));

            if(message->broadcast == false)
            {
                if(message->recv_handle)
                {
                    message->buffer_len = 1;
                    message->recv_handle(message);
                }
            }
            break;

        case MODBUS_FUNCTION_CODE_READ_INPUT_MULTIPLE_BITS:
            do
            {
                retval = modbus_read_input_bits(lib_modbus_fd, message->register_addr, send_frame_len, message->buffer);
            }while(-- message->retransmission && (retval != send_frame_len));

            if(message->broadcast == false)
            {
                if(message->recv_handle)
                {
                    message->buffer_len = retval;
                    message->recv_handle(message);
                }
            }
            break;

        case MODBUS_FUNCTION_CODE_WRITE_SIGNLE_REGISTER:
            value = message->buffer[0] << 8;
            value |= message->buffer[1];
            do
            {
                retval = modbus_write_register(lib_modbus_fd, message->register_addr, value);
            }while(-- message->retransmission && (retval != 1));
            break;

        case MODBUS_FUNCTION_CODE_READ_SIGNLE_REGISTER:
            do
            {
                retval = modbus_read_registers(lib_modbus_fd, message->register_addr, 1, (uint16_t*)message->buffer);
            }while(-- message->retransmission && (retval != 1));

            if(message->broadcast == false)
            {
                if(message->recv_handle)
                {
                    message->buffer_len = retval;
                    message->recv_handle(message);
                }
            }
            break;

        case MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_REGISTERS:
            do
            {
                retval = modbus_write_registers(lib_modbus_fd, message->register_addr, send_frame_len, (uint16_t*)message->buffer);
            }while(-- message->retransmission && (retval != send_frame_len));
            break;

        case MODBUS_FUNCTION_CODE_READ_MULTIPLE_REGISTERS:
            do
            {
                retval = modbus_read_registers(lib_modbus_fd, message->register_addr, send_frame_len, (uint16_t*)message->buffer);
            }while(-- message->retransmission && (retval != send_frame_len));

            if(message->broadcast == false)
            {
                if(message->recv_handle)
                {
                    message->buffer_len = retval;
                    message->recv_handle(message);
                }
            }
            break;

        case MODBUS_FUNCTION_CODE_READ_INPUT_REGISTER:
            do
            {
                retval = modbus_read_input_registers(lib_modbus_fd, message->register_addr, 1, (uint16_t*)message->buffer);
            }while(-- message->retransmission && (retval != 1));

            if(message->broadcast == false)
            {
                if(message->recv_handle)
                {
                    message->buffer_len = retval;
                    message->recv_handle(message);
                }
            }
            break;

        case MOSBUS_FUNCTION_CODE_READ_INPUT_MULTIPLE_REGISTERS:
            do
            {
                retval = modbus_read_input_registers(lib_modbus_fd, message->register_addr, send_frame_len, (uint16_t*)message->buffer);
            }while(-- message->retransmission && (retval != send_frame_len));

            if(message->broadcast == false)
            {
                if(message->recv_handle)
                {
                    message->buffer_len = retval;
                    message->recv_handle(message);
                }
            }
            break;

        case MODBUS_FUNCTION_CODE_DO_NOTHING:
            break;

        default:
            syslog_warning("[modbus]:no support code:%d", message->code);
            goto done;
    }

    return;

done:
    message->done = true;
}

void* modbus_work_thread_function(void* arg)
{
    object_management_t     *object = (object_management_t* )arg;
    RING_BUFFER             *work_queue = object->work_queue;
    modbus_port_handle_t    *message= NULL;
    modbus_t                *lib_modbus_fd = (modbus_t*)object->object_private;
    unsigned char           *send_recv_buffer = NULL;
    int                     done_counts = 0;

    if(object == NULL)
    {
        syslog_error("[modbus]:work thread argument is NULL, except (object_management_t *object)");
        goto error;
    }

    if(work_queue == NULL)
    {
        syslog_error("[modbus]:The object->work_queue is NULL");
        goto error;
    }

    if(lib_modbus_fd == NULL)
    {
        syslog_error("[modbus]:The lib modubs fd is NULL");
        goto error;
    }

    send_recv_buffer = (unsigned char*)malloc(RS485_MODBUS_MTU);
    if(send_recv_buffer == NULL)
    {
        syslog_error("[modbus]:The send receive buffer malloc fail");
        goto error;
    }

    /* TODO: add the thread exit clean */

    while(1)
    {
        /* wait the queue have a value in it */
        sem_wait(&(object->queue_sem));

        /* after have change it use sem */
        if(Ringbuf_Empty(work_queue) == true)
        {
            continue;
        }

        /* get the message */
        message = (modbus_port_handle_t*)Ringbuf_Peek(work_queue);
        if(message == NULL)
        {
            continue;
        }

        /* get the message from device profile */
        message->buffer = send_recv_buffer;
        message->buffer_len = RS485_MODBUS_MTU;
        message->done = true;

        /* FIXME: process the message */
        /* The message default is have finish
         * but some message maybe have many times to send device
         * so , you used it ok, please set message->done is ture
         * */
        done_counts = DO_SOMETHINGS_MAX_COUNTS;
        do
        {
            message_process(message, lib_modbus_fd);
        }while(!(message->done) && --done_counts);

        /* free the message */
        Ringbuf_Pop(work_queue, (uint8_t*)message);
    } // end while


    /* have not running to this */
    free(send_recv_buffer);

error:
    modbus_service_deinit(object);
    /** add the thread detach attr */
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}


int modbus_service_init(object_management_t* object)
{
    struct timeval timeout;
    int ret= 0;
    char parity;

    if(object == NULL)
        return -EPERM;

    /* I have default the MODBUS have use the
     * 1 start bit,
     * 8 data bit,
     * 1 odd parity bit
     * 1 stop bit*/
    switch(object->port.parity)
    {
        case RS485_PORT_TYPE_NONE:
            parity = 'N';
            break;
        case RS485_PORT_TYPE_EVEN:
            parity = 'E';
            break;
        case RS485_PORT_TYPE_ODD:
            parity = 'O';
            break;

        default:
            parity = 'E';
            break;
    }

#if (PANNO_S_ITEM_GATEWAY_JOHNSON_CONTROLS == 1)
    parity = 'N';
#endif

    object->object_private = modbus_new_rtu(object->port.interface_name,
            object->port.baud_rate,
            parity,8,1);
    if(object->object_private == NULL)
        return -EIO;

    /* initial the /dev/tty */
    ret = modbus_connect(object->object_private);
    if(ret < 0)
    {
        syslog_error("[modbus]: modbus port initial fail:%d", ret);
        return -EPERM;
    }

    /* set the receive timeout 500ms*/
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;
    modbus_set_response_timeout(object->object_private, &timeout);
    /* set debug */
    modbus_set_debug(object->object_private, true);

    return 0;
}

void modbus_service_deinit(object_management_t* object)
{
    if(object == NULL)
        return;

    if(object->object_private == NULL)
        return;

    modbus_close(object->object_private);

    modbus_free(object->object_private);
}
