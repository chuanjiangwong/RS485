/**
 * www.enno.com
 *
 * @file :	device.c
 *
 * @date : 	Mar 24, 2016
 *
 * @author: wong
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>

#include "device.h"
#include "enum.h"
#include "syslog/log.h"
#include "timer_task.h"
#include "object.h"
#include "protocol/general/general.h"
#include "enumtxt.h"
#include "item_config.h"

#ifndef RS485_DEVICE_MAX_NUMBERS
#define RS485_DEVICE_MAX_NUMBERS                (100)
#endif

#ifndef RS485_DEVICE_NAME_MAX_LENGTH
#define RS485_DEVICE_NAME_MAX_LENGTH            (108)
#endif

#ifndef RS485_DEVICE_CLASS_MAX_SAVED
#define RS485_DEVICE_CLASS_MAX_SAVED            (16)
#endif


/** define the device management table */
static device_management_t                          *glb_device_manage[RS485_DEVICE_MAX_NUMBERS] = { NULL };

/** define the device management lock */
static pthread_mutex_t                              device_management_lock;

/** save the curtain factory */
static int                                          glb_device_class[RS485_DEVICE_CLASS_MAX_SAVED] = { -1 };



/* -----------------------------------------*/
/**
 * @brief find_device_class_saved
 *  some device just need a timer task is device class,
 *  so find the device class whother have created.
 *
 * @param[in] factory : The factory name
 *
 * @return  if have created, return true , or return false.
 */
/* -----------------------------------------*/
static inline bool find_device_class_saved(rs485_factory_name_enum factory)
{
    int i=0;

    for(i=0; i<RS485_DEVICE_CLASS_MAX_SAVED; i++)
    {
        if(glb_device_class[i] == factory)
        {
            return true;
        }
        else if(glb_device_class[i] == -1)
        {
            glb_device_class[i] = factory;
            return false;
        }
    }

    return false;
}


/* -----------------------------------------*/
/**
 * @brief find_available_device_id
 *  find a available device ID
 *
 * @ingroup device_management
 *
 * @return  return the device id, but The not positive value is a error.
 */
/* -----------------------------------------*/
static inline int find_available_device_id(void)
{
    int id = -1;
    int i;

    /* the id 0 have no used, just for look good */
    for(i=1; i<RS485_DEVICE_MAX_NUMBERS; i++)
    {
        if(glb_device_manage[i] == NULL)
        {
            id = i;
            break;
        }
    }

    return id;
}


/* -----------------------------------------*/
/**
 * @brief check_device_id
 *  check the device is legal
 *
 * @param[in] device_id : The need to check device id.
 *
 * @return  if device id is legal return true, and return false.
 */
/* -----------------------------------------*/
inline bool check_device_id(int device_id)
{
    if(device_id <= 0 || device_id >= RS485_DEVICE_MAX_NUMBERS)
        return false;

    if(glb_device_manage[device_id] == NULL)
        return false;
    else
        return true;
}

/* -----------------------------------------*/
/**
 * @brief set_timer
 *  create device timer task
 *
 * @param[in,out] new_device :  The struct "device_management_t"
 *
 * @return if should create a timer task, return true, or return false.
 */
/* -----------------------------------------*/
static inline bool set_timer(device_management_t* new_device)
{
    static int curtain_numbers = 0;

    switch(new_device->device_type)
    {
        case RS485_DEVICE_TYPE_CURTAIN:
            curtain_numbers ++;
            new_device->timer->tick = 20;
            new_device->timer->device_id = new_device->device_id;
            new_device->timer->command = RS485_CURTAIN_GET_DEVICE_INFO;
            new_device->timer->timeout = new_device->device_status_period;
            switch(new_device->factory_name)
            {
                case RS485_CURTAIN_CUSTOMIZE_AOKE_ALL:
                    new_device->timer->function = device_timer_task_handle_curtain_aoke_init;
                    break;

                case RS485_CURTAIN_CUSTOMIZE_DOOYA_ALL:
                    new_device->timer->function = device_timer_task_handle_curtain_doya_init;
                    break;

                /* TODO: */

                default:
                    new_device->timer->function = device_timer_task_handle_curtain_init;
            }
            if(find_device_class_saved(new_device->factory_name) == true)
                return false;
            break;

        case RS485_DEVICE_TYPE_AIR_CONDITION:
            new_device->timer->tick = 0;
            new_device->timer->device_id = new_device->device_id;
            new_device->timer->command = RS485_AIR_GET_DEVICE_INFO;
            new_device->timer->timeout = new_device->device_status_period;
            new_device->timer->function = device_timer_task_handle_demo;
            switch(new_device->factory_name)
            {
                case RS485_AIRCONDITION_MODBUS_DAIKIN_DTA116A621_BOX:
                    new_device->timer->tick = 60; //just to judgetment the HA system is ok
                    if(find_device_class_saved(new_device->factory_name) == true)
                        return false;
                    break;

                default:
                    break;
            }
            break;

        case RS485_DEVICE_TYPE_FRESH_AIR:
            new_device->timer->tick = 300; // skip the first period
            new_device->timer->device_id = new_device->device_id;
            new_device->timer->command = RS485_FRESH_AIR_GET_DEVICE_INFO;
            new_device->timer->timeout = new_device->device_status_period;
            new_device->timer->function = device_timer_task_handle_demo;
            break;

        default:
            syslog_warning("[device]:unknown device, set timer func NULL");
            new_device->timer->tick = 0;
            new_device->timer->device_id = new_device->device_id;
            new_device->timer->command = RS485_FRESH_AIR_GET_DEVICE_INFO;
            new_device->timer->timeout = new_device->device_status_period;
            new_device->timer->function = NULL;
            return false;
    }

    return true;
}

