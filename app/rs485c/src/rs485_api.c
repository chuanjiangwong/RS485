/*
 * =====================================================================================
 *
 *          @file:  rs485_api.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Apr 5, 2016 9:44:03 AM
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
#include "panel_s_application.h"

/** define the socket unix domain path */
#ifndef RS485_UNIX_DOMAIN_SOCKET_PATH
#define RS485_UNIX_DOMAIN_SOCKET_PATH   "/home/user/bin/rs485d/rs485_unix_domain_service"
#endif

#ifndef RS485_KNX_MAP_MAX_NUMBERS
#define RS485_KNX_MAP_MAX_NUMBERS       (100)
#endif

#ifndef RS485_KNX_MAP_FLUSH_TIME
#define RS485_KNX_MAP_FLUSH_TIME        (1) //1s
#endif

/*
 ****************************************************************
 * The device config
 *
 *
 ***************************************************************
 */
typedef enum
{
/* air conditioner */
    RS485_DEVICE_CONFIG_AIR_CONDITION_YORK,
    RS485_DEVICE_CONFIG_AIR_CONDITION_PANASONNIC,

/* curtain */
    RS485_DEVICE_CONFIG_CURTAIN_DOYA,
    RS485_DEVICE_CONFIG_CURTAIN_AOKE,

/* fresh air */
    RS485_DEVICE_CONFIG_FRESH_AIR_LORELEY,
}rs485_device_config_enum;
/*
 ****************************************************************
 * The device config end
 *
 *
 ***************************************************************
 */

typedef struct knx_map
{
    int                                 knx_id;
    int                                 value;
    int                                 device_id;
    int                                 object_id;
    int                                 command;
    bool                                write_flag;
}knx_map_t;


/** define the unix domain socket */
static struct sockaddr_un               sockaddr_addr;
/** define the socket id */
static int                              socket_fd = 0;
/** define the socket length */
static unsigned int                     socket_len = 0;
/** the knx map */
static knx_map_t                        glb_knx_map[RS485_KNX_MAP_MAX_NUMBERS];
static bool                             knx_map_have_update = false;
/** thread flush */
static pthread_t                        rs485_knx_map_thread;
/** knx map lock */
static pthread_mutex_t                  map_lock;

static bool 							rs485d_running_status = false;

static void* rs485_knx_map_thread_handle(void* arg);




static int rs485_knx_map_init(void)
{
    int error = 0;

//-------------------------------------------start----------------------------------------------////
//change log by zlt 2016.06.06 for latest svn 773.
//configure thread attribute to detached. 
    pthread_attr_t attr;

    error = pthread_attr_init(&attr);
    if(error != 0)
    {
        perror("Init pthread attr error!n");
        printf("*******************exit********************func:%s, line:%d", __func__, __LINE__);
        exit(0);
    }
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
//--------------------------------------------end----------------------------------------------////
    memset(glb_knx_map, 0x00, sizeof(glb_knx_map));

    error = pthread_mutex_init(&map_lock, NULL);
    if(error)
    {
        syslog_error("[client]:mutex initial fail:%d", error);
        return error;
    }

    error = pthread_create(&rs485_knx_map_thread, &attr, rs485_knx_map_thread_handle, NULL);
    if(error)
    {
        syslog_error("[client]:thread create fail:%d", error);
        return error;
    }
    pthread_attr_destroy(&attr);

    return 0;
}


static int rs485_client_init(void)
{
    int error = 0;

    /* create a socket */
	socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(socket_fd < 0)
	{
        syslog_error("%s", "unix socket create fail");
		return -EPERM;
	}

    memset(&sockaddr_addr, 0x00, sizeof(struct sockaddr_un));
	sockaddr_addr.sun_family = AF_UNIX;
    sprintf(sockaddr_addr.sun_path, "%s%05d", RS485_UNIX_DOMAIN_SOCKET_PATH, getpid());
	socket_len = offsetof(struct sockaddr_un, sun_path) + strlen(sockaddr_addr.sun_path);

    unlink(sockaddr_addr.sun_path);

    error = bind(socket_fd, (struct sockaddr*)&sockaddr_addr, socket_len);
    if(error < 0)
    {
        syslog_error("%s", "bind socket error");
        return -EPERM;
    }

    error = chmod(sockaddr_addr.sun_path , S_IRWXU);
    if(error < 0)
    {
        syslog_error("%s", "chmod error");
        return -EPERM;
    }

    /* fill socket address struct with server's address */
    memset(&sockaddr_addr, 0x00, sizeof(sockaddr_addr));
    sockaddr_addr.sun_family = AF_UNIX;
    strcpy(sockaddr_addr.sun_path, RS485_UNIX_DOMAIN_SOCKET_PATH);
    socket_len = offsetof(struct sockaddr_un, sun_path) + strlen(RS485_UNIX_DOMAIN_SOCKET_PATH);

    error = connect(socket_fd, (struct sockaddr*)&sockaddr_addr, socket_len);
    if(error < 0)
    {
        syslog_error("conect error:%d", error);
        return -EPERM;
    }

	return 0;
}


