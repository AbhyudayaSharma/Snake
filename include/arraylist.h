#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <string.h>

//////////// USEFUL MACROS ///////////
// Easy way to get to value
#define listGet(type, listptr, index) (((type*) (listptr)->array)[index])
#define listIsEmpty(listptr) (!!list->_size)

/*
 * An implementation like a Java ArrayList
 * XXX: BEWARE:=> performs only a one level deep copy.
 * Doesn't keep references.
 * Call freeArrayList when done with the work 
 */
typedef struct {
    void* array;
    size_t _size;
    int _mallocedLen;
    size_t elementSize;
} ArrayList;

/*
 * Initializes an ArrayList with initial size 'size'
 * and member size 'memSize'.
 */
void initArrayList(ArrayList* list, size_t size, size_t memSize);

/* Returns the length of the ArrayList */
size_t listGetLength(const ArrayList* list);

void listAddLast(ArrayList* list, const void* item);
void listSet(ArrayList* list, size_t index, const void* item);

/* A free'd ArrayList can be reinitialized */
void freeArrayList(ArrayList* list);

#endif /* ARRAYLIST_H */

