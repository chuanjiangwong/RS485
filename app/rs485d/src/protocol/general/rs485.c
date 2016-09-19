/*
 * =====================================================================================
 *
 *          @file:  rs485.c
 *
 *    Description:  The rs485 driver interface
 *
 *        Version:  1.0
 *        Created:  03/28/2016 11:16:47 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
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
#include <sched.h>
#include <linux/serial.h>       /* for struct serial_struct */
#include <math.h>       /* for calculation of custom divisor */
#include <sys/ioctl.h>

/* Local includes */
#include "adapter.h"
#include "protocol/general/general.h"
#include "protocol/general/rs485.h"
#include "syslog/log.h"

#include <sys/select.h>
#include <sys/time.h>

#ifndef RS485_DEBUG
#define RS485_DEBUG                                         (1)
#endif

/* Posix serial programming reference:
http://www.easysw.com/~mike/serial/serial.html */

/* Use ionice wrapper to improve serial performance:
   $ sudo ionice -c 1 -n 0 ./bin/bacserv 12345
*/

/* handle returned from open() */
static int RS485_Handle = -1;
/* baudrate settings are defined in <asm/termbits.h>, which is
   included by <termios.h> */
static unsigned int RS485_Baud = B38400;
//static unsigned int RS485_Baud = B9600;
/* serial port name, /dev/ttyS0,
  /dev/ttyUSB0 for USB->RS485 from B&B Electronics USOPTL4 */
//static char *RS485_Port_Name = "/dev/ttyUSB0";
static char *RS485_Port_Name = "/dev/ttyS0";
//static char *RS485_Port_Name = "/dev/ttyS1";
/* some terminal I/O have RS-485 specific functionality */
#ifndef RS485MOD
#define RS485MOD 0
#endif
/* serial I/O settings */
static struct termios RS485_oldtio;
/* for setting custom divisor */
static struct serial_struct RS485_oldserial;
/* indicator of special baud rate */
static bool RS485_SpecBaud = false;


#define _POSIX_SOURCE 1 /* POSIX compliant source */

/*********************************************************************
* DESCRIPTION: Configures the interface name
* RETURN:      none
* ALGORITHM:   none
* NOTES:       none
*********************************************************************/
void rs485_set_interface(
    char *ifname)
{
    /* note: expects a constant char, or char from the heap */
    if (ifname) {
        RS485_Port_Name = ifname;
    }
}

/*********************************************************************
* DESCRIPTION: Returns the interface name
* RETURN:      none
* ALGORITHM:   none
* NOTES:       none
*********************************************************************/
const char *rs485_interface(
    void)
{
    return RS485_Port_Name;
}

/****************************************************************************
* DESCRIPTION: Returns the baud rate that we are currently running at
* RETURN:      none
* ALGORITHM:   none
* NOTES:       none
*****************************************************************************/
uint32_t rs485_get_baud_rate(
    void)
{
    uint32_t baud = 0;

    switch (RS485_Baud) {
        case B0:
            baud = 0;
            break;
        case B50:
            baud = 50;
            break;
        case B75:
            baud = 75;
            break;
        case B110:
            baud = 110;
            break;
        case B134:
            baud = 134;
            break;
        case B150:
            baud = 150;
            break;
        case B200:
            baud = 200;
            break;
        case B300:
            baud = 300;
            break;
        case B600:
            baud = 600;
            break;
        case B1200:
            baud = 1200;
            break;
        case B1800:
            baud = 1800;
            break;
        case B2400:
            baud = 2400;
            break;
        case B4800:
            baud = 4800;
            break;
        case B9600:
            baud = 9600;
            break;
        case B19200:
            baud = 19200;
            break;
        case B38400:
            if (!RS485_SpecBaud) {
                /* Linux asks for custom divisor
                   only when baud is set on 38400 */
                baud = 38400;
            } else {
                baud = 76800;
            }
            break;
        case B57600:
            baud = 57600;
            break;
        case B115200:
            baud = 115200;
            break;
        case B230400:
            baud = 230400;
            break;
        default:
            baud = 9600;
    }

    return baud;
}


/****************************************************************************
* DESCRIPTION: Sets the baud rate for the chip USART
* RETURN:      none
* ALGORITHM:   none
* NOTES:       none
*****************************************************************************/
bool rs485_set_baud_rate(
    uint32_t baud)
{
    bool valid = true;

    RS485_SpecBaud = false;
    switch (baud) {
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
            RS485_SpecBaud = true;
            break;
        case 115200:
            RS485_Baud = B115200;
            break;
        case 230400:
            RS485_Baud = B230400;
            break;
        default:
            valid = false;
            break;
    }

    if (valid) {
        /* FIXME: store the baud rate */
    }

    return valid;
}


int rs485_send_handle_frame(volatile struct mstp_port_handle* mstp_port)
{
    int n= 0;
    int frame_len = 0;

    if(mstp_port == NULL)
        return -EINVAL;

    if(mstp_port->send_handle == NULL)
    {
        syslog_error("[rs485]:mstp_port->send_handle is NULL");
        return -EPERM;
    }

    /* package a send buffer */
    frame_len = mstp_port->send_handle(mstp_port);
    if(frame_len < 0)
    {
        syslog_error("[rs485]:make package error:%d", frame_len);
        return frame_len;
    }

    n = write(RS485_Handle, mstp_port->package_buffer, frame_len);
    if(n != frame_len)
    {
        syslog_error("[rs485]:write data error%d", n);
        return -EIO;
    }

#if (RS485_DEBUG)
    int i=0;
    syslog_format("%s", "[rs485]:send frame:[ ");
    for(i=0; i<frame_len; i++)
        syslog_format("%02x ", mstp_port->package_buffer[i]);
    syslog_format("%s", "]\n");
#endif

    return 0;
}


