/**
 * www.enno.com
 *
 * @file :	service.h
 *
 * @date : 	Mar 15, 2016
 *
 * @author: wong
 *
 *
 */

#ifndef INCLUDE_SERVICE_H_
#define INCLUDE_SERVICE_H_

#include <pthread.h>

#include <stdbool.h>

#include "adapter.h"

/* ----------------------------------------- */
/** @addtogroup service     Service management
 *  @ingroup management
 *
 *  Functions to rs485 server have offer the service.
 *
 *
 *  @{
 */

/* -----------------------------------------*/
/**
 * @brief define the thread pool struct
 */
/* -----------------------------------------*/
typedef struct
{
    /** the thread Id */
    pthread_t*                  thread;
    /** the thread addr argument */
    pthread_attr_t*             attr;
    /** the thread service function */
    void*                       (*function)(void* arg);
    /** the thread function argument */
    void*                       arg;
    /** the thread create status, It's will used to clean it */
    bool                        thread_status;
}thread_pool_t;


/* -----------------------------------------*/
/**
 * @brief rs485_service_start
 *  The rs485 service start
 *
 * @note : It's should have not return, until have a error
 *
 * @return  0 is success, and others is fail.
 */
/* -----------------------------------------*/
extern int rs485_service_start(void);

/* -----------------------------------------*/
/**
 * @brief rs485_send_msg_to_client
 *  send The message to a client
 *
 * @param[in] clifd         : The client socket descriptor
 * @param[in] buffer        : The data buffer you want to send
 * @param[in] buffer_len    : The data buffer length
 *
 * @return  The send buffer length, If have a error have return negative value.
 */
/* -----------------------------------------*/
extern int rs485_send_msg_to_client(int clifd, void* buffer, int buffer_len);

/* -----------------------------------------*/
/**
 * @brief rs485_recv_msg_from_client
 *  recvieve a message from client
 *
 * @param[in] clifd         : The client socket descriptor
 * @param[out] buffer       : The buffer have recevie data
 * @param[in] buffer_len    : Teh recevie buffer length
 *
 * @return  The receive data length, If have a error have return negative value.
 */
/* -----------------------------------------*/
extern int rs485_recv_msg_from_client(int clifd, void* buffer, int buffer_len);

/* -----------------------------------------*/
/**
 * @brief send_msg_to_adapter
 *  send a message to self,
 *
 * @param[in] adapter       : The message struct
 *
 * @return  0 is success, and others is fail.
 */
/* -----------------------------------------*/
extern int send_msg_to_adapter(const adapter_t* adapter);

/** @} */




#endif /* INCLUDE_SERVICE_H_ */
