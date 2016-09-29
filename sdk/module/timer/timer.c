
/*
 * =====================================================================================
 *
 *       Filename:  timer.c
 *
 *    Description:  offer the timer task
 *
 *        Version:  1.0
 *        Created:  Sep 20, 2016 9:20:26 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wang@enno.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#ifdef CONFIG_MODULE_TIMER_TICK_TYPE_SELECT
#include <sys/select.h>
#endif

#include "../../include/klist.h"
#include "wtimer.h"


/* ------------------------------------------------------------------*/
/**
 * @brief The timer struct
 */
/* ------------------------------------------------------------------*/
struct timer_task
{
    /* a task period, ms */
    unsigned int        period;
    /* the task time count */
    unsigned int        tick;
    /* the current timer enalbe status */
    bool                enable;
    /* the list management*/
    struct list_head    list;
    /* the task function handle */
    timer_task_func     function;
    /* the private argument */
    void* 				private;
};


/* a timer pthread */
static pthread_t timer_pthread;

/* a list */
static struct list_head timer_list = LIST_HEAD_INIT(timer_list);

/* The pthread running status */
static bool timer_task_start_status = false;

/* The timer tick percision */
#ifdef CONFIG_MODULE_TIMER_PERCISION_10MS
static int timer_percision = 10;
#endif

#ifdef CONFIG_MODULE_TIMER_PERCISION_50MS
static int timer_percision = 50;
#endif

#ifdef CONFIG_MODULE_TIMER_PERCISION_100MS
static int timer_percision = 100;
#endif

#ifdef CONFIG_MODULE_TIMER_PERCISION_500MS
static int timer_percision = 500;
#endif

#ifdef CONFIG_MODULE_TIMER_PERCISION_1S
static int timer_percision = 1000;
#endif

#ifdef CONFIG_MODULE_TIMER_PERCISION_10S
static int timer_percision = 10000;
#endif

/* ------------------------------------------------------------------*/
/**
 * @brief The timer tick
 *  just choice which tick
 */
/* ------------------------------------------------------------------*/
static void timer_tick(void)
{
#ifdef CONFIG_MODULE_TIMER_TICK_TYPE_SLEEP
    sleep(timer_percision / 1000);
#endif

#ifdef CONFIG_MODULE_TIMER_TICK_TYPE_SELECT
    struct timeval tv;
    int     error = 0;

    tv.tv_sec = timer_percision / 1000;
    tv.tv_usec = (timer_percision % 1000) * 1000;

    do
    {
        error = select(0, NULL, NULL, NULL, &tv);
    }while(error < 0 && error == EINTR);
#endif
}



/* ------------------------------------------------------------------*/
/**
 * @brief The timer ptherad, handle timer task
 *
 * @param[in] arg :     The pthread argument, have no used
 *
 * @returns The pthread return.
 */
/* ------------------------------------------------------------------*/
static void* timer_task_pthread_handle(void* arg)
{
    struct timer_task    *pos = NULL;
    struct timer_task   *node = NULL;

    while(1)
    {
        /* test cancel point */
//        pthread_testcancel();

        /* ticker */
        timer_tick();

        /* if the list is empty, so, continue ticker */
        if(list_empty_careful(&timer_list))
            continue;


        /* if the list isn't empty, handle the task*/
        list_for_each_entry_safe(pos, node, &timer_list, list)
        {
        	if(pos->enable == false)
                continue;

            pos->tick += timer_percision;
            if(pos->tick < pos->period)
                continue;

            pos->tick = 0;
            if(pos->function)
                pos->function(pos->private);
        }
    }

    return NULL;
}


/* ------------------------------------------------------------------*/
/**
 * @brief start a timer
 *
 * @returns 0 is success, others is fail.
 */
/* ------------------------------------------------------------------*/
int timer_start(void)
{
    int error = 0;

    if(timer_task_start_status == true)
        return -EPERM;

    /* create the timer pthread */
    error = pthread_create(&timer_pthread, NULL,
                timer_task_pthread_handle, NULL);
    if(error)
        goto error;

    /* set the pthread is detach */
    pthread_detach(pthread_self());

    /* set timer pthread running status */
    timer_task_start_status = true;

    return 0;

error:
    return error;
}


/* ------------------------------------------------------------------*/
/**
 * @brief stop a timer, delete a timer ptherad
 *
 * @returns 0 is success, others is fail.
 */
/* ------------------------------------------------------------------*/
int timer_stop(void)
{
    int error = 0;
    struct timer_task    *pos = NULL;
    struct timer_task   *node = NULL;

    if(timer_task_start_status == false)
        return 0;

    error = pthread_cancel(timer_pthread);
    if(error)
        return error;

    /* TODO: add to delete the list task */
    list_for_each_entry_safe(pos, node, &timer_list, list)
    {
        list_del(&pos->list);
        free(pos);
    }

    timer_task_start_status = false;

    return 0;
}

int timer_restart(void)
{
    int error = 0;

    error = timer_stop();
    if(error)
        return error;

    sleep(1);

    error = timer_start();
    if(error)
        return error;

    return 0;
}


int timer_task_register(timer_id* id,
        unsigned int prev,
        unsigned int period,
        timer_task_func function,
		void* arg,
        bool enable)
{
    struct timer_task *new_task = NULL;

    if(function == NULL || period == 0)
        return -EINVAL;

    new_task = (struct timer_task*)malloc(sizeof(struct timer_task));
    if(new_task == NULL)
        return -ENOMEM;

    new_task->enable = enable;
    new_task->tick = prev;
    new_task->period = period;
    new_task->function = function;
    new_task->private = arg;
    list_add_tail(&new_task->list, &timer_list);

    *id = new_task;

    return 0;
}


int timer_task_unregister(timer_id id)
{
    struct timer_task *task = (struct timer_task*)id;

    if(task == NULL)
        return -EINVAL;

    list_del(&task->list);
    free(task);

    return 0;
}



