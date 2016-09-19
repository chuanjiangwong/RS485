/**
 * www.enno.com
 *
 * @file :	service.c
 *
 * @date : 	Mar 14, 2016
 *  
 * @author: chuanjiang.wong
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <sys/select.h>

#include "service.h"
#include "syslog/log.h"
#include "adapter.h"
#include "enum.h"
#include "message_queue.h"

/* macro define */
/** define the socket receive buffer length */
#define RECEIVE_BUFFER_LENGTH		    (2048)
/** set the unix domain sinstallocket path */
#ifndef RS485_UNIX_DOMAIN_PATH
#define RS485_UNIX_DOMAIN_PATH          "/home/user/bin/rs485d/rs485_unix_domain_service"
#endif


#define NALLOC                          (10)

/* extern function statement*/
extern void* adapter_thread_function(void* arg);
extern void* timer_task_thread_function(void* arg);

/* the global varial statement*/
extern int glb_config_adapter_message_queue_depth;
extern int glb_config_client_max_numbers;



/* struct define  */
typedef struct
{
    int     fd;
    uid_t   uid;
}client_t;



/* static global variable define */
/** define the socket receive buffer */
static char 							receive_buffer[RECEIVE_BUFFER_LENGTH];
/** define the socket id*/
static int                              socket_fd;
/** define the unix domain socket length */

static unsigned int                     socket_len;
/** define the unix domain socket struct */
static struct sockaddr_un 		        addr;
/** define the unix domain socket path variable , It's max length is 108*/
static char                             unix_domain_path[108] = {0};
/** define the adapter message queue */
static struct message_queue             adapter_message_queue;
/** define the adapter thread */
static pthread_t                        adapter_thread;
/** define the timer task thread */
static pthread_t                        timer_task_thread;
/** define the rs485 service thread pool struct */
static thread_pool_t                    rs485_thread_pool[] =
{
    /* thread */    /* thread_addr */    /* function */             /*argument */       /* thrad staus */
    {&adapter_thread,       NULL,           adapter_thread_function,    &adapter_message_queue, false},
    {&timer_task_thread,    NULL,           timer_task_thread_function, NULL,                   false},
};

static client_t                         *client = NULL;

static int                              client_size = 0;


/* -----------------------------------------*/
/**
 * @brief rs485_thread_pool_create
 *  create linux thread pool
 *
 * @ingroup service
 *
 * @param pool[]    :   The thread pool struct, just statement initialization
 * @param numbers   :   The thread pool array numbers
 *
 * @return 0 is success, others is fial.
 */
/* -----------------------------------------*/
static int rs485_thread_pool_create(thread_pool_t* pool, int numbers)
{
    int i = 0;
    int error = 0;

    if(pool == NULL)
        return -EINVAL;

    for(i=0; i<numbers; i++)
    {
        error = pthread_create(pool[i].thread,
                pool[i].attr,
                pool[i].function,
                pool[i].arg);

        if(error)
            goto error;

        pool[i].thread_status = true;

    }

    return 0;

error:
    return error;
}

/* -----------------------------------------*/
/**
 * @brief rs485_thread_pool_clean
 *  clean the linux thread haved create
 *
 * @ingroup service
 */
/* -----------------------------------------*/
static void rs485_thread_pool_clean(void)
{
    int i = 0;

    for(i=0; i<sizeof(rs485_thread_pool)/sizeof(thread_pool_t); i++)
    {
        if(rs485_thread_pool[i].thread_status)
            pthread_exit(rs485_thread_pool[i].thread);
    }
}


/* -----------------------------------------*/
/**
 * @brief rs485_service_create
 *  create a unix domain socket communicate, used to offer rs485 service
 *
 * @param[out] socket_fd        :   The socket id, have create it.
 * @param[in] unix_domain_path  :   The unix domain socket have bind a file path.
 *
 * @return 0 is success, others is fail.
 */
/* -----------------------------------------*/
static int rs485_service_listen(int* socket_fd, const char* unix_domain_path)
{
    char path[108];
    int ret = 0;

    if(socket_fd == NULL || unix_domain_path == NULL)
    {
        return -EINVAL;
    }

    /* if the unix domain is error path, so use the defalut path */
    if(unix_domain_path[0] != '/')
        strcpy(path, RS485_UNIX_DOMAIN_PATH);
    else
        strncpy(path, unix_domain_path, sizeof(path));

    syslog_info("[service]:server bind path: %s", path);

    /* create a socket */
    *socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(*socket_fd < 0)
    {
        syslog_error("%s", "create socket fail");
        return -EPERM;
    }

    unlink(path);

    /*set socket information*/
    memset(&addr, 0x00, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);
    socket_len = offsetof(struct sockaddr_un, sun_path) + strlen(path);

    /*bind the socket */
	ret = bind(*socket_fd, (struct sockaddr*)&addr, socket_len);
    if(ret < 0)
	{
		syslog_error("%s", "bind error");
		close(*socket_fd);
        unlink(path);
		return -EPERM;
	}

    ret = listen(*socket_fd, glb_config_client_max_numbers);
    if(ret < 0)
    {
        syslog_error("%s", "listen error");
        close(*socket_fd);
        unlink(path);
        return -EPERM;
    }

    return 0;
}

