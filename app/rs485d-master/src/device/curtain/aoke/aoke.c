/*
 * =====================================================================================
 *
 *          @file:  aoke.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Apr 1, 2016 7:55:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include <errno.h>

#include "enum.h"
#include "syslog/log.h"
#include "protocol/general/general.h"


/**
 * @ingroup curtain_ao_ke
 * 	ao_ke curtain on RS485
 *	baud rate	: 9600
 *	stop bit	: 1
 *	data bit	: 8
 *	parity bit	: NULL
 *
 *	format:
 *	-------------------------------------------
 *	|SI |ADDR0 |ADDR1 |ADDR2 |CMD |DATA |CHECK|
 *	-------------------------------------------
*/


#define RS485_CURTAIN_AO_KE_SI					0x9a


#define RS485_CURTAIN_AO_KE_COMMAND_CONTROL		0x0a
#define RS485_CURTAIN_AO_KE_COMMAND_POSTION		0xdd
#define RS485_CURTAIN_AO_KE_COMMAND_SETTING		0xd5
#define RS485_CURTAIN_AO_KE_COMMAND_GETTING		0xcc
#define RS485_CURTAIN_AO_KE_COMMAND_ADDRING		0xaa
#define RS485_CURTAIN_AO_KE_COMMAND_DELETE		0xa6
#define RS485_CURTAIN_AO_KE_COMMAND_POINT		0xda


#define RS485_CURTAIN_AO_KE_CONTROL_UP			0xdd
#define RS485_CURTAIN_AO_KE_CONTROL_STOP		0x0d
#define RS485_CURTAIN_AO_KE_CONTROL_DOWN		0xee
#define RS485_CURTAIN_AO_KE_CONTROL_SET_ADDR	0xaa
#define RS485_CURTAIN_AO_KE_CONTROL_DELETE_ADDR	0xa6
#define RS485_CURTAIN_AO_KE_CONTROL_MIDDLE_1	0x01
#define RS485_CURTAIN_AO_KE_CONTROL_MIDDLE_2	0x02
#define RS485_CURTAIN_AO_KE_CONTROL_MIDDLE_3	0x03
#define RS485_CURTAIN_AO_KE_CONTROL_MIDDLE_4	0x04


#define RS485_CURTIAN_AO_KE_POSTION				0x00

#define RS485_CURTIAN_AO_KE_SETTING_HANDLE		0x01

#define RS485_CURTIAN_AO_KE_GETTING_STATUS		0xcc

#define RS485_CURTAIN_AO_KE_ADDRING				0xaa

#define RS485_CURTAIN_AO_KE_DELETE				0xa6

#define RS485_CURTAIN_AO_KE_POINT_UP			0xdd
#define RS485_CURTAIN_AO_KE_POINT_MIDDLE		0xcc
#define RS485_CURTAIN_AO_KE_POINT_DOWN			0xda
#define RS485_CURTAIN_AO_KE_POINT_SAVE			0xaa
#define RS485_CURTAIN_AO_KE_POINT_DELETE		0x00

typedef struct
{
	unsigned char 		d1;
	unsigned char 		d2;
	unsigned char 		d3;
	unsigned char		d4;
	unsigned char		d5;
}rs485_curtain_ao_ke_send_package_t;



#ifdef RS485_DEBUG
static void	printf_array(unsigned char array[], int len)
{
	int i;

	if(len > 0)
	{
		for(i=0; i<len; i++)
		{
			RS485_DEBUGF(DBG_RS485_CURTAIN, ("0x%02x ", array[i]));
		}
	}

	RS485_DEBUGF(DBG_RS485_CURTAIN, ("\n"));
}
#endif


static unsigned char get_check(rs485_curtain_ao_ke_send_package_t* package)
{
	unsigned char result = 0;

	result ^= package->d1;
	result ^= package->d2;
	result ^= package->d3;
	result ^= package->d4;
	result ^= package->d5;

	return result;
}

#if 0
static int rs485_curtain_ao_ke_send_package(int port, rs485_curtain_ao_ke_send_package_t* package)
{
	unsigned char send_buffer[12];

	if(package == NULL)
		return -1;

	send_buffer[0] = RS485_CURTAIN_AO_KE_SI;
	send_buffer[1] = package->d1;
	send_buffer[2] = package->d2;
	send_buffer[3] = package->d3;
	send_buffer[4] = package->d4;
	send_buffer[5] = package->d5;
	send_buffer[6] = get_check(package);

	rs485_send(port, send_buffer, 7);
#ifdef RS485_DEBUG
    printf_array(send_buffer, 7);
#endif


	return 0;
}



