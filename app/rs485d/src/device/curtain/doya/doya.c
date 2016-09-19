/*
 * =====================================================================================
 *
 *          @file:  doya.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Apr 1, 2016 7:54:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

/*
 * @file rs485_curtain_dooy.c
 * @author chuanjiang.wong
 *
 *  Created on: Aug 12, 2015
 *      Author: wong
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>
#include <errno.h>

#include "enum.h"
#include "syslog/log.h"
#include "adapter.h"
#include "device.h"
#include "protocol/general/general.h"


/**
	du_ya curtain on RS485
	baud rate	: 9600
	stop bit	: 1
	data bit	: 8
	parity bit	: NULL
*/


typedef enum
{
	COMMAND_OPEN,
	COMMAND_CLOSE,
	COMMAND_STOP,
	COMMAND_PERCENT,
	COMMAND_DELETE,
	COMMAND_REFACTORY,

//	RW_ADDR_LOW,
//	RW_ADDR_HIGH,
	WO_ADDR,
	RO_PERCENT,
	RW_DIRECTION,
	RW_HANDLE,
	RW_SWITCH_PASSIVE,
	RW_SWITCH_ACTIVE,
	RO_VERSION

}command;





struct package
{
//	unsigned char start;
	unsigned char addr_low;		//addr don't is 0x00, 0xff
	unsigned char addr_high;    //default addr is 0xfefe
	unsigned char command;
	unsigned char data_addr;
	unsigned char data[4];
//	unsigned char crc[2];       //crc16_l, crc16_h

	int cmd;
};

#define CURTAIN_COMMAND						0x03
#define CURTAIN_READ						0x01
#define CURTAIN_WRITE						0x02

//command
#define CURTAIN_COMMAND_OPEN				0x01
#define CURTAIN_COMMAND_CLOSE				0x02
#define CURTAIN_COMMAND_STOP				0x03
#define CURTAIN_COMMAND_PERCENT				0x04
#define CURTAIN_COMMAND_DELETE				0x07
#define CURTAIN_COMMAND_REFACTORY			0x08

//read write
#define CURTAIN_READ_WRITE_ADDR_LOW			0x00
#define CURTAIN_READ_WRITE_ADDR_HIGH		0x01
#define CURTAIN_READ_WRITE_ADDR				0x00
#define CURTAIN_READ_WRITE_PERCENT			0x02
#define CURTAIN_READ_WRITE_DIRECTION		0x03
#define CURTAIN_READ_WRITE_HANDLE			0x04
#define CURTAIN_READ_WRITE_SWITCH_PASSIVE 	0x27
#define CURTAIN_READ_WRITE_SWITCH_ACTIVE	0x28
#define CURTAIN_READ_WRITE_VERSION			0xfe


#define MOTOR_POSITIVE						0x00
#define MOTOR_NEGATIVE						0x01

#define HANDLE_ENABLE						0x00
#define HANDLE_DISABLE				 		0x01

#define SWITCH_PASSIVE_DOUBLE_REBOUND		0x01
#define SWITCH_PASSIVE_DOUBLE_NO_REBOUND	0x02
#define SWITCH_PASSIVE_ELECTRONIC_DC246		0x03
#define SWITCH_PASSIVE_SINGLE_CYCLE			0x04

#define SWITCH_ACTIVE_DOUBLE_LINE			0x00
#define SWITCH_ACTIVE_SINGLE_LINE			0x01



static inline int rs485_send(int port, void*buffer, int len)
{
	return len;
}

static inline int rs485_read(int port, void*buffer, int len)
{
	return len;
}


