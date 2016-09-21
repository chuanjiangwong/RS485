
/*
 * =====================================================================================
 *
 *       Filename:  timer.h
 *
 *    Description:  offer the timer task
 *
 *        Version:  1.0
 *        Created:  Sep 20, 2016 10:29:59 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wang@enno.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */
#ifndef SDK_INCLUDE_WTIMER_H_
#define SDK_INCLUDE_WTIMER_H_


#include <stdbool.h>

/** The timer task function define */
typedef int (*timer_task_func)(void* arg);

/** define the timer_id type */
typedef void* timer_id;



/* ------------------------------------------------------------------*/
/**
 * @brief start a timer
 *
 * @returns 0 is success, others is fail.
 */
/* ------------------------------------------------------------------*/
extern int timer_start(void);

/* ------------------------------------------------------------------*/
/**
 * @brief stop a timer, delete a timer ptherad
 *
 * @returns 0 is success, others is fail.
 */
/* ------------------------------------------------------------------*/
extern int timer_stop(void);

/* ------------------------------------------------------------------*/
/**
 * @brief delete older timer, and create a new timer
 *
 * @returns 0 is success, others is fail.
 */
/* ------------------------------------------------------------------*/
extern int timer_restart(void);


/* ------------------------------------------------------------------*/
/**
 * @brief register a timer task
 *
 * @param[out] id :     if timer task register success, return the timer id
 * @param[in] prev :    The timer task preset value, The unit is (ms)
 * @param[in] period :  The timer task period, The unit is (ms)
 * @param[in] function :The timer task back call function.
 * @param[in] arg :     The timer task function argument
 * @param[in] enable :  enable a task while create it.
 *
 * @returns  0 is success, others is fail.
 */
/* ------------------------------------------------------------------*/
extern int timer_task_register(timer_id* id,
        unsigned int prev,
        unsigned int period,
        timer_task_func function,
		void* arg,
        bool enable);


/* ------------------------------------------------------------------*/
/**
 * @brief unregister a timer task
 *
 * @param[in] id :  The timer id what you have want to delete it
 *
 * @returns  0 is success,  others is fail.
 */
/* ------------------------------------------------------------------*/
extern int timer_task_unregister(timer_id id);


#endif /* SDK_INCLUDE_WTIMER_H_ */