int rs485_recv_handle_frame(volatile struct mstp_port_handle* mstp_port)
{
    int n= 0,error=0;
    struct timeval waiter;
    fd_set input;

    if(mstp_port == NULL)
        return -EINVAL;

    /* have wait sometimes */
    if(mstp_port->timeout_ms == 0)
    {
        syslog_warning("[rs485]:%s", "mstp_port->timeout_ms is 0");
        return -EPERM;
    }

    waiter.tv_sec = mstp_port->timeout_ms/1000;
    waiter.tv_usec = mstp_port->timeout_ms%1000 * 1000;

    /* select initialize */
    FD_ZERO(&input);
    FD_SET(RS485_Handle, &input);
    n = select(RS485_Handle + 1, &input, NULL, NULL, &waiter);
    /* have a select error */
    if(n < 0)
    {
        syslog_error("[rs485]:select error:%d", n);
        return n;
    }
    /* have time out */
    else if(n == 0)
    {
        syslog_debug("[rs485]:wait read data timeout:[%ums]", mstp_port->timeout_ms);
        return -EIO;
    }

    /* have new data receive */
    if(FD_ISSET(RS485_Handle, &input))
    {
        /* read the data */
        n = read(RS485_Handle, mstp_port->package_buffer, mstp_port->package_buffer_len);
        if(n <= 0) {
            syslog_error("[rs485]:read error:%d", n);
            return -EIO;
        }

        /* set the package buffer len */
        mstp_port->package_buffer_len = n;

#if (RS485_DEBUG)
        int i=0;
        syslog_format("%s", "[rs485]:recv frame:[ ");
        for(i=0; i<n; i++)
            syslog_format("(%d)%02x ", i, mstp_port->package_buffer[i]);
        syslog_format("%s", "]\n");
#endif

        /* process the result */
        if(mstp_port->recv_handle)
        {
            error = mstp_port->recv_handle(mstp_port);
            if(error < 0)
            {
                syslog_warning("[rs485]:rs485 process error:%d", error);
                return error;
            }
        }
    }

    return 0;
}


void rs485_cleanup(
    void)
{
    /* restore the old port settings */
    tcsetattr(RS485_Handle, TCSANOW, &RS485_oldtio);
    ioctl(RS485_Handle, TIOCSSERIAL, &RS485_oldserial);
    close(RS485_Handle);
}


void rs485_initialize(
    void)
{
    struct termios newtio;
    struct serial_struct newserial;

    syslog_info("[general]:RS485: Initializing %s", RS485_Port_Name);
    /*
       Open device for reading and writing.
       Blocking mode - more CPU effecient
     */
    RS485_Handle = open(RS485_Port_Name, O_RDWR | O_NOCTTY /*| O_NDELAY */ );
    if (RS485_Handle < 0) {
        perror(RS485_Port_Name);
        exit(-1);
    }
#if 0
    /* non blocking for the read */
    fcntl(RS485_Handle, F_SETFL, FNDELAY);
#else
    /* efficient blocking for the read */
    fcntl(RS485_Handle, F_SETFL, 0);
#endif
    /* save current serial port settings */
    tcgetattr(RS485_Handle, &RS485_oldtio);
    /* we read the old serial setup */
    ioctl(RS485_Handle, TIOCGSERIAL, &RS485_oldserial);
    /* we need a copy of existing settings */
    memcpy(&newserial, &RS485_oldserial, sizeof(struct serial_struct));
    /* clear struct for new port settings */
    bzero(&newtio, sizeof(newtio));
    /*
       BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
       CRTSCTS : output hardware flow control (only used if the cable has
       all necessary lines. See sect. 7 of Serial-HOWTO)
       CS8     : 8n1 (8bit,no parity,1 stopbit)
       CLOCAL  : local connection, no modem contol
       CREAD   : enable receiving characters
     */
    newtio.c_cflag = RS485_Baud | CS8 | CLOCAL | CREAD | RS485MOD;
    /* Raw input */
    newtio.c_iflag = 0;
    /* Raw output */
    newtio.c_oflag = 0;
    /* no processing */
    newtio.c_lflag = 0;
    /* activate the settings for the port after flushing I/O */
    tcsetattr(RS485_Handle, TCSAFLUSH, &newtio);
    if (RS485_SpecBaud) {
/*         76800, custom divisor must be set
        newserial.flags |= ASYNC_SPD_CUST;
        newserial.custom_divisor =
            round(((float) newserial.baud_base) / 76800);
         we must check that we calculated some sane value;
           small baud bases yield bad custom divisor values
        baud_error =
            fabs(1 -
            ((float) newserial.baud_base) /
            ((float) newserial.custom_divisor) / 76800);
        if ((newserial.custom_divisor == 0) || (baud_error > 0.02)) {
             bad divisor
            fprintf(stderr, "bad custom divisor %d, base baud %d\n",
                newserial.custom_divisor, newserial.baud_base);
            exit(EXIT_FAILURE);
        }
         if all goes well, set new divisor
        ioctl(RS485_Handle, TIOCSSERIAL, &newserial);*/
    }

    /* destructor */
    atexit(rs485_cleanup);
    /* flush any data waiting */
    usleep(200000);
    tcflush(RS485_Handle, TCIOFLUSH);
    /* ringbuffer */
    syslog_info("[general]:RS485: Initializing (%s) success", RS485_Port_Name);
}


