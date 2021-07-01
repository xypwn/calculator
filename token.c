/* vim: set filetype=c: */

#include "token.h"

#include <assert.h>
#include <malloc.h>

void ExprToken_uninit_recursive(Token* obj) {
    if(obj->type == TokenTypeExpr)  {
        /* Uninit lhs recursively */
        if(obj->data.expr.lhs)
            ExprToken_uninit_recursive(obj->data.expr.lhs);
        /* Uninit op */
        if(obj->data.expr.op) {
            assert(obj->data.expr.op->type == TokenTypeOp);
            free(obj->data.expr.op);
        }
        /* Uninit rhs recursively */
        if(obj->data.expr.rhs)
            ExprToken_uninit_recursive(obj->data.expr.rhs);
    } else {
        /* If it's not an ExprToken, the type must be NumToken */
        assert(obj->type == TokenTypeNum);
    }
    free(obj); /* Free the token itself */
}

void Token_print(const Token* obj, FILE* file) {
    switch(obj->type) {
        default:
            fprintf(file, "(Invalid)");
            break;
        case TokenTypeNull:
            fprintf(file, "(Null)");
            break;
        case TokenTypeNum:
            fprintf(file, "(Num, %Lf)", obj->data.num);
            break;
        case TokenTypeSep:
            fprintf(file, "(Sep, '%c')", obj->data.sep.sym);
            break;
        case TokenTypeOp:
            fprintf(file, "(Op, '%c')", obj->data.op);
            break;
        case TokenTypeExpr:
            fprintf(file, "(Expr)");
            break;
    }
}

static void Token_print_as_tree_(const Token* obj, FILE* file, size_t depth) {
    /* Put spaces in front for hierarchical view */
    size_t i;
    for(i = 0; i < depth * 4; i++)
        fputc(' ', file);
    Token_print(obj, file); /* Print the token itself */
    fputc('\n', file); /* Print newline */
    /* Print the children one layer deeper */
    if(obj->type == TokenTypeExpr) {
        if(obj->data.expr.lhs != NULL)
            Token_print_as_tree_(obj->data.expr.lhs, file, depth + 1);
        if(obj->data.expr.op != NULL)
            Token_print_as_tree_(obj->data.expr.op, file, depth + 1);
        if(obj->data.expr.rhs != NULL)
            Token_print_as_tree_(obj->data.expr.rhs, file, depth + 1);
    }
}

void Token_print_as_tree(const Token* obj, FILE* file) {
    Token_print_as_tree_(obj, file, 0);
}