static void printf_device(const device_management_t* device)
{
    syslog_info("---------------------------------------");
    syslog_info("-----------new device------------------");
    syslog_info("---------------------------------------");
    syslog_info("         devie name: %s", device->device_name);
    syslog_info("       factory name: %s", get_enum_txt_device_factory(device->factory_name));
    syslog_info("          object id: %d", device->object_id);
    syslog_info("        object type: %s", get_enum_txt_rs485_protocol_type(device->object_type));
    syslog_info("          device id: %d", device->device_id);
    syslog_info("        device type: %s", get_enum_txt_rs485_device_type(device->device_type));
    syslog_info(" device address len: %d", device->device_addr_len);
    syslog_info("     device address: 0x%02x 0x%02x 0x%02x 0x%02x", device->device_addr[0], device->device_addr[1], device->device_addr[2], device->device_addr[3]);
    syslog_info("           time out: %u(ms)", device->time_out);
    syslog_info("      device period: %u(s)", device->device_status_period);
    syslog_info("     retransmission: %u", device->retransmission);
    syslog_info(" device information: ");
    syslog_info("                   1)running:%s", get_enum_txt_bool(device->device_info->runing));
    syslog_info("                   2)error:%s", get_enum_txt_bool(device->device_info->error));
    syslog_info("                   3)read status:%s", get_enum_txt_bool(device->device_info->read_status));
    syslog_info("              timer: 0x%08x", (unsigned int)device->timer);
    syslog_info("     device private: 0x%08x", (unsigned int)device->device_private);
    syslog_info("    private numbers: %d", device->device_private_numbers);
    syslog_info("---------------------------------------");
}

