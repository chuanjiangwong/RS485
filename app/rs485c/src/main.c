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


int main(int argc, char* argv[])
{
    while(1)
    {
        main_menu();

        switch(get_input())
        {
            case 'a':
                break;

            case 'b':
                break;

            case 'c':
                break;

            case 'd':
                break;

            case 'e':
                break;

            case 'f':
                break;

            case 'g':
                break;

            case 'q':
                break;

            default:
                break;
        }

    }


    return 0;
}

