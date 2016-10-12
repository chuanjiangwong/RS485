
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

typedef uint8_t* wbitmap_t;

/*
 * Allocate a bitmap with the input size
 *
 * Return: struct representing a new bitmap
 * Side Effects: errno set to ENOMEM if could not allocate
 */
wbitmap_t alloc_bitmap(unsigned int);

/*
 * Free the bitmap
 *
 * Return: 0 on success, nonzero value on error
 * Side Effects: errno set to EINVAL if invalid bitmap
 */
int check_bitmap(wbitmap_t, unsigned int);

/*
 * Free the bitmap
 *
 * Return: None
 * Side Effects: input pointer is invalidated
 */
void free_bitmap(wbitmap_t);

/*
 * Set the passed index to 1 in the bitmap
 *
 * Return: 0 if success, nonzero otherwise.
 * Side Effects: errno set
 */
int set_bitmap(wbitmap_t, unsigned int);

/*
 * Set the passed value in the bitmap to 0
 *
 * Return: 0 if success, nonzero otherwise.
 * Side Effects: errno set
 */
int unset_bitmap(wbitmap_t, unsigned int);




#endif /* SDK_INCLUDE_WBITMAP_H_ */