static const unsigned char crc_high[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const unsigned char crc_low[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};


static void modbus_crc16(unsigned char result[2], const unsigned char* pucFrame, int usLen)

{
    unsigned char ucCRCHi = 0xFF;
    unsigned char ucCRCLo = 0xFF;
    int iIndex;

    while( usLen-- )
    {
        iIndex = ucCRCLo ^ *( pucFrame++ );
        ucCRCLo = (unsigned char)( ucCRCHi ^ crc_high[iIndex] );
        ucCRCHi = crc_low[iIndex];
    }

    result[0] = ucCRCLo;
    result[1] = ucCRCHi;
}

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


static int send_package(int port, const struct package* package)
{
	unsigned char buffer[12];


	if(package->cmd == WO_ADDR)
	{
		if(package->data[1] == 0x00 || (package->data[2] == 0x00))
			return -1;

		if(package->data[1] == 0xff || (package->data[2] == 0xff))
			return -1;
	}

	buffer[0] = 0x55;
	buffer[1] = package->addr_low;
	buffer[2] = package->addr_high;
	buffer[3] = package->command;
	buffer[4] = package->data_addr;

	switch(package->cmd)
	{
		//command
		case COMMAND_OPEN:
		case COMMAND_CLOSE:
		case COMMAND_STOP:
		case COMMAND_DELETE:
		case COMMAND_REFACTORY:
			modbus_crc16(buffer+5, buffer, 5);
			rs485_send(port, buffer, 7);
			break;
		case COMMAND_PERCENT:
			buffer[5] = package->data[0];
			modbus_crc16(buffer+6, buffer, 6);
			rs485_send(port, buffer, 8);
			break;

		//write only
		case WO_ADDR:
			buffer[5] = package->data[0];    //data length
			buffer[6] = package->data[1];	 //address low
			buffer[7] = package->data[2];	 //address high
			modbus_crc16(buffer+8, buffer, 8);
			rs485_send(port, buffer, 10);
			break;

		//read only
		case RO_PERCENT:
		case RO_VERSION:
			buffer[5] = package->data[0];    //data length
			modbus_crc16(buffer+6, buffer, 6);
			rs485_send(port, buffer, 8);
			break;

		//read_write
		case RW_DIRECTION:
		case RW_HANDLE:
		case RW_SWITCH_PASSIVE:
		case RW_SWITCH_ACTIVE:
			if(package->command == CURTAIN_READ)
			{
				buffer[5] = package->data[0];

				modbus_crc16(buffer+6, buffer, 6);
				rs485_send(port, buffer, 8);
			}
			else
			{
				buffer[5] = package->data[0];
				buffer[6] = package->data[1];
				modbus_crc16(buffer+7, buffer, 7);
				rs485_send(port, buffer, 9);
			}
			break;
	}

#ifdef RS485_DEBUG
    printf_array(buffer, 10);
#endif

	return 0;
}

#if 0
int receive_package(int port, struct package* package)
{
	unsigned char buffer[12];
	unsigned char chk[2];
	int repair = 50;

	while(repair--)
	{
		rs485_read(port, buffer, 1);
		if(buffer[0] == 0x55)
			break;
	}
	if(repair == 0)
		return -1;


	if(4 != rs485_read(port, buffer, 4))
		return -1;

	package->command = buffer[3];
	package->data_addr = buffer[4];

	if(buffer[3] == CURTAIN_COMMAND)
	{
		if(buffer[4] == CURTAIN_COMMAND_PERCENT)
		{
			rs485_read(port, buffer+5, 3);

			modbus_crc16(chk, buffer, 6);
			if(buffer[6] == chk[0] && (buffer[7] == chk[1]))
				return 0;
		}
		else
		{
			rs485_read(port, buffer+5, 2);
			modbus_crc16(chk, buffer, 5);
			if(buffer[5] == chk[0] && (buffer[6] == chk[1]))
				return 0;
		}
	}
	else if(buffer[3] == CURTAIN_WRITE || (buffer[3] == CURTAIN_READ))
	{
		rs485_read(port, buffer+5, 3);
		if(buffer[5] != 1)
			return -1;

		modbus_crc16(chk, buffer, 6);
		if(buffer[6] != chk[0] || (buffer[7] == chk[1]))
			return -1;

		package->data[0] = buffer[5];
		return 0;
	}
	else
		return -1;

	return -1;
}

#endif



int open_curtain_no_reply(int port, const unsigned char addr[2])
{
	struct package package;

	package.addr_low = addr[0];
	package.addr_high = addr[1];

	package.cmd = COMMAND_OPEN;
	package.command = CURTAIN_COMMAND;

	package.data_addr = CURTAIN_COMMAND_OPEN;

	send_package(port, &package);

	return 0;
}

int close_curtain_no_reply(int port, const unsigned char addr[2])
{
	struct package package;

	package.addr_low = addr[0];
	package.addr_high = addr[1];

	package.cmd = COMMAND_CLOSE;
	package.command = CURTAIN_COMMAND;

	package.data_addr = CURTAIN_COMMAND_CLOSE;

	send_package(port, &package);

	return 0;
}

int stop_curtain_no_reply(int port, const unsigned char addr[2])
{
	struct package package;

	package.addr_low = addr[0];
	package.addr_high = addr[1];

	package.cmd = COMMAND_STOP;
	package.command = CURTAIN_COMMAND;

	package.data_addr = CURTAIN_COMMAND_STOP;

	send_package(port, &package);

	return 0;
}

int percent_curtain_no_reply(int port, const unsigned char addr[2],
		unsigned char data)
{
	struct package package;

	package.addr_low = addr[0];
	package.addr_high = addr[1];

	package.cmd = COMMAND_PERCENT;
	package.command = CURTAIN_COMMAND;

	package.data_addr = CURTAIN_COMMAND_PERCENT;
	package.data[0] = data;

	send_package(port, &package);

	return 0;
}

int delete_track_curtain_no_reply(int port, const unsigned char addr[2])
{
	struct package package;

	package.addr_low = addr[0];
	package.addr_high = addr[1];

	package.cmd = COMMAND_DELETE;
	package.command = CURTAIN_COMMAND;

	package.data_addr = CURTAIN_COMMAND_DELETE;

	send_package(port, &package);

	return 0;
}

int refactory_curtain_no_reply(int port, const unsigned char addr[2])
{
	struct package package;

	package.addr_low = addr[0];
	package.addr_high = addr[1];

	package.cmd = COMMAND_REFACTORY;
	package.command = CURTAIN_COMMAND;

	package.data_addr = CURTAIN_COMMAND_REFACTORY;

	send_package(port, &package);

	return 0;
}

int set_addr_curtain_no_reply(int port, const unsigned char addr[2],
		unsigned char set[2])
{
	struct package package;

	package.addr_low = addr[0];
	package.addr_high = addr[1];

	package.cmd = WO_ADDR;
	package.command = CURTAIN_WRITE;

	package.data_addr = CURTAIN_READ_WRITE_ADDR;
	package.data[0] = 0x02;
	package.data[1] = set[0];
	package.data[2] = set[1];

	send_package(port, &package);

	return 0;
}

int set_direction_curtain_no_reply(int port, const unsigned char addr[2],
		unsigned char same)
{
	struct package package;

	package.addr_low = addr[0];
	package.addr_high = addr[1];

	package.cmd = RW_DIRECTION;
	package.command = CURTAIN_WRITE;

	package.data_addr = CURTAIN_READ_WRITE_DIRECTION;
	package.data[0] = same;

	send_package(port, &package);

	return 0;
}

int set_handle_enable_curtain_no_reply(int port, const unsigned char addr[2],
		unsigned char enable)
{
	struct package package;

	package.addr_low = addr[0];
	package.addr_high = addr[1];

	package.cmd = RW_HANDLE;
	package.command = CURTAIN_WRITE;

	package.data_addr = CURTAIN_READ_WRITE_HANDLE;
	package.data[0] = enable;

	send_package(port, &package);

	return 0;
}

int set_switch_passive_curtain_no_reply(int port, const unsigned char addr[2],
		unsigned char type)
{
	struct package package;

	package.addr_low = addr[0];
	package.addr_high = addr[1];

	package.cmd = RW_SWITCH_PASSIVE;
	package.command = CURTAIN_WRITE;

	package.data_addr = CURTAIN_READ_WRITE_SWITCH_PASSIVE;
	package.data[0] = type;

	send_package(port, &package);

	return 0;
}

int set_switch_active_curtain_no_reply(int port, const unsigned char addr[2],
		unsigned char type)
{
	struct package package;

	package.addr_low = addr[0];
	package.addr_high = addr[1];

	package.cmd = RW_SWITCH_ACTIVE;
	package.command = CURTAIN_WRITE;

	package.data_addr = CURTAIN_READ_WRITE_SWITCH_ACTIVE;

	send_package(port, &package);

	return 0;
}

int read_percent_curtain_no_reply(int port, const unsigned char addr[2])
{
	struct package package;

	package.addr_low = addr[0];
	package.addr_high = addr[1];

	package.cmd = RO_PERCENT;
	package.command = CURTAIN_READ;

	package.data_addr = CURTAIN_READ_WRITE_PERCENT;

	send_package(port, &package);

	return 0;
}

int read_version_curtain_no_reply(int port, const unsigned char addr[2])
{
	struct package package;

	package.addr_low = addr[0];
	package.addr_high = addr[1];

	package.cmd = RO_VERSION;
	package.command = CURTAIN_READ;

	package.data_addr = CURTAIN_READ_WRITE_VERSION;

	send_package(port, &package);

	return 0;
}


static int doya_send_package(unsigned char* send_buffer, int buffer_len, const struct package* package)
{
    int package_len = 0;
    unsigned char buffer[20];

    if(send_buffer == NULL)
        return -1;

	if(package->cmd == WO_ADDR)
	{
		if(package->data[1] == 0x00 || (package->data[2] == 0x00))
			return -1;

		if(package->data[1] == 0xff || (package->data[2] == 0xff))
			return -1;
	}

	buffer[0] = 0x55;
	buffer[1] = package->addr_low;
	buffer[2] = package->addr_high;
	buffer[3] = package->command;
	buffer[4] = package->data_addr;

	switch(package->cmd)
	{
		//command
		case COMMAND_OPEN:
		case COMMAND_CLOSE:
		case COMMAND_STOP:
		case COMMAND_DELETE:
		case COMMAND_REFACTORY:
			modbus_crc16(buffer+5, buffer, 5);
            package_len = 7;
			break;
		case COMMAND_PERCENT:
			buffer[5] = package->data[0];
			modbus_crc16(buffer+6, buffer, 6);
            package_len = 8;
			break;

		//write only
		case WO_ADDR:
			buffer[5] = package->data[0];    //data length
			buffer[6] = package->data[1];	 //address low
			buffer[7] = package->data[2];	 //address high
			modbus_crc16(buffer+8, buffer, 8);
            package_len = 10;
			break;

		//read only
		case RO_PERCENT:
		case RO_VERSION:
			buffer[5] = package->data[0];    //data length
			modbus_crc16(buffer+6, buffer, 6);
            package_len = 8;
			break;

		//read_write
		case RW_DIRECTION:
		case RW_HANDLE:
		case RW_SWITCH_PASSIVE:
		case RW_SWITCH_ACTIVE:
			if(package->command == CURTAIN_READ)
			{
				buffer[5] = package->data[0];

				modbus_crc16(buffer+6, buffer, 6);
                package_len = 8;
			}
			else
			{
				buffer[5] = package->data[0];
				buffer[6] = package->data[1];
				modbus_crc16(buffer+7, buffer, 7);
                package_len = 9;
			}
			break;

        default:
            syslog_info("%s", "cmd unknown");
	}

    if(buffer_len < package_len)
    {
        syslog_warning("The buffer len :%d  < package len:%d", buffer_len, package_len);
        return -1;
    }

    memcpy(send_buffer, buffer, package_len);

	return package_len;
}


#if 0

int doya_send_package_handle(unsigned char* send_buffer,
        unsigned int send_buffer_len,
        unsigned char* addr,
        int addr_len,
        bool broadcast,
        int command,
        int (value))
{
	struct package package;

    if(addr_len < 2)
    {
        syslog_warning("%s", "address length error");
        return -1;
    }

    memset(&package, 0x00, sizeof(package));

    if(broadcast)
    {
        package.addr_low = 0x00;
        package.addr_high = 0x00;
    }
    else
    {
	    package.addr_low = addr[0];
	    package.addr_high = addr[1];
    }

	switch(command)
	{
        case RS485_DOYA_CURTAIN_OPEN:
	        package.cmd = COMMAND_OPEN;
	        package.command = CURTAIN_COMMAND;
	        package.data_addr = CURTAIN_COMMAND_OPEN;
            break;
        case RS485_DOYA_CURTAIN_CLOSE:
	        package.cmd = COMMAND_CLOSE;
	        package.command = CURTAIN_COMMAND;
	        package.data_addr = CURTAIN_COMMAND_CLOSE;
            break;
        case RS485_DOYA_CURTAIN_SET_PERCENT:
	        package.cmd = COMMAND_PERCENT;
	        package.command = CURTAIN_COMMAND;
	        package.data_addr = CURTAIN_COMMAND_PERCENT;
	        package.data[0] = value;
            break;
        case RS485_DOYA_CURTAIN_RESET:
            break;

        case RS485_DOYA_CURTAIN_GET_DEVICE_INFO:
	        package.cmd = RO_PERCENT;
	        package.command = CURTAIN_READ;
	        package.data_addr = CURTAIN_READ_WRITE_PERCENT;
            break;
        default:
        	return -1;
	}

    return doya_send_package(send_buffer, send_buffer_len, &package);
}

#endif

int doya_send_package_handle(volatile void* arg)
{
    mstp_port_handle_t          *handle = (mstp_port_handle_t*)arg;
	struct package package;

    if(handle == NULL)
        return -1;

    if(handle->package_buffer == NULL)
        return -1;

    if(handle->address_len < 2)
    {
        syslog_warning("%s", "address length error");
        return -1;
    }

    memset(&package, 0x00, sizeof(package));

    if(handle->broadcast)
    {
        package.addr_low = 0x00;
        package.addr_high = 0x00;
    }
    else
    {
	    package.addr_low = handle->address[0];
	    package.addr_high = handle->address[1];
    }

	switch(handle->method)
	{
        case RS485_CURTAIN_OPEN:
	        package.cmd = COMMAND_OPEN;
	        package.command = CURTAIN_COMMAND;
	        package.data_addr = CURTAIN_COMMAND_OPEN;
            break;
        case RS485_CURTAIN_CLOSE:
	        package.cmd = COMMAND_CLOSE;
	        package.command = CURTAIN_COMMAND;
	        package.data_addr = CURTAIN_COMMAND_CLOSE;
            break;
        case RS485_CURTAIN_SET_PERCENT:
	        package.cmd = COMMAND_PERCENT;
	        package.command = CURTAIN_COMMAND;
	        package.data_addr = CURTAIN_COMMAND_PERCENT;
	        package.data[0] = handle->value;
            break;
        case RS485_CURTAIN_RESET:
            break;

        case RS485_CURTAIN_GET_DEVICE_INFO:
	        package.cmd = RO_PERCENT;
	        package.command = CURTAIN_READ;
	        package.data_addr = CURTAIN_READ_WRITE_PERCENT;
            break;
        default:
        	return -1;
	}

    return doya_send_package(handle->package_buffer, handle->package_buffer_len, &package);
}


#if 0
int doya_recv_package_handle(int device_id,
        unsigned char* recv_buffer,
        unsigned int recv_buffer_len,
        int command,
        void* UNUSED(arg))
{
    unsigned char crc[2] = {0, 0};
    read_device_return_t    new_device_info;

    if(recv_buffer == NULL)
    {
        return -EINVAL;
    }

    switch(command)
    {
        case RS485_CURTAIN_OPEN:
        case RS485_CURTAIN_CLOSE:
        case RS485_CURTAIN_SET_PERCENT:
            modbus_crc16(crc, recv_buffer, recv_buffer_len - 2);
            if(crc[0] != recv_buffer[recv_buffer_len-2] ||
                    crc[1] != recv_buffer[recv_buffer_len-1])
            {
                syslog_error("crc error");
                return -EIO;
            }
            break;
        case RS485_CURTAIN_GET_DEVICE_INFO:
            if(recv_buffer_len != 7)
            {
                return -EIO;
            }

            if(crc[0] != recv_buffer[recv_buffer_len-2] ||
                    crc[1] != recv_buffer[recv_buffer_len-1])
            {
                syslog_error("crc error");
                return -EIO;
            }

            /*TODO: process the info */
            memset(&new_device_info, 0x00, sizeof(new_device_info));
            new_device_info.error = false;
            new_device_info.runing = true;
            new_device_info.profile.curtain.current_percent = recv_buffer[4];
            set_read_device_information(&new_device_info, device_id);
            break;

        default:
            return -EPERM;
    }

    return 0;
}

#endif

int doya_recv_package_handle(volatile void* arg)
{
    mstp_port_handle_t      *handle = (mstp_port_handle_t*)arg;
    unsigned char           crc[2] = {0, 0};
    read_device_return_t    new_device_info;

    if(handle == NULL)
    {
        return -EINVAL;
    }

    if(handle->package_buffer == NULL)
    {
        return -EINVAL;
    }

    switch(handle->method)
    {
        case RS485_CURTAIN_OPEN:
        case RS485_CURTAIN_CLOSE:
        case RS485_CURTAIN_SET_PERCENT:
            /**FIXME:because of the controler have need to access to the two dooya curtains
             * just have one address, so, have no chech the reply value.*/
#ifdef DOYA_ONLY_ONE_ADDRESS
            modbus_crc16(crc, handle->package_buffer, handle->package_buffer_len - 2);
            if(crc[0] != handle->package_buffer[handle->package_buffer_len -2] ||
                    crc[1] != handle->package_buffer[handle->package_buffer_len -1])
            {
                syslog_error("[dooya]:crc error");
                return -EIO;
            }
#else
            /** sleep 300ms */
            usleep(300000);
#endif
            break;
        case RS485_CURTAIN_GET_DEVICE_INFO:
            if(handle->package_buffer_len != 7)
            {
                return -EIO;
            }

            if(crc[0] != handle->package_buffer[handle->package_buffer_len-2] ||
                    crc[1] != handle->package_buffer[handle->package_buffer_len-1])
            {
                syslog_error("[dooya]:crc error");
                return -EIO;
            }

            /*TODO: process the info */
            memset(&new_device_info, 0x00, sizeof(new_device_info));
            new_device_info.error = false;
            new_device_info.runing = true;
            new_device_info.profile.curtain.current_percent = handle->package_buffer[4];
            set_read_device_information(&new_device_info, handle->device_id);
            break;

        default:
            return -EPERM;
    }
#ifdef DOYA_ONLY_ONE_ADDRESS
    return 0;
#else
    return -EIO;
#endif
}


