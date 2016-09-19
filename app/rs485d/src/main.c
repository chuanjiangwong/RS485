/**
 * www.enno.com
 *
 * @file :	main.c
 *
 * @date : 	Mar 14, 2016
 *  
 * @author: chuanjiang.wong
 *
 *
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

#include "service.h"
#include "syslog/log.h"
#include "rs485.h"


#define LOCKFILE 			"/var/run/rs485.pid"
#define LOCKMODE 			(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


#ifdef CONFIG_APP_DEBUG_TO_LINUX_SYSLOG

static sigset_t				mask;



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
				break;

			case SIGTERM:
				syslog(LOG_INFO, "got SIGTERM; exiting");
				exit(0);

			default:
				syslog(LOG_INFO, "unexpected signal %d\n", signo);
		}
	}

    /** add the thread detach attr */
    pthread_detach(pthread_self());
}


void daemonize(const char* cmd)
{
	int 				i,fd0, fd1, fd2;
	pid_t				pid;
	struct rlimit		r1;
	struct sigaction 	sa;

	/*clear file creation mask.*/
	umask(0);

	/*get maximum number of file descriptors.*/
	if(getrlimit(RLIMIT_NOFILE, &r1) < 0)
		fprintf(stderr, "%s: can't get file limit", cmd);

	/*Become a session leader to loss controlling TTY*/
	if((pid = fork()) < 0)
		fprintf(stderr, "%s; can't fork", cmd);
	else if(pid != 0)  /*parent*/
		exit(0);

	setsid();

	/*Ensure future opens won't allocate controlling TTYs.*/
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGHUP, &sa, NULL) < 0)
		fprintf(stderr, "%s:can't ignore SIGHUP", cmd);
	if((pid = fork()) < 0)
		fprintf(stderr, "%s; can't fork", cmd);
	else if(pid != 0) /*parent*/
		exit(0);

	/*Change the current working directory to the root
	 * so we wong't prevent file systems from being unmounted.
	 */
	if(chdir("/") < 0)
		fprintf(stderr, "%s; can't change directory to /", cmd);

	/*close all open file descriptors.*/
	if(r1.rlim_max == RLIM_INFINITY)
		r1.rlim_max = 1024;
	for(i=0; i<r1.rlim_max; i++)
		close(i);

	/*attach file descriptors 0, 1, and 2 to /dev/null.*/
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	/*initialize the log file*/
	openlog(cmd, LOG_CONS | LOG_PERROR, LOG_DAEMON);
	if(fd0 != 0 || fd1 != 1 || fd2 != 2)
	{
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
		exit(1);
	}
}


static int lockfile(int fd)
{
	struct flock 	file_lock;

	file_lock.l_type = F_WRLCK;
	file_lock.l_start = 0;
	file_lock.l_whence = SEEK_SET;
	file_lock.l_len = 0;

	return fcntl(fd, F_SETLK, &file_lock);
}


static int already_running(void)
{
	int 			fd;
	char 			buf[16];

	fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);
	if(fd < 0)
	{
		syslog(LOG_ERR, "can't open %s: %s", LOCKFILE, strerror(errno));
		exit(1);
	}

	if(lockfile(fd) < 0)
	{
		if(errno == EACCES || errno == EAGAIN)
		{
			close(fd);
			return (1);
		}
		syslog(LOG_ERR, "cat't lock %s: %s", LOCKFILE, strerror(errno));
		exit(1);
	}

	ftruncate(fd, 0);
	sprintf(buf, "%ld", (long)getpid());
	write(fd, buf, strlen(buf)+1);

	return 0;
}
#endif


int main(int argc, char* argv[])
{
#ifdef CONFIG_APP_DEBUG_TO_LINUX_SYSLOG
	int				err;
	pthread_t		tid;
	char			*cmd;
	struct sigaction sa;

	if((cmd = strrchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd ++;


	/*become a daemon.*/
	daemonize(cmd);

	/*make sure only one copy of the daemon is running.*/
	if(already_running())
	{
		syslog(LOG_ERR, "daemon already running");
		exit(1);
	}

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

	/*create a thread to handle SIGHUP and SIGTERM.*/
	if((err = pthread_create(&tid, NULL, signal_handle_pthread, 0)) !=0 )
	{
		syslog(LOG_ERR, "can't create thread");
		exit(1);
	}
#endif

    syslog_info("[main]:RS485 Version: %s, Build Date: "__DATE__" "__TIME__"", APP_PROJECT_VERSION);
	syslog_info("[main]:%s", "rs485 daemonize success...");
	/*Proceed with the rest of the daemon.*/
    rs485_service_start();
    /*TODO*/
	return 0;
}
