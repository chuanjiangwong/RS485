/*
 * =====================================================================================
 *
 *          @file:  item_config.c
 *
 *    Description:  item_config
 *
 *        Version:  1.0
 *        Created:  Apr 11, 2016 8:23:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <autoconf.h>

#include "item_config.h"
#include "enum.h"
#include "adapter.h"
#include "enumtxt.h"
#include "syslog/log.h"


#ifndef PANNO_S_ITEM_DEFAULT
#define PANNO_S_ITEM_DEFAULT                        (1)
#endif


#ifndef PANNO_S_ITEM_WENRUDE
#define PANNO_S_ITEM_WENRUDE                        (0)
#endif


#ifndef PANNO_S_ITEM_ARMANI
#define PANNO_S_ITEM_ARMANI                         (0)
#endif


#ifndef PANNO_S_ITEM_SHAOCHENGGUOJI
#define PANNO_S_ITEM_SHAOCHENGGUOJI                 (0)
#endif


#ifndef PANNO_S_ITEM_GATEWAY_HITACHI
#define PANNO_S_ITEM_GATEWAY_HITACHI                (0)
#endif


#ifndef PANNO_S_ITEM_GATEWAY_JOHNSON_CONTROLS
#define PANNO_S_ITEM_GATEWAY_JOHNSON_CONTROLS       (0)
#endif


static void _mount_device(adapter_t* adapter, int object_id, rs485_factory_name_enum device, unsigned char addr)
{
    switch(device)
    {
        case RS485_AIRCONDITION_BACNET_YORK_KELONG_QINGDAO_BOX:
            adapter->message_content.mount_device.device_type = RS485_DEVICE_TYPE_AIR_CONDITION;
            adapter->message_content.mount_device.object_id = object_id;
            adapter->message_content.mount_device.object_type = RS485_PROTOCOL_TYPE_BACNET;
            adapter->message_content.mount_device.factory_name = RS485_AIRCONDITION_BACNET_YORK_KELONG_QINGDAO_BOX;
            adapter->message_content.mount_device.time_out = 1000;
            adapter->message_content.mount_device.retransmission = 2;
            adapter->message_content.mount_device.device_status_period = 60 * 5; // 5 min;
            adapter->message_content.mount_device.support_reply = true;
            strncpy(adapter->message_content.mount_device.device_name, "york, GuangZhou kelong",
                    sizeof(adapter->message_content.mount_device.device_name));
            adapter->message_content.mount_device.device_addr[0] = addr;
            adapter->message_content.mount_device.device_addr_len = 1;
            break;

        case RS485_AIRCONDITION_CUSTOMIZE_PANASONNIC_UNKNOWN_SHENZHEN:
            adapter->message_content.mount_device.device_type = RS485_DEVICE_TYPE_AIR_CONDITION;
            adapter->message_content.mount_device.object_id = object_id;
            adapter->message_content.mount_device.object_type = RS485_PROTOCOL_TYPE_GENERAL;
            adapter->message_content.mount_device.factory_name = RS485_AIRCONDITION_CUSTOMIZE_PANASONNIC_UNKNOWN_SHENZHEN;
            adapter->message_content.mount_device.time_out = 500;
            adapter->message_content.mount_device.retransmission = 2;
            adapter->message_content.mount_device.device_status_period = 60 * 5; // 5 min;
            adapter->message_content.mount_device.support_reply= true;
            strncpy(adapter->message_content.mount_device.device_name, "panasonnic, GuangZhou",
                    sizeof(adapter->message_content.mount_device.device_name));
            adapter->message_content.mount_device.device_addr[0] = addr;
            adapter->message_content.mount_device.device_addr_len = 1;
            break;

        case RS485_AIRCONDITION_MODBUS_DAIKIN_DTA116A621_BOX:
            adapter->message_content.mount_device.device_type = RS485_DEVICE_TYPE_AIR_CONDITION;
            adapter->message_content.mount_device.object_id = object_id;
            adapter->message_content.mount_device.object_type = RS485_PROTOCOL_TYPE_MODBUS;
            adapter->message_content.mount_device.factory_name = RS485_AIRCONDITION_MODBUS_DAIKIN_DTA116A621_BOX;
            adapter->message_content.mount_device.time_out = 500;
            adapter->message_content.mount_device.retransmission = 2;
            adapter->message_content.mount_device.device_status_period = 60; // 1 min;
            adapter->message_content.mount_device.support_reply= true;
            strncpy(adapter->message_content.mount_device.device_name, "daikin, ShangHai",
                    sizeof(adapter->message_content.mount_device.device_name));
            adapter->message_content.mount_device.device_addr[0] = addr;
            adapter->message_content.mount_device.device_addr_len = 1;
            break;

        case RS485_CURTAIN_CUSTOMIZE_AOKE_ALL:
            adapter->message_content.mount_device.device_type = RS485_DEVICE_TYPE_CURTAIN;
            adapter->message_content.mount_device.object_id = object_id;
            adapter->message_content.mount_device.object_type = RS485_PROTOCOL_TYPE_GENERAL;
            adapter->message_content.mount_device.factory_name = RS485_CURTAIN_CUSTOMIZE_AOKE_ALL;
            adapter->message_content.mount_device.time_out = 500;
            adapter->message_content.mount_device.retransmission = 2;
            adapter->message_content.mount_device.device_status_period = 15; // 10 s;
            adapter->message_content.mount_device.support_reply= false;
            strncpy(adapter->message_content.mount_device.device_name, "aoke, GuangZhou",
                    sizeof(adapter->message_content.mount_device.device_name));
            adapter->message_content.mount_device.device_addr[0] = addr;
            adapter->message_content.mount_device.device_addr[1] = 0xfe;
            adapter->message_content.mount_device.device_addr[2] = 0xfe;
            adapter->message_content.mount_device.device_addr_len = 3;
            break;

        case RS485_CURTAIN_CUSTOMIZE_DOOYA_ALL:
            adapter->message_content.mount_device.device_type = RS485_DEVICE_TYPE_CURTAIN;
            adapter->message_content.mount_device.object_id = object_id;
            adapter->message_content.mount_device.object_type = RS485_PROTOCOL_TYPE_GENERAL;
            adapter->message_content.mount_device.factory_name = RS485_CURTAIN_CUSTOMIZE_DOOYA_ALL;
            adapter->message_content.mount_device.time_out = 500;
            adapter->message_content.mount_device.retransmission = 2;
            adapter->message_content.mount_device.device_status_period = 15;    //10 s
            adapter->message_content.mount_device.support_reply= true;
            strncpy(adapter->message_content.mount_device.device_name, "dooya, GuangZhou",
                    sizeof(adapter->message_content.mount_device.device_name));
#if 0
            adapter->message_content.mount_device.device_addr[0] = addr;
            adapter->message_content.mount_device.device_addr[1] = 0xfe;
#endif
            adapter->message_content.mount_device.device_addr_len = 2;
            break;

        case RS485_FRESHAIR_CUSTOMIZE_LORELEY_UNKNOWN_GUANGZHOU:
            adapter->message_content.mount_device.device_type = RS485_DEVICE_TYPE_FRESH_AIR;
            adapter->message_content.mount_device.object_id = object_id;
            adapter->message_content.mount_device.object_type = RS485_PROTOCOL_TYPE_GENERAL;
            adapter->message_content.mount_device.factory_name = RS485_FRESHAIR_CUSTOMIZE_LORELEY_UNKNOWN_GUANGZHOU;
            adapter->message_content.mount_device.time_out = 500;
            adapter->message_content.mount_device.retransmission = 2;
            adapter->message_content.mount_device.device_status_period = 60 * 2; // 2 min;
            adapter->message_content.mount_device.support_reply = true;
            strncpy(adapter->message_content.mount_device.device_name, "lereley, SenZhen",
                    sizeof(adapter->message_content.mount_device.device_name));
            adapter->message_content.mount_device.device_addr[0] = addr;
            adapter->message_content.mount_device.device_addr_len = 1;
            break;

        case RS485_AIRCONDITION_MODBUS_HITACHI_VRV_ALL_BOX:
            adapter->message_content.mount_device.device_type = RS485_DEVICE_TYPE_AIR_CONDITION;
            adapter->message_content.mount_device.object_id = object_id;
            adapter->message_content.mount_device.object_type = RS485_PROTOCOL_TYPE_MODBUS;
            adapter->message_content.mount_device.factory_name = RS485_AIRCONDITION_MODBUS_HITACHI_VRV_ALL_BOX;
            adapter->message_content.mount_device.time_out = 500;
            adapter->message_content.mount_device.retransmission = 2;
            adapter->message_content.mount_device.device_status_period = 60 * 5; // 5 min;
            adapter->message_content.mount_device.support_reply= true;
            strncpy(adapter->message_content.mount_device.device_name, "hitachi, QingDao",
                    sizeof(adapter->message_content.mount_device.device_name));
            adapter->message_content.mount_device.device_addr[0] = addr;
            adapter->message_content.mount_device.device_addr_len = 1;
            break;

        case RS485_AIRCONDITION_MODBUS_YORK_JOHNSON_CONTROLS_BOX:
            adapter->message_content.mount_device.device_type = RS485_DEVICE_TYPE_AIR_CONDITION;
            adapter->message_content.mount_device.object_id = object_id;
            adapter->message_content.mount_device.object_type = RS485_PROTOCOL_TYPE_MODBUS;
            adapter->message_content.mount_device.factory_name = RS485_AIRCONDITION_MODBUS_YORK_JOHNSON_CONTROLS_BOX;
            adapter->message_content.mount_device.time_out = 500;
            adapter->message_content.mount_device.retransmission = 2;
            adapter->message_content.mount_device.device_status_period = 60 * 5; // 5 min;
            adapter->message_content.mount_device.support_reply= true;
            strncpy(adapter->message_content.mount_device.device_name, "Johnson, WuXi",
                    sizeof(adapter->message_content.mount_device.device_name));
            adapter->message_content.mount_device.device_addr[0] = addr;
            adapter->message_content.mount_device.device_addr_len = 1;
            break;

        default:
            syslog_warning("have no support device, factory:%d", device);
            break;
    }
}






void panno_s_item_config(adapter_t* adapter,
        rs485_device_type_enum device_type,
        unsigned char device_addr)
{
/* The item config */
#ifdef CONFIG_OLD_ITEM_CONFIG_DEFAULT
  /*
   * The wenrude item have two RS485 CURTAIN and a fresh air device
   * have a dooya, and aoke,
   *
   * so should config the device mac like :
   *
   * ------------------------------------------------------------
   *    MAC configure
   * ------------------------------------------------------------
   *    dooya: start ~ end: 0xef ~ 0xfe(239 ~ 254), curtain
   * ------------------------------------------------------------
   *    aoke:  start ~ end: 0xdf ~ 0xee(223 ~ 238), curtain
   * ------------------------------------------------------------
   *  leorey:  start ~ end: 0xcf ~ 0xde(207 ~ 222), fresh air
   * ------------------------------------------------------------
   *  panasonnic:  start ~ end: 0x01 ~ 0x0f(001 ~ 015), air condition
   * ------------------------------------------------------------
   *
   *
   */
    /* dooya curtain */
    if(device_addr >= 0xef && device_addr <= 0xfe)
    {
        if(device_type != RS485_DEVICE_TYPE_CURTAIN)
        {
            syslog_warning("[config]: device type:(%s) inval, except (curtain)", get_enum_txt_rs485_device_type(device_type));
            return;
        }

        _mount_device(adapter,
                adapter->message_content.mount_device.object_id,
				RS485_CURTAIN_CUSTOMIZE_DOOYA_ALL,
                device_addr);
    }
    /* aoke curtain */
    else if(device_addr >= 0xdf && device_addr <= 0xee)
    {
        if(device_type != RS485_DEVICE_TYPE_CURTAIN)
        {
            syslog_warning("[config]: device type:(%s) inval, except (curtain)", get_enum_txt_rs485_device_type(device_type));
            return;
        }

        _mount_device(adapter,
                adapter->message_content.mount_device.object_id,
				RS485_CURTAIN_CUSTOMIZE_AOKE_ALL,
                device_addr);
    }
    /* leorey fresh air */
    else if(device_addr >= 0xcf && device_addr <= 0xde)
    {
        if(device_type != RS485_DEVICE_TYPE_FRESH_AIR)
        {
            syslog_warning("[config]: device type:(%s) inval, except (fresh air)", get_enum_txt_rs485_device_type(device_type));
            return;
        }

        _mount_device(adapter,
                adapter->message_content.mount_device.object_id,
				RS485_FRESHAIR_CUSTOMIZE_LORELEY_UNKNOWN_GUANGZHOU,
                device_addr);
    }
    /* panasonnic aircondition */
    else if(device_addr >= 0x01 && device_addr <= 0x0f)
    {
        if(device_type != RS485_DEVICE_TYPE_AIR_CONDITION)
        {
            syslog_warning("[config]: device type:(%s) inval, except (air condition)", get_enum_txt_rs485_device_type(device_type));
            return;
        }

        _mount_device(adapter,
                adapter->message_content.mount_device.object_id,
				RS485_AIRCONDITION_CUSTOMIZE_PANASONNIC_UNKNOWN_SHENZHEN,
                device_addr);
    }