int create_device(adapter_t* adapter)
{
    int                 error = -1;
    int                 device_id = 0;
    int                 device_name_len = 0;
    device_management_t *new_device = NULL;
    bool                should_create_timer = true;

    /* this function  have config the panno_s itme */
#ifdef PANNO_S_ITEM_CONFIG
    panno_s_item_config(adapter,
            adapter->message_content.mount_device.device_type,
            adapter->message_content.mount_device.device_addr[0]);
#endif

    /* find the available device id */
    device_id = find_available_device_id();
    if(device_id < 0)
    {
        syslog_warning("[device]:%s", "The device management have full");
        return -EPERM;
    }

    /* find the object have create */
    if(check_object_id(adapter->message_content.mount_device.object_id) == false)
    {
        syslog_warning("[device]:The object id :[%d] inval", adapter->message_content.mount_device.object_id);
        return -EPERM;
    }

    /* have check the object_id type */
    if(get_object_type(adapter->message_content.mount_device.object_id) !=
            adapter->message_content.mount_device.object_type)
    {
        syslog_warning("[device]:The object type:[%d] inval", adapter->message_content.mount_device.object_type);
        return -EPERM;
    }
    /* have check is support the device */
    if(check_device_support(adapter) == false)
    {
        syslog_warning("[device]:%s", "have no support device");
        return -EPERM;
    }
    /*TODO: check the device haved exist*/

    /* get object nubers */
    if(check_object_numbers_have_idle(adapter->message_content.mount_device.object_id) == false)
    {
        syslog_warning("[device]:%s", "The bus have full");
        return -EPERM;
    }

    /* malloc a buffer to save the device information */
    new_device = (device_management_t*)malloc(sizeof(device_management_t));
    if(new_device == NULL)
    {
        syslog_warning("[device]:%s", "malloc fail.");
        return -ENOMEM;
    }
    memset(new_device, 0x00, sizeof(device_management_t));

    /* full the device pointer */
    new_device->device = new_device;

    /* full the device name */
    device_name_len = strlen(adapter->message_content.mount_device.device_name);
    new_device->device_name = malloc(device_name_len + 1 + 4 - (device_name_len + 1)%4);
    if(new_device->device_name == NULL)
    {
        syslog_warning("[device]:%s", "malloc fail");
        goto after_fail_device_name;
    }
    strcpy(new_device->device_name, adapter->message_content.mount_device.device_name);

    /* full the factroy name */
    new_device->factory_name = adapter->message_content.mount_device.factory_name;

    /* full the object id */
    new_device->object_id = adapter->message_content.mount_device.object_id;

    /* full the object type */
    new_device->object_type = adapter->message_content.mount_device.object_type;

    /* full the device id */
    new_device->device_id = device_id;

    /* full the device type */
    new_device->device_type = adapter->message_content.mount_device.device_type;

    /* full the device address len */
    new_device->device_addr_len = adapter->message_content.mount_device.device_addr_len;

    /* full the device_addr */
    memcpy(new_device->device_addr, adapter->message_content.mount_device.device_addr, new_device->device_addr_len);

    /* full the timer out */
    new_device->time_out = adapter->message_content.mount_device.time_out;

    /* full the get device status period */
    new_device->device_status_period = adapter->message_content.mount_device.device_status_period;

    /* full the retransmission count */
    new_device->retransmission = adapter->message_content.mount_device.retransmission;

    /* full the device support reply */
    new_device->support_reply = adapter->message_content.mount_device.support_reply;

    /* full the device information */
    new_device->device_info = (read_device_return_t*)malloc(sizeof(read_device_return_t));
    if(new_device->device_info == NULL)
    {
        syslog_error("[device]:%s", "malloc fail");
        goto after_fail_device_info;
    }
    memset(new_device->device_info, 0x00, sizeof(read_device_return_t));
    new_device->device_info->runing = true;
    new_device->device_info->error = false;

    /* create the timer */
    new_device->timer = (timer_task_t*)malloc(sizeof(timer_task_t));
    if(new_device->timer == NULL)
    {
        syslog_warning("[device]:%s", "malloc fail");
        goto after_fail_timer_malloc;
    }
    memset(new_device->timer, 0x00, sizeof(timer_task_t));
    /* full the timer task information */
    should_create_timer = set_timer(new_device);
    if(should_create_timer)
    {
        error = create_device_timer_task(new_device->timer);
        if(error)
        {
            syslog_warning("[device]:%s", "create timer fail");
            goto after_fail_timer_create;
        }
    }

    /* full the device private profile */
    /* TODO: add the method */
    new_device->device_private = get_support_device_profile(new_device->factory_name);
    new_device->device_private_numbers = get_support_device_profile_numbers(new_device->factory_name);

    /* add to object */
    mount_a_device_to_object(new_device->object_id, new_device->device_id);

    /* put new_device to glb device management */
    glb_device_manage[device_id] = new_device;

    /* print the device */
    printf_device(new_device);

    /* return the device id */
    return device_id;

after_fail_timer_create:
    free(new_device->timer);
after_fail_timer_malloc:
    free(new_device->device_info);
after_fail_device_info:
    free(new_device->device_name);
after_fail_device_name:
    free(new_device);
    return error;
}


int delete_device(int object_id, int device_id)
{
    if(check_device_id(device_id) == false)
    {
    	syslog_warning("[device]:device id : [%d] error or device is NULL", device_id);
    	return -EPERM;
    }

    if(check_object_id(object_id) == false)
    {
    	syslog_warning("[device]:object id :[%d] error or device is NULL", object_id);
    	return -EPERM;
    }

    /* delete timer task */
    delete_device_timer_task(glb_device_manage[device_id]->timer);

    /* unmount the device from objcet */
    unmount_a_device_from_object(object_id, device_id);

    /* free the device name */
    free(glb_device_manage[device_id]->device_name);

    /* free the device */
    free(glb_device_manage[device_id]);

    /* set the device manage have a idle */
    glb_device_manage[device_id] = NULL;

    return 0;
}


inline int get_device_name(char* out, int out_len, int device_id)
{
    if(check_device_id(device_id) == false)
        return -EPERM;

    if(glb_device_manage[device_id] == NULL)
        return -EPERM;

    strncpy(out, glb_device_manage[device_id]->device_name, out_len);

    return 0;
}

