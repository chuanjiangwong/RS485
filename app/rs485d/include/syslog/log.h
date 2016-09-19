/**
 * www.enno.com
 *
 * @file :	log.h
 *
 * @date : 	Mar 15, 2016
 *
 * @author: wong
 *
 *
 */

#ifndef INCLUDE_SYSLOG_LOG_H_
#define INCLUDE_SYSLOG_LOG_H_



#ifdef CONFIG_APP_DEBUG_TO_LINUX_SYSLOG
#include <syslog.h>
#define syslog_error(_fmt_, ...)    syslog(LOG_ERR, _fmt_",file:%s,line:%d", ##__VA_ARGS__, __FILE__, __LINE__)
#define syslog_warning(_fmt_, ...)  syslog(LOG_WARNING, _fmt_",file:%s,line:%d", ##__VA_ARGS__, __FILE__, __LINE__)
#define syslog_info(_fmt_, ...)     syslog(LOG_INFO, _fmt_, ##__VA_ARGS__)
//#define syslog_debug(_fmt_, ...)    syslog(LOG_DEBUG, _fmt_, ##__VA_ARGS__)
#define syslog_debug(_fmt_, ...)
#define syslog_format(_fmt_, ...)
#endif


#ifdef CONFIG_APP_DEBUG_TO_TERMINAL
#define syslog_error(_fmt_, ...)    printf(_fmt_",file:%s,line:%d\n", ##__VA_ARGS__, __FILE__, __LINE__)
#define syslog_warning(_fmt_, ...)  printf(_fmt_",file:%s,line:%d\n", ##__VA_ARGS__, __FILE__, __LINE__)
#define syslog_info(_fmt_, ...)     printf(_fmt_"\n", ##__VA_ARGS__)
#define syslog_debug(_fmt_, ...)    printf(_fmt_"\n", ##__VA_ARGS__)
#define syslog_format               printf
#endif


#ifdef CONFIG_APP_DEBUG_DISABLE
#define syslog_error(...)
#define syslog_warning(...)
#define syslog_info(...)
#define syslog_debug(...)
#define syslog_format(...)
#endif


#endif /* INCLUDE_SYSLOG_LOG_H_ */
