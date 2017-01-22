/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  The rs485 client test file
 *
 *        Version:  1.0
 *        Created:  09/18/2016 09:24:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>


#include "enum.h"
#include "log.h"
#include "adapter.h"
#include "panel_s_dev_control.h"
#include "rs485_api.h"




static void main_menu(void)
{
	puts("--------------------------------------");
	puts("-------rs485 service test-------------");
	puts("-------a. create object  -------------");
	puts("-------b. delete object  -------------");
	puts("-------c. mount device to object -----");
	puts("-------d. unmount device from object -");
	puts("-------e. write vlaue to device ------");
	puts("-------f. read vlaue from device -----");
	puts("-------g. stop the rs485 service -----");
	puts("-------q. exit -----------------------");
	puts("--------------------------------------");
}

/* ------------------------------------------------------------------*/
/**
 * @brief  get input from terminal
 *
 * @returns  return the input
 */
/* ------------------------------------------------------------------*/
static char get_input(void)
{
    char ch;
    do
    {
        ch = getchar();
    }while(ch == '\n');

    return ch;
}


#ifdef CONFIG_RS485C_PROTOCOL_BACNET
static struct rs485_init rs485_device[] =
{
    /* device_type */ /* knx id */ /* device address */
    {Air_Conditioner,       71,         {0x11}},
    {Air_Conditioner,       78,         {0x13}},
    {Air_Conditioner,       85,         {0x14}},
};

static struct rs485_bus_init rs485_bus =
{
    .protocol = RS485_PROTOCOL_TYPE_BACNET,
    .interface.baud_rate = 9600,
    .interface.parity = RS485_PORT_TYPE_NONE,
};
#endif



#ifdef CONFIG_RS485C_PROTOCOL_GENERAL
static struct rs485_init rs485_device[] =
{
    /* device_type */ /* knx id */ /* device address */
    {RS485_Curtain,         1,         {0xfe}},
    {RS485_Curtain,         5,         {0xee}},
    {Fresh_Air,             8,         {0xcf}},
    {Air_Conditioner,       71,        {0x01}},
};

static struct rs485_bus_init rs485_bus =
{
    .protocol = RS485_PROTOCOL_TYPE_GENERAL,
    .interface.baud_rate = 9600,
    .interface.parity = RS485_PORT_TYPE_NONE,
};
#endif




#ifdef CONFIG_RS485C_PROTOCOL_MODBUS
static struct rs485_init rs485_device[] =
{
#ifdef CONFIG_RS485C_DEVICE_YORK_JOHNSON_CONTROLS_BOX
    /* device_type */ /* knx id */ /* device address */
    {Air_Conditioner,       71,         {0x12},  RS485_AIRCONDITION_MODBUS_YORK_JOHNSON_CONTROLS_BOX},
    {Air_Conditioner,       78,         {0x13},  RS485_AIRCONDITION_MODBUS_YORK_JOHNSON_CONTROLS_BOX},
    {Air_Conditioner,       85,         {0x14},  RS485_AIRCONDITION_MODBUS_YORK_JOHNSON_CONTROLS_BOX},
#endif

    {Air_Conditioner,       71,         {0x5},  RS485_AIRCONDITION_MODBUS_MISTSUBISHI_MAC_CCS_01M},
    {Air_Conditioner,       78,         {0x3},  RS485_AIRCONDITION_MODBUS_MISTSUBISHI_MAC_CCS_01M},
    {Air_Conditioner,       85,         {0x2},  RS485_AIRCONDITION_MODBUS_MISTSUBISHI_MAC_CCS_01M},
};

static struct rs485_bus_init rs485_bus =
{
    .protocol = RS485_PROTOCOL_TYPE_MODBUS,
    .interface.baud_rate = 9600,
    .interface.parity = RS485_PORT_TYPE_NONE,
};
#endif


static read_device_return_t     device_profile;


void fresh_air_test(void)
{
    rs485_get_device_info(&device_profile, 8);
    syslog_info("device runing:%d, error:%d ", device_profile.runing,
                device_profile.error);
    syslog_info("[client]: fresh level: %d", device_profile.profile.fresh_air.fresh_level);
    syslog_info("[client]: fresh pm2.5: %d", device_profile.profile.fresh_air.pm2_5);
    syslog_info("[client]: fresh humidity: %d", device_profile.profile.fresh_air.room_humidity);
    syslog_info("[client]: fresh temperature: %d", device_profile.profile.fresh_air.room_temperature);
                rs485_get_device_info(&device_profile, 71);
    syslog_info("device runing:%d, error:%d ", device_profile.runing,
                device_profile.error);
}



int main(int argc, char* argv[])
{
    int error = 0;

    error = rs485_device_init(rs485_device, sizeof(rs485_device)/sizeof(struct rs485_init),
            &rs485_bus);

    if(error)
    {
        syslog_error("%s", "rs485 client init fail");
        return error;
    }


    while(1)
    {
        main_menu();

        switch(get_input())
        {
            case 'a':
                rs485_device_init(rs485_device, sizeof(rs485_device)/sizeof(struct rs485_init),
                        &rs485_bus);
                break;

            case 'b':
                rs485_daemon_restart();
                break;

            case 'c':
                break;

            case 'd':
                break;

            case 'e':
                /* set air conditon mode */
                rs485_send_to_device(75, 28);
                rs485_send_to_device(71, 1);
                break;

            case 'f':
                break;

            case 'g':
                break;

            case 'q':
                exit(0);
                break;

            default:
                syslog_info("[client]: %s", "unknown command");
        }

    }


    return 0;
}

