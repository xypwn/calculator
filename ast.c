/* vim: set filetype=c: */

#include "ast.h"

#include "ptr_stack.h"

#include <malloc.h>
#include <assert.h>
#include <math.h>

void AST_init(AST* obj) {
    obj->root = NULL;
}

static Token* new_default_ExprToken_() {
    Token* tkn = malloc(sizeof(Token));
    tkn->type = TokenTypeExpr;
    tkn->data.expr.lhs = NULL; /* Left operand */
    tkn->data.expr.op = NULL; /* Operator */
    tkn->data.expr.rhs = NULL; /* Right operand */
    return tkn;
}

static Token* new_NumToken_with_num_(NumToken val) {
    Token* tkn = malloc(sizeof(Token));
    tkn->type = TokenTypeNum;
    tkn->data.num = val;
    return tkn;
}

static Token* new_OpToken_with_sym_(OpToken val) {
    Token* tkn = malloc(sizeof(Token));
    tkn->type = TokenTypeOp;
    tkn->data.op = val;
    return tkn;
}

Result AST_parse_from_TokenList(AST* obj, const TokenList* tokens) {
    /* If root weren't NULL, the AST was either not initialized, or
     * it was already populated */
    assert(obj->root == NULL);

    obj->root = new_default_ExprToken_();

    PtrStack node_stack; /* Always has curr_node at the top, followed by curr_node's parent, etc.. */
    PtrStack_init(&node_stack);
    Token* curr_node = obj->root; /* Node means ExprToken in this case */
    PtrStack_push(&node_stack, curr_node);
    /* Linearly iterate through every token the lexer generated */
    const TokenListItem* curr;
    for(curr = tokens->front; curr != NULL; curr = curr->next) {
        switch(curr->val.type) {
            default:
                return Result_err("Found invalid token type while building AST");
                break;
            case TokenTypeSep: {
                if(curr->val.data.sep.sym == '(') {
                    /* Create a new sub expression as a child of the current expression */
                    Token* new_tkn = new_default_ExprToken_();

                    /* The left and right hand operands get filled from left to right;
                     * insert the new sub expression into the next free operand slot,
                     * free meaning unassigned or NULL */
                    if(curr_node->data.expr.lhs == NULL)
                        curr_node->data.expr.lhs = new_tkn;
                    else if (curr_node->data.expr.rhs == NULL)
                        curr_node->data.expr.rhs = new_tkn;
                    else {
                        free(new_tkn); /* Free new_tkn, as it is unused due to an error */
                        return Result_err("Found more than 2 operands for 1 operator while building AST");
                        break;
                    }

                    /* Push the new curr_node onto the pointer stack to allow to
                     * go a layer back, as needed in case of an rparen */
                    curr_node = new_tkn;
                    PtrStack_push(&node_stack, curr_node);

                } else /* if(curr->val.data.sep.sym == ')') */ {
                    /* Go back a layer, effectively changing curr_node to its parent */
                    PtrStack_pop(&node_stack);
                    curr_node = node_stack.top->ptr;
                }
                break;
            }
            case TokenTypeNum: {
                assert(curr_node->type == TokenTypeExpr);
                Token* num_tkn = new_NumToken_with_num_(curr->val.data.num);

                /* Fill the curr_node expression operands from left to right */
                if(curr_node->data.expr.lhs == NULL)
                    curr_node->data.expr.lhs = num_tkn;
                else if (curr_node->data.expr.rhs == NULL)
                    curr_node->data.expr.rhs = num_tkn;
                else {
                    free(num_tkn); /* Free num_tkn, as it is unused due to an error */
                    return Result_err("Found more than 2 operands for 1 operator while building AST");
                    break;
                }

                break;
            }
            case TokenTypeOp: {
                if(curr_node->data.expr.op == NULL) {
                    /* Fill the expression token's operator field with exactly the same
                     * data as the current token */
                    Token* op_tkn = new_OpToken_with_sym_(curr->val.data.op);
                    curr_node->data.expr.op = op_tkn;
                } else {
                    return Result_err("Found more than 1 operator in a single expression while building the AST");
                    break;
                }
                break;
            }
        }
    }
    PtrStack_uninit(&node_stack);
    return Result_noerr();
}

static long double AST_eval_node_(Token* node) {
    assert(node->type == TokenTypeExpr);
    long double a, b;
    if(node->data.expr.lhs == NULL)
        /* TODO: Error handling */
        return 0;
    if(node->data.expr.lhs->type == TokenTypeExpr)
        a = AST_eval_node_(node->data.expr.lhs);
    else /* if(node->data.expr.lhs->type == TokenTypeNum) */
        a = node->data.expr.lhs->data.num;

    if(node->data.expr.rhs == NULL)
        /* If there is no right hand side expression, just return the left hand one */
        return a;
    if(node->data.expr.rhs->type == TokenTypeExpr)
        b = AST_eval_node_(node->data.expr.rhs);
    else /* if(node->data.expr.rhs->type == TokenTypeNum) */
        b = node->data.expr.rhs->data.num;

    switch(node->data.expr.op->data.op) {
    case '+':
        return a + b;
        break;
    case '-':
        return a - b;
        break;
    case '*':
        return a * b;
        break;
    case '/':
        return a / b;
        break;
    case '^':
        return pow(a, b);
        break;
    default:
        /* TODO: Error handling */
        break;
    }
    return 0;
}

long double AST_evaluate(AST* obj) {
    return AST_eval_node_(obj->root);
}

void AST_uninit(AST* obj) {
    if(obj->root) 
        /* uninit and deallocate the root token and its children */
        ExprToken_uninit_recursive(obj->root);
}
