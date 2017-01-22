
/*
 * =====================================================================================
 *
 *       Filename:  wbitmap.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Oct 12, 2016 10:07:22 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#ifndef SDK_INCLUDE_WBITMAP_H_
#define SDK_INCLUDE_WBITMAP_H_

#include <stdint.h>
#include <stddef.h>

typedef uint8_t* wbitmap_t;

/*
 * Allocate a bitmap with the input size
 *
 * Return: struct representing a new bitmap
 * Side Effects: errno set to ENOMEM if could not allocate
 */
extern wbitmap_t alloc_bitmap(unsigned int all_size);

/*
 * Free the bitmap
 *
 * Return: 0 on success, nonzero value on error
 * Side Effects: errno set to EINVAL if invalid bitmap
 */
extern int check_bitmap(wbitmap_t bitmap, unsigned int index);

/*
 * Free the bitmap
 *
 * Return: None
 * Side Effects: input pointer is invalidated
 */
extern void free_bitmap(wbitmap_t bitmap);

/*
 * Set the passed index to 1 in the bitmap
 *
 * Return: 0 if success, nonzero otherwise.
 * Side Effects: errno set
 */
extern int set_bitmap(wbitmap_t bitmap, unsigned int index);

/*
 * Set the passed value in the bitmap to 0
 *
 * Return: 0 if success, nonzero otherwise.
 * Side Effects: errno set
 */
extern int unset_bitmap(wbitmap_t bitmap, unsigned int index);



extern int find_next_zero_bit(wbitmap_t const bitmap, unsigned int all_size);



typedef void* wid_table_t;

extern wid_table_t id_table_init(size_t data_size, size_t deep);
extern int get_unused_id(wid_table_t w);
extern int id_table_set(wid_table_t w, int id, void const * data);
extern int id_table_get(wid_table_t w, void* out, int id);
extern int id_table_install(wid_table_t w, int id, void* data);
extern void id_table_uninstall(wid_table_t w, int id);
extern void id_table_release(wid_table_t w);

#endif /* SDK_INCLUDE_WBITMAP_H_ */
