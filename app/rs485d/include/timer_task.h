/**
 * www.enno.com
 *
 * @file :	timer_task.h
 *
 * @date : 	Mar 15, 2016
 *
 * @author: wong
 *
 *
 */

#ifndef INCLUDE_TIMER_TASK_H_
#define INCLUDE_TIMER_TASK_H_

#include <stdbool.h>

#include "enum.h"


/* ----------------------------------------- */
/** @addtogroup timer   Timer management
 *  @ingroup management
 *
 *  Functions to create or delete the timer task on timer task thread.
 *
 *
 *  @{
 */


typedef int (*timer_proc_func)(int device_id, int command);

/* -----------------------------------------*/
/**
 * @brief timer task struct
 */
/* -----------------------------------------*/
typedef struct
{
    /** The timer tick time, sleep every tick */
    unsigned int                tick;
    /** The timeout time, when the tick >= timeout, process */
    unsigned int                timeout;
    /** The timer task function, timeout have call it. */
    timer_proc_func             function;
    /** The function argument */
    int                         device_id;
    /** The get the device information */
    int                         command;
}timer_task_t;



/* -----------------------------------------*/
/**
 * @brief timer_task_thread_function
 *  The timer task therad start function, just return when the have a error
 *
 * @param[in] arg : The thread argument, unsed.
 *
 * @return : The thread return value.
 *
 * FIXME : The thread join status, have no set.
 */
/* -----------------------------------------*/
extern void* timer_task_thread_function(void* arg);

/* -----------------------------------------*/
/**
 * @brief create_deivce_timer_task
 *  create a device timer task , The timer task min tick is 10 second
 *
 * @ingroup timer_task
 *
 * @param[in] task : The timer task sturct.
 *
 * @return  0 is success , others is fail.
 *
 * @note    The task argument, have used by timer list, so The task struct
 *      you must malloc a buffer
 */
/* -----------------------------------------*/
extern int create_device_timer_task(timer_task_t* task);

/* -----------------------------------------*/
/**
 * @brief delete_device_timer_task
 *  delete a device timer task from The timer list.
 *
 * @ingroup timer_task
 *
 * @param[in] task :    The timer task sturct, you have remote from the list.
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int delete_device_timer_task(timer_task_t* task);


/* -----------------------------------------*/
/**
 * @brief device_timer_task_handle_demo
 *  timer task handle fucntion demo
 *
 * @param[in] device_id : The device Id.
 * @param[in] command   : Get the device information command ,defined by enum.h
 *
 * @return  0 is success, others is fail.
 */
/* -----------------------------------------*/
extern int device_timer_task_handle_demo(int device_id, int command);

/** @} */

extern int device_timer_task_handle_curtain_init(int device_id, int command);

extern int device_timer_task_handle_curtain_aoke_init(int device_id, int command);

extern int device_timer_task_handle_curtain_doya_init(int device_id, int command);


#endif /* INCLUDE_TIMER_TASK_H_ */
