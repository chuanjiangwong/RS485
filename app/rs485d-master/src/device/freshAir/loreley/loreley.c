/*
 * =====================================================================================
 *
 *          @file:  loreley.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Apr 1, 2016 7:57:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <errno.h>

#include "enum.h"
#include "syslog/log.h"
#include "adapter.h"
#include "device.h"
#include "protocol/general/general.h"

/**package protocol*/
#define SOI_SEND                                                0xaa
#define SOI_RECEIVE                                             0x55
#define ADR_BROADCAST                                           0xff
#define ADR_DEFAULT                                             0x01
#define RTN_SEND                                                0x60
#define RTN_RECEIVE_CMD_RIGHT                                   0x01
#define RTN_RECEIVE_CHK_ERROR                                   0x02
#define RTN_RECEIVE_CMD_INVALID                                 0x03
#define EOI      												0x0d


#define RS485_NEW_TREND_SET_ADDR_CID							0x20
#define RS485_NEW_TREND_GET_ADDR_CID							0x21
#define RS485_NEW_TREND_GET_INFO_CID							0x22
#define RS485_NEW_TREND_SET_ARG_CID								0x23
#define RS485_NEW_TREND_RESTART_CID								0x24


#define RS485_NEW_TREND_MODE_WAITING							0x00
#define RS485_NEW_TREND_MODE_AUTOING							0x01
#define RS485_NEW_TREND_MODE_OUT_CRC							0x02
#define RS485_NEW_TREND_MODE_IN_CRC								0x03
#define RS485_NEW_TREND_MODE_KILLING							0x04

#define RS485_NEW_TREND_RUN_STATUS_OFF							0x00
#define RS485_NEW_TREND_RUN_STATUS_ON							0x01

#define RS485_NEW_TREND_STATUS_ERROR							0x01
#define RS485_NEW_TREND_STATUS_NORMAL							0x00



#define NEW_TREND_PACKAGE_MAX									20


static const unsigned char rs485_set_new_trend_table[2][5] = {
{0x01, 0x01, 0x00, 0x00, 0x00},   	//ON
{0x01, 0x00, 0x00, 0x00, 0x00}		//OFF
};



/**static function define */
static unsigned char calculate_sum_check(unsigned char* value, int length)
{
    unsigned char result = 0;
    int i;

    if((length == 0) && (value == NULL))
        return result;

    for(i=0; i<length; i++)
        result += *(value++);

    result = ~(result%256) + 1;

    return result;
}

#ifdef RS485_DEBUG
static void	printf_array(unsigned char array[], int len)
{
	int i;

	if(len > 0)
	{
		for(i=0; i<len; i++)
		{
			RS485_DEBUGF(DBG_RS485_NEW_TREND, ("0x%02x ", array[i]));
		}
	}

	RS485_DEBUGF(DBG_RS485_NEW_TREND, ("\n"));
}

static void printf_device_info(const struct rs485_new_trend_info* info)
{
	RS485_DEBUGF(DBG_RS485_NEW_TREND, ("new trend device info-------------:\n"));
	RS485_DEBUGF(DBG_RS485_NEW_TREND, ("temperature: %d.%d\n", info->temp_int, info->temp_point));
	RS485_DEBUGF(DBG_RS485_NEW_TREND, ("humidity: %d.%d\n", info->humi_int, info->humi_point));
	RS485_DEBUGF(DBG_RS485_NEW_TREND, ("PM2.5: %d\n", info->pm2_5_int));
	RS485_DEBUGF(DBG_RS485_NEW_TREND, ("run status: %d\n", info->run_status));
	RS485_DEBUGF(DBG_RS485_NEW_TREND, ("error status: %d\n", info->error));
	RS485_DEBUGF(DBG_RS485_NEW_TREND, ("new trend device info -----------end\n"));
}
#endif




