/**
 * www.enno.com
 *
 * @file :	log.h
 *
 * @data : 	Mar 15, 2016
 *  
 * @author: wong
 *
 *
 */

#ifndef INCLUDE_SYSLOG_LOG_H_
#define INCLUDE_SYSLOG_LOG_H_

#include <syslog.h>

#if 0
#define syslog_error(_fmt_, ...)   syslog(LOG_ERR, _fmt_",file:%s,line:%d", ##__VA_ARGS__, __FILE__, __LINE__)
#define syslog_warning(_fmt_, ...) syslog(LOG_WARNING, _fmt_",file:%s,line:%d", ##__VA_ARGS__, __FILE__, __LINE__)
#define syslog_info(_fmt_, ...)    syslog(LOG_INFO, _fmt_, ##__VA_ARGS__)
#endif

#define syslog_error(_fmt_, ...)   printf(_fmt_",file:%s,line:%d\n", ##__VA_ARGS__, __FILE__, __LINE__)
#define syslog_warning(_fmt_, ...) printf(_fmt_",file:%s,line:%d\n", ##__VA_ARGS__, __FILE__, __LINE__)
#define syslog_info(_fmt_, ...)    printf(_fmt_"\n", ##__VA_ARGS__)


#endif /* INCLUDE_SYSLOG_LOG_H_ */
