
/*
 * =====================================================================================
 *
 *       Filename:  interface.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Oct 13, 2016 5:22:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#ifndef APP_RS485D_MASTER_SRC_PROTOCOL_CUSTOM_INTERFACE_H_
#define APP_RS485D_MASTER_SRC_PROTOCOL_CUSTOM_INTERFACE_H_


extern int interface_send_a_frame(int fd, const void* frame, int len);
extern int interface_recv_a_frame(int fd, void* frame, int len, unsigned int timeout);

extern int interface_initialize(int *fd, struct interface_profile const * interface);
extern void interface_cleanup(i);


#endif /* APP_RS485D_MASTER_SRC_PROTOCOL_CUSTOM_INTERFACE_H_ */
