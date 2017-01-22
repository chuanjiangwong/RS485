
/*
 * =====================================================================================
 *
 *       Filename:  wlog.h
 *
 *    Description:  The sdk log define
 *
 *        Version:  1.0
 *        Created:  Sep 29, 2016 2:52:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), wangchuanjiang1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#ifndef SDK_INCLUDE_WLOG_H_
#define SDK_INCLUDE_WLOG_H_


#ifdef CONFIG_LOG_TO_SYSLOG
#include <syslog.h>

#define wlog_error(_mod_name_, _fmt_, ...) \
    do \
    { \
        printf("[%s]%s" _fmt_ ", func:%s, line:%d\n\r", _mod_name_, " Error: ", ##__VA_ARGS__, __func__, __LINE__);\
        syslog(LOG_ERR, "[%s]" _fmt_ "func:%s, line:%d", _mod_name_, ##__VA_ARGS__, __FUNC__, __LINE__); \
    }while(0)



#define wlog_warning(_mod_name_, _fmt_, ...)                                        \
    do \
    { \
    	printf("[%s]%s" _fmt_ ", func:%s, line:%d\n\r", _mod_name_, " Warn: ", ##__VA_ARGS__, __func__, __LINE__); \
        syslog(LOG_WARNING, "[%s]" _fmt_ "func:%s, line:%d", _mod_name_, ##__VA_ARGS__, __FUNC__, __LINE__); \
    }while(0)

/* General debug function. User can map his own debug functions to this */
#define wlog_debug(_mod_name_, _fmt_, ...)                          \
		syslog(LOG_DUG, "[%s] \n\r" _fmt_ , _mod_name_, ##__VA_ARGS__)


#define wlog_info(_mod_name_, _fmt_, ...)                          \
    do \
    { \
    	printf("[%s] \n\r" _fmt_, _mod_name_, ##__VA_ARGS__); \
        syslog(LOG_INFO, "[%s] " _fmt_ , _mod_name_, ##__VA_ARGS__); \
    }while(0)


#define wlog_entry(_fmt_, ...)                                        \
		printf("> %s (" _fmt_ ")\n\r", __func__, ##__VA_ARGS__)

#define wlog_exit(_fmt_, ...)                                        \
		printf("< %s" _fmt_ "\n\r", __func__, ##__VA_ARGS__)


#else

#include <stdio.h>

#define wlog_error(_mod_name_, _fmt_, ...)                                        \
	printf("[%s] Error: " _fmt_ ", func:%s, line:%d \n\r", _mod_name_, ##__VA_ARGS__, __func__, __LINE__)

#define wlog_warning(_mod_name_, _fmt_, ...)                                        \
	printf("[%s] Warn: " _fmt_ ", func:%s, line:%d \n\r", _mod_name_, ##__VA_ARGS__, __func__, __LINE__)

/* General debug function. User can map his own debug functions to this */
#define wlog_debug(_mod_name_, _fmt_, ...)                          \
	printf("[%s]" _fmt_ "\n\r", _mod_name_, ##__VA_ARGS__)

#define wlog_info(_mod_name_, _fmt_, ...)                          \
	printf("[%s]" _fmt_ "\n\r", _mod_name_, ##__VA_ARGS__)

#define wlog_entry(_fmt_, ...)                                        \
	printf("> %s (" _fmt_ ")\n\r", __func__, ##__VA_ARGS__)

#define wlog_exit(_fmt_, ...)                                        \
	printf("< %s" _fmt_ "\n\r", __func__, ##__VA_ARGS__)

#endif


#endif /* SDK_INCLUDE_WLOG_H_ */
