/*
 * =====================================================================================
 *
 *          @file:  panasonnic.c
 *
 *    Description:   	This interface used in air network protocol,theprotocol based
 *                   on RS485 transmission, the module because it is a master-slave mode
 *                   communication, so will RS232 through shielded twisted pair cable into
 *                   RS485 signal.
 *
 *        Version:  1.0
 *        Created:  04/01/2016 04:56:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <error.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <errno.h>

#include "enum.h"
#include "adapter.h"
#include "protocol/general/general.h"
#include "syslog/log.h"



/**get air conditioner address*/
#define GET_AIR_ADDR_CID_SEND	                                0x01
#define GET_AIR_ADDR

/**get air conditioner information*/
#define GET_AIR_INFO_CID_SEND	                                0x02

#define GET_AIR_INFO_TELECONTROLLER_MODE_AUTO	                0x00
#define	GET_AIR_INFO_TELECONTROLLER_MODE_DRY	                0x02
#define GET_AIR_INFO_TELECONTROLLER_MODE_COLD	                0x03
#define GET_AIR_INFO_TELECONTROLLER_MODE_HOT	                0x04
#define GET_AIR_INFO_TELECONTROLLER_MODE_WIND	                0x06

#define GET_AIR_INFO_ON				                            0x01
#define GET_AIR_INFO_OFF			                            0x00

#define GET_AIR_INFO_SET_WIND_RATE_LOW		                    0x03
#define GET_AIR_INFO_SET_WIND_RATE_MIDDLE	                    0x05
#define GET_AIR_INFO_SET_WIND_RATE_HIGH		                    0x07
#define GET_AIR_INFO_SET_WIND_RATE_MOST		                    0x09
#define GET_AIR_INFO_SET_WIND_RATE_AUTO		                    0x0a
#define GET_AIR_INFO_SET_WIND_RATE_MUTE		                    0x0b

#define GET_AIR_INFO_SET_WIND_DIRECTION_HANDL1	                0x01
#define GET_AIR_INFO_SET_WIND_DIRECTION_HANDL2					0x02
#define GET_AIR_INFO_SET_WIND_DIRECTION_HANDL3					0x03
#define GET_AIR_INFO_SET_WIND_DIRECTION_HANDL4					0x04
#define GET_AIR_INFO_SET_WIND_DIRECTION_HANDL5					0x05
#define GET_AIR_INFO_SET_WIND_DIRECTION_FOCUS	                0x0e
#define GET_AIR_INFO_SET_WIND_DIRECTION_WIDE	                0x0f

#define GET_AIR_INFO_SET_TEMPERATURE(x)		                    (x)/2
#define GET_AIR_INFO_SET_TEMPERATURE_UP		                    0xe1
#define GET_AIR_INFO_SET_TEMPERATURE_STD	                    0xe0
#define GET_AIR_INFO_SET_TEMPERATURE_DOWN	                    0xff
#define GET_AIR_INFO_SET_TEMPERATURE_ADD_5	                    0xca
#define GET_AIR_INFO_SET_TEMPERATURE_ADD_1	                    0xc2
#define GET_AIR_INFO_SET_TEMPERATURE_DRY	                    0xc0
#define GET_AIR_INFO_SET_TEMPERATURE_SUB_1	                    0xde
#define GET_AIR_INFO_SET_TEMPERATURE_SUB_5	                    0xd6

#define GET_AIR_INFO_TEMPERATURE_UNDERFLOW	                    0x80
#define GET_AIR_INFO_TEMPERATURE_OVERFLOW	                    0x7f
#define GET_AIR_INFO_TEMPERATURE_UNKNOWN	                    0x7e

#define GET_AIR_INFO_ERROR_CODE_NORMAL		                    0x00

/**set air conditioner arguments*/
#define SET_AIR_ARG_CID_SEND			                        0x03

#define SET_AIR_ARG_CONFIG_OPT_ALL		                        0x00
#define SET_AIR_ARG_CONFIG_OPT_SWITCH		                    0x01
#define SET_AIR_ARG_CONFIG_OPT_MODE		                        0x02
#define SET_AIR_ARG_CONFIG_OPT_WIND_DIRECTION	                0x03
#define SET_AIR_ARG_CONFIG_OPT_WIND_RATE	                    0x04
#define SET_AIR_ARG_CONFIG_OPT_TEMPERATURE	                    0x05
#define SET_AIR_ARG_CONFIG_OPT_KEEP		                        0xff

