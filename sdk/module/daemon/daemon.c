/*
 * =====================================================================================
 *
 *       Filename:  daemon.c
 *
 *    Description:  daemon module, for Linux
 *
 *        Version:  1.0
 *        Created:  09/19/2016 03:37:01 PM
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


#define LOCKFILE 			"/var/run/daemon.pid"
#define LOCKMODE 			(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


extern void* signal_handle_pthread(void* arg);
extern void sighup_block_all_signal(void);



static void daemonize(const char* cmd)
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


static int already_running(const char* lock_pid)
{
	int 			fd;
	char 			buf[16];

	fd = open(lock_pid, O_RDWR | O_CREAT, LOCKMODE);
	if(fd < 0)
	{
		syslog(LOG_ERR, "can't open %s: %s", lock_pid, strerror(errno));
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



int daemon_init(const char* pid_file,
		const char* daemon_name,
		bool single_instance)
{
	int				err;
	pthread_t		tid;
	const char			*cmd = "daemon";
	const char			*lock_file = LOCKFILE;

	if(daemon_name)
		cmd = daemon_name;

	if(pid_file)
		lock_file = pid_file;

	/*become a daemon.*/
	daemonize(cmd);

	/*make sure only one copy of the daemon is running.*/
	if(single_instance)
	{
		if (already_running(lock_file))
		{
			syslog(LOG_ERR, "daemon already running");
			exit(1);
		}
	}

	/*Restore SIGHUP default and block all signals.*/
	sighup_block_all_signal();

	/*create a thread to handle SIGHUP and SIGTERM.*/
	if((err = pthread_create(&tid, NULL, signal_handle_pthread, 0)) !=0 )
	{
		syslog(LOG_ERR, "can't create thread");
		exit(1);
	}

	return 0;
}
