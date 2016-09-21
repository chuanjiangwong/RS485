
/*
 * =====================================================================================
 *
 *       Filename:  daemon.h
 *
 *    Description:  daemon module, for Linux
 *
 *        Version:  1.0
 *        Created:  Sep 19, 2016 4:38:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wang@enno.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */
#ifndef SDK_INCLUDE_WDAEMON_H_
#define SDK_INCLUDE_WDAEMON_H_

#include <stdbool.h>



/* ------------------------------------------------------------------*/
/**
 * @brief init a process daemonzie
 *
 * @param[in] pid_file :        if you have single instance need a pid file
 * @param[in] daemon_name :     The daemon name
 * @param[in] single_instance : whether single instance or not
 *
 * @returns  0 is success, others is fail.
 */
/* ------------------------------------------------------------------*/
extern int daemon_init(const char* pid_file,
		const char* daemon_name,
		bool single_instance);


/* ------------------------------------------------------------------*/
/**
 * @brief The daemon signale call back function
 *
 * @param[in] signal :      The signal number
 * @param[in] handle :      The signal handle function
 * @param[in] arg :         The signal handle function argument
 *
 * @returns  0 is success, others is fail.
 */
/* ------------------------------------------------------------------*/
extern int signal_handle_register(int signal,
		void(*handle)(void* arg),
		void* arg);


#endif /* SDK_INCLUDE_WDAEMON_H_ */
