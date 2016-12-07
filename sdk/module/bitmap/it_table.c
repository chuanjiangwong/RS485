
/*
 * =====================================================================================
 *
 *       Filename:  it_table.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Oct 12, 2016 5:42:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chuanjiang.wong (wong), chuanjiang.wong1991@gmail.com
 *   Organization:  www.enno.com
 *
 * =====================================================================================
 */



#include <string.h>
#include <stdlib.h>

#include <errno.h>
#include <pthread.h>

#include <wbitmap.h>



struct table
{
    wbitmap_t       map;
    void*           table;
    size_t          limit;
    size_t          element_size;
};




wid_table_t id_table_init(size_t data_size, size_t deep)
{
    struct table *table = NULL;

    if(!(data_size || deep))
    {
        errno = EINVAL;
        goto done;
    }

    table = calloc(1, sizeof(struct table));
    if(!table)
    {
        errno = ENOMEM;
        goto done;
    }

    table->limit = deep;
    table->element_size = data_size;
    table->table = calloc(table->limit, table->element_size);
    if(!table->table)
    {
        errno = ENOMEM;
        goto calloc_table_fail;
    }

    table->map = alloc_bitmap(table->limit);
    if(!table->map)
    {
        goto calloc_bitmap_fail;
    }

    return table;

calloc_bitmap_fail:
    free(table->table);
calloc_table_fail:
    free(table);
done:
    return NULL;
}


int get_unused_id(wid_table_t w)
{
    struct table *table = w;
    return find_next_zero_bit(table->map, table->limit);
}


int id_table_set(wid_table_t w, int id, void const * data)
{
    struct table *table = w;

    if(!(table && data))
        return -EINVAL;

    if(!check_bitmap(table->map, id))
        return -EPERM;

    memcpy(table->table + id, data, table->element_size);

    return 0;
}


int id_table_get(wid_table_t w, void* out, int id)
{
    struct table *table = w;
    if(!(table && out))
        return -EINVAL;

    if(!check_bitmap(table->map, id))
        return -EPERM;

    memcpy(out, table->table + id, table->element_size);

    return 0;
}

void id_table_uninstall(wid_table_t w, int id)
{
    struct table *table = w;
    if(!table)
        return;

    if(!check_bitmap(table->map, id))
        return;

    memset(table->table + id, 0, table->element_size);

    unset_bitmap(table->map, id);
}

int id_table_install(wid_table_t w, int id, void* data)
{
    struct table *table = w;
    if(!table)
        return -EINVAL;

    if(check_bitmap(table->map, id))
        return -EPERM;

    if(data)
        memcpy(table->table + id, data, table->element_size);

    return set_bitmap(table->map, id);
}


void id_table_release(wid_table_t w)
{
    struct table *table = w;
    if(!table)
        return;

    if(table->map)
        free(table->map);

    free_bitmap(table->map);

    free(table);
}



