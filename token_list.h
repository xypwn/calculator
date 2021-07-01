/* vim: set filetype=c: */

#ifndef _TOKEN_LIST_H_
#define _TOKEN_LIST_H_

#include "token.h"

typedef struct TokenListItem TokenListItem;
struct TokenListItem {
    TokenListItem* next;
    TokenListItem* prev;
    Token val;
};

typedef struct TokenList TokenList;
struct TokenList {
    TokenListItem* front;
    TokenListItem* back;
    size_t size;
};

extern void TokenList_init(TokenList* obj);

/* All inserted / appended elements must be heap allocated in advance */
extern void TokenList_push_back(TokenList* obj, TokenListItem* data);
extern void TokenList_push_front(TokenList* obj, TokenListItem* data);

extern void TokenList_insert_before(TokenList* obj, TokenListItem* itm, TokenListItem* data);
extern void TokenList_insert_after(TokenList* obj, TokenListItem* itm, TokenListItem* data);

extern void TokenList_remove(TokenList* obj, TokenListItem* itm);

/* Frees all TokenStreamItems */
extern void TokenList_uninit(TokenList* obj);

#endif /* _TOKEN_LIST_H_ */