#define SET_AIR_ARG_ON				                            0x00
#define SET_AIR_ARG_OFF				                            0x01

#define SET_AIR_ARG_TELECONTROLLER_MODE_AUTO	                0x00
#define SET_AIR_ARG_TELECONTROLLER_MODE_DRY	                    0x02
#define SET_AIR_ARG_TELECONTROLLER_MODE_COLD	                0x03
#define SET_AIR_ARG_TELECONTROLLER_MODE_HOT	                    0x04
#define SET_AIR_ARG_TELECONTROLLER_MODE_WIND	                0x06

#define SET_AIR_ARG_WIND_RATE_LOW                               0x03
#define SET_AIR_ARG_WIND_RATE_MIDDLE                            0x05
#define SET_AIR_ARG_WIND_RATE_HIGH                              0x07
#define SET_AIR_ARG_WIND_RATE_MOST                              0x09
#define SET_AIR_ARG_WIND_RATE_AUTO                              0x0a
#define SET_AIR_ARG_WIND_RATE_MUTE                              0x0b

#define SET_AIR_ARG_WIND_DIRECTION_HANDL	                    (0x01 | 0x02 | 0x03 | 0x04 | 0x05)
#define SET_AIR_ARG_WIND_DIRECTION_FOCUS	                    0x0e
#define SET_AIR_ARG_WIND_DIRECTION_WIDE	                        0x0f

#define SET_AIR_ARG_TEMPERATURE(x)                              temperature_to_bin(x)

/**when the air conditioner unusual, will reset it */
#define RESET_AIR_CID_SEND                                      0x04

/**package protocol*/
#define SOI_SEND                                                0xaa
#define SOI_RECEIVE                                             0x55
#define ADR_BROADCAST                                           0xff
#define ADR_DEFAULT                                             0x01
#define RTN_SEND                                                0x60
#define RTN_RECEIVE_CMD_RIGHT                                   0x01
#define RTN_RECEIVE_CHK_ERROR                                   0x02
#define RTN_RECEIVE_CMD_INVALID                                 0x03
#define EOI                                                     0x0d


/* error type*/
#define SEND_ERROR                                           	-2
#define RECEIVE_ERROR                                        	-3
#define RECEIVE_CHK_ERROR                                    	-4
#define RETURN_DATA_INVALID_ERROR                            	-5
#define RETURN_CHK_ERROR                                     	-6
#define ARG_ERROR                                            	-7
#define UNKNOW_ERROR											-8

/**define air conditioner transmit frame, it's up RS485
struct Package_air_transmit
{
                  -----------
    unsigned char |soi;     |
                  -----------
    unsigned char |addr;    |
                  -----------
    unsigned char |cid;     |
                  -----------
    unsigned char |rtn;     |
                  -----------
    unsigned char |length;  |
                  -----------
           length |data;    |
                  -----------
    unsigned char |sum_chk; |
                  -----------
    unsigned char |eoi;     |
                  -----------
};
*/



#ifndef ERROR
#define ERROR 										-1
#endif

#define PACKAGE_MAX                                 20

#define HIGH_CHAR(x)								(x)>>4
#define LOW_CHAR(x)									(x) & 0xff

/**static function declare */
static unsigned char        calculate_sum_check     (unsigned char* value, int length);
#if 0
static int                  send_data_to_package    (int port, unsigned char addr, unsigned char  msg_cid,
                                                     const unsigned char* data, int data_len);
