/* vim: set filetype=c: */

#include "token_list.h"

#include <malloc.h>

void TokenList_init(TokenList* obj) {
    obj->front = NULL;
    obj->back = NULL;
    obj->size = 0;
}

static void TokenList_add_initial_item_(TokenList* obj, TokenListItem* data) {
    obj->front = obj->back = data;
    data->prev = data->next = NULL;
    obj->size++;
}

void TokenList_push_back(TokenList* obj, TokenListItem* data) {
    if(obj->size == 0) {
        TokenList_add_initial_item_(obj, data);
        return;
    }
    data->prev = obj->back;
    obj->back->next = data;
    obj->back = data;
    data->next = NULL;
    obj->size++;
}


void TokenList_push_front(TokenList* obj, TokenListItem* data) {
    if(obj->size == 0) {
        TokenList_add_initial_item_(obj, data);
        return;
    }
    data->next = obj->front;
    obj->front->prev = data;
    obj->front = data;
    data->prev = NULL;
    obj->size++;
}

void TokenList_insert_before(TokenList* obj, TokenListItem* itm, TokenListItem* data) {
    if(itm == obj->front) {
        TokenList_push_front(obj, data);
        return;
    }
    /* See insert_after for details */
    data->prev = itm->prev;
    data->next = itm;
    itm->prev->next = data;
    itm->prev = data;
    obj->size++;
}

void TokenList_insert_after(TokenList* obj, TokenListItem* itm, TokenListItem* data) {
    if(itm == obj->back) {
        TokenList_push_back(obj, data);
        return;
    }
    /* Initial state
     * +---+ -> +---+ -> +---+
     * |itm|    |   |    |   |
     * +---+ <- +---+ <- +---+
     *     +----+
     *     |data|
     *     +----+               */

    /* Make data point to its new neighbours
     * +---+ -> +---+ -> +---+
     * |itm|    |   |    |   |
     * +---+ <- +---+ <- +---+
     *  ^          ^
     *  |          |
     *  |  +----+ -+
     *  |  |data|
     *  +- +----+               */
    data->next = itm->next;
    data->prev = itm;

    /* Make new datas' neighbours point to it
     * +---+ -+   +---+ -> +---+
     * |itm|  |   |   |    |   |
     * +---+  |   +---+ <- +---+
     *  ^     |   |   ^
     *  | +---+   |   |
     *  | |       +---+--+
     *  | |           |  |
     *  | +-> +----+ -+  |
     *  |     |data|     |
     *  +---- +----+ <---+         */

     /* +---+ -> +----+ -> +---+ -> +---+
     *  |itm|    |data|    |   |    |   |
     *  +---+ <- +----+ <- +---+ <- +---+ */
    itm->next->prev = data;
    itm->next = data;

    obj->size++;
}

void TokenList_remove(TokenList* obj, TokenListItem* itm) {
    if(obj->size == 1) {
        obj->front = NULL;
        obj->back = NULL;
    }
    else if(itm == obj->front) {
        itm->next->prev = itm->prev;
        obj->front = itm->next;
    }
    else if(itm == obj->back) {
        itm->prev->next = itm->next;
        obj->back = itm->prev;
    } else {
        itm->prev->next = itm->next;
        itm->next->prev = itm->prev;
    }
    free(itm);
    obj->size--;
}

void TokenList_uninit(TokenList* obj) {
    TokenListItem* curr = obj->front;
    while(curr != NULL) {
        TokenListItem* next = curr->next;
        free(curr);
        curr = next;
    }
}
