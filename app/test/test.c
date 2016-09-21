/*
 ============================================================================
 Name        : test.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "wtimer.h"


static int timer_handle(void* arg)
{
    unsigned int *count = (unsigned int*)arg;

    *(count) += 1;

    printf("[handle]: %d\n", *(count));

    return 0;
}

static unsigned int timer_count = 0;

struct timer_task
{
    timer_id        id;
    unsigned int    perv;
    unsigned int    period;
    timer_task_func func;
    void*           arg;
    bool            enable;
};

struct timer_task glb_task[] =
{
    {NULL, 1000, 3000, timer_handle, &timer_count, true},
};



int main(void)
{
    int error = 0;
    unsigned int count = 2;

    puts("This is a test app");

   error = timer_start();
   if(error)
   {
       printf("timer init fail: %d\n", error);
       exit(1);
   }

   error = timer_task_register(&glb_task->id,
           glb_task->perv,
           glb_task->period,
           glb_task->func,
           glb_task->arg,
           glb_task->enable);
   if(error)
   {
       printf("timer ingster fail:%d\n", error);
       exit(1);
   }

   printf("glb_task->id: %08x\n", glb_task[0].id);

   while(1)
   {
       sleep(1);
       printf("sleep %d\n", count++);

       if(count % 10 == 0)
       {
    	   error = timer_task_unregister(glb_task->id);
    	   if(error)
    		   printf("unregister error: %d", error);
       }
       else if(count % 10 == 1)
       {
    	   error = timer_task_register(&glb_task->id,
    	           glb_task->perv,
    	           glb_task->period,
    	           glb_task->func,
    	           glb_task->arg,
    	           glb_task->enable);
    	   if(error)
    	   {
    	       printf("timer ingster fail:%d\n", error);
    	       exit(1);
    	   }
       }

       if(count % 60 == 0)
       {
    	   error = timer_stop();
    	   if(error)
    	   {
    		   printf("timer stop error: %d", error);
    	   }

    	   error = timer_start();
    	   if(error)
    	   {
    		   printf("timer start error: %d", error);
    	   }
       }
   }

	return EXIT_SUCCESS;
}