/* -----------------------------------------*/
/**
 * @brief rs485_service_create_clean
 *  have clean the rs485 socket communicate
 *
 * @ingroup service
 */
/* -----------------------------------------*/
void rs485_service_create_clean(void)
{
    if(socket_fd > 0)
    {
        close(socket_fd);
    }
}

int rs485_receive_from_client(int clifd, void* buffer, int buffer_len)
{
    return read(clifd, buffer, buffer_len);
}

int rs485_send_msg_to_client(int clifd, void* buffer, int buffer_len)
{
    return write(clifd, buffer, buffer_len);
}


static int process_client_request(void* buf, int nread, int clifd, int UNUSED(uid))
{
	adapter_t *adapter = NULL;

	/* send the client message to adapter process */
	/* TODO */
	if(nread != sizeof(adapter_t))
	{
		syslog_warning("%s", "The socket receive length is error");
		goto error;
	}

	adapter = message_queue_message_alloc_blocking(&adapter_message_queue);
	memcpy(adapter, buf, nread);
	adapter->socket_fd = clifd;
	message_queue_write(&adapter_message_queue, adapter);

	return 0;

error:
    return -EPERM;
}


int send_msg_to_adapter(const adapter_t* adapter)
{
    adapter_t               *msg;

    if(adapter == NULL)
        return -EPERM;

	msg = message_queue_message_alloc_blocking(&adapter_message_queue);

	memcpy(msg, adapter, sizeof(adapter_t));
	msg->socket_fd = -1;
	message_queue_write(&adapter_message_queue, msg);

    return 0;
}

static void client_alloc(void)
{
    int i;

    if(client == NULL)
        client = malloc(NALLOC * sizeof(client_t));
    else
        client = realloc(client, (client_size + NALLOC)*sizeof(client_t));

    if(client == NULL)
    {
        syslog_error("%s", "client alloc fail");
        return ;
    }

    /* initialize the new entries */
    for(i=client_size; i<client_size + NALLOC; i++)
        client[i].fd = -1;

    client_size += NALLOC;
}


static int client_add(int fd, uid_t uid)
{
    int i;

    if(client == NULL)
        client_alloc();

again:
    for(i=0; i<client_size; i++)
    {
        if(client[i].fd == -1)
        {
            client[i].fd = fd;
            client[i].uid = uid;
            return (i);
        }
    }

    /* client array full, time to realloc for more */
    client_alloc();
    goto again;

    return 0;
}

static void client_del(int fd)
{
    int i;

    for(i=0; i<client_size; i++)
    {
        if(client[i].fd == fd)
            client[i].fd = -1;

        return ;
    }

    syslog_error("can't find client entry for fd:%d", fd);
}


#ifdef client_debug
static void printf_client_file_stat(const struct stat* sb)
{
   switch (sb->st_mode & S_IFMT) 
   {
       case S_IFBLK:  syslog_info("block device");            break;
       case S_IFCHR:  syslog_info("character device");        break;
       case S_IFDIR:  syslog_info("directory");               break;
       case S_IFIFO:  syslog_info("FIFO/pipe");               break;
       case S_IFLNK:  syslog_info("symlink");                 break;
       case S_IFREG:  syslog_info("regular file");            break;
       case S_IFSOCK: syslog_info("socket");                  break;
       default:       syslog_info("unknown?");                break;
   }

   syslog_info("I-node number:            %ld", (long) sb->st_ino);

   syslog_info("Mode:                     %lo (octal)",
           (unsigned long) sb->st_mode);

   syslog_info("Link count:               %ld", (long) sb->st_nlink);
   syslog_info("Ownership:                UID=%ld   GID=%ld",
           (long) sb->st_uid, (long) sb->st_gid);

   syslog_info("Preferred I/O block size: %ld bytes",
           (long) sb->st_blksize);
   syslog_info("File size:                %lld bytes",
           (long long) sb->st_size);
   syslog_info("Blocks allocated:         %lld",
           (long long) sb->st_blocks);

   syslog_info("Last status change:       %s", ctime(&sb->st_ctime));
   syslog_info("Last file access:         %s", ctime(&sb->st_atime));
   syslog_info("Last file modification:   %s", ctime(&sb->st_mtime));
}
#endif