static int send_message_to_rs485d(void* buffer, int buffer_len)
{
    int n = 0;

    if(buffer == NULL || buffer_len <= 0)
        return -EPERM;

    n = write(socket_fd, buffer, buffer_len);

    if(n != buffer_len)
    {
        syslog_warning("%s", "send message to rs485 fail");
        return -EPERM;
    }

    return n;
}


static int recv_message_from_rs485d(void* buffer, int buffer_len)
{
    int n = 0;

    if(buffer == NULL || buffer_len <= 0)
        return -EPERM;

    n = read(socket_fd, buffer, buffer_len);
    if(n<= 0)
        syslog_warning("%s", "recv message from rs485 fail");

    return n;
}


static unsigned int get_baud_rate(unsigned int rate)
{
    switch (rate)
    {
        case RS485_PORT_BAUD_RATE_2400:
            return 2400;
        case RS485_PORT_BAUD_RATE_4800:
            return 4800;
        case RS485_PORT_BAUD_RATE_9600:
            return 9600;
        case RS485_PORT_BAUD_RATE_14400:
            return 14400;
        case RS485_PORT_BAUD_RATE_19200:
            return 19200;
        case RS485_PORT_BAUD_RATE_38400:
            return 38400;
        case RS485_PORT_BAUD_RATE_57600:
            return 57600;
        case RS485_PORT_BADU_RATE_115200:
            return 115200;

        default:
            return 9600;
    }
}

static int create_object(const struct rs485_bus_init * bus)
{
    adapter_t       adapter;
    int             n = 0;

    memset(&adapter, 0x00, sizeof(adapter_t));

    adapter.message_type = SERVICE_CREATE_RS485_OBJECT;
    adapter.message_length = sizeof(adapter_t);
    strcpy(adapter.message_content.new_object.object_name, "enno");
    adapter.message_content.new_object.object_type = bus->protocol;
    /** FIXME: The device if have a VRV adapter , It's maybe have 64 device.
     * if have a under connect, The max is 32*/
    adapter.message_content.new_object.mount_device_max = 64;
    adapter.message_content.new_object.port.baud_rate = get_baud_rate(bus->interface.baud_rate);
    strcpy(adapter.message_content.new_object.port.interface_name, "/dev/ttyS1");
    adapter.message_content.new_object.port.parity = bus->interface.parity;
    adapter.message_content.new_object.address[0] = 0x01;
    adapter.message_content.new_object.address_len = 1;

    n = send_message_to_rs485d(&(adapter), sizeof(adapter_t));
    if(n != sizeof(adapter_t))
    {
        return -1;
    }

    n = recv_message_from_rs485d(&(adapter), sizeof(adapter_t));
    if(n != sizeof(adapter_t))
    {
        return -1;
    }

    n = adapter.message_content.new_object_return.object_id;
    syslog_info("[client]create object id:%d", n);

    return n;
}

static int delete_object(int object_id)
{
	adapter_t 		adapter;
	int 			n = 0;

	adapter.message_type = SERVICE_DELETE_RS485_OBJECT;
	adapter.message_length = sizeof(adapter_t);
	adapter.message_content.delete_object.object_id = object_id;

    n = send_message_to_rs485d(&(adapter), sizeof(adapter_t));
    if(n != sizeof(adapter_t))
    {
        return -1;
    }

    n = recv_message_from_rs485d(&(adapter), sizeof(adapter_t));
    if(n != sizeof(adapter_t))
    {
        return -1;
    }

    n = adapter.message_content.delete_object_return.delete_status;
    syslog_info("[client]:delete object status, id:%d", n);

	return n;
}


