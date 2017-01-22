/**
 * www.enno.com
 *
 * @file :	enum.h
 *
 * @date : 	Mar 15, 2016
 *
 * @author: wong
 *
 *
 */

#ifndef INCLUDE_ENUM_H_
#define INCLUDE_ENUM_H_




/** define the timer task thread run status */
/* -----------------------------------------*/
/**
 * @brief define the thread status
 */
/* -----------------------------------------*/
typedef enum
{
    TIMER_TASK_THREAD_STATUS_START,
    TIMER_TASK_THREAD_STATUS_INIT,
    TIMER_TASK_THREAD_STATUS_ADDING,
    TIMER_TASK_THREAD_STATUS_DELETEING,
    TIMER_TASK_THREAD_STATUS_RUNNING,
    TIMER_TASK_THREAD_STATUS_STOP,
    TIMER_TASK_THREAD_STATUS_UNKNOWN,
}timer_task_thread_status_enum;


/** define the adapter thread run status */
typedef enum
{
    ADAPTER_THREAD_STATUS_START,
    ADAPTER_THREAD_STATUS_INIT,
    ADAPTER_THREAD_STATUS_RUNNING,
    ADAPTER_THREAD_STATUS_STOP,
    ADAPTER_THREAD_STATUS_UNKNOWN,
}adapter_thread_status_enum;


/** define the object thread run status */
typedef enum
{
    OBJECT_THREAD_STATUS_START,
    OBJECT_THREAD_STATUS_INIT,
    OBJECT_THREAD_STATUS_RUNNING,
    OBJECT_THREAD_STATUS_STOP,
    OBJECT_THREAD_STATUS_UNKNOWN,
}object_thread_status_enum;






/* -----------------------------------------*/
/**
 * @brief define the service type
 */
/* -----------------------------------------*/
/** define the adapter message type */
typedef enum
{
    SERVICE_CREATE_RS485_OBJECT,
    SERVICE_DELETE_RS485_OBJECT,
    SERVICE_MOUNT_DEVICE_TO_OBJECT,
    SERVICE_UNMOUNT_DEVICE_FROM_OBJECT,
    SERVICE_WRITE_VALUE_TO_DEVICE,
    SERVICE_READ_VALUE_FROM_DEVICE,
    SERVICE_SYSTEM_UPDATE_START,
    SERVICE_SYSTEM_UPDATE_STOP,
    SERVICE_UNKNOWN,
}rs485_service_type_enum;




/* -----------------------------------------*/
/**
 * @brief define the device type
 */
/* -----------------------------------------*/
/** define the rs485 device type */
typedef enum
{
    RS485_DEVICE_TYPE_AIR_CONDITION,
    RS485_DEVICE_TYPE_CURTAIN,
    RS485_DEVICE_TYPE_FRESH_AIR,
    RS485_DEVICE_TYPE_UNKNOWN,
}rs485_device_type_enum;



/* -----------------------------------------*/
/**
 * @brief define the protocol type
 */
/* -----------------------------------------*/
/** define the rs485 protocol type */
typedef enum
{
    RS485_PROTOCOL_TYPE_BACNET,
    RS485_PROTOCOL_TYPE_MODBUS,
    RS485_PROTOCOL_TYPE_GENERAL,
    RS485_PROTOCOL_TYPE_UNKNOWN,
}rs485_protocol_type_enum;


typedef enum
{
    RS485_PORT_TYPE_NONE,
    RS485_PORT_TYPE_EVEN,
    RS485_PORT_TYPE_ODD,
}rs485_port_parity_type_enum;


/* -----------------------------------------*/
/**
 * @brief define the support information
 * @note: This namespace is :
 *      prefix_deviceType_depend_factory_series
 */
/* -----------------------------------------*/
/** define the factory name */
typedef enum
{
#if 0
    RS485_FACTORY_YORK,
    RS485_FACTORY_PANASONNIC,
    RS485_FACTORY_DAIKIN_DTA116A621,
    RS485_FACTORY_DOYA,
    RS485_FACTORY_AOKE,
    RS485_FACTORY_LORELEY,
    RS485_FACTORY_HITACHI_VRV_MANAGER,
    RS485_FACTORY_JOHNSON_CONTROLS_FOR_YORK,
    RS485_FACTORY_UNKNOWN,
#endif

    RS485_CURTAIN_CUSTOMIZE_DOOYA_ALL,
    RS485_CURTAIN_CUSTOMIZE_AOKE_ALL,
    RS485_FRESHAIR_CUSTOMIZE_LORELEY_UNKNOWN_GUANGZHOU,
    RS485_AIRCONDITION_CUSTOMIZE_PANASONNIC_UNKNOWN_SHENZHEN,
    RS485_AIRCONDITION_BACNET_YORK_KELONG_QINGDAO_BOX,
    RS485_AIRCONDITION_MODBUS_YORK_JOHNSON_CONTROLS_BOX,
    RS485_AIRCONDITION_MODBUS_DAIKIN_DTA116A621_BOX,
    RS485_AIRCONDITION_MODBUS_HITACHI_VRV_ALL_BOX,
    RS485_AIRCONDITION_MODBUS_MISTSUBISHI_MAC_CCS_01M,

}rs485_factory_name_enum;