static char inline 			to_temperature			(unsigned char value);
static inline int           get_rtn_right           (unsigned char rtn);
#endif
/* panansonic air conditioner comammd
AA 01 03 60 05 05 00 00 3C 00 56 0D		30
AA 01 03 60 05 05 00 00 3A 00 58 0D		29
AA 01 03 60 05 05 00 00 38 00 5A 0D		28
AA 01 03 60 05 05 00 00 36 00 5C 0D		27
AA 01 03 60 05 05 00 00 34 00 5E 0D		26
AA 01 03 60 05 05 00 00 32 00 60 0D		25
AA 01 03 60 05 05 00 00 30 00 62 0D		24
AA 01 03 60 05 05 00 00 2E 00 64 0D		23
AA 01 03 60 05 05 00 00 2C 00 66 0D		22
AA 01 03 60 05 05 00 00 2A 00 68 0D		21
AA 01 03 60 05 05 00 00 28 00 6A 0D		20
AA 01 03 60 05 05 00 00 26 00 6C 0D		19
AA 01 03 60 05 05 00 00 24 00 6E 0D		18
AA 01 03 60 05 05 00 00 22 00 70 0D		27
AA 01 03 60 05 05 00 00 20 00 72 0D		16
AA 01 03 60 05 04 00 0E 00 00 85 0D		自动
AA 01 03 60 05 04 00 05 00 00 8E 0D		手动5
AA 01 03 60 05 04 00 04 00 00 8F 0D		手动4
AA 01 03 60 05 04 00 03 00 00 90 0D		手动3
AA 01 03 60 05 04 00 02 00 00 91 0D		手动2
AA 01 03 60 05 04 00 01 00 00 92 0D		手动1
AA 01 03 60 05 03 00 A0 00 00 F4 0D		自动
AA 01 03 60 05 03 00 70 00 00 24 0D		高
AA 01 03 60 05 03 00 50 00 00 44 0D		中
AA 01 03 60 05 03 00 30 00 00 64 0D		低
AA 01 03 60 05 02 60 00 00 00 35 0D		送风
AA 01 03 60 05 02 40 00 00 00 55 0D		制暖
AA 01 03 60 05 02 30 00 00 00 65 0D		制冷
AA 01 03 60 05 02 20 00 00 00 75 0D		抽湿
AA 01 03 60 05 02 00 00 00 00 95 0D		自动
AA 01 03 60 05 01 00 00 00 00 96 0D		关机
AA 01 03 60 05 01 01 00 00 00 95 0D    开机
 */

const unsigned char air_command_table[34][5] = {
{0x05, 0x00, 0x00, 0x20, 0x00},					//16     0
{0x05, 0x00, 0x00, 0x22, 0x00},					//17     1
{0x05, 0x00, 0x00, 0x24, 0x00},					//18     2
{0x05, 0x00, 0x00, 0x26, 0x00},					//19     3
{0x05, 0x00, 0x00, 0x28, 0x00},					//20     4
{0x05, 0x00, 0x00, 0x2A, 0x00},					//21     5
{0x05, 0x00, 0x00, 0x2C, 0x00},					//22     6
{0x05, 0x00, 0x00, 0x2E, 0x00},					//23     7
{0x05, 0x00, 0x00, 0x30, 0x00},					//24     8
{0x05, 0x00, 0x00, 0x32, 0x00},					//25     9
{0x05, 0x00, 0x00, 0x34, 0x00},					//26     10
{0x05, 0x00, 0x00, 0x36, 0x00},					//27     11
{0x05, 0x00, 0x00, 0x38, 0x00},					//28     12
{0x05, 0x00, 0x00, 0x3A, 0x00},					//29     13
{0x05, 0x00, 0x00, 0x3C, 0x00},					//30     14

{0x04, 0x00, 0x0E, 0x00, 0x00},					//auto      15
{0x04, 0x00, 0x05, 0x00, 0x00},					//hand5     16
{0x04, 0x00, 0x04, 0x00, 0x00},					//hand4     17
{0x04, 0x00, 0x03, 0x00, 0x00},					//hand3     18
{0x04, 0x00, 0x02, 0x00, 0x00},					//hand2     19
{0x04, 0x00, 0x01, 0x00, 0x00},					//hand1     20

{0x03, 0x00, 0xA0, 0x00, 0x00},					//auto      21
{0x03, 0x00, 0x70, 0x00, 0x00},					//high      22
{0x03, 0x00, 0x50, 0x00, 0x00},					//middle    23
{0x03, 0x00, 0x30, 0x00, 0x00},					//low       24
{0x03, 0x00, 0x90, 0x00, 0x00},					//wind rate most     25
{0x03, 0x00, 0xB0, 0x00, 0x00},					//wind rate mute     26

{0x02, 0x60, 0x00, 0x00, 0x00},					//sending     27
{0x02, 0x40, 0x00, 0x00, 0x00},					//heating     28
{0x02, 0x30, 0x00, 0x00, 0x00},					//colding     29
{0x02, 0x20, 0x00, 0x00, 0x00},					//drying      30
{0x02, 0x00, 0x00, 0x00, 0x00},					//autoing     31

{0x01, 0x00, 0x00, 0x00, 0x00},					//off     32
{0x01, 0x01, 0x00, 0x00, 0x00}					//on      33
};

