/* vim: set filetype=c: */

#ifndef _LEX_H_
#define _LEX_H_

#include <stdlib.h>
#include <stdbool.h>
#include "token.h"
#include "token_list.h"
#include "ptr_stack.h"
#include "error.h"

#define LEX_MAX_CONTEXT_SIZE 256

typedef struct Lex Lex;
struct Lex {
    /** Output **/
    TokenList tokens;
    /* Stats */
    size_t out_written_total; /* Total number of tokens put out */
    size_t out_read_total; /* Total number of read chars */

    /** Internal **/
    /* Stores a stack of pointers to LParen TokenListItems. Using this,
     * we can always know where each paren's matching paren is located,
     * which allows for a fast operator precedence implementation. */
    PtrStack ptr_stack_;
    /* Holds chars of text that haven't completely been lexed yet */
    char context_[LEX_MAX_CONTEXT_SIZE];
    size_t context_size_;
};

extern void Lex_init(Lex* obj);

extern Result Lex_lex_char(Lex* obj, char in);

extern Result Lex_finish_lex(Lex* obj);

/* This function MUST be called exactly once for every operator , as it transforms the
 * expression into a representation which allows for an easy creation of a tertiary tree.
 * The order of calls to this function is what dictates the actual precedence.
 * Usage: Call this function once for each operator precedence:
 * opsym is a string with the operator characters to apply precedence to, for example "+-";
 * right_to_left specifies the direction. For most math operators, this should be false, but
 * for pow for example, it should be true.
 * You should call this function for the operators with highest priority, i.e. pow first.
 * An example setup would be:
 * Lex_apply_op_precedence(&lex, "^", true);
 * Lex_apply_op_precedence(&lex, "*:", false);
 * Lex_apply_op_precedence(&lex, "+-", false); */
extern Result Lex_apply_op_precedence(Lex* obj, const char* opsyms, bool right_to_left);

extern void Lex_uninit(Lex* obj);

#endif /* _LEX_H_ */
