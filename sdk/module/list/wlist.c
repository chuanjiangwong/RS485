
/*
 * =====================================================================================
 *
 *       Filename:  wlist.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Sep 27, 2016 2:15:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), wangchuanjiang1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */


#include <klist.h>
#include <wlist.h>


void wlist_init(struct wlist *k, void (*get)(struct wlist_node *),
		void (*put)(struct wlist_node *))
{
	INIT_LIST_HEAD(&k->w_list);
    pthread_mutex_init(&k->w_lock, NULL);
	k->get = get;
	k->put = put;
}

static void add_head(struct wlist *k, struct wlist_node *n)
{
	pthread_mutex_lock(&k->w_lock);
	list_add(&n->n_node, &k->w_list);
	pthread_mutex_unlock(&k->w_lock);
}

static void add_tail(struct wlist *k, struct wlist_node *n)
{
	pthread_mutex_lock(&k->w_lock);
	list_add_tail(&n->n_node, &k->w_list);
	pthread_mutex_unlock(&k->w_lock);
}

static void wlist_node_init(struct wlist *k, struct wlist_node *n)
{
	INIT_LIST_HEAD(&n->n_node);
	n->n_wlist = k;
	if (k->get)
		k->get(n);
}

/**
 * klist_add_head - Initialize a klist_node and add it to front.
 * @n: node we're adding.
 * @k: klist it's going on.
 */
void wlist_add_head(struct wlist_node *n, struct wlist *k)
{
	wlist_node_init(k, n);
	add_head(k, n);
}

/**
 * klist_add_tail - Initialize a klist_node and add it to back.
 * @n: node we're adding.
 * @k: klist it's going on.
 */
void wlist_add_tail(struct wlist_node *n, struct wlist *k)
{
	wlist_node_init(k, n);
	add_tail(k, n);
}

/**
 * klist_add_after - Init a klist_node and add it after an existing node
 * @n: node we're adding.
 * @pos: node to put @n after
 */
void wlist_add_after(struct wlist_node *n, struct wlist_node *pos)
{
	struct wlist *k = pos->n_wlist;

	wlist_node_init(k, n);
	pthread_mutex_lock(&k->w_lock);
	list_add(&n->n_node, &pos->n_node);
	pthread_mutex_unlock(&k->w_lock);
}

/**
 * klist_add_before - Init a klist_node and add it before an existing node
 * @n: node we're adding.
 * @pos: node to put @n after
 */
void wlist_add_before(struct wlist_node *n, struct wlist_node *pos)
{
	struct wlist *k = pos->n_wlist;

	wlist_node_init(k, n);
	pthread_mutex_lock(&k->w_lock);
	list_add_tail(&n->n_node, &pos->n_node);
	pthread_mutex_unlock(&k->w_lock);
}

static void wlist_release(struct wlist_node *n)
{
    list_del(&n->n_node);
    n->n_wlist = NULL;
}


/**
 * klist_del - Decrement the reference count of node and try to remove.
 * @n: node we're deleting.
 */
void wlist_del(struct wlist_node *n)
{
	struct wlist *k = n->n_wlist;
	void (*put)(struct wlist_node *) = k->put;

	pthread_mutex_lock(&k->w_lock);
    wlist_release(n);
	pthread_mutex_unlock(&k->w_lock);

	if (put)
		put(n);
}

/**
 * klist_remove - Decrement the refcount of node and wait for it to go away.
 * @n: node we're removing.
 */
void wlist_remove(struct wlist_node *n)
{
	wlist_del(n);
}


void wlist_delete(struct wlist* w)
{
    pthread_mutex_destroy(&w->w_lock);
    list_del(&w->w_list);
}

/**
 * klist_node_attached - Say whether a node is bound to a list or not.
 * @n: Node that we're testing.
 */
int wlist_node_attached(struct wlist_node *n)
{
	return (n->n_wlist != NULL);
}

/**
 * klist_iter_init_node - Initialize a klist_iter structure.
 * @k: klist we're iterating.
 * @i: klist_iter we're filling.
 * @n: node to start with.
 *
 * Similar to klist_iter_init(), but starts the action off with @n,
 * instead of with the list head.
 */
void wlist_iter_init_node(struct wlist *k, struct wlist_iter *i,
			  struct wlist_node *n)
{
	i->i_wlist = k;
	i->i_head = &k->w_list;
	i->i_cur = n;
}

/**
 * klist_iter_init - Iniitalize a klist_iter structure.
 * @k: klist we're iterating.
 * @i: klist_iter structure we're filling.
 *
 * Similar to klist_iter_init_node(), but start with the list head.
 */
void wlist_iter_init(struct wlist *k, struct wlist_iter *i)
{
	wlist_iter_init_node(k, i, NULL);
}

/**
 * klist_iter_exit - Finish a list iteration.
 * @i: Iterator structure.
 *
 * Must be called when done iterating over list, as it decrements the
 * refcount of the current node. Necessary in case iteration exited before
 * the end of the list was reached, and always good form.
 */
void wlist_iter_exit(struct wlist_iter *i)
{
	if (i->i_cur) {
		wlist_del(i->i_cur);
		i->i_cur = NULL;
	}
}

static struct wlist_node *to_wlist_node(struct list_head *n)
{
	return container_of(n, struct wlist_node, n_node);
}

/**
 * klist_next - Ante up next node in list.
 * @i: Iterator structure.
 *
 * First grab list lock. Decrement the reference count of the previous
 * node, if there was one. Grab the next node, increment its reference
 * count, drop the lock, and return that next node.
 */
struct wlist_node *wlist_next(struct wlist_iter *i)
{
	struct list_head *next;
	struct wlist_node *lnode = i->i_cur;
	struct wlist_node *wnode = NULL;
	void (*put)(struct wlist_node *) = i->i_wlist->put;

	pthread_mutex_lock(&i->i_wlist->w_lock);
	if (lnode) {
		next = lnode->n_node.next;
	} else
		next = i->i_head->next;

	if (next != i->i_head) {
		wnode = to_wlist_node(next);
	}
	i->i_cur = wnode;
	pthread_mutex_unlock(&i->i_wlist->w_lock);
	if (put && lnode)
		put(lnode);
	return wnode;
}