static int mount_device(int object_id,
        rs485_device_type_enum device_type,
        rs485_factory_name_enum factory,
        unsigned char addr[4])
{
    adapter_t       adapter;
    int             n = 0;
    static int 		device_id;

    memset(&adapter, 0x00, sizeof(adapter_t));

    adapter.message_type = SERVICE_MOUNT_DEVICE_TO_OBJECT;
    adapter.message_length = sizeof(adapter_t);

    /* which device have on server jugement */
    adapter.message_content.mount_device.object_id = object_id;
    adapter.message_content.mount_device.device_type = device_type;
    adapter.message_content.mount_device.factory_name = factory;
//    adapter.message_content.mount_device.device_addr[0] = addr;
    memcpy(adapter.message_content.mount_device.device_addr, addr,
    	sizeof(adapter.message_content.mount_device.device_addr));

    n = send_message_to_rs485d(&(adapter), sizeof(adapter_t));
    if(n != sizeof(adapter_t))
    {
        return -1;
    }

    n = recv_message_from_rs485d(&(adapter), sizeof(adapter_t));
    if(n != sizeof(adapter_t))
    {
        return -1;
    }

    device_id = adapter.message_content.mount_device_return.device_id;
    if(device_id > 0)
        syslog_info("[client]:mount device success, device id:%d", device_id);
    else
        syslog_warning("[client]:mount device fail, return value:%d", device_id);

    return device_id;
}


static int unmount_device(int object_id, int device_id)
{
	adapter_t adapter;
	int n = 0;

	adapter.message_type = SERVICE_UNMOUNT_DEVICE_FROM_OBJECT;
	adapter.message_length = sizeof(adapter_t);
	adapter.message_content.unmount_device.object_id = object_id;
	adapter.message_content.unmount_device.device_id = device_id;

	n = send_message_to_rs485d(&(adapter), sizeof(adapter_t));
    if(n != sizeof(adapter_t))
    {
        return -1;
    }

	n = recv_message_from_rs485d(&(adapter), sizeof(adapter_t));
    if(n != sizeof(adapter_t))
    {
        return -1;
    }

	n = adapter.message_content.unmount_device_return.unmount_status;
    if(n<0)
        syslog_info("unmount device fail:%d, device id:%d, object id:%d", n, device_id, object_id);
    else
        syslog_info("unmount device :%d success", device_id);

	return n;
}

static int write_device(int device_id, int method, int value, bool broadcast)
{
	adapter_t adapter;
	int n = 0;

    memset(&adapter, 0x00, sizeof(adapter_t));

	adapter.message_type = SERVICE_WRITE_VALUE_TO_DEVICE;
	adapter.message_length = sizeof(adapter_t);

	adapter.message_content.write.device_id = device_id;
	adapter.message_content.write.device_method = method;
	adapter.message_content.write.method_value = value;
    adapter.message_content.write.broadcast = broadcast;

	n = send_message_to_rs485d(&(adapter), sizeof(adapter_t));
    if(n != sizeof(adapter_t))
    {
        return -1;
    }
    syslog_info("[client]:send write device id:%d, method:%d, value:%d", adapter.message_content.write.device_id,
    	adapter.message_content.write.device_method,
    	adapter.message_content.write.method_value);

	n = recv_message_from_rs485d(&(adapter), sizeof(adapter_t));
    if(n != sizeof(adapter_t))
    {
        return -1;
    }

	n = adapter.message_content.write_return.write_status;
	syslog_info("[client]:write status:%d", n);

    return n;
}

static int read_device(read_device_return_t* profile, int device_id)
{
	adapter_t adapter;
	int n = 0;

    memset(&adapter, 0x00, sizeof(adapter_t));

	adapter.message_type = SERVICE_READ_VALUE_FROM_DEVICE;
	adapter.message_length = sizeof(adapter_t);
    adapter.message_content.read.device_id = device_id;

	n = send_message_to_rs485d(&(adapter), sizeof(adapter_t));
    if(n != sizeof(adapter_t))
    {
        return -1;
    }

    n = recv_message_from_rs485d(&(adapter), sizeof(adapter_t));
    if(n != sizeof(adapter_t))
    {
        return -1;
    }

    /* server get information fail */
    if(adapter.message_content.read_return.read_status == false)
    {
        return -1;
    }

    memcpy(profile, &(adapter.message_content.read_return), sizeof(read_device_return_t));

    return 0;
}