#if 0
static int send_data_to_package(int port, unsigned char addr,
		unsigned char msg_cid, const unsigned char* data, int data_len)
{
    unsigned char send_buf[NEW_TREND_PACKAGE_MAX];
    int package_len = 0;

    send_buf[0] = SOI_SEND;
    send_buf[1] = addr;
    send_buf[2] = msg_cid;
    send_buf[3] = RTN_SEND;

    switch(msg_cid)
    {

    case RS485_NEW_TREND_GET_ADDR_CID:
    case RS485_NEW_TREND_GET_INFO_CID:
    case RS485_NEW_TREND_RESTART_CID:
        send_buf[4] = 0;
        package_len = 7;
        send_buf[5] = calculate_sum_check(send_buf+1, package_len-3);
        send_buf[6] = EOI;
        break;
    case RS485_NEW_TREND_SET_ADDR_CID:
    	send_buf[4] = 1;
    	package_len = 8;
    	send_buf[5] = data[0];
    	send_buf[6] = calculate_sum_check(send_buf+1, package_len-3);
    	send_buf[7] = EOI;
    	break;
    case RS485_NEW_TREND_SET_ARG_CID:
        send_buf[4] = 5;
        send_buf[5] = data[0];
        send_buf[6] = data[1];
        send_buf[7] = data[2];
        send_buf[8] = data[3];
        send_buf[9] = data[4];
        package_len = 12;
        send_buf[10] = calculate_sum_check(send_buf+1, package_len-3);
        send_buf[11] = EOI;
        break;
    default:
        return -1;
    }

    if(package_len != rs485_send(port, send_buf, package_len))
        return -1;


    printf_array(send_buf, package_len);


#ifdef NEW_TREND_DEBUG
    send_buf[0] = 0x55;
    send_buf[1] = addr;
    send_buf[2] = msg_cid;
    send_buf[3] = 0x01;
    if(msg_cid == RS485_NEW_TREND_SET_ADDR_CID ||
       msg_cid == RS485_NEW_TREND_GET_ADDR_CID)
    {
    	package_len = 8;
    	send_buf[4] = 0x01;
    	send_buf[5] = data[0];
    	send_buf[6] = calculate_sum_check(send_buf+1, package_len -3);
    	send_buf[7] = EOI;
    }
    else if(msg_cid == RS485_NEW_TREND_GET_INFO_CID)
    {
    	//mode=auto, status=open, temp=25, humi=50%, pm2.5=100, error = 0
    	package_len = 18;
    	send_buf[4] = 11;	       //len
    	send_buf[5] = 0x01;		   //auto
    	send_buf[6] = 0x01;		   //open
    	send_buf[7] = 25;		   //temp
    	send_buf[8] = 0;
    	send_buf[9] = 50;		   //humi
    	send_buf[10] = 0;
    	send_buf[11] = 00;
    	send_buf[12] = 100;		   //pm2.5
    	send_buf[13] = 0x00;	   //idle
    	send_buf[14] = 0x00;	   //error status
    	send_buf[15] = 0x00;	   //error level

    	send_buf[16] = calculate_sum_check(send_buf+1, package_len -3);
    	send_buf[17] = EOI;
    }
    else
    {
    	package_len = 7;
    	send_buf[4] = 0x00;
    	send_buf[5] = calculate_sum_check(send_buf+1, package_len -3);
    	send_buf[6] = EOI;

    }

    RS485_DEBUGF(DBG_RS485_NEW_TREND, ("new trend: except receive info:"));
    printf_array(send_buf, package_len);
#endif

    return 0;
}


static int process_new_trend_info(struct rs485_new_trend_info* info, unsigned char* buffer, int buffer_len)
{
//	switch(buffer[0])
//	{
//		case RS485_NEW_TREND_MODE_WAITING:
//			break;
//		case RS485_NEW_TREND_MODE_AUTOING:
//			break;
//		case RS485_NEW_TREND_MODE_OUT_CRC:
//			break;
//		case RS485_NEW_TREND_MODE_IN_CRC:
//			break;
//		case RS485_NEW_TREND_MODE_KILLING:
//			break;
//	}

	switch(buffer[1])
	{
		case RS485_NEW_TREND_RUN_STATUS_OFF:
			info->run_status = RS485_NEW_TREND_AUTO_OFF;
			break;
		case RS485_NEW_TREND_RUN_STATUS_ON:
			info->run_status = RS485_NEW_TREND_AUTO_ON;
			break;
	}

	info->temp_int 		= buffer[2];
	info->temp_point 	= buffer[3];
	info->humi_int 		= buffer[4];
	info->humi_point 	= buffer[5];

	info->pm2_5_int		= buffer[6] * 256 + buffer[7];
//	info->pm2_5_point	= buffer[7];

	switch(buffer[9])
	{
		case RS485_NEW_TREND_STATUS_ERROR:
			info->error = -1;
			break;
		case RS485_NEW_TREND_STATUS_NORMAL:
			info->error = 0;
			break;
	}

#ifdef RS485_DEBUG
	printf_device_info(info);
#endif

	return 0;
}


