/* vim: set filetype=c: */

#ifndef _AST_H_
#define _AST_H_

#include "token.h"
#include "token_list.h"
#include "error.h"

typedef struct AST AST;
struct AST {
    Token* root;
};

extern void AST_init(AST* obj);

/* Can be called exactly once after AST_init */
extern Result AST_parse_from_TokenList(AST* obj, const TokenList* tokens);

extern long double AST_evaluate(AST* obj);

extern void AST_uninit(AST* obj);

#endif /* _AST_H_ */
