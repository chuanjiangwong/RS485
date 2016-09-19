/*
 * handle_property.h
 *
 *  Created on: Nov 19, 2015
 *      Author: wong
 */

#ifndef ENNO_BACNET_INCLUDE_BACNET_SERVICE_HANDLE_PROPERTY_H_
#define ENNO_BACNET_INCLUDE_BACNET_SERVICE_HANDLE_PROPERTY_H_

#include "enum.h"
#include "object.h"

/* ----------------------------------------- */
/** @addtogroup bacnet_handle_property  Handle_property
 *  @ingroup bacnet
 *
 *  Function of BACnet handle property.
 *
 *
 *  @{
 */
/* -----------------------------------------*/

/* -----------------------------------------*/
/**
 * @brief bacnet write arg struct
 */
/* -----------------------------------------*/
typedef struct
{
    /** device id, This device id is BACnet instance id */
	unsigned int 			device_id;
    /** bacnet object type */
	int						object_type;
    /** bacnet object instance */
	int						object_instance;
    /** bacnet object property */
	int						object_property;
    /** bacnet property priority default is 16 */
	int						object_property_priority;
    /** bacnet property index, have no index is -1 */
	unsigned int			object_property_index;
    /** bacnet property value type */
	int						object_property_value_type;
    /** bacnet property value */
	char					object_property_value[32];
}bacnet_write_args_t;


#define BACNET_READ_ARGS_OBJECT_MAX			10
/* -----------------------------------------*/
/**
 * @brief bacnet read property struct
 */
/* -----------------------------------------*/
typedef struct
{
    /** device id, This device id is BACnet instance id */
	unsigned int 			device_id;
    /** read object property numbers */
	int						object_numbers;
    /** read object property type array */
	int						object_type[BACNET_READ_ARGS_OBJECT_MAX];
    /** read object property instance */
	int						object_instance[BACNET_READ_ARGS_OBJECT_MAX];
    /** read object property */
	int						object_property[BACNET_READ_ARGS_OBJECT_MAX];
}bacnet_read_args_t;



/** @} */


#endif /* ENNO_BACNET_INCLUDE_BACNET_SERVICE_HANDLE_PROPERTY_H_ */