/**static function define */
#ifdef RTN
static inline int get_rtn_right(unsigned char rtn)
{
    switch(rtn)
    {
    case RTN_RECEIVE_CHK_ERROR:
        return RETURN_CHK_ERROR;
        break;
    case RTN_RECEIVE_CMD_INVALID:
        return RETURN_DATA_INVALID_ERROR;
        break;
    case RTN_RECEIVE_CMD_RIGHT:
        return 0;
        break;
    default:
        return UNKNOW_ERROR;
    }
}
#endif
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
			RS485_DEBUGF(DBG_RS485_AIR_CONDITION, ("0x%02x ", array[i]));
		}
	}

	RS485_DEBUGF(DBG_RS485_AIR_CONDITION, ("\n"));unsigned char* recv_buffer, unsigned int recv_buffer_len, void* arg
}

static void printf_air_info(const struct rs485_air_info* info)
{
	RS485_DEBUGF(DBG_RS485_AIR_CONDITION, ("|   mode:%02d  |      run:%02d      |\n", info->mode, info->run));
	RS485_DEBUGF(DBG_RS485_AIR_CONDITION, ("|   rate:%02d  |      direction:%02d|\n", info->wind_rate, info->wind_dir));
	RS485_DEBUGF(DBG_RS485_AIR_CONDITION, ("|   set  temperature:%d             |\n", info->set_temp));
	RS485_DEBUGF(DBG_RS485_AIR_CONDITION, ("|   room temperature int:%d         |\n", info->room_temp_int));
	RS485_DEBUGF(DBG_RS485_AIR_CONDITION, ("|   room temperature point:%d       |\n", info->room_temp_point));
	RS485_DEBUGF(DBG_RS485_AIR_CONDITION, ("|   pipe temperature:%d             |\n", info->pipe_temp));
	RS485_DEBUGF(DBG_RS485_AIR_CONDITION, ("|   out  temperature:%d             |\n", info->out_door_temp));
	RS485_DEBUGF(DBG_RS485_AIR_CONDITION, ("|   error status:%d                 |\n", info->error));
}
#endif




#if 0
int get_data_from_package(int port, unsigned char* buffer, int* len)
{
    unsigned char   receive_buf[PACKAGE_MAX];
    int             result = 0, package_len = 0;
    int				repair = 1000;unsigned char* recv_buffer, unsigned int recv_buffer_len, void* arg

// it is repair
    while(repair--)
    {
    	if(rs485_read(port, receive_buf, 1))
    	{
    		if(receive_buf[0] == SOI_RECEIVE)
    		{
    			break;
    		}
    	}
    }

     result = rs485_read(port, receive_buf+1, 4);
     if(result < 0)
        return RECEIVE_ERROR;

	switch (receive_buf[2])
	{
		case GET_AIR_ADDR_CID_SEND:
		result = get_rtn_right(receive_buf[3]);
		if (result)
			return result;

		if (receive_buf[4] != 1)                               //���ݵĳ���Ϊ1��
			return UNKNOW_ERROR;

		if(3 != rs485_read(port, receive_buf+5, 3))
			return UNKNOW_ERROR;

		buffer[0] = receive_buf[5];                                    //д����
		package_len = 8;
		if (receive_buf[package_len - 2]
				!= calculate_sum_check(receive_buf + 1, package_len - 3))
			return RECEIVE_CHK_ERROR;

		*len = 1;
		break;

	case GET_AIR_INFO_CID_SEND:
		result = get_rtn_right(receive_buf[3]);
		if (result)
			return result;

		if (receive_buf[4] != 11)                                //���ݵĳ���Ϊ11��
			return UNKNOW_ERROR;

		if(13 != rs485_read(port, receive_buf+5, 13))
			return UNKNOW_ERROR;

		memcpy(buffer, receive_buf + 5, 11);                             //д����
		package_len = 18;
		if (receive_buf[package_len - 2]unsigned char* recv_buffer, unsigned int recv_buffer_len, void* arg
				!= calculate_sum_check(receive_buf + 1, package_len - 3))
			return RECEIVE_CHK_ERROR;

		*len = 11;
		break;

	case SET_AIR_ARG_CID_SEND:
	case RESET_AIR_CID_SEND:
		result = get_rtn_right(receive_buf[3]);
		if (result)
			return result;

		if (receive_buf[4] != 0)
			return UNKNOW_ERROR;

		if (2 != rs485_read(port, receive_buf + 5, 2))
			return RECEIVE_ERROR;

		package_len = 7;
		if (receive_buf[package_len - 2]
				!= calculate_sum_check(receive_buf + 1, package_len - 3))
			return RECEIVE_CHK_ERROR;

		*len = 0;
		break;

	default:
		return UNKNOW_ERROR;
	}#if 0

#ifdef RS485_DEBUG
    printf_array(receive_buf, package_len);
#endif
    return 0;
}