/**
 * @ingroup new_trend_interface
 * 	RS485 initial new trend device
 *
 * @return 0 is success, others is fail.
 */
int rs485_new_trend_init(void)
{
	return 0;
}


/**
 * @ingroup new_trend_interface
 * 	Send a command that get the new trend information
 *
 * @param[in] addr The address what you want to get the information
 *
 * @return 0 is success, others is fail.
 */
int rs485_get_new_trend_info(unsigned char addr)
{
	return  send_data_to_package(RS485_PORT, addr, RS485_NEW_TREND_GET_INFO_CID,
			NULL, 0);
}


/**
 * @ingroup new_trend_interface
 *	Process a new trend information into the rs485_new_trend_info struct.
 *
 * @param[out] info the new trend device information result.
 *
 * @return 0 is success, others is fail
 */
int rs485_process_new_trend_info(struct rs485_new_trend_info* info)
{
	unsigned char buffer[RS485_RECV_BUFFER_LENGTH];
	struct rs485_new_trend_info new_trend_info;

	if(14 == rs485_read(RS485_PORT, buffer, 14))
	{
		process_new_trend_info(&new_trend_info, buffer+1, 11);

		if(memcmp(&new_trend_info, info, sizeof(struct rs485_new_trend_info)) == 0)
		{
			new_trend_info.update_status = NEW_TREND_INFO_NO_CHANGE;
		}
		else
		{
			new_trend_info.update_status = NEW_TREND_INFO_HAVE_CHANGE;
		}

		memcpy(info, &new_trend_info, sizeof(struct rs485_new_trend_info));
		return 0;
	}

	return -1;
}


/**
 * @ingroup new_trend_interface
 * 	Open a RS485 new trend device
 *
 * @param[in] addr The address what you want to open the new trend device
 *
 * @return 0 is success, others is fail
 */
int rs485_new_trend_open(unsigned char addr)
{
	return  send_data_to_package(RS485_PORT, addr, RS485_NEW_TREND_SET_ARG_CID,
			rs485_set_new_trend_table[RS485_NEW_TREND_AUTO_ON - RS485_NEW_TREND_AUTO_ON],
			5);
}


/**
 * @ingroup new_trend_interface
 * 	Close a RS485 new trend device
 *
 * @param[in] addr The address what you want to close the new trend device
 *
 * @return 0 is success, others is fail
 */
int rs485_new_trend_close(unsigned char addr)
{
	return  send_data_to_package(RS485_PORT, addr, RS485_NEW_TREND_SET_ARG_CID,
			rs485_set_new_trend_table[RS485_NEW_TREND_AUTO_OFF - RS485_NEW_TREND_AUTO_ON],
			5);
}


/**
 * @ingroup new_trend_interface
 * 	Restart a RS485 new trend device
 *
 * @param[in] addr The address what you want to restart the new trend device
 *
 * @return 0 is success, others is fail
 */
int rs485_new_trend_restart(unsigned char addr)
{
	return  send_data_to_package(RS485_PORT, addr, RS485_NEW_TREND_RESTART_CID,
			NULL, 0);
}


/**
 * @ingroup new_trend_interface
 * 	Set a RS485 new trend device address
 *
 * @param[in] addr_old	The address what new trend default address or old address
 * @param[in] addr_new	The address what you want to set
 *
 * @return 0 is success, others is fail
 */
int rs485_new_trend_set_addr(unsigned char addr_old, unsigned char addr_new)
{
	return send_data_to_package(RS485_PORT, addr_old, RS485_NEW_TREND_SET_ADDR_CID,
			&addr_new, 1);
}


