/* vim: set filetype=c: */

#include "ptr_stack.h"

#include <malloc.h>

void PtrStack_init(PtrStack* obj) {
    obj->top = NULL;
    obj->size = 0;
}

void PtrStack_push(PtrStack* obj, void* ptr) {
    PtrStackItem* itm = malloc(sizeof(PtrStackItem));
    itm->ptr = ptr;
    itm->prev = obj->top;
    obj->top = itm;
    obj->size++;
}
void PtrStack_pop(PtrStack* obj) {
    PtrStackItem* itm = obj->top;
    obj->top = itm->prev;
    free(itm);
    obj->size--;
}

void PtrStack_uninit(PtrStack* obj) {
    PtrStackItem* curr = obj->top;
    while(curr != NULL) {
        PtrStackItem* prev = curr->prev;
        free(curr);
        curr = prev;
    }
}