static int send_data_to_package(int port, unsigned char addr,
		unsigned char msg_cid, const unsigned char* data, int data_len)
{
    unsigned char send_buf[PACKAGE_MAX];
    int package_len = 0;

    if(data_len + 7 >= PACKAGE_MAX)
        return ARG_ERROR;

    memset(send_buf, 0, PACKAGE_MAX);
    send_buf[0] = SOI_SEND;
    send_buf[1] = addr;
    send_buf[2] = msg_cid;
    send_buf[3] = RTN_SEND;

    switch(msg_cid)
    {
    case GET_AIR_ADDR_CID_SEND:
    case RESET_AIR_CID_SEND:
    case GET_AIR_INFO_CID_SEND:
        send_buf[4] = 0;
        package_len = 7;
        send_buf[5] = calculate_sum_check(send_buf+1, package_len-3);
        send_buf[6] = EOI;
        break;
    case SET_AIR_ARG_CID_SEND:
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
        return ARG_ERROR;
    }

    if(package_len != rs485_send(port, send_buf, package_len))
        return SEND_ERROR;

#ifdef RS485_DEBUG
    printf_array(send_buf, package_len);
#endif
    return 0;
}

static char inline to_temperature(unsigned char value)
{
	int result = 0;

	if(value >= 0x81)
		result = (value - 0x81) - 127;
	else if(value <= 0x7d)
		result = value;
	else
		result = 0;

	return result;
}



/**
 * @ingroup air_conditioner_panansonic
 * 	Get the panansonic air conditioner signal address
 *
 * @param[in] port The RS485 port.
 * @param[in] addr The address what you want to expect.
 *
 * @return 0 is success, others is fail.
 */
int rs485_air_conditioner_panansonic_get_address(int port, unsigned char* addr)
{
    return send_data_to_package(port, *addr, GET_AIR_ADDR_CID_SEND, NULL, 0);
}


/**
 * @ingroup air_conditioner_panansonic
 * 	Get the panansonic air conditioner device information
 *
 * @param[in] port The RS485 port.
 * @param[in] addr The address what you want to get information
 *
 * @return 0 is success, others is fail.
 */
int rs485_air_conditioner_panansonic_get_info(int port, unsigned char addr)
{
    return send_data_to_package(port, addr, GET_AIR_INFO_CID_SEND, NULL, 0);
}


/**
 * @ingroup air_conditioner_panansonic
 * 	Control the panansonic air conditioner used to a command
 *
 * @param[in] port The RS485 port.
 * @param[in] arg  The air conditioner arg.
 * @param[in] addr The address what you want to set
 *
 * @return 0 is success, others is fail.
 */
int rs485_air_conditioner_panansonic_set_arg(int port, unsigned char* arg, unsigned char addr)
{
     return send_data_to_package(port, addr, SET_AIR_ARG_CID_SEND, arg, 5);
}


/**
 * @ingroup air_conditioner_panansonic
 * 	Restart the panansonic air conditioner
 *
 * @param[in] port The RS485 port.
 * @param[in] addr The address what you want to restart.
 *
 * @return 0 is success, others is fail.
 */
int rs485_air_conditioner_panansonic_reset(int port, unsigned char addr)
{
   return send_data_to_package(port, addr, RESET_AIR_CID_SEND, NULL, 0);
}


/**
 * @ingroup air_conditioner_panansonic
 * 	process the air conditioner information that receive
 *
 * @param[in] data 	The information package have receive.
 * @param[in] len  	The information package have receive length.
 * @param[out] info process the information into the rs485_air_info struct.
 *
 * @return 0 is success, others is fail.
 */