/**
 * @ingroup curtain_ao_ke
 *
 * @param[in] port the rs485 port
 *
 * @return 0 is success, others is fail
 *
 */
int rs485_curtain_ao_ke_init(int port)
{
	return 0;
}

/**
 * @ingroup curtain_ao_ke
 *	input a ao_ke curtain address to close the curtain
 *
 * @param[in] port The RS485 port.
 * @param[in] addr the curtain address id1 .and id2=0xfe, id3=0x00
 *
 * @return 0 is success, others is fail
 */
int rs485_curtain_ao_ke_open(int port, unsigned char addr[3])
{
	rs485_curtain_ao_ke_send_package_t package;

	package.d1 = addr[0];
	package.d2 = addr[1];
	package.d3 = addr[2];
	package.d4 = RS485_CURTAIN_AO_KE_COMMAND_CONTROL;
	package.d5 = RS485_CURTAIN_AO_KE_CONTROL_DOWN;

	rs485_curtain_ao_ke_send_package(port, &package);

	return 0;
}

/**
 * @ingroup curtain_ao_ke
 *	input a ao_ke curtain address to close the curtain
 *
 * @param[in] port The RS485 port.
 * @param[in] addr the curtain address id1 .and id2=0xfe, id3=0x00
 *
 * @return 0 is success, others is fail
 */
int rs485_curtain_ao_ke_close(int port, unsigned char addr[3])
{
	rs485_curtain_ao_ke_send_package_t package;

	package.d1 = addr[0];
	package.d2 = addr[1];
	package.d3 = addr[2];
	package.d4 = RS485_CURTAIN_AO_KE_COMMAND_CONTROL;
	package.d5 = RS485_CURTAIN_AO_KE_CONTROL_UP;

	rs485_curtain_ao_ke_send_package(port, &package);

	return 0;
	return 0;
}

/**
 * @ingroup curtain_ao_ke
 * 	input a ao_ke curtain address and percent to set curtain percent
 *
 * @param[in] port The RS485 port.
 * @param[in] addr the curtain address id1. and id2=0xfe, id3=0x00
 * @param[in] percent the curtain set percent
 *
 * @return 0 is success, others is fail
 */
int rs485_curtain_ao_ke_set_percent(int port, unsigned char addr[3], unsigned char percent)
{
	rs485_curtain_ao_ke_send_package_t package;

	package.d1 = addr[0];
	package.d2 = addr[1];
	package.d3 = addr[2];
	package.d4 = RS485_CURTAIN_AO_KE_COMMAND_POSTION;
	package.d5 = percent;

	rs485_curtain_ao_ke_send_package(port, &package);

	return 0;
}


 /**
  * @ingroup curtain_ao_ke
  *
  * @param[in] port The RS485 port.
  * @param[in] addr the curtain address id1. and id2=0xf3, id3=0x00
  *
  * @return 0 is success, others is fail
  */
int rs485_curtain_ao_ke_stop(int port, unsigned char addr[3])
{
	rs485_curtain_ao_ke_send_package_t package;

	package.d1 = addr[0];
	package.d2 = addr[1];
	package.d3 = addr[2];
	package.d4 = RS485_CURTAIN_AO_KE_COMMAND_CONTROL;
	package.d5 = RS485_CURTAIN_AO_KE_CONTROL_STOP;

	rs485_curtain_ao_ke_send_package(port, &package);

	return 0;
}

#endif


#if 0