/* client's name can't be older than this (sec) */
#define STALE           (30)
static int serv_accept(int listenfd, uid_t *uidptr)
{
    int                 clifd;
    int                 err, rval;
    socklen_t           len;
    time_t              staletime;
    struct sockaddr_un  un;
    struct stat         statbuf;

    len = sizeof(un);

    clifd = accept(listenfd, (struct sockaddr*)&un, &len);
    if(clifd < 0)
    {
        syslog_error("%s", "accept error");
        return -1;
    }

    /* obtain the client's uid from its calling address */
    len -= offsetof(struct sockaddr_un, sun_path);
    un.sun_path[len] = 0;

    err = stat(un.sun_path, &statbuf);
    if(err < 0)
    {
        syslog_error("%s", "stat error");
        rval = -2;
        goto errout;
    }

#ifdef client_debug
    printf_client_file_stat(&statbuf);
#endif

#ifdef S_ISSOCK
    if(S_ISSOCK(statbuf.st_mode) == 0)
    {
        rval = -3;
        goto errout;
    }
#endif

    if((statbuf.st_mode & (S_IRWXG | S_IRWXO)) ||
            (statbuf.st_mode & S_IRWXU) != S_IRWXU)
    {
        rval = -4;
        goto errout;
    }

    staletime = time(NULL) - STALE;
    if(statbuf.st_atime < staletime ||
            statbuf.st_ctime < staletime ||
            statbuf.st_mtime < staletime)
    {
        rval = -5;
        goto errout;
    }



    if(uidptr != NULL)
        *uidptr = statbuf.st_uid;       /* return uid of caller */

    unlink(un.sun_path);                /* we're done with pathname now */

    return clifd;

errout:
    err = errno;
    close(clifd);
    errno = err;
    return (rval);
}

/* -----------------------------------------*/
/**
 * @brief rs485_service_running
 *  The rs485 service function, It's wait the client requests. It's block
 *
 * @ingroup service
 *
 * @param[in] path  :    The communicate unix path.
 *
 * @return  0 is success, and others is fail.
 */
/* -----------------------------------------*/
static int rs485_service_running(const char* path)
{
    int             i, n, maxfd, maxi, listenfd, clifd, nread;
    uid_t           uid = 0;
    fd_set          rset, allset;

    FD_ZERO(&allset);

    /*  obtain fd to listen for client requests on */
    if(rs485_service_listen(&listenfd, path))
    {
        syslog_error("%s", "listen error");
        goto error;
    }
    FD_SET(listenfd, &allset);
    maxfd = listenfd;
    maxi = -1;

    while(1)
    {
        rset = allset; /*  rset gets modified each time around */
        n = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if(n < 0)
        {
            syslog_error("%s", "select error");
            goto error;
        }

        if(FD_ISSET(listenfd, &rset))
        {
            /* accept new client request */
            clifd = serv_accept(listenfd, &uid);
            if(clifd < 0)
            {
                syslog_error("%s", "accept error");
            }

            i = client_add(clifd, uid);
            FD_SET(clifd, &allset);
            if(clifd > maxfd)
                maxfd = clifd; /* max fd for select */

            if(i > maxi)
            {
                maxi = i;       /* max index in client[] array */
            }

            syslog_info("[service]:new connection: uid:%d, fd:%d", uid, clifd);
            continue;
        }

        for(i=0; i<=maxi; i++)
        {
            if((clifd = client[i].fd) < 0)
                continue;

            if(FD_ISSET(clifd, &rset))
            {
                nread = read(clifd, receive_buffer, sizeof(receive_buffer));
                if(nread < 0)
                {
                    syslog_error("read error on fd:%d", clifd);
                }
                else if(nread == 0)
                {
                    syslog_info("[service]:closed : uid:%d, fd:%d", client[i].uid, clifd);
                    client_del(clifd);
                    FD_CLR(clifd, &allset);
                    close(clifd);
                }
                else
                {
                    /* TODO: process the request */
                    process_client_request(receive_buffer, nread, clifd, client[i].uid);

                }
            }

        }
    }
    return 0;

error:
    return -EPERM;
}

/* -----------------------------------------*/
/**
 * @brief rs485_service_running_clean
 *  Have clean the service running
 *
 * @ingroup service
 */
/* -----------------------------------------*/
static void rs485_service_running_clean(void)
{

}


int rs485_service_start(void)
{
	int 					error = 0;

    /* create the adapter message queue */
    error = message_queue_init(&adapter_message_queue, sizeof(adapter_t), glb_config_adapter_message_queue_depth);
    if(error)
    {
        syslog_error("%s", "adapter message queue create fail");
        return error;
    }

    /* create thread, sisit of adapter and the timer thread */
    error = rs485_thread_pool_create(rs485_thread_pool,
                sizeof(rs485_thread_pool)/sizeof(thread_pool_t));
    if(error)
    {
        syslog_error("%s", "thread pool create fail");
        goto thread_pool_create_fail;
    }

    sleep(1);

    syslog_info("[service]:rs485 service start success...");
    error = rs485_service_running(unix_domain_path);
    if(error)
    {
        syslog_error("%s", "rs485 service runing error");
        goto service_running_fail;
    }

	return 0;


service_running_fail:
    rs485_service_running_clean();
thread_pool_create_fail:
    rs485_thread_pool_clean();

    message_queue_destroy(&adapter_message_queue);

    syslog_error("%s", "system exit...");
    exit(error);
}