static int service_stop(void)
{
	adapter_t adapter;
	int n = 0;

    memset(&adapter, 0x00, sizeof(adapter_t));

	adapter.message_type = SERVICE_SYSTEM_UPDATE_START;
	adapter.message_length = sizeof(adapter_t);

	n = send_message_to_rs485d(&(adapter), sizeof(adapter_t));
    if(n != sizeof(adapter_t))
    {
        return -1;
    }

    return 0;
}



static void knx_id_map_air_conditon_init(const struct rs485_init* device, int object_id, int device_id)
{
    /* initial the knx id */
     /* switch */
    if(device->knx_id >= RS485_KNX_MAP_MAX_NUMBERS)
        return;

     glb_knx_map[device->knx_id].knx_id = device->knx_id;
     glb_knx_map[device->knx_id].value = 0;
     glb_knx_map[device->knx_id].command = RS485_AIR_SWITCH;
     glb_knx_map[device->knx_id].object_id = object_id;
     glb_knx_map[device->knx_id].device_id = device_id;

     /* current temperature */
     glb_knx_map[device->knx_id + 1].knx_id = device->knx_id + 1;
     glb_knx_map[device->knx_id + 1].value = 0;
     glb_knx_map[device->knx_id + 1].command = 0;
     glb_knx_map[device->knx_id + 1].object_id = object_id;
     glb_knx_map[device->knx_id + 1].device_id = device_id;

     /* current humidity */
     glb_knx_map[device->knx_id + 2].knx_id = device->knx_id + 2;
     glb_knx_map[device->knx_id + 2].value = 0;
     glb_knx_map[device->knx_id + 2].command = 0;
     glb_knx_map[device->knx_id + 2].object_id = object_id;
     glb_knx_map[device->knx_id + 2].device_id = device_id;

     /* air condition mode */
     glb_knx_map[device->knx_id + 3].knx_id = device->knx_id + 3;
     glb_knx_map[device->knx_id + 3].value = 0;
     glb_knx_map[device->knx_id + 3].command = RS485_AIR_MODE;
     glb_knx_map[device->knx_id + 3].object_id = object_id;
     glb_knx_map[device->knx_id + 3].device_id = device_id;

     /* air condition temperature */
     glb_knx_map[device->knx_id + 4].knx_id = device->knx_id + 4;
     glb_knx_map[device->knx_id + 4].value = 0;
     glb_knx_map[device->knx_id + 4].command = RS485_AIR_SET_TEMP;
     glb_knx_map[device->knx_id + 4].object_id = object_id;
     glb_knx_map[device->knx_id + 4].device_id = device_id;

     /* air condition swing*/
     glb_knx_map[device->knx_id + 5].knx_id = device->knx_id + 5;
     glb_knx_map[device->knx_id + 5].value = 0;
     glb_knx_map[device->knx_id + 5].command = RS485_AIR_SWING;
     glb_knx_map[device->knx_id + 5].object_id = object_id;
     glb_knx_map[device->knx_id + 5].device_id = device_id;

     /* air condition fan */
     glb_knx_map[device->knx_id + 6].knx_id = device->knx_id + 6;
     glb_knx_map[device->knx_id + 6].value = 0;
     glb_knx_map[device->knx_id + 6].command = RS485_AIR_FAN;
     glb_knx_map[device->knx_id + 6].object_id = object_id;
     glb_knx_map[device->knx_id + 6].device_id = device_id;
}


static void knx_id_map_curtain_init(const struct rs485_init* device, int object_id, int device_id)
{
    /* initial the knx id */
     /* switch */
    if(device->knx_id >= RS485_KNX_MAP_MAX_NUMBERS)
        return;

     glb_knx_map[device->knx_id].knx_id = device->knx_id;
     glb_knx_map[device->knx_id].value = 0;
     glb_knx_map[device->knx_id].command = RS485_CURTAIN;
     glb_knx_map[device->knx_id].object_id = object_id;
     glb_knx_map[device->knx_id].device_id = device_id;
}


static void knx_id_map_fresh_air_init(const struct rs485_init* device, int object_id, int device_id)
{
    /* initial the knx id */
     /* switch */
    if(device->knx_id >= RS485_KNX_MAP_MAX_NUMBERS)
        return;

     glb_knx_map[device->knx_id].knx_id = device->knx_id;
     glb_knx_map[device->knx_id].value = 0;
     glb_knx_map[device->knx_id].command = RS485_FRESH_AIR;
     glb_knx_map[device->knx_id].object_id = object_id;
     glb_knx_map[device->knx_id].device_id = device_id;
}