int rs485_air_conditioner_panansonic_process_info(const unsigned char* data, int len, struct rs485_air_info* info)
{
	if(data == NULL)
		return -1;

	if(len == 11)
	{
		switch(HIGH_CHAR(data[0]))
		{
			case GET_AIR_INFO_TELECONTROLLER_MODE_AUTO:
				info->mode = RS485_AIR_PANANSONIC_AUTOING;
				break;
			case GET_AIR_INFO_TELECONTROLLER_MODE_DRY:
				info->mode = RS485_AIR_PANANSONIC_DRYING;
				break;
			case GET_AIR_INFO_TELECONTROLLER_MODE_COLD:
				info->mode = RS485_AIR_PANANSONIC_COLDING;
				break;
			case GET_AIR_INFO_TELECONTROLLER_MODE_HOT:
				info->mode = RS485_AIR_PANANSONIC_HEATING;
				break;
			case GET_AIR_INFO_TELECONTROLLER_MODE_WIND:
				info->mode = RS485_AIR_PANANSONIC_SENDING;
				break;
			default:
				info->mode = RS485_AIR_PANANSONIC_AUTOING;
		}

		switch(LOW_CHAR(data[0]))
		{
			case GET_AIR_INFO_ON:
				info->run = RS485_AIR_PANANSONIC_ON;
				break;
			case GET_AIR_INFO_OFF:
				info->run = RS485_AIR_PANANSONIC_OFF;
				break;
			default:
				info->run = RS485_AIR_PANANSONIC_OFF;
		}

		switch(HIGH_CHAR(data[1]))
		{
			case GET_AIR_INFO_SET_WIND_RATE_LOW:
				info->wind_rate = RS485_AIR_PANANSONIC_LOW;
				break;
			case GET_AIR_INFO_SET_WIND_RATE_MIDDLE:
				info->wind_rate = RS485_AIR_PANANSONIC_MIDDLE;
				break;
			case GET_AIR_INFO_SET_WIND_RATE_HIGH:
				info->wind_rate = RS485_AIR_PANANSONIC_HIGH;
				break;
			case GET_AIR_INFO_SET_WIND_RATE_MOST:
				info->wind_rate = RS485_AIR_PANANSONIC_WIND_RATE_MOST;
				break;
			case GET_AIR_INFO_SET_WIND_RATE_AUTO:
				info->wind_rate = RS485_AIR_PANANSONIC_WIND_AUTO;
				break;
			case GET_AIR_INFO_SET_WIND_RATE_MUTE:
				info->wind_rate = RS485_AIR_PANANSONIC_WIND_RATE_MUTE;
				break;
			default:
				info->wind_rate = RS485_AIR_PANANSONIC_WIND_AUTO;
		}

		switch(LOW_CHAR(data[1]))
		{
			case GET_AIR_INFO_SET_WIND_DIRECTION_HANDL1:
				info->wind_dir = RS485_AIR_PANANSONIC_HAND1;
				break;
			case GET_AIR_INFO_SET_WIND_DIRECTION_HANDL2:
				info->wind_dir = RS485_AIR_PANANSONIC_HAND2;
				break;
			case GET_AIR_INFO_SET_WIND_DIRECTION_HANDL3:
				info->wind_dir = RS485_AIR_PANANSONIC_HAND3;
				break;
			case GET_AIR_INFO_SET_WIND_DIRECTION_HANDL4:
				info->wind_dir = RS485_AIR_PANANSONIC_HAND4;
				break;
			case GET_AIR_INFO_SET_WIND_DIRECTION_HANDL5:
				info->wind_dir = RS485_AIR_PANANSONIC_HAND5;
				break;
			case GET_AIR_INFO_SET_WIND_DIRECTION_FOCUS:
				info->wind_dir = RS485_AIR_PANANSONIC_DIRECTION_AUTO;
				break;
			case GET_AIR_INFO_SET_WIND_DIRECTION_WIDE:
				info->wind_dir = RS485_AIR_PANANSONIC_DIRECTION_AUTO;
				break;
			default:
				info->wind_dir = RS485_AIR_PANANSONIC_DIRECTION_AUTO;
		}

		info->set_temp 		  = to_temperature(data[2])/2;
		info->room_temp_point = (int)data[3] * 128;
		info->room_temp_int	  = to_temperature(data[4]);
		info->pipe_temp		  = to_temperature(data[5]);
		info->out_door_temp   = to_temperature(data[6]);

		if(data[9] == GET_AIR_INFO_ERROR_CODE_NORMAL)
			info->error = 0;
		else
			info->error = -1;
	}
	RS485_DEBUGF(DBG_RS485_AIR_CONDITION, ("air:process_air_condition_info():\n"));
#ifdef RS485_DEBUG
    printf_air_info(info);
#endif

	return 0;
}

#endif







