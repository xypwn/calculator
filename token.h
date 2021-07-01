/* vim: set filetype=c: */

#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <stdio.h>

typedef struct Token Token;
struct Token;
typedef struct TokenListItem TokenListItem;
struct TokenListItem;

typedef long double NumToken;
typedef struct SepToken SepToken;
struct SepToken {
    char sym;
    TokenListItem* matching;
};
typedef char OpToken;
typedef struct ExprToken ExprToken;
struct ExprToken {
    Token* lhs;
    Token* op;
    Token* rhs;
};

typedef struct Token Token;
struct Token {
    enum {
        TokenTypeNull, /* Invalid type */
        TokenTypeNum,
        TokenTypeSep,
        TokenTypeOp,
        TokenTypeExpr,
    } type;

    union {
        NumToken num;
        SepToken sep;
        OpToken op;
        ExprToken expr;
    } data;
};

/* Recursively frees Token of type ExprToken and its children */
extern void ExprToken_uninit_recursive(Token* obj);

extern void Token_print(const Token* obj, FILE* file);

extern void Token_print_as_tree(const Token* obj, FILE* file);

#endif /* _TOKEN_H_ */