/**
 * @ingroup new_trend_interface
 * 	Get a RS485 new trend device address
 *
 * @note This function just to send a command ,so you should to receive a address.
 *
 * @param[in] addr The address what you want to expect
 *
 * @return 0 is success, others is fail
 */
int rs485_new_trend_get_addr(unsigned char addr)
{
	return send_data_to_package(RS485_PORT, addr, RS485_NEW_TREND_GET_ADDR_CID,
			&addr, 1);
}

#endif





static int loreley_send_package(unsigned char* send_buffer,
        int send_buffer_len,
        unsigned char addr,
		unsigned char msg_cid, const unsigned char* data)
{
    unsigned char send_buf[NEW_TREND_PACKAGE_MAX];
    int package_len = 0;

    send_buf[0] = SOI_SEND;
    send_buf[1] = addr;
    send_buf[2] = msg_cid;
    send_buf[3] = RTN_SEND;

    switch(msg_cid)
    {

        case RS485_NEW_TREND_GET_ADDR_CID:
        case RS485_NEW_TREND_GET_INFO_CID:
        case RS485_NEW_TREND_RESTART_CID:
            send_buf[4] = 0;
            package_len = 7;
            send_buf[5] = calculate_sum_check(send_buf+1, package_len-3);
            send_buf[6] = EOI;
            break;
        case RS485_NEW_TREND_SET_ADDR_CID:
            send_buf[4] = 1;
            package_len = 8;
            send_buf[5] = data[0];
            send_buf[6] = calculate_sum_check(send_buf+1, package_len-3);
            send_buf[7] = EOI;
            break;
        case RS485_NEW_TREND_SET_ARG_CID:
            send_buf[4] = 5;
            send_buf[5] = data[0];
            send_buf[6] = data[1];
            send_buf[7] = data[2];
            send_buf[8] = data[3];
            send_buf[9] = data[4];
            package_len = 12;
            send_buf[10] = calculate_sum_check(send_buf+1, package_len-3);
            send_buf[11] = EOI;
            break;
        default:
            return -1;
    }
    if(send_buffer_len < package_len)
        return -1;

    memcpy(send_buffer, send_buf, package_len);

    return package_len;
}

#if 0
int loreley_send_package_handle(unsigned char* send_buffer,
        unsigned int send_buffer_len,
        unsigned char* addr,
        int addr_len,
        bool broadcast,
        int command,
        int value)
{
    int nbyte = 0;

    if(send_buffer == NULL)
        return -1;

    if(addr_len != 1)
        return -1;

    switch(command)
    {
        case RS485_FRESH_AIR_AUTO_ON:
	        nbyte = loreley_send_package(send_buffer,
                    send_buffer_len,
                    *addr,
                    RS485_NEW_TREND_SET_ARG_CID,
			        rs485_set_new_trend_table[0]);
            break;

        case RS485_FRESH_AIR_AUTO_OFF:
	        nbyte = loreley_send_package(send_buffer,
                    send_buffer_len,
                    *addr,
                    RS485_NEW_TREND_SET_ARG_CID,
			        rs485_set_new_trend_table[1]);
            break;

        case RS485_FRESH_AIR_GET_DEVICE_INFO:
	        nbyte = loreley_send_package(send_buffer,
                    send_buffer_len,
                    *addr,
                    RS485_NEW_TREND_GET_INFO_CID,
			        NULL);
            break;

        default:
            syslog_warning("[loreley]:unknown device command:%d", command);
            return -1;
    }

    return nbyte;
}

#endif


int loreley_send_package_handle(volatile void* arg)
{
    int                             nbyte = 0;
    mstp_port_handle_t              *handle = (mstp_port_handle_t*)arg;

    if(handle == NULL)
        return -1;

    if(handle->package_buffer == NULL)
        return -1;

    if(handle->address_len != 1)
        return -1;

    switch(handle->method)
    {
        case RS485_FRESH_AIR_AUTO_ON:
	        nbyte = loreley_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    RS485_NEW_TREND_SET_ARG_CID,
			        rs485_set_new_trend_table[0]);
            break;

        case RS485_FRESH_AIR_AUTO_OFF:
	        nbyte = loreley_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    RS485_NEW_TREND_SET_ARG_CID,
			        rs485_set_new_trend_table[1]);
            break;

        case RS485_FRESH_AIR_GET_DEVICE_INFO:
	        nbyte = loreley_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    RS485_NEW_TREND_GET_INFO_CID,
			        NULL);
            break;

        default:
            syslog_warning("[loreley]:unknown device command:%d", handle->method);
            return -1;
    }

    return nbyte;
}