#endif

#ifdef CONFIG_OLD_ITEM_CONFIG_AIR_CONDITION_YORK_KELONG
  /*
   * so should config the device mac like :
   *
   * ------------------------------------------------------------
   *    MAC configure
   * ------------------------------------------------------------
   *    york: start ~ end: 0x01 ~ 0xfe(1 ~ 254), air condition
   * ------------------------------------------------------------
   *
   *
   */
    /* YORK curtain */
    if(device_addr >= 0x01 && device_addr <= 0xfe)
    {
        if(device_type != RS485_DEVICE_TYPE_AIR_CONDITION)
        {
            syslog_warning("[config]: device type:(%s) inval, except (air condition)", get_enum_txt_rs485_device_type(device_type));
            return;
        }

        _mount_device(adapter,
                adapter->message_content.mount_device.object_id,
				RS485_AIRCONDITION_BACNET_YORK_KELONG_QINGDAO_BOX,
                device_addr);
    }
#endif

#ifdef CONFIG_OLD_ITEM_CONFIG_AIR_CONDITION_DAIKIN_DTA116A621
  /*
   * The GATEWAY daikin item just have a KNX net
   *
   * so should config the device mac like :
   *
   * ------------------------------------------------------------
   *    MAC configure
   * ------------------------------------------------------------
       daikin: start ~ end: 0x00 ~ 0x3f (0 ~ 63), air condition    (Modbus)
            note:
                日立空调，其适配器拨码开关的地址默认
                设置为（2）, knx配置的空调地址为室内机单台空调的
                地址。
   * ------------------------------------------------------------
   */
    if(device_addr >= 0x00 && device_addr <= 0x3f)
    {
        if(device_type != RS485_DEVICE_TYPE_AIR_CONDITION)
        {
            syslog_warning("[config]: device type:(%s) inval, except (air condition)", get_enum_txt_rs485_device_type(device_type));
            return;
        }

        _mount_device(adapter,
                adapter->message_content.mount_device.object_id,
				RS485_AIRCONDITION_MODBUS_DAIKIN_DTA116A621_BOX,
                device_addr);
    }

