/*
 * =====================================================================================
 *
 *       Filename:  signal.c
 *
 *    Description:  The daemon signal handle
 *
 *        Version:  1.0
 *        Created:  09/19/2016 03:43:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wang@enno.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <pthread.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


static sigset_t				mask;


struct signal_handle_instance
{
    int     signal;
    void    *arg;
    void (*handle)(void* arg);
};

struct signal_handle_instance handle_register[] =
{
    {SIGHUP,    NULL, NULL},
    {SIGTERM,   NULL, NULL},
};




void* signal_handle_pthread(void* arg)
{
	int err, signo;

	while(true)
	{
		err = sigwait(&mask, &signo);
		if(err != 0)
		{
			syslog(LOG_ERR, "signal wait failed");
			exit(1);
		}

		switch(signo)
		{
			case SIGHUP:
				syslog(LOG_INFO, "Re-reading configuration file");
                if(handle_register[0].handle)
                    handle_register[0].handle(handle_register[0].arg);
				break;

			case SIGTERM:
				syslog(LOG_INFO, "got SIGTERM; exiting");
                if(handle_register[1].handle)
                    handle_register[1].handle(handle_register[1].arg);
				exit(0);

			default:
				syslog(LOG_INFO, "unexpected signal %d\n", signo);
		}
	}

}


void sighup_block_all_signal(void)
{
	struct sigaction sa;
	int err = 0;

	/*Restore SIGHUP default and block all signals.*/
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGHUP, &sa, NULL) < 0)
		syslog(LOG_ERR, "can't restore SIGHUP default");
	sigfillset(&mask);
	if((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
	{
		syslog(LOG_ERR, "SIG_BLOCK error");
		exit(1);
	}
}


int signal_handle_register(int signal, void(*handle)(void* arg), void* arg)
{
    if(handle)
        return -EINVAL;

    switch(signal)
    {
        case SIGHUP:
            handle_register[0].arg = arg;
            handle_register[0].handle = handle;
            break;
        case SIGTERM:
            handle_register[1].arg = arg;
            handle_register[1].handle = handle;
            break;

        default:
            return -EPERM;
    }

    return 0;
}