int rs485_device_init(struct rs485_init device[], int numbers,
        const struct rs485_bus_init * bus)
{
    int n=0, i=0;
    int object_id = 0;
    struct timeval  recv_timeout;

    if(device == NULL)
    {
        syslog_error("%s", "argument error");
        return -EINVAL;
    }

    /* create a socket to connect server */
    n = rs485_client_init();
    if(n < 0)
        return -EPERM;

    /* create the rs485 obejct */
    object_id = create_object(bus);
    if(object_id < 0)
    {
        syslog_error("[client]:%s", "create object fail");
        return object_id;
    }

    /* initial the knx map table */
    n = rs485_knx_map_init();
    if(n < 0)
        return -EPERM;

    /* inittialze the device  */
    for(i=0; i<numbers; i++)
    {
        switch(device[i].device_type)
        {
            case RS485_Curtain:
                /* doya curtian */
                n = mount_device(object_id, RS485_DEVICE_TYPE_CURTAIN, device[i].factory, device[i].address);
                if(n > 0)
                {
                    knx_id_map_curtain_init(&device[i], object_id, n);
                }
                else
                {
                	glb_knx_map[device[i].knx_id].object_id = -1;
                	glb_knx_map[device[i].knx_id].device_id = -1;
                    syslog_warning("[client]:mount device fail:%d, knx id:%d, mac:0x%02x", n, device[i].knx_id, device[i].address);
                }
                break;

            case Fresh_Air:
                n = mount_device(object_id, RS485_DEVICE_TYPE_FRESH_AIR, device[i].factory, device[i].address);
                if(n > 0)
                {
                    knx_id_map_fresh_air_init(&device[i], object_id, n);
                }
                else
                {
                	glb_knx_map[device[i].knx_id].object_id = -1;
                	glb_knx_map[device[i].knx_id].device_id = -1;
                    syslog_warning("[client]:mount device fail:%d, knx id:%d, mac:0x%02x", n, device[i].knx_id, device[i].address);
                }
                break;

            case Air_Conditioner:
                n = mount_device(object_id, RS485_DEVICE_TYPE_AIR_CONDITION, device[i].factory, device[i].address);
                if(n > 0)
                {
                    knx_id_map_air_conditon_init(&device[i], object_id, n);
                }
                else
                {
                	glb_knx_map[device[i].knx_id].object_id = -1;
                	glb_knx_map[device[i].knx_id].device_id = -1;
                    syslog_warning("[client]:mount device fail:%d, knx id:%d, mac:0x%02x", n, device[i].knx_id, device[i].address);
                }
                break;

            default:
                syslog_warning("[client]:unknown rs485 device type:%d, knx id:%d", device[i].device_type, device[i].knx_id);
                continue;
        }

    }

    /* set socket opt */
    recv_timeout.tv_sec = 1;
    recv_timeout.tv_usec = 0;
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));

    rs485d_running_status = true;

    return 0;
}

void rs485_device_deinit(void)
{
    int i = 0;

    for(i=0; i< RS485_KNX_MAP_MAX_NUMBERS; i++)
    {
        if(glb_knx_map[i].device_id > 0 && glb_knx_map[i].object_id > 0)
        {
            unmount_device(glb_knx_map[i].object_id, glb_knx_map[i].device_id);
            glb_knx_map[i].device_id = -1;
        }
    }

    /* delete the object */
    for(i=0; i< RS485_KNX_MAP_MAX_NUMBERS; i++)
    {
        if(glb_knx_map[i].object_id > 0)
        {
            delete_object(glb_knx_map[i].object_id);
            break;
        }
    }
}


void rs485_daemon_restart(void)
{
    int i = 0;

    /* step1. delete object */
    for(i=0; i<RS485_KNX_MAP_MAX_NUMBERS; i++)
    {
        if(glb_knx_map[i].object_id > 0)
        {
            delete_object(glb_knx_map[i].object_id);
            break;
        }
    }

    /* step2. close the socket */
    if(socket_fd > 0)
        close(socket_fd);
}



int rs485_send_to_device(int knx_id, int value)
{
    if(knx_id > RS485_KNX_MAP_MAX_NUMBERS)
        return -EINVAL;

    if(glb_knx_map[knx_id].device_id <= 0)
    {
    	syslog_info("[client]:device id :%d inval", glb_knx_map[knx_id].device_id);
    	return -EINVAL;
    }

	if(rs485d_running_status == false)
		return -EPERM;

    pthread_mutex_lock(&map_lock);
    glb_knx_map[knx_id].value = value;
    glb_knx_map[knx_id].write_flag = true;
    knx_map_have_update = true;
    pthread_mutex_unlock(&map_lock);

    return 0;
}