#if 0
int loreley_recv_package_handle(int device_id,
        unsigned char* recv_buffer,
        unsigned int recv_buffer_len,
        int command,
        void* UNUSED(arg))
{
    read_device_return_t    new_device_info;
    unsigned char           crc;

    if(recv_buffer == NULL)
    {
        return -EINVAL;
    }

    /* package head and tail check */
    if(recv_buffer[0] != SOI_RECEIVE || recv_buffer[recv_buffer_len -1] != EOI)
        return -EIO;

    /* package crc check */
    crc = calculate_sum_check(recv_buffer+1, recv_buffer_len-3);
    if(recv_buffer[recv_buffer_len -2] != crc)
    {
        syslog_warning("[loreley]:crc error:cal:0x%02x, recv:0x%02x", crc, recv_buffer[recv_buffer_len - 2]);
        return -EIO;

    }

    switch(command)
    {
        case RS485_FRESH_AIR_AUTO_OFF:
        case RS485_FRESH_AIR_AUTO_ON:
            break;

        case RS485_FRESH_AIR_GET_DEVICE_INFO:
            if(recv_buffer_len != 18)
                return -EIO;

            new_device_info.profile.fresh_air.room_temperature = recv_buffer[7];
            new_device_info.profile.fresh_air.room_humidity = recv_buffer[9];
            new_device_info.profile.fresh_air.pm2_5 = recv_buffer[11];
            new_device_info.profile.fresh_air.fresh_level = 1;

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

int loreley_recv_package_handle(volatile void* arg)
{
    mstp_port_handle_t      *handle = (mstp_port_handle_t*)arg;
    read_device_return_t    new_device_info;
    unsigned char           crc;

    if(handle == NULL)
    {
        return -EINVAL;
    }

    if(handle->package_buffer == NULL)
    {
        return -EINVAL;
    }

    /* package head and tail check */
    if(handle->package_buffer[0] != SOI_RECEIVE || handle->package_buffer[handle->package_buffer_len-1] != EOI)
    {
        syslog_warning("[loreley]: OI check fail, header:%02x, tail:%02x", handle->package_buffer[0], 
                handle->package_buffer[handle->package_buffer_len - 1]);
        return -EIO;
    }

    /* package crc check */
    crc = calculate_sum_check(handle->package_buffer+1, handle->package_buffer_len-3);
    if(handle->package_buffer[handle->package_buffer_len-2] != crc)
    {
        syslog_warning("[loreley]:crc error:cal:0x%02x, recv:0x%02x", crc, handle->package_buffer[handle->package_buffer_len- 2]);
        return -EIO;
    }

    switch(handle->method)
    {
        case RS485_FRESH_AIR_AUTO_OFF:
        case RS485_FRESH_AIR_AUTO_ON:
            break;

        case RS485_FRESH_AIR_GET_DEVICE_INFO:
            if(handle->package_buffer_len != 18)
            {
                syslog_debug("[loreley]: get device infor len:%d, should: 18", handle->package_buffer_len);
                return -EIO;
            }

            /*TODO: process the info , refman define doc*/
            memset(&new_device_info, 0x00, sizeof(new_device_info));

            new_device_info.profile.fresh_air.room_temperature = handle->package_buffer[7];
            new_device_info.profile.fresh_air.room_humidity = handle->package_buffer[9];
            new_device_info.profile.fresh_air.pm2_5 = handle->package_buffer[11];
            new_device_info.profile.fresh_air.fresh_level = 1;

            new_device_info.error = handle->package_buffer[14];
            new_device_info.runing = handle->package_buffer[15];
            set_read_device_information(&new_device_info, handle->device_id);
            break;

        default:
            return -EPERM;
    }

    return 0;
}