inline int get_device_type(int device_id)
{
    if(check_device_id(device_id) == false)
        return -EPERM;

    if(glb_device_manage[device_id] == NULL)
        return -EPERM;

    return glb_device_manage[device_id]->device_type;
}

inline int get_device_protocol(int device_id)
{
    if(check_device_id(device_id) == false)
        return -EPERM;


    if(glb_device_manage[device_id] == NULL)
        return -EPERM;

    return glb_device_manage[device_id]->object_type;
}


inline int get_device_addr(unsigned char* addr, unsigned int addr_len, int device_id)
{
    if(check_device_id(device_id) == false)
        return -EPERM;

    if(glb_device_manage[device_id] == NULL)
        return -EPERM;

    if(addr_len > 4)
        return -EPERM;

    memcpy(addr, glb_device_manage[device_id]->device_addr, addr_len);

    return 0;
}

inline timer_task_t* get_device_timer(int device_id)
{
    if(check_device_id(device_id) == false)
        return NULL;

    if(glb_device_manage[device_id] == NULL)
        return NULL;

    return glb_device_manage[device_id]->timer;
}


inline struct device_profile* get_device_private(int device_id)
{
    if(check_device_id(device_id) == false)
        return NULL;

    if(glb_device_manage[device_id] == NULL)
        return NULL;

    return glb_device_manage[device_id]->device_private;
}

inline int get_device_private_numbers(int device_id)
{
    return glb_device_manage[device_id]->device_private_numbers;
}

inline int get_device_object_id(int device_id)
{
    if(check_device_id(device_id) == false)
        return -EPERM;

    if(glb_device_manage[device_id] == NULL)
        return -EPERM;

    return glb_device_manage[device_id]->object_id;
}


inline int get_device_factory_name(int device_id)
{
    if(check_device_id(device_id) == false)
        return -EPERM;

    if(glb_device_manage[device_id] == NULL)
        return -EPERM;

    return glb_device_manage[device_id]->factory_name;
}

inline int get_device_retransmission(int device_id)
{
    if(check_device_id(device_id) == false)
        return -EPERM;

    if(glb_device_manage[device_id] == NULL)
        return -EPERM;

    return glb_device_manage[device_id]->retransmission;
}

inline int get_device_timeout_ms(int device_id)
{
    if(check_device_id(device_id) == false)
        return -EPERM;

    if(glb_device_manage[device_id] == NULL)
        return -EPERM;

    return glb_device_manage[device_id]->time_out;
}

inline int get_device_address_len(int device_id)
{
    if(check_device_id(device_id) == false)
        return -EPERM;

    if(glb_device_manage[device_id] == NULL)
        return -EPERM;

    return glb_device_manage[device_id]->device_addr_len;
}

device_management_t* get_device_management(int device_id)
{
    if(check_device_id(device_id) == false)
        return NULL;

    if(glb_device_manage[device_id] == NULL)
        return NULL;

    return glb_device_manage[device_id];
}


int device_managemnt_init(void)
{
    int error = 0;
    int i = 0;

    /* static global varialble initilize */
    error = pthread_mutex_init(&device_management_lock, NULL);
    if(error)
    {
        syslog_warning("[device]: device management fail");
        return error;
    }

    /* initilize the device management pointer */
    for(i=0; i<RS485_DEVICE_MAX_NUMBERS; i++)
    {
        glb_device_manage[i] = NULL;
    }

    /* initilize the curtain factory save */
    for(i=0; i<RS485_DEVICE_CLASS_MAX_SAVED; i++)
    {
        glb_device_class[i] = -1;
    }

    /* TODO: others device init */

    return error;
}


int set_read_device_information(const read_device_return_t* info, int device_id)
{
    if(info == NULL)
        return -EINVAL;

    if(check_device_id(device_id) == false)
        return -EPERM;

    pthread_mutex_lock(&device_management_lock);
    memcpy(glb_device_manage[device_id]->device_info, info, sizeof(read_device_return_t));
    pthread_mutex_unlock(&device_management_lock);

    return 0;
}

int get_read_device_information(read_device_return_t* out, int device_id)
{
    int error = 0;

    if(out == NULL)
        return -EINVAL;

    if(check_device_id(device_id) == false)
        return -EPERM;

    error = pthread_mutex_trylock(&device_management_lock);
    if(error)
        return -EBUSY;
    memcpy(out, glb_device_manage[device_id]->device_info, sizeof(read_device_return_t));
    pthread_mutex_unlock(&device_management_lock);

    return 0;
}


