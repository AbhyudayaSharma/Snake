#include "arraylist.h"

// macOS Clang doesn't have <malloc.h>
#ifdef __APPLE__
#include <stdlib.h>
#else
#include <malloc.h>
#endif // __APPLE__
#include <string.h>
#include <assert.h>

void initArrayList(ArrayList* list, size_t size, size_t memSize) {
    list->array = malloc(size * memSize);

    // bad alloc
    if (list->array == NULL) {
        assert(0 && "Out of memory!");
    }

    list->elementSize = memSize;
    list->_mallocedLen = size;
    list->_size = 0;
}

void freeArrayList(ArrayList* list) {
    free(list->array);
    list->array = NULL;
}

void listAddLast(ArrayList* list, const void* item) {
    // First increase alloced memory if necessary
    if (list->_size == list->_mallocedLen - 1) {
        void* newArray = realloc(list->array, list->elementSize * list->_mallocedLen * 2);
        if (newArray != NULL) {
            list->_mallocedLen *= 2;
            list->array = newArray;
        } else {
            freeArrayList(list);
            assert(0 && "No more memory!");
        }
    }

    // Add the element
    char* mem = list->array;
    memcpy(mem + (list->elementSize * list->_size), item, list->elementSize); 
    list->_size++;
}

size_t listGetLength(const ArrayList* list) {
    return list->_size;
}

void listSet(ArrayList* list, size_t index, const void* item) {
    if (index > list->_size - 1) {
        assert(0 && "Index out of bounds");
    }

    memcpy(((char*) list->array) + list->elementSize * index, item, list->elementSize);
}

