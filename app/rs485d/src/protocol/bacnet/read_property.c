/*
 * read_property.c
 *
 *  Created on: Nov 3, 2015
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

#include "protocol/bacnet/handle_property.h"


static BACNET_READ_ACCESS_DATA 		read_access_data[BACNET_READ_ARGS_OBJECT_MAX];
static BACNET_PROPERTY_REFERENCE 	read_access_data_property[BACNET_READ_ARGS_OBJECT_MAX];


/* needed to filter incoming messages */
static uint8_t 						Request_Invoke_ID = 0;
static BACNET_ADDRESS 				Target_Address;


void My_Read_Property_Ack_Handler(
    uint8_t * service_request,
    uint16_t service_len,
    BACNET_ADDRESS * src,
    BACNET_CONFIRMED_SERVICE_ACK_DATA * service_data)
{

}

/*
void cleanup(
    void)
{
    BACNET_READ_ACCESS_DATA *rpm_object;
    BACNET_READ_ACCESS_DATA *old_rpm_object;
    BACNET_PROPERTY_REFERENCE *rpm_property;
    BACNET_PROPERTY_REFERENCE *old_rpm_property;

    rpm_object = Read_Access_Data;
    old_rpm_object = rpm_object;
    while (rpm_object) {
        rpm_property = rpm_object->listOfProperties;
        while (rpm_property) {
            old_rpm_property = rpm_property;
            rpm_property = rpm_property->next;
            free(old_rpm_property);
        }
        old_rpm_object = rpm_object;
        rpm_object = rpm_object->next;
        free(old_rpm_object);
    }
}
*/

void My_Read_Property_Multiple_Ack_Handler(
    uint8_t * service_request,
    uint16_t service_len,
    BACNET_ADDRESS * src,
    BACNET_CONFIRMED_SERVICE_ACK_DATA * service_data)
{
//    int len = 0;
//    int i;
//
//
//    for(i = 0; i < BACNET_READ_ARGS_OBJECT_MAX; i++)
//	{
//		len = rpm_ack_decode_object_id(service_request, service_len,
//		      &(read_access_data[i]->object_type),
//		      &(read_access_data[i]->object_instance));
//
//		if(len <= 0)
//		{
//			read_access_data[i]->next = NULL;
//			break;
//		}
//
//		service_request += len;
//		service_len -= len;
//
//		len = rmp_ack_decode_object_property(service_request, service_len,
//				&(read_access_data_property[i]->propertyIdentifier),
//				&(read_access_data_property[i]->propertyArrayIndex));
//	}
//
}


static int memcpy_read_args(bacnet_read_args_t* args)
{
	int i;
	static bool init = false;

	if(args->object_numbers <= 0 && args->object_numbers >= BACNET_READ_ARGS_OBJECT_MAX)
		return -1;

	if(init == true)
		return 0;

	for(i=0; i<args->object_numbers; i++)
	{
		read_access_data[i].object_type = args->object_type[i];
		read_access_data[i].object_instance = args->object_instance[i];

		read_access_data[i].listOfProperties = &read_access_data_property[i];
		read_access_data_property[i].propertyIdentifier = args->object_property[i];
		read_access_data_property[i].propertyArrayIndex = BACNET_ARRAY_ALL;
		read_access_data_property[i].next = NULL;

		if(i < args->object_numbers - 1)
		{
			read_access_data[i].next = &read_access_data[i + 1];
		}
		else
		{
			read_access_data[i].next = NULL;
		}
	}

	init = true;

	return 0;
}


int bacnet_read_property(bacnet_read_args_t* args)
{
	unsigned 		max_apdu = 0;
	bool 			found = false;
	uint8_t 		buffer[MAX_PDU] = { 0 };
	int 			i = 0;

	memcpy_read_args(args);

	found = address_bind_request(args->device_id, &max_apdu, &Target_Address);

	for(i = 0; i < 1; i++)
	{
		if(found)
		{
			Request_Invoke_ID = Send_Read_Property_Multiple_Request(&buffer[0],
			        sizeof(buffer), args->device_id, read_access_data);
		}
		else
		{
			printf("have no found  MAC address\n");
			return -1;
		}
	}

	return 0;
}