static int panasonnic_send_package(unsigned char* out,
        int out_len,
        unsigned char addr,
		unsigned char msg_cid,
        const unsigned char* data,
        int data_len)
{
    unsigned char send_buf[PACKAGE_MAX];
    int package_len = 0;

    if(data_len + 7 >= PACKAGE_MAX)
        return ARG_ERROR;

    memset(send_buf, 0, PACKAGE_MAX);
    send_buf[0] = SOI_SEND;
    send_buf[1] = addr;
    send_buf[2] = msg_cid;
    send_buf[3] = RTN_SEND;

    switch(msg_cid)
    {
    case GET_AIR_ADDR_CID_SEND:
    case RESET_AIR_CID_SEND:
    case GET_AIR_INFO_CID_SEND:
        send_buf[4] = 0;
        package_len = 7;
        send_buf[5] = calculate_sum_check(send_buf+1, package_len-3);
        send_buf[6] = EOI;
        break;
    case SET_AIR_ARG_CID_SEND:
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
        return ARG_ERROR;
    }

    if(out_len < package_len)
        return -EPERM;

    memcpy(out, send_buf, package_len);

    return package_len;
}


#if 0

int panasonnic_send_package_handle(unsigned char* send_buffer,
        unsigned int send_buffer_len,
        unsigned char* addr,
        int addr_len,
        bool broadcast,
        int command,
        int UNUSED(value))
{
    int             package_len = 0;

    if(send_buffer == NULL || addr == NULL)
        return -EPERM;

    if(addr_len != 1)
        return -EPERM;

    switch(command)
    {
        case RS485_PANASONNIC_AIR_SET_TEMP_16:
        case RS485_PANASONNIC_AIR_SET_TEMP_17:
        case RS485_PANASONNIC_AIR_SET_TEMP_18:
        case RS485_PANASONNIC_AIR_SET_TEMP_19:
        case RS485_PANASONNIC_AIR_SET_TEMP_20:
        case RS485_PANASONNIC_AIR_SET_TEMP_21:
        case RS485_PANASONNIC_AIR_SET_TEMP_22:
        case RS485_PANASONNIC_AIR_SET_TEMP_23:
        case RS485_PANASONNIC_AIR_SET_TEMP_24:
        case RS485_PANASONNIC_AIR_SET_TEMP_25:
        case RS485_PANASONNIC_AIR_SET_TEMP_26:
        case RS485_PANASONNIC_AIR_SET_TEMP_27:
        case RS485_PANASONNIC_AIR_SET_TEMP_28:
        case RS485_PANASONNIC_AIR_SET_TEMP_29:
        case RS485_PANASONNIC_AIR_SET_TEMP_30:

        case RS485_PANASONNIC_AIR_SWING_AUTO:
        case RS485_PANASONNIC_AIR_SWING_HAND5:
        case RS485_PANASONNIC_AIR_SWING_HAND4:
        case RS485_PANASONNIC_AIR_SWING_HAND3:
        case RS485_PANASONNIC_AIR_SWING_HAND2:
        case RS485_PANASONNIC_AIR_SWING_HAND1:

        case RS485_PANASONNIC_AIR_FAN_AUTO:
        case RS485_PANASONNIC_AIR_FAN_HIGH:
        case RS485_PANASONNIC_AIR_FAN_MIDDLE:
        case RS485_PANASONNIC_AIR_FAN_LOW:
        case RS485_PANASONNIC_AIR_FAN_MOST:
        case RS485_PANASONNIC_AIR_FAN_MUTE:

        case RS485_PANASONNIC_AIR_MODE_FANING:
        case RS485_PANASONNIC_AIR_MODE_HEATING:
        case RS485_PANASONNIC_AIR_MODE_COOLING:
        case RS485_PANASONNIC_AIR_MODE_DRYING:
        case RS485_PANASONNIC_AIR_MODE_AUTOING:

        case RS485_PANASONNIC_AIR_OFF:
        case RS485_PANASONNIC_AIR_ON:
            package_len = panasonnic_send_package(send_buffer,
                    send_buffer_len,
                    addr[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[command - RS485_PANASONNIC_AIR_SET_TEMP_16],
                    5);
            break;

        case RS485_PANASONNIC_AIR_RESET:
            package_len = panasonnic_send_package(send_buffer,
                    send_buffer_len,
                    addr[0],
                    RESET_AIR_CID_SEND,
                    NULL,
                    0);
            break;

        case RS485_PANASONNIC_AIR_GET_DEVICE_INFO:
            package_len = panasonnic_send_package(send_buffer,
                    send_buffer_len,
                    addr[0],
                    GET_AIR_INFO_CID_SEND,
                    NULL,
                    0);
            break;
        default:
            break;
    }

    return package_len;
}

#endif

int panasonnic_send_package_handle(volatile void* arg)
{
    mstp_port_handle_t      *handle = (mstp_port_handle_t*)arg;
    int                     package_len = 0;

    if(handle == NULL)
        return -EPERM;

    if(handle->package_buffer == NULL)
        return -EPERM;

    if(handle->address_len != 1)
        return -EPERM;

    switch(handle->method)
    {
#ifdef PANASONNIC_EXTEND
        case RS485_AIR_SET_TEMP_16:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[0],
                    5);
            break;
        case RS485_AIR_SET_TEMP_17:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[1],
                    5);
            break;
#endif
        case RS485_AIR_SET_TEMP_18:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[2],
                    5);
            break;
        case RS485_AIR_SET_TEMP_19:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[3],
                    5);
            break;
        case RS485_AIR_SET_TEMP_20:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[4],
                    5);
            break;
        case RS485_AIR_SET_TEMP_21:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[5],
                    5);
            break;
        case RS485_AIR_SET_TEMP_22:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[6],
                    5);
            break;
        case RS485_AIR_SET_TEMP_23:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[7],
                    5);
            break;
        case RS485_AIR_SET_TEMP_24:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[8],
                    5);
            break;
        case RS485_AIR_SET_TEMP_25:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[9],
                    5);
            break;
        case RS485_AIR_SET_TEMP_26:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[10],
                    5);
            break;
        case RS485_AIR_SET_TEMP_27:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[11],
                    5);
            break;
        case RS485_AIR_SET_TEMP_28:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[12],
                    5);
            break;
        case RS485_AIR_SET_TEMP_29:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[13],
                    5);
            break;
        case RS485_AIR_SET_TEMP_30:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[14],
                    5);
            break;
        case RS485_AIR_SWING_AUTO:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[15],
                    5);
            break;
        case RS485_AIR_SWING_LEFT_RIGHT:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[16],
                    5);
            break;
        case RS485_AIR_SWING_UP_DOWN:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[17],
                    5);
            break;
        case RS485_AIR_SWING_UP_DOWN_LEFT_RIGHT:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[18],
                    5);
            break;
