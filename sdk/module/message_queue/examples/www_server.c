/*
 * Copyright (c) 2012 Jeremy Pepper
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of message_queue nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This sets up a thread pool to handle blocking file I/O operations. Socket
 * I/O is multiplexed with select in the main thread; socket writes are queued
 * through a message queue, sending a signal if necessary to wake the main
 * thread up if it's waiting in select.
 *
 * So, this example demonstrates two uses of a message queue:
 *   * Distributing work to a thread pool
 *   * Using the actor model to avoid having shared state between threads
 */

/*
 * This is an example of how to use the message queue. This is NOT a
 * production-ready Web server, or even a good example of how to write a Web
 * server. Please ignore the HTTP bits and focus on the message queue.
 */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include "../message_queue.h"

// Forward-declare HTTP handling functions

static int open_http_listener();
static void handle_client_data(int fd);
static void handle_client_request(int fd, char *request);
static void generate_client_reply(int fd, const char *filename);
static int copy_data(int rfd, int fd);

// Message queue related code

#ifndef WORKER_THREADS
#define WORKER_THREADS 32
#endif

struct www_op {
	enum {OP_BEGIN, OP_READ_BLOCK, OP_EXIT} operation;
	const char *filename;
	int rfd, fd;
};

struct io_op {
	char buf[1024];
	int len, pos;
	int fd, rfd;
	int close_pending;
};

static struct message_queue worker_queue;
static struct message_queue io_queue;

static pthread_t main_thread;
static pthread_t worker_threads[WORKER_THREADS];

static int main_blocked;

static void *worker_threadproc(void *dummy) {
	while(1) {
		struct www_op *message = message_queue_read(&worker_queue);
		switch(message->operation) {
		case OP_BEGIN:
			generate_client_reply(message->fd, message->filename);
			break;
		case OP_READ_BLOCK:
			copy_data(message->rfd, message->fd);
			break;
		case OP_EXIT:
			message_queue_message_free(&worker_queue, message);
			return NULL;
		}
		message_queue_message_free(&worker_queue, message);
	}
	return NULL;
}

static void threadpool_init() {
	message_queue_init(&worker_queue, sizeof(struct www_op), 512);
	for(int i=0;i<WORKER_THREADS;++i) {
		pthread_create(&worker_threads[i], NULL, &worker_threadproc, NULL);
	}
}

static void threadpool_destroy() {
	for(int i=0;i<WORKER_THREADS;++i) {
		struct www_op *poison = message_queue_message_alloc_blocking(&worker_queue);
		poison->operation = OP_EXIT;
		message_queue_write(&worker_queue, poison);
	}
	for(int i=0;i<WORKER_THREADS;++i) {
		pthread_join(worker_threads[i], NULL);
	}
	message_queue_destroy(&worker_queue);
}

static void wake_main_thread() {
	if(__sync_lock_test_and_set(&main_blocked, 0)) {
		pthread_kill(main_thread, SIGUSR1);
	}
}

// A terrible and incomplete HTTP server follows.

// Utility functions

static const char *errno_to_http_status() {
	switch(errno) {
	case EACCES:
		return "403 Permission Denied";
	case EMFILE:
		return "503 Service Unavailable";
	case ENOENT:
		return "404 File Not Found";
	default:
		return "500 Internal Server Error";
	}
}

static int is_valid_filename_char(char c) {
	return (c >= '0' && c <= '9') ||
	       (c >= 'a' && c <= 'z') ||
	       (c >= 'A' && c <= 'Z') ||
	       (c == '.') || (c == '_');
}

// HTTP request processing pipeline

struct client_state {
	enum {CLIENT_INACTIVE, CLIENT_READING, CLIENT_WRITING} state;
	char buf[1024];
	int pos;
	struct io_op *write_op;
};

struct client_state client_data[FD_SETSIZE];

static int open_http_listener() {
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if(fd >= 0) {
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(8080);
		addr.sin_addr.s_addr = INADDR_ANY;
		if(bind(fd, (struct sockaddr *)&addr, sizeof(addr))) {
			close(fd);
			return -1;
		}
		if(listen(fd, 128)) {
			close(fd);
			return -1;
		}
	}
	return fd;
}

static void handle_client_data(int fd) {
	int r;
	if((r = read(fd, client_data[fd].buf+client_data[fd].pos, 1024-client_data[fd].pos)) > 0) {
		client_data[fd].pos += r;
		if(client_data[fd].pos >= 4 && !strncmp(client_data[fd].buf+client_data[fd].pos-4, "\r\n\r\n", 4)) {
			client_data[fd].buf[client_data[fd].pos] = '\0';
			client_data[fd].state = CLIENT_INACTIVE;
			handle_client_request(fd, client_data[fd].buf);
			return;
		}
	} else {
		client_data[fd].state = CLIENT_INACTIVE;
		close(fd);
	}
}

static void handle_client_request(int fd, char *request) {
	if(!strncmp(request, "GET /", 5)) {
		char *filename = request+5;
		char *filename_end = filename;
		while(is_valid_filename_char(*filename_end))
			++filename_end;
		*filename_end = '\0';
		struct www_op *message = message_queue_message_alloc_blocking(&worker_queue);
		message->operation = OP_BEGIN;
		message->filename = filename;
		message->fd = fd;
		message_queue_write(&worker_queue, message);
	} else {
		close(fd);
	}
}

