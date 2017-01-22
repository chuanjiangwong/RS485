
/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  main entry
 *
 *        Version:  1.0
 *        Created:  Oct 11, 2016 9:25:20 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#include <errno.h>

#include <stdio.h>

#include <core.h>
#include <register.h>


static struct interface_profile ttys1 =
{
    .port_name = "/dev/ttyS1",
    .baud_rate = 9600,
    .data_bits = INTERFACE_DATA_BITS_8,
    .parity = INTERFACE_PARITY_NULL,
    .stop_bits = INTERFACE_STOP_BITS_1,
};

static struct interface_profile ttys2 =
{
    .port_name = "/dev/ttyS2",
    .baud_rate = 9600,
    .data_bits = INTERFACE_DATA_BITS_8,
    .parity = INTERFACE_PARITY_NULL,
    .stop_bits = INTERFACE_STOP_BITS_1,
};

static struct interface_profile ttys3 =
{
    .port_name = "/dev/ttyS3",
    .baud_rate = 9600,
    .data_bits = INTERFACE_DATA_BITS_8,
    .parity = INTERFACE_PARITY_NULL,
    .stop_bits = INTERFACE_STOP_BITS_1,
};
int main(int argc, char* argv[])
{
    bus_initcalls(&ttys1, "custom");
    bus_initcalls(&ttys2, "custom");
    bus_initcalls(&ttys3, "custom");
    driver_initcalls("custom-/dev/ttyS1");
    driver_initcalls("custom-/dev/ttyS2");
    driver_initcalls("custom-/dev/ttyS3");

    while(1)
    {
    	puts(" drvier init calls .....");
        sleep(5);
        driver_initcalls("custom-/dev/ttyS1");
        driver_initcalls("custom-/dev/ttyS2");
        driver_initcalls("custom-/dev/ttyS3");
    }

	return 0;
}