#ifdef PANASONNIC_EXTEND
        case RS485_AIR_SWING_HAND2:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[19],
                    5);
            break;
        case RS485_AIR_SWING_HAND1:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[20],
                    5);
            break;
#endif
        case RS485_AIR_FAN_AUTO:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[21],
                    5);
            break;

        case RS485_AIR_FAN_HIGH:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[22],
                    5);
            break;
        case RS485_AIR_FAN_MIDDLE:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[23],
                    5);
            break;
        case RS485_AIR_FAN_LOW:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[24],
                    5);
            break;
#ifdef PANSONNIC_EXTEND
        case RS485_AIR_FAN_MOST:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[25],
                    5);
            break;
        case RS485_AIR_FAN_MUTE:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[26],
                    5);
            break;
#endif

        case RS485_AIR_MODE_FANING:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[27],
                    5);
            break;
        case RS485_AIR_MODE_HEATING:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[28],
                    5);
            break;
        case RS485_AIR_MODE_COOLING:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[29],
                    5);
            break;
        case RS485_AIR_MODE_DRYING:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[30],
                    5);
            break;
        case RS485_AIR_MODE_AUTOING:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[31],
                    5);
            break;

        case RS485_AIR_OFF:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[32],
                    5);
            break;
        case RS485_AIR_ON:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    SET_AIR_ARG_CID_SEND,
                    air_command_table[33],
                    5);
            break;

#ifdef PANASONNIC_EXTEND
        case RS485_AIR_RESET:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    RESET_AIR_CID_SEND,
                    NULL,
                    0);
            break;
#endif

        case RS485_AIR_GET_DEVICE_INFO:
            package_len = panasonnic_send_package(handle->package_buffer,
                    handle->package_buffer_len,
                    handle->address[0],
                    GET_AIR_INFO_CID_SEND,
                    NULL,
                    0);
            break;
        default:
            break;
    }

    return package_len;
}


int panasonnic_recv_package_handle(volatile void* arg)
{
    mstp_port_handle_t      *handle = (mstp_port_handle_t*)arg;

    if(handle == NULL)
        return -1;

    return 0;
}