static void generate_client_reply(int fd, const char *filename) {
	int rfd = open(filename, O_RDONLY);
	struct io_op *message = message_queue_message_alloc_blocking(&io_queue);
	if(rfd >= 0) {
		struct stat st;
		if(!fstat(rfd, &st)) {
			snprintf(message->buf, 1024, "HTTP/1.0 200 OK\r\nContent-type: text/ascii\r\nContent-Length: %lu\r\n\r\n", (unsigned long)st.st_size);
			message->buf[1023]  = '\0';
			message->len = strlen(message->buf);
			message->pos = 0;
			message->fd = fd;
			message->rfd = rfd;
			message->close_pending = 0;
			message_queue_write(&io_queue, message);
			wake_main_thread();
			return;
		}
	}
	snprintf(message->buf, 1024, "HTTP/1.0 %s\r\n\r\n", errno_to_http_status());
	message->len = strlen(message->buf);
	message->pos = 0;
	message->fd = fd;
	message->rfd = rfd;
	message->close_pending = 1;
	message->buf[1024] = '\0';
	message_queue_write(&io_queue, message);
	wake_main_thread();
}

static int copy_data(int rfd, int fd) {
	struct io_op *message = message_queue_message_alloc_blocking(&io_queue);
	message->len = read(rfd, message->buf, 1024);
	message->pos = 0;
	message->fd = fd;
	message->rfd = rfd;
	if(message->len) {
		message->close_pending = 0;
	} else {
		message->len = 0;
		message->close_pending = 1;
	}
	message_queue_write(&io_queue, message);
	wake_main_thread();
}

static void service_io_message_queue() {
	struct io_op *message;
	while(message = message_queue_tryread(&io_queue)) {
		client_data[message->fd].state = CLIENT_WRITING;
		client_data[message->fd].write_op = message;
	}
}

static void handle_signal(int signal) {
}

int main(int argc, char *argv[]) {
	main_thread = pthread_self();
	signal(SIGUSR1, &handle_signal);
	signal(SIGPIPE, SIG_IGN);
	message_queue_init(&io_queue, sizeof(struct io_op), 128);
	threadpool_init();
	int fd = open_http_listener();
	if(fd >= 0) {
		while(1) {
			fd_set rfds, wfds;
			int max_fd, r;
			main_blocked = 1;
			__sync_synchronize();
			service_io_message_queue();
			FD_ZERO(&rfds);
			FD_ZERO(&wfds);
			max_fd = 0;
			FD_SET(fd, &rfds);
			for(int i=0;i<FD_SETSIZE;++i) {
				if(client_data[i].state == CLIENT_READING) {
					FD_SET(i, &rfds);
					max_fd = i;
				} else if(client_data[i].state == CLIENT_WRITING) {
					FD_SET(i, &wfds);
					max_fd = i;
				}
			}
			max_fd = fd > max_fd ? fd : max_fd;
			r = select(max_fd+1, &rfds, &wfds, NULL, NULL);
			main_blocked = 0;
			__sync_synchronize();
			if(r < 0 && errno != EINTR) {
				perror("Error in select");
				return -1;
			}
			if(r > 0) {
				if(FD_ISSET(fd, &rfds)) {
					struct sockaddr_in peer_addr;
					socklen_t peer_len = sizeof(peer_addr);
					int cfd = accept(fd, (struct sockaddr *)&peer_addr, &peer_len);
					if(cfd >= 0) {
						int flags = fcntl(cfd, F_GETFL, 0);
						fcntl(fd, F_SETFL, flags | O_NONBLOCK);
						client_data[cfd].state = CLIENT_READING;
						client_data[cfd].pos = 0;
					}
				}
				for(int i=0;i<FD_SETSIZE;++i) {
					if(i != fd && FD_ISSET(i, &rfds)) {
						handle_client_data(i);
					} else if(i != fd && FD_ISSET(i, &wfds)) {
						int r = write(i, client_data[i].write_op->buf+client_data[i].write_op->pos, client_data[i].write_op->len-client_data[i].write_op->pos);
						if(r >= 0) {
							client_data[i].write_op->pos += r;
							if(client_data[i].write_op->pos == client_data[i].write_op->len) {
								client_data[i].state = CLIENT_INACTIVE;
								if(client_data[i].write_op->close_pending) {
									close(client_data[i].write_op->rfd);
									close(i);
								} else {
									struct www_op *message = message_queue_message_alloc_blocking(&worker_queue);
									message->operation = OP_READ_BLOCK;
									message->fd = i;
									message->rfd = client_data[i].write_op->rfd;
									message_queue_write(&worker_queue, message);
								}
								message_queue_message_free(&io_queue, client_data[i].write_op);
							}
						} else {
							close(client_data[i].write_op->rfd);
							close(i);
							message_queue_message_free(&io_queue, client_data[i].write_op);
							client_data[i].state = CLIENT_INACTIVE;
						}
					}
				}
			}
			service_io_message_queue();
		}
	} else {
		perror("Error listening on *:8080");
	}
}
