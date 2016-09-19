/*
 * =====================================================================================
 *
 *          @file:  item_config.h
 *
 *    Description:  The pannoS config interface
 *
 *        Version:  1.0
 *        Created:  Apr 11, 2016 8:24:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_ITEM_CONFIG_H_
#define INCLUDE_ITEM_CONFIG_H_

#include "enum.h"
#include "adapter.h"

/* ----------------------------------------- */
/** @addtogroup item    Item management
 *  @ingroup management
 *
 *  This moduble have offter the item configure.
 *
 *  <pre>
 *      Default config is : user defiend protocol,
 *
 *      The different item have a different device, so you need to configure it.
 *
 *  </pre>
 *  @{
 */


/** The pannoS item define , if have no define it, This information have write by client. */
#define PANNO_S_ITEM_CONFIG


/* -----------------------------------------*/
/**
 * @brief panno_s_item_config
 *  This function is offter the pannoS item config
 *
 * @param[in,out] adapter   : The adapter struct, have write some device information in it.
 * @param[in] device_type   : The device type, air condition/curtain/fresh air/...
 * @param[in] device_addr   : The device addr, just for pannoS KNX 1 byte address.
 */
/* -----------------------------------------*/
extern void panno_s_item_config(adapter_t* adapter,
        rs485_device_type_enum device_type,
        unsigned char device_addr);

/*
 ****************************************************
 * config the panno_s item
 ****************************************************
 */

/** The pannoS default item confiure */
#define PANNO_S_ITEM_DEFAULT                        (1)

/** The Chengdu wenrude item configure */
#define PANNO_S_ITEM_WENRUDE                        (0)

/** The Chengdu armani item configure */
#define PANNO_S_ITEM_ARMANI                         (0)

/** The Chengdu shaochengguoji item configure */
#define PANNO_S_ITEM_SHAOCHENGGUOJI                 (0)

/** The Chengdu wangjiangmingmen item configure */
#define PANNO_S_ITEM_GATEWAY_HITACHI                (0)

/** The gateway itme johnson controls*/
#define PANNO_S_ITEM_GATEWAY_JOHNSON_CONTROLS       (0)

/** @} */
#endif /* INCLUDE_ITEM_CONFIG_H_ */
