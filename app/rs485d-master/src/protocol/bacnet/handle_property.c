/*
 * handle_property.c
 *
 *  Created on: Nov 19, 2015
 *      Author: wong
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>       /* for time */

#include "bacdef.h"
#include "config.h"
#include "bactext.h"
#include "bacerror.h"
#include "iam.h"
#include "arf.h"
#include "tsm.h"
#include "address.h"
#include "npdu.h"
#include "apdu.h"
#include "datalink.h"
#include "whois.h"
/* some def needed */
#include "rpm.h"
#include "filename.h"
#include "handlers.h"
#include "client.h"
#include "txbuf.h"
#include "dlenv.h"

#include <pthread.h>

#include "protocol/bacnet/handle_property.h"
#include "protocol/bacnet/read_property.h"
#include "protocol/bacnet/write_property.h"
#include "protocol/bacnet/device_client.h"
#include "adapter.h"
#include "syslog/log.h"

#ifndef MAX_PROPERTY_VALUES
#define MAX_PROPERTY_VALUES 64
#endif


/* global stame*/
extern int glb_config_bacnet_object_instance;


extern void My_Read_Property_Ack_Handler(uint8_t * service_request,
        uint16_t service_len, BACNET_ADDRESS * src,
        BACNET_CONFIRMED_SERVICE_ACK_DATA * service_data);
extern void MyWritePropertySimpleAckHandler(BACNET_ADDRESS * src,
        uint8_t invoke_id);
extern void My_Read_Property_Multiple_Ack_Handler(uint8_t * service_request,
        uint16_t service_len, BACNET_ADDRESS * src,
        BACNET_CONFIRMED_SERVICE_ACK_DATA * service_data);




static void MyErrorHandler(BACNET_ADDRESS * src, uint8_t invoke_id,
        BACNET_ERROR_CLASS error_class, BACNET_ERROR_CODE error_code)
{

}

static void MyAbortHandler(BACNET_ADDRESS * src, uint8_t invoke_id,
        uint8_t abort_reason,
        bool server)
{

}

static void MyRejectHandler(BACNET_ADDRESS * src, uint8_t invoke_id,
        uint8_t reject_reason)
{

}


static void Init_Service_Handlers(
    void)
{
    Device_Init(NULL);
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_WHO_IS, handler_who_is);

    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_I_AM, handler_i_am_bind);

    apdu_set_unrecognized_service_handler_handler
        (handler_unrecognized_service);

    apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_PROPERTY,
        handler_read_property);

    /* handle the data coming back from confirmed requests */
    apdu_set_confirmed_ack_handler(SERVICE_CONFIRMED_READ_PROPERTY,
        My_Read_Property_Ack_Handler);

    apdu_set_confirmed_ack_handler(SERVICE_CONFIRMED_READ_PROP_MULTIPLE,
        My_Read_Property_Multiple_Ack_Handler);

    apdu_set_confirmed_simple_ack_handler(SERVICE_CONFIRMED_WRITE_PROPERTY,
        MyWritePropertySimpleAckHandler);
    /* handle any errors coming back */
    apdu_set_error_handler(SERVICE_CONFIRMED_READ_PROPERTY, MyErrorHandler);
    apdu_set_error_handler(SERVICE_CONFIRMED_WRITE_PROPERTY, MyErrorHandler);
    apdu_set_abort_handler(MyAbortHandler);
    apdu_set_reject_handler(MyRejectHandler);
}


static int enno_dlenv_init(const object_management_t* object)
{
    /* set max info framendeclas */
    dlmstp_set_max_info_frames(1);

    /* set the master device */
    if(object->mount_device_max< 127)
        dlmstp_set_max_master(object->mount_device_max);
    else
        dlmstp_set_max_master(127);

    /* set the port baud rate */
    dlmstp_set_baud_rate(object->port.baud_rate);

    /* set the mac address */
    dlmstp_set_mac_address(object->address[0]);

    /* set the bacnet apdu timeout */
    apdu_timeout_set(60000);

    /* set the port infterface */
    if(datalink_init((char*)object->port.interface_name)
            == false)
    {
        syslog_error("BACnet datalink initialize fail");
        return -EPERM;
    }

    return dlenv_register_as_foreign_device();

}


int bacnet_service_init(object_management_t* object)
{
    Device_Set_Object_Instance_Number(glb_config_bacnet_object_instance);
    address_init();
    Init_Service_Handlers();
    enno_dlenv_init(object);
    atexit(datalink_cleanup);

    return 0;
}



