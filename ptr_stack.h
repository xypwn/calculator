/* vim: set filetype=c: */

#ifndef _PTR_STACK_H_
#define _PTR_STACK_H_

#include <stddef.h>

/* THIS STRUCT IS ONLY FOR HOLDING POINTERS AS REFERENCES, NOT 
 * HEAP ALLOCATED OBJECTS. IT DOES NOT AUTOMATICALLY ALLOC OR FREE
 * ANYTHING BUT INTERNAL OBJECTS. */

typedef struct PtrStackItem PtrStackItem;
struct PtrStackItem {
    PtrStackItem* prev;
    void* ptr;
};

typedef struct PtrStack PtrStack;
struct PtrStack {
    PtrStackItem* top;
    size_t size;
};

extern void PtrStack_init(PtrStack* obj);

extern void PtrStack_push(PtrStack* obj, void* ptr);
extern void PtrStack_pop(PtrStack* obj);

extern void PtrStack_uninit(PtrStack* obj);

#endif /* _PTR_STACK_H_ */