int aoke_send_package_handle(unsigned char* send_buffer,
        unsigned int send_buffer_len,
        unsigned char* addr,
        int addr_len,
        bool broadcast,
        int command,
        int value)
{
    unsigned int                        package_len = 7;
	rs485_curtain_ao_ke_send_package_t  package;

    if(send_buffer == NULL)
        return -1;

    if(addr_len != 3)
        return -1;

    if(send_buffer_len < package_len)
        return -1;

    /* set the broadcast address */
    if(broadcast)
    {
        addr[0] = 0x00;
        addr[1] = 0xff;
        addr[2] = 0xff;
    }

    switch(command)
    {
        case RS485_CURTAIN_OPEN:
            package.d1 = addr[0];
            package.d2 = addr[1];
            package.d3 = addr[2];
            package.d4 = RS485_CURTAIN_AO_KE_COMMAND_CONTROL;
            package.d5 = RS485_CURTAIN_AO_KE_CONTROL_DOWN;
            break;

        case RS485_CURTAIN_CLOSE:
            package.d1 = addr[0];
            package.d2 = addr[1];
            package.d3 = addr[2];
            package.d4 = RS485_CURTAIN_AO_KE_COMMAND_CONTROL;
            package.d5 = RS485_CURTAIN_AO_KE_CONTROL_UP;
            break;

        case RS485_CURTAIN_SET_PERCENT:
            package.d1 = addr[0];
            package.d2 = addr[1];
            package.d3 = addr[2];
            package.d4 = RS485_CURTAIN_AO_KE_COMMAND_POSTION;
            package.d5 = value;
            break;

        case RS485_CURTAIN_GET_DEVICE_INFO:
            package.d1 = 0x00;
            package.d2 = 0x00;
            package.d3 = 0x00;
            package.d4 = 0x00;
            package.d5 = 0x00;
            break;

        default:
            syslog_warning("[aoke]:unknown command:%d", command);
            return -1;

    }

	send_buffer[0] = RS485_CURTAIN_AO_KE_SI;
	send_buffer[1] = package.d1;
	send_buffer[2] = package.d2;
	send_buffer[3] = package.d3;
	send_buffer[4] = package.d4;
	send_buffer[5] = package.d5;
	send_buffer[6] = get_check(&package);

    return package_len;
}
#endif

int aoke_send_package_handle(volatile void* arg)
{
    mstp_port_handle_t                  *handle = (mstp_port_handle_t*)arg;
    unsigned int                        package_len = 7;
	rs485_curtain_ao_ke_send_package_t  package;

    if(handle == NULL)
        return -1;

    if(handle->package_buffer== NULL)
        return -1;

    if(handle->address_len != 3)
        return -1;

    if(handle->package_buffer_len < package_len)
        return -1;

    /* set the broadcast address */
    if(handle->broadcast)
    {
        handle->address[0] = 0x00;
        handle->address[1] = 0xff;
        handle->address[2] = 0xff;
    }

    switch(handle->method)
    {
        case RS485_CURTAIN_OPEN:
            package.d1 = handle->address[0];
            package.d2 = handle->address[1];
            package.d3 = handle->address[2];
            package.d4 = RS485_CURTAIN_AO_KE_COMMAND_CONTROL;
            package.d5 = RS485_CURTAIN_AO_KE_CONTROL_DOWN;
            break;

        case RS485_CURTAIN_CLOSE:
            package.d1 = handle->address[0];
            package.d2 = handle->address[1];
            package.d3 = handle->address[2];
            package.d4 = RS485_CURTAIN_AO_KE_COMMAND_CONTROL;
            package.d5 = RS485_CURTAIN_AO_KE_CONTROL_UP;
            break;

        case RS485_CURTAIN_SET_PERCENT:
            package.d1 = handle->address[0];
            package.d2 = handle->address[1];
            package.d3 = handle->address[2];
            package.d4 = RS485_CURTAIN_AO_KE_COMMAND_POSTION;
            package.d5 = handle->value;
            break;

        case RS485_CURTAIN_GET_DEVICE_INFO:
            package.d1 = 0x00;
            package.d2 = 0x00;
            package.d3 = 0x00;
            package.d4 = 0x00;
            package.d5 = 0x00;
            break;

        default:
            syslog_warning("[aoke]:unknown command:%d", handle->method);
            return -1;

    }

	handle->package_buffer[0] = RS485_CURTAIN_AO_KE_SI;
	handle->package_buffer[1] = package.d1;
	handle->package_buffer[2] = package.d2;
	handle->package_buffer[3] = package.d3;
	handle->package_buffer[4] = package.d4;
	handle->package_buffer[5] = package.d5;
	handle->package_buffer[6] = get_check(&package);

    return package_len;
}


#if 0
int aoke_recv_package_handle(int device_id,
        unsigned char* recv_buffer,
        unsigned int recv_buffer_len,
        int command,
        void* UNUSED(arg))
{
    return 0;
}
#endif

int aoke_recv_package_handle(volatile void* arg)
{
    return 0;
}
