/*
 * write_property.c
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
#include "syslog/log.h"

#ifndef MAX_PROPERTY_VALUES
#define MAX_PROPERTY_VALUES 			64
#endif

#ifndef RETRANSMISSION_TIMES
#define RETRANSMISSION_TIMES		 	3
#endif

static uint8_t Rx_Buf[MAX_MPDU] = { 0 };

static BACNET_APPLICATION_DATA_VALUE Target_Object_Property_Value[MAX_PROPERTY_VALUES];

static uint8_t Request_Invoke_ID = 0;
static BACNET_ADDRESS Target_Address;
static bool Error_Detected = false;

void MyWritePropertySimpleAckHandler(BACNET_ADDRESS * src, uint8_t invoke_id)
{
	if(address_match(&Target_Address, src) && (invoke_id == Request_Invoke_ID))
	{
		syslog_debug("receive request invoke ID: %d", invoke_id);
	}
}

int bacnet_write_property(const bacnet_write_args_t* args)
{
	bool 				found = false;
	bool				success = false;
	unsigned int 		max_apdu = 0;
	unsigned int		time_out = 1000;  /**500ms*4, 2s*/
	uint16_t 			pdu_len = 0;
	int  				i = 0;
    BACNET_ADDRESS 		src;

	if(args->object_property_value_type > 10)
	{
		Target_Object_Property_Value[i].context_tag =
		        args->object_property_value_type;
		Target_Object_Property_Value[i].context_specific = true;
	}
	else
	{
		Target_Object_Property_Value[i].context_specific = false;
	}
	Target_Object_Property_Value[i].next = NULL;

    /* get the address */
	found = address_bind_request(args->device_id, &max_apdu, &Target_Address);
	if(!found)
	{
		syslog_warning("[bacnet]:have no found address:%d", args->device_id);
		return -1;
	}

    /* have no app parse */
#if 0
	bool status = bacapp_parse_application_data(args->object_property_value_type,
	        args->object_property_value, &Target_Object_Property_Value[i]);
	if(!status)
	{
		/* FIXME: show the expected entry format for the tag */
		syslog_error("[bacnet]: unable to parse the tag value");
		return -1;
	}
#endif
	for(i = 0; i < 2; i++)
	{
		if(Request_Invoke_ID == 0)
		{
			Request_Invoke_ID = Send_Write_Property_Request(args->device_id,
			        args->object_type, args->object_instance,
			        args->object_property, &Target_Object_Property_Value[0],
			        args->object_property_priority,
			        args->object_property_index);
		}
		else if(tsm_invoke_id_free(Request_Invoke_ID))
		{
			success = true;
			break;
		}
		else if(tsm_invoke_id_failed(Request_Invoke_ID))
		{
			tsm_free_invoke_id(Request_Invoke_ID);
			Error_Detected = true;
			/* try again or abort? */
			break;
		}

		/* returns 0 bytes on timeout */
		pdu_len = datalink_receive(&src, &Rx_Buf[0], MAX_MPDU, time_out);

		/* process */
		if(pdu_len)
		{
			npdu_handler(&src, &Rx_Buf[0], pdu_len);
		}
	}

	/*clear the ID*/
	if(success == false)
	{
		tsm_free_invoke_id(Request_Invoke_ID);
	}
	Request_Invoke_ID = 0;

	/*have a error*/
	if(Error_Detected)
		return -1;


	return 0;
}
