/*
 * =====================================================================================
 *
 *       Filename:  interface.c
 *
 *    Description:  interface
 *
 *        Version:  1.0
 *        Created:  Oct 13, 2016 11:51:45 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Linux includes */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <linux/serial.h>       /* for struct serial_struct */
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>

/* Local includes */
#include <wlog.h>
#include "core.h"

#define wlog_e(...)         wlog_error("cus:if", ##__VA_ARGS__)
#define wlog_d(...)         wlog_debug("cus:if", ##__VA_ARGS__)
#define wlog_i(...)         wlog_info("cus:if", ##__VA_ARGS__)
#define wlog_w(...)         wlog_warning("cus:if", ##__VA_ARGS__)


#ifndef RS485MOD
#define RS485MOD 0
#endif


/* serial I/O settings */
static struct termios RS485_oldtio;
/* for setting custom divisor */
static struct serial_struct RS485_oldserial;



static unsigned int get_baud_rate(uint32_t baud)
{
	static unsigned int RS485_Baud = B9600;

	switch (baud)
	{
		case 0:
			RS485_Baud = B0;
			break;
		case 50:
			RS485_Baud = B50;
			break;
		case 75:
			RS485_Baud = B75;
			break;
		case 110:
			RS485_Baud = B110;
			break;
		case 134:
			RS485_Baud = B134;
			break;
		case 150:
			RS485_Baud = B150;
			break;
		case 200:
			RS485_Baud = B200;
			break;
		case 300:
			RS485_Baud = B300;
			break;
		case 600:
			RS485_Baud = B600;
			break;
		case 1200:
			RS485_Baud = B1200;
			break;
		case 1800:
			RS485_Baud = B1800;
			break;
		case 2400:
			RS485_Baud = B2400;
			break;
		case 4800:
			RS485_Baud = B4800;
			break;
		case 9600:
			RS485_Baud = B9600;
			break;
		case 19200:
			RS485_Baud = B19200;
			break;
		case 38400:
			RS485_Baud = B38400;
			break;
		case 57600:
			RS485_Baud = B57600;
			break;
		case 76800:
			RS485_Baud = B38400;
			break;
		case 115200:
			RS485_Baud = B115200;
			break;
		case 230400:
			RS485_Baud = B230400;
			break;
		default:
			RS485_Baud = B9600;
			break;
	}

	return RS485_Baud;
}


static unsigned int parse_c_cflag(struct interface_profile const * interface)
{
    unsigned int cflags = 0;

    /* set control options */
    cflags |= CLOCAL | CREAD | RS485MOD;

    /* set databits */
    cflags &= ~CSIZE;
    switch(interface->data_bits)
    {
        case INTERFACE_DATA_BITS_5:
            cflags |= CS5;
            break;
        case INTERFACE_DATA_BITS_6:
            cflags |= CS6;
            break;
        case INTERFACE_DATA_BITS_7:
            cflags |= CS7;
            break;
        case INTERFACE_DATA_BITS_8:
            cflags |= CS8;
            break;

        default:
            cflags |= CS8;
    }

    /* set stopbits */
    switch(interface->stop_bits)
    {
        case INTERFACE_STOP_BITS_1:
            cflags &= ~ CSTOPB;
            break;
        case INTERFACE_STOP_BITS_2:
            cflags |= CSTOPB;
            break;

        default:
            cflags &= ~ CSTOPB;
            break;
    }

    /* set parity bits */
    switch(interface->parity)
    {
        case INTERFACE_PARITY_NULL:
            cflags &= ~ PARENB;
            break;
        case INTERFACE_PARITY_ODD:
            cflags |= PARENB;
            cflags &= ~ PARODD;
            break;
        case INTERFACE_PARITY_EVEN:
            cflags |= PARENB;
            cflags |= PARODD;
            break;

        default:
            cflags &= ~ PARENB;
            break;
    }

    /*set baudrate*/
    cflags |= get_baud_rate(interface->baud_rate);

    return cflags;
}




