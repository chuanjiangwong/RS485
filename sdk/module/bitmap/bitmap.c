
/*
 * =====================================================================================
 *
 *       Filename:  bitmap.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Oct 12, 2016 10:06:20 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <wbitmap.h>
#include <errno.h>



wbitmap_t alloc_bitmap(unsigned int size) {
    size_t buffSize = (size / 8) + 1;
    wbitmap_t bmp = (wbitmap_t)calloc(buffSize, sizeof(uint8_t));

    if (bmp == NULL) {
        errno = ENOMEM;
    }

    return bmp;
}

int check_bitmap(wbitmap_t bitmap, unsigned int index) {

    if (bitmap == NULL) {
        errno = EINVAL;
        return errno;
    }

    unsigned int byte =  index / 8;
    uint8_t offset = index % 8;

    uint8_t shift = (0x1 << offset) & bitmap[byte];
    return (shift != 0);
}

void free_bitmap(wbitmap_t bitmap) {
    if (bitmap != NULL) {
        free(bitmap);
    }
}

int set_bitmap(wbitmap_t bitmap, unsigned int index) {

    if (bitmap == NULL) {
        errno = EINVAL;
        return errno;
    }

    unsigned int byte =  index / 8;
    uint8_t offset = index % 8;

    bitmap[byte] |= (0x1 << offset);

    return 0;
}

int unset_bitmap(wbitmap_t bitmap, unsigned int index) {
    if (bitmap == NULL) {
        errno = EINVAL;
        return errno;
    }

    unsigned int byte =  index / 8;
    uint8_t offset = index % 8;

    bitmap[byte] &= (~(0x1 << offset));

    return 0;
}