typedef enum
{
    MODBUS_FUNCTION_CODE_WRITE_SIGNLE_COIL,
    MODBUS_FUNCTION_CODE_READ_SIGNLE_COIL,
    MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_COILS,
    MODBUS_FUNCTION_CODE_READ_MULTIPLE_COILS,
    MODBUS_FUNCTION_CODE_READ_INPUT_BIT,
    MODBUS_FUNCTION_CODE_READ_INPUT_MULTIPLE_BITS,

    MODBUS_FUNCTION_CODE_WRITE_SIGNLE_REGISTER,
    MODBUS_FUNCTION_CODE_READ_SIGNLE_REGISTER,
    MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_REGISTERS,
    MODBUS_FUNCTION_CODE_READ_MULTIPLE_REGISTERS,
    MODBUS_FUNCTION_CODE_READ_INPUT_REGISTER,
    MOSBUS_FUNCTION_CODE_READ_INPUT_MULTIPLE_REGISTERS,

    MODBUS_FUNCTION_CODE_DO_NOTHING,
}modbus_function_code_enum;


typedef enum
{
    BACNET_FUNCTION_CODE_WRITE_PROPERTY,
    BACNET_FUNCTION_CODE_READ_PROPERTY,
    BACNET_FUNCTION_CODE_WRITE_MULTIPLE_PROPERTY,
    BACNET_FUNCTION_CODE_READ_MULTIPLE_PROPERTY,

    BACNET_FUNCTION_CODE_DO_NOTHING,
}bacnet_function_code_enum;


/* -----------------------------------------*/
/**
 * device method define
 */
/* -----------------------------------------*/

/** define the device support method */
typedef enum
{
    /* air conddition */
	RS485_AIR_SET_TEMP = 10,
	RS485_AIR_SET_TEMP_18 = 18,						//18 C
	RS485_AIR_SET_TEMP_19 = 19,						//19 C
	RS485_AIR_SET_TEMP_20 = 20,						//20 C
	RS485_AIR_SET_TEMP_21 = 21,						//21 C
	RS485_AIR_SET_TEMP_22 = 22,						//22 C
	RS485_AIR_SET_TEMP_23 = 23,						//23 C
	RS485_AIR_SET_TEMP_24 = 24,						//24 C
	RS485_AIR_SET_TEMP_25 = 25,						//25 C
	RS485_AIR_SET_TEMP_26 = 26,						//26 C
	RS485_AIR_SET_TEMP_27 = 27,						//27 C
	RS485_AIR_SET_TEMP_28 = 28,						//28 C
	RS485_AIR_SET_TEMP_29 = 29,						//29 C
	RS485_AIR_SET_TEMP_30 = 30,                     //30 C

	RS485_AIR_SWING = 40,
	RS485_AIR_SWING_AUTO = 41,						//wind direction auto
	RS485_AIR_SWING_UP_DOWN = 42,
	RS485_AIR_SWING_LEFT_RIGHT = 43,
	RS485_AIR_SWING_UP_DOWN_LEFT_RIGHT = 44,

	RS485_AIR_FAN = 50,
	RS485_AIR_FAN_AUTO = 51,						//wind rate auto
	RS485_AIR_FAN_HIGH = 52,						//wind rate	high
	RS485_AIR_FAN_MIDDLE = 53,						//wind rate middle
	RS485_AIR_FAN_LOW = 54,							//wind rate low

	RS485_AIR_MODE = 60,
	RS485_AIR_MODE_FANING = 61,						//send wind mode
	RS485_AIR_MODE_HEATING = 62,					//doing heat mode
	RS485_AIR_MODE_COOLING = 63,				    //doing cold mode
	RS485_AIR_MODE_DRYING = 64,					    //doing dry mode
	RS485_AIR_MODE_AUTOING = 65,					//doing auto mode

	RS485_AIR_SWITCH = 70,
	RS485_AIR_OFF = 71,								//off
	RS485_AIR_ON = 72,								//on
	RS485_AIR_RESTART = 73,

    RS485_AIR_GET_DEVICE_INFO = 81,


	/* curtain */
    RS485_CURTAIN = 100,
	RS485_CURTAIN_OPEN = 101,
	RS485_CURTAIN_CLOSE = 102,
	RS485_CURTAIN_SET_PERCENT = 103,
	RS485_CURTAIN_RESET = 104,
    RS485_CURTAIN_GET_DEVICE_INFO = 105,

	/*fresh air*/
    RS485_FRESH_AIR = 200,
	RS485_FRESH_AIR_AUTO_ON = 201,
	RS485_FRESH_AIR_AUTO_OFF = 202,
	RS485_FRESH_AIR_RESET = 203,
    RS485_FRESH_AIR_GET_DEVICE_INFO = 204,

}rs485_device_method_enum;





/** define the unused mac */
#define UNUSED(x)   UNUSED_ ## x __attribute__((unused))





#endif /* INCLUDE_ENUM_H_ */