static void frame_debug(const uint8_t* frame, int len, bool is_send)
{
    int i=0;
    if(is_send)
        printf("%s", "[if]: send frame [");
    else
        printf("%s", "[if]: recv frame <");

    for(i=0; i<len; i++)
        printf("%02x ", frame[i]);

    if(is_send)
        printf("%s", "]\n");
    else
        printf("%s", ">\n");
}


int interface_send_a_frame(int fd, const void* frame, int len)
{
    if(!frame)
        return -EINVAL;

    frame_debug(frame, len, true);

    return  write(fd, frame, len);
}


int interface_recv_a_frame(int fd, void* frame, int len, unsigned int timeout)
{
	int retval = 0;
	struct timeval waiter;
	fd_set input;

    if(!frame)
        return -EINVAL;

	waiter.tv_sec = timeout / 1000;
	waiter.tv_usec = timeout % 1000 * 1000;

	/* select initialize */
	FD_ZERO(&input);
	FD_SET(fd, &input);
	retval = select(fd + 1, &input, NULL, NULL, &waiter);
	/* have a select error */
	if (retval < 0)
	{
		wlog_w("select error: %d", retval);
        goto done;
	}
	/* have time out */
	else if (retval == 0)
	{
        retval = -ETIMEDOUT;
        goto done;
	}

	/* have new data receive */
	if (FD_ISSET(fd, &input))
	{
		/* read the data */
		retval = read(fd, frame, len);
		if (retval <= 0)
		{
			wlog_e("read error:%d", retval);
		}

        frame_debug(frame, len, false);
	}

done:
	return retval;
}


void interface_cleanup(int fd)
{

    wlog_i("clean : %d", fd);

	/* restore the old port settings */
	tcsetattr(fd, TCSANOW, &RS485_oldtio);
	ioctl(fd, TIOCSSERIAL, &RS485_oldserial);
	close(fd);
}

int interface_initialize(int * fd, struct interface_profile const * interface)
{
	struct termios newtio;
	struct serial_struct newserial;

    if(!interface)
        return -EINVAL;

    parse_c_cflag(interface);

	/*
	 Open device for reading and writing.
	 Blocking mode - more CPU effecient
	 */
	*fd = open(interface->port_name, O_RDWR | O_NOCTTY);
	if (*fd < 0)
	{
        wlog_e("open device: '%s' fail: %d", interface->port_name, *fd);
        return *fd;
	}

	/* efficient blocking for the read */
	fcntl(*fd, F_SETFL, O_NDELAY);

	/* save current serial port settings */
	tcgetattr(*fd, &RS485_oldtio);
	/* we read the old serial setup */
	ioctl(*fd, TIOCGSERIAL, &RS485_oldserial);
	/* we need a copy of existing settings */
	memcpy(&newserial, &RS485_oldserial, sizeof(struct serial_struct));
	/* clear struct for new port settings */
	memset(&newtio, 0, sizeof(newtio));
	/*
	 BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
	 CRTSCTS : output hardware flow control (only used if the cable has
	 all necessary lines. See sect. 7 of Serial-HOWTO)
	 CS8     : 8n1 (8bit,no parity,1 stopbit)
	 CLOCAL  : local connection, no modem contol
	 CREAD   : enable receiving characters
	 */
//	newtio.c_cflag = RS485_Baud | CS8 | CLOCAL | CREAD | RS485MOD;
	newtio.c_cflag = parse_c_cflag(interface);

	/* Raw input */
	newtio.c_iflag = 0;
	/* Raw output */
	newtio.c_oflag = 0;
	/* no processing */
	newtio.c_lflag = 0;
	/* activate the settings for the port after flushing I/O */
	tcsetattr(*fd, TCSAFLUSH, &newtio);

	/* flush any data waiting */
	usleep(200000);
	tcflush(*fd, TCIOFLUSH);

	wlog_i("Initializing '%s' success", interface->port_name);

    return 0;
}