#endif

#ifdef CONFIG_OLD_ITEM_CONFIG_AIR_CONDITION_HITACHI_VRV_MANAGER
  /*
   * The GATEWAY HITACHI item just have a KNX net
   *
   * so should config the device mac like :
   *
   * ------------------------------------------------------------
   *    MAC configure
   * ------------------------------------------------------------
       hitachi: start ~ end: 0x00 ~ 0x7f (0 ~ 128), air condition    (Modbus)
            note:
                日立空调，其适配器拨码开关的地址默认
                设置为（2）, knx配置的空调地址为室内机单台空调的
                地址。
   * ------------------------------------------------------------
   */
    if(device_addr >= 0x00 && device_addr <= 0x7f)
    {
        if(device_type != RS485_DEVICE_TYPE_AIR_CONDITION)
        {
            syslog_warning("[config]: device type:(%s) inval, except (air condition)", get_enum_txt_rs485_device_type(device_type));
            return;
        }

        _mount_device(adapter,
                adapter->message_content.mount_device.object_id,
				RS485_AIRCONDITION_MODBUS_HITACHI_VRV_ALL_BOX,
                device_addr);
    }
#endif

#ifdef CONFIG_OLD_ITEM_CONFIG_AIR_CONDITION_YORK_JOHNSON
  /*
   * The GATEWAY Johnson Controls item just have a KNX net
   *
   * so should config the device mac like :
   *
   * ------------------------------------------------------------
   *    MAC configure
   * ------------------------------------------------------------
       johnson: start ~ end: 0x00 ~ 0x7f (0 ~ 64), air condition    (Modbus)
            note:
                YORK，其适配器拨码开关的地址默认
                设置为（2）, knx配置的空调地址为室内机单台空调的
                地址。
   * ------------------------------------------------------------
   */
    if(device_addr >= 0x00 && device_addr <= 0x3f)
    {
        if(device_type != RS485_DEVICE_TYPE_AIR_CONDITION)
        {
            syslog_warning("[config]: device type:(%s) inval, except (air condition)", get_enum_txt_rs485_device_type(device_type));
            return;
        }

        _mount_device(adapter,
                adapter->message_content.mount_device.object_id,
				RS485_AIRCONDITION_MODBUS_YORK_JOHNSON_CONTROLS_BOX,
                device_addr);
    }

#endif


/* The new config, have use the device factory by ETS */
#ifdef CONFIG_NEW_ITEM_CONFIG

    _mount_device(adapter,
            adapter->message_content.mount_device.object_id,
            adapter->message_content.mount_device.factory_name,
            device_addr);

#endif

}