int rs485_get_device_info(read_device_return_t* out, int knx_id)
{
    int device_id;
    int error;

    if(out == NULL)
        return -EINVAL;

    if(knx_id < 0 || knx_id > RS485_KNX_MAP_MAX_NUMBERS)
        return -EINVAL;

    if(rs485d_running_status == false)
        return -EPERM;

    device_id = glb_knx_map[knx_id].device_id;

    if(device_id <= 0)
    {
    	syslog_info("[client]:device id: %d, inval, knx_id:%d", device_id, knx_id);
    	return 0;
    }

    error = read_device(out, device_id);
    if(error)
    {
        syslog_error("read device profile fail:%d", error);
        return error;
    }

    syslog_info("[client]:read profile success, device id:%d", device_id);

    return 0;
}

int rs485_server_stop(void)
{
    rs485d_running_status = false;
    return service_stop();
}

static void flush_value_to_device(int knx_id, int value)
{
    int command = 0;
    int n = 0;

    switch(glb_knx_map[knx_id].command)
    {
        case RS485_AIR_SWING:
            /*TODO: have no swing*/
            break;

        case RS485_AIR_SET_TEMP:
            if(value< RS485_AIR_SET_TEMP_18)
                command = RS485_AIR_SET_TEMP_18;
            else if(value > RS485_AIR_SET_TEMP_30)
                command = RS485_AIR_SET_TEMP_30;
            else
                command = value;
            break;

        case RS485_AIR_MODE:
            switch(value)
            {
                case 0:
                    command = RS485_AIR_MODE_COOLING;
                    break;
                case 1:
                    command = RS485_AIR_MODE_HEATING;
                    break;
                case 2:
                    command = RS485_AIR_MODE_FANING;
                    break;
                case 3:
                    command = RS485_AIR_MODE_AUTOING;
                    break;
                case 4:
                    command = RS485_AIR_MODE_DRYING;
                    break;
                default:
                    syslog_warning("air mode value error:%d", value);
                    return;
            }
            break;
        case RS485_AIR_FAN:
            switch(value)
            {
                case 0:
                    command = RS485_AIR_FAN_LOW;
                    break;
                case 1:
                    command = RS485_AIR_FAN_MIDDLE;
                    break;
                case 2:
                    command = RS485_AIR_FAN_HIGH;
                    break;
                case 3:
                    command = RS485_AIR_FAN_AUTO;
                    break;

                default:
                    syslog_warning("air fan value error:%d", value);
                    return ;
            }
            break;
        case RS485_AIR_SWITCH:
            if(value)
                command = RS485_AIR_ON;
            else
                command = RS485_AIR_OFF;
            break;

        case RS485_CURTAIN:
            /* FIXME; I have default the 0% is close, and 100% is open */
            if(value <= 0)
                command = RS485_CURTAIN_CLOSE;
            else if(value >= 100)
                command = RS485_CURTAIN_OPEN;
            else
                command = RS485_CURTAIN_SET_PERCENT;
            break;

        case RS485_FRESH_AIR:
            if(value)
                command = RS485_FRESH_AIR_AUTO_ON;
            else
                command = RS485_FRESH_AIR_AUTO_OFF;
            break;

            /* others command */
        default:
            command = glb_knx_map[knx_id].command;
    }

    n = write_device(glb_knx_map[knx_id].device_id, command, value, false);
    if(n < 0)
    {
        syslog_error("write device error: %d", n);
    }

}


static void* rs485_knx_map_thread_handle(void* UNUSED(arg))
{
    int i = 0;

    while(1)
    {
        thread_status.rs485_knx_map_watching ++;
        /* wait the buffer have wirte */
        sleep(RS485_KNX_MAP_FLUSH_TIME);

        /* have a right */
        if(knx_map_have_update == false)
            continue;

        pthread_mutex_lock(&map_lock);
        for(i=0; i<RS485_KNX_MAP_MAX_NUMBERS; i++)
        {
            if(glb_knx_map[i].write_flag)
            {
                glb_knx_map[i].write_flag = false;
                flush_value_to_device(i, glb_knx_map[i].value);
            }
            usleep(1);
        }
        knx_map_have_update = false;
        pthread_mutex_unlock(&map_lock);


    }
    return NULL;
}








