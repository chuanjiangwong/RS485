/*
 * =====================================================================================
 *
 *          @file:  rs485.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Apr 21, 2016 5:08:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        @author:  chuanjing.wong, chuanjing.wang@enno.com
 *   Organization:  ENNO, www.enno.com
 *
 * =====================================================================================
 */

#ifndef INCLUDE_RS485_H_
#define INCLUDE_RS485_H_


/**
 * @defgroup management         Management
 *
 * The RS485 service management
 *
 */

/**
 *  @defgroup protocol          Protocol
 *
 *  The rs485 support protocl module
 *
 *
 *
 *
 *  @defgroup bacnet            BACnet
 *  @ingroup protocol
 *
 *  The BACnet protocol
 *
 *
 *
 *
 *  @defgroup modbus            Modbus
 *  @ingroup protocol
 *
 *  The Modbus protocl
 *
 *
 *
 *
 *  @defgroup general           General
 *  @ingroup protocol
 *
 *  The user defined protocl
 *
 */


/**
 *  @defgroup device            Device
 *
 *  The rs485 support device module
 *
 *
 *
 *
 *  @defgroup air_condition     AirConditon
 *  @ingroup device
 *
 *  The air conditon device
 *
 *
 *
 *
 *
 *  @defgroup curtain           Curtain
 *  @ingroup device
 *
 *  The curtain device
 *
 *
 *
 *
 *  @defgroup fresh_air         FreshAir
 *  @ingroup device
 *
 *  The fresh air device
 *
 */

/**
 * @mainpage RS485 SERVER API Documentation
 * This documents the RS485 API, Modbus/BACnet/General, and sample applications. <br>
 *
 *  - The high-level handler interface can be found in the Modules tab.
 *  - Specifics for each file can be found in the Files tab.
 *  - A full list of all functions is provided in the index of the
 *  Files->Globals subtab.
 *
 * While all the central files are included in the file list, not all important
 * functions have been given the javadoc treatment, nor have Modules (chapters)
 * been created yet for all groupings.  If you are doing work in an under-
 * documented area, please add the javadoc comments at least to the API calls,
 * and consider adding doxygen's module grouping for your area of interest.
 *
 * See doc/README.doxygen for notes on building and extending this document. <br>
 * In particular, if you have graphviz installed, you can enhance this
 * documentation by turning on the function call graphs feature.
 *
 *
 * <pre>
 *      Copyright: www.enno.com
 *
 *      Author: chuanjiang.wang
 *      E-mail: chuanjiang.wang@enno.com
 * </pre>
 */


#define RS485_MAJOR_NUM                             (0)
#define RS485_MINOR_NUM                             (31)


#endif /* INCLUDE_RS485_H_ */
