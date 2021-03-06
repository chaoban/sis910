/**
 *\file hash.c
 *\brief To provide general HashTable-related API
 *\author nf
 *\date 2008-04-03
 *
 *$Id: hash.c 624 2008-04-03 08:20:38Z  $
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "hash.h"

/**
 *\brief To Constuct a Hashtable with size slot 
 *\author nf
 *\param size
 *\date 2008-04-03
 *\return a pointer to the Hashtable
 */
hash_table *hashConstructTable(size_t size)
{
    size_t i;
    bucket **temp;
    hash_table *table;

    table = malloc(sizeof(hash_table));
    if (table == NULL)
        return NULL;

    table->size = size;
    table->table = (bucket * *)malloc(sizeof(bucket *) * size);
    temp = table->table;

    if (temp == NULL) {
        free(table);
        return NULL;
    }

    for (i = 0; i < size; i++)
        temp[i] = NULL;

    return table;
}



/*
 * \brief Hashes a string to produce an unsigned short, which should be
 * sufficient for most purposes.
 *
 *\param string
 *\return the hash value
*/
unsigned hashHash(char *string)
{
    unsigned ret_val = 0;
    int i;

    while (*string) {
        i = tolower(*string);
        ret_val ^= i;
        ret_val <<= 1;
        string++;
    }
    return ret_val;
}


/**
 *\brief Insert 'key' into hash table.
 *\author nf
 *\param key
 *\param data
 *\param table
 *\date 2008-04-03
 *\return  Returns pointer to old data associated with the key, if any, or
 * NULL if the key wasn't in the table previously.
*/
void *hashInsert(char *key, void *data, hash_table * table)
{
    unsigned val = hashHash(key) % table->size;
    bucket *ptr;
    /*
       ** NULL means this bucket hasn't been used yet.  We'll simply
       ** allocate space for our new bucket and put our data there, with
       ** the table pointing at it.
     */
    if (NULL == (table->table)[val]) {
        (table->table)[val] = (bucket *) malloc(sizeof(bucket));
        if (NULL == (table->table)[val])
            return NULL;
        (table->table)[val]->key = strdup(key);
        (table->table)[val]->next = NULL;
        (table->table)[val]->data = data;
        return (table->table)[val]->data;
    }

    /*
       ** This spot in the table is already in use.  See if the current string
       ** has already been inserted, and if so, increment its count.
     */

    for (ptr = (table->table)[val]; NULL != ptr; ptr = ptr->next){
        if (0 == strcasecmp(key, ptr->key)) {
            void *old_data;
            old_data = ptr->data;
            ptr->data = data;
            return old_data;
        }
    }
    /*
       ** This key must not be in the table yet.  We'll add it to the head of
       ** the list at this spot in the hash table.  Speed would be
       ** slightly improved if the list was kept sorted instead.  In this case,
       ** this code would be moved into the loop above, and the insertion would
       ** take place as soon as it was determined that the present key in the
       ** list was larger than this one.
     */
    ptr = (bucket *) malloc(sizeof(bucket));
    if (NULL == ptr)
        return 0;

    ptr->key = strdup(key);
    ptr->data = data;
    ptr->next = (table->table)[val];
    (table->table)[val] = ptr;
    return data;

}


/**
 *\brief  Look up a key and return the associated data.  
 *\author nf
 *\param key
 *\param table
 *\date 2008-04-03
 *\return Returns NULL if the key is not in the table.
*/
void *hashLookup(char *key, hash_table * table)
{
    unsigned val = hashHash(key) % table->size;
    bucket *ptr;

    if (NULL == (table->table)[val])
        return NULL;
    for (ptr = (table->table)[val]; NULL != ptr; ptr = ptr->next) {
        if (0 == strcasecmp(key, ptr->key))
            return ptr->data;
    }
    return NULL;
}


/**
 *\brief  Delete a key from the hash table 
 *\author nf
 *\param key
 *\param table
 *\date 2008-04-03
 *\return associated data, or NULL if not present.
*/
void *hashDel(char *key, hash_table * table)
{
    unsigned val = hashHash(key) % table->size;
    void *data;
    bucket *ptr, *last = NULL;

    if (NULL == (table->table)[val])
        return NULL;

    /*
       ** Traverse the list, keeping track of the previous node in the list.
       ** When we find the node to delete, we set the previous node's next
       ** pointer to point to the node after ourself instead.  We then delete
       ** the key from the present node, and return a pointer to the data it
       ** contains.
     */
    for (last = NULL, ptr = (table->table)[val]; NULL != ptr; last = ptr, ptr = ptr->next) {
        if (0 == strcmp(key, ptr->key)) {
            if (last != NULL) {
                data = ptr->data;
                last->next = ptr->next;
                free(ptr->key);
                free(ptr);
                return data;
            }
            /*
               ** If 'last' still equals NULL, it means that we need to
               ** delete the first node in the list. This simply consists
               ** of putting our own 'next' pointer in the array holding
               ** the head of the list.  We then dispose of the current
               ** node as above.
             */
            else {
                data = ptr->data;
                (table->table)[val] = ptr->next;
                free(ptr->key);
                free(ptr);
                
                return data;
            }
        }
    }

    /*
       ** If we get here, it means we didn't find the item in the table.
       ** Signal this by returning NULL.
     */

    return NULL;
}



/*
*/

/**
 *\brief Frees a complete table by iterating over it and freeing each node.
 * the second parameter is the address of a function it will call with a
 * pointer to the data associated with each node.  This function is
 * responsible for freeing the data, or doing whatever is needed with
 * it. Pass "NULL" if you don't need to free anything.
 *
 *\author nf
 *\param table
 *\param func a callback function pointer
 *\date 2008-04-03
 *\return
 */
void hashFreeTable(hash_table * table, void (*func) (void *))
{
    /* Changed
     * enumerate( table, hashFreeNode);
     * here I expand the enumerate function into this function so I can
     * avoid the dodgy globals which prevent me from freeing nested hash
     * tables. 
     */
    unsigned i;
    bucket *temp;
    void *data;
    for (i = 0; i < table->size; i++) {
        if ((table->table)[i] != NULL) {
            while (temp = (table->table)[i]) {
                data = hashDel(temp->key, table);
                //assert(data);
                if (func)
                    func(data);
            }
        }
    }
    free(table->table);
    table->table = NULL;
    table->size = 0;
    free(table);
}



/**
 *\brief Simply invokes the function given as the second parameter for each
 * node in the table, passing it the key and the associated data.
 *
 *\author nf
 *\param table
 *\param func a callback function pointer
 *\date 2008-04-03
 *\return
*/
void hashEnumerate(hash_table * table, void (*func) (char *, void *))
{
    unsigned i;
    bucket *temp;
    for (i = 0; i < table->size; i++) {
        if ((table->table)[i] != NULL) {
            for (temp = (table->table)[i]; NULL != temp; temp = temp->next) {
                func(temp->key, temp->data);
            }
        }
    }
}


/**
 *\brief compared to hashEnumerate,hashEnumerate2 just add a parameter fp which is useful to file operation
 *\author nf
 *\param table
 *\param func
 *\param fp
 *\date 2008-04-03
 *\return
 */
void hashEnumerate2(hash_table * table, void (*func) (char *, void *, FILE *), FILE * fp)
{
    unsigned i;
    bucket *temp;
    for (i = 0; i < table->size; i++) {
        if ((table->table)[i] != NULL) {
            for (temp = (table->table)[i]; NULL != temp; temp = temp->next) {
                func(temp->key, temp->data, fp);
            }
        }
    }
}
