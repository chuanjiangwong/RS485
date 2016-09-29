
/*
 * =====================================================================================
 *
 *       Filename:  wlist.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Sep 27, 2016 2:15:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), wangchuanjiang1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#ifndef SDK_INCLUDE_WLIST_H_
#define SDK_INCLUDE_WLIST_H_

#include <klist.h>
#include <pthread.h>


struct wlist_node;
struct wlist {
	pthread_mutex_t		w_lock;
	struct list_head	w_list;
	void			(*get)(struct wlist_node *);
	void			(*put)(struct wlist_node *);
};

#define WLIST_INIT(_name, _get, _put)					\
	{ .w_lock	= pthread_mutex_init(&_name.w_lock, NULL),	\
	  .w_list	= LIST_HEAD_INIT(_name.w_list),			\
	  .get		= _get,						\
	  .put		= _put, }

#define DEFINE_WLIST(_name, _get, _put)					\
	struct wlist _name = WLIST_INIT(_name, _get, _put)

extern void wlist_init(struct wlist *k, void (*get)(struct wlist_node *),
		       void (*put)(struct wlist_node *));

struct wlist_node {
	struct wlist		*n_wlist;
	struct list_head	n_node;
};

extern void wlist_add_tail(struct wlist_node *n, struct wlist *k);
extern void wlist_add_head(struct wlist_node *n, struct wlist *k);
extern void wlist_add_after(struct wlist_node *n, struct wlist_node *pos);
extern void wlist_add_before(struct wlist_node *n, struct wlist_node *pos);

extern void wlist_del(struct wlist_node *n);
extern void wlist_remove(struct wlist_node *n);

extern int wlist_node_attached(struct wlist_node *n);


struct wlist_iter {
	struct wlist		*i_wlist;
	struct list_head	*i_head;
	struct wlist_node	*i_cur;
};


extern void wlist_iter_init(struct wlist *k, struct wlist_iter *i);
extern void wlist_iter_init_node(struct wlist *k, struct wlist_iter *i,
				 struct wlist_node *n);
extern void wlist_iter_exit(struct wlist_iter *i);
extern struct wlist_node *wlist_next(struct wlist_iter *i);



#endif /* SDK_INCLUDE_WLIST_H_ */
