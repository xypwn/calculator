/* vim: set filetype=c: */

#include "lex.h"

#include <assert.h>

void Lex_init(Lex* obj) {
    TokenList_init(&obj->tokens);
    PtrStack_init(&obj->ptr_stack_);

    obj->out_read_total = 0;
    obj->out_written_total = 0;

    obj->context_size_ = 0;
}

Result Lex_lex_char(Lex* obj, char in) {
    /* If the current char is part of a Num token */
    if((in >= '0' && in <= '9') || in == '.') {
        /* Don't let contexts become as large as the predefined size, as
         * we still need space for the null string terminator char \0 */
        if(obj->context_size_ < LEX_MAX_CONTEXT_SIZE - 1) {
            obj->context_[obj->context_size_] = in;
            obj->context_size_++;
        }
    /* If the current char is not part of a number, but the
     * previous one was, write the number as a token */
    } else {
        if(obj->context_size_) {
            /* Add the context string terminator */
            obj->context_[obj->context_size_] = '\0';
            obj->context_size_++;
            /* Write the token */
            TokenListItem* itm = malloc(sizeof(TokenListItem));
            itm->val.type = TokenTypeNum;
            itm->val.data.num = strtold(obj->context_, NULL);
            TokenList_push_back(&obj->tokens, itm);
            obj->context_size_ = 0;
        }
        /* If the char is not a whitespace, newline, string terminator, etc.. */
        if(!(in == ' ' || in == '\t' || in == '\n' || in == '\0')) {
            switch(in) {
                case '(':
                case ')': {
                    TokenListItem* itm = malloc(sizeof(TokenListItem));
                    itm->val.type = TokenTypeSep;
                    itm->val.data.sep.sym = in;
                    
                    if(in == '(')
                        PtrStack_push(&obj->ptr_stack_, (void*)itm);
                    else /* if in == ')' */ {
                        /* Throw an error, if the pointer stack has no top element */
                        if(obj->ptr_stack_.size == 0)
                            return Result_err("Found ')' without matching '('");
                        /* Matching LParen */
                        TokenListItem* match = obj->ptr_stack_.top->ptr;
                        /* Define the RParen's matching LParen */
                        itm->val.data.sep.matching = match;
                        /* Define the LParen's matching RParen */
                        match->val.data.sep.matching = itm;
                        /* Pop the top element */
                        PtrStack_pop(&obj->ptr_stack_);
                    }

                    TokenList_push_back(&obj->tokens, itm);
                    return Result_noerr();
                    break;
                }
                case '+':
                case '-':
                case '*':
                case '/':
                case '^': {
                    TokenListItem* itm = malloc(sizeof(TokenListItem));
                    itm->val.type = TokenTypeOp;
                    itm->val.data.op = in;
                    TokenList_push_back(&obj->tokens, itm);
                    return Result_noerr();
                    break;
                }
                default:
                    return Result_err("Unrecognized character");
                    break;
            }
        }
    }
    return Result_noerr();
}

Result Lex_finish_lex(Lex* obj) {
    Result res = Lex_lex_char(obj, '\n');
    if(res.has_err)
        return res;
    if(obj->ptr_stack_.size) {
        return Result_err("Found '(' without matching ')'");
    }
    return Result_noerr();
}

static bool contains_char_(char c, const char* charset) {
    size_t i;
    for(i = 0; charset[i] != '\0'; i++) {
        if(charset[i] == c)
            return true;
    }
    return false;
}

Result Lex_apply_op_precedence(Lex* obj, const char* opsyms, bool right_to_left) {
    /* Iterate through tokens, where curr is the current token */
    TokenListItem* const begin = right_to_left ? obj->tokens.back : obj->tokens.front;
    TokenListItem* curr;
    for(curr = begin; curr != NULL; curr = right_to_left ? curr->prev : curr->next) {
        if(curr->val.type != TokenTypeOp)
            continue;
        if(curr->prev == NULL || curr->next == NULL) {
            return Result_err("Found an operator with at least one missing operand");
            continue;
        }
        if(!contains_char_(curr->val.data.op, opsyms))
            continue;
        /* Items right and left, takes into consideration potential parentheses: 
         * For example, if we have (4+3)*5, lofop will be the position of the left paren,
         * and rofop will be the position of the 5. This is possible due to each paren
         * having a pointer to its matching paren (for details on how it works, refer to
         * Lex_lex_char()). */
        TokenListItem* lofop = NULL; /* Item left of operator */
        TokenListItem* rofop = NULL; /* Item right of operator */
        /* lofop */
        if(curr->prev->val.type == TokenTypeNum)
            lofop = curr->prev;
        else if(curr->prev->val.type == TokenTypeSep)
            /* Find the matching paren and position lofop there */
            lofop = curr->prev->val.data.sep.matching;
        else {
            return Result_err("Invalid operand type left of operator");
        }

        /* rofop: essentially the same as lofop */
        if(curr->next->val.type == TokenTypeNum)
            rofop = curr->next;
        else if(curr->next->val.type == TokenTypeSep)
            rofop = curr->next->val.data.sep.matching;
        else {
            return Result_err("Invalid operand type right of operator");
        }

        assert(lofop != NULL);
        assert(rofop != NULL);

        /* Don't add any parens, if they would be redundant.
         * We know that they are redundant, if lofop has an
         * LParen left of it and rofop has an RParen right of it*/
        if(
                /* Check for LParen left of lofop */
                lofop->prev != NULL &&
                lofop->prev->val.type == TokenTypeSep &&
                    lofop->prev->val.data.sep.sym == '(' &&
                /* Check for RParen right of rofop */
                rofop->next != NULL &&
                rofop->next->val.type == TokenTypeSep &&
                    rofop->next->val.data.sep.sym == ')'
          ) continue;


        /* Insert parens to make the AST builder handle precedence */
        TokenListItem* lp = malloc(sizeof(TokenListItem));
        TokenListItem* rp = malloc(sizeof(TokenListItem));

        lp->val.type = TokenTypeSep;
        lp->val.data.sep.sym = '(';
        lp->val.data.sep.matching = rp;
        TokenList_insert_before(&obj->tokens, lofop, lp);

        rp->val.type = TokenTypeSep;
        rp->val.data.sep.sym = ')';
        rp->val.data.sep.matching = lp;
        TokenList_insert_after(&obj->tokens, rofop, rp);
    }
    return Result_noerr();
}

void Lex_uninit(Lex* obj) {
    TokenList_uninit(&obj->tokens);
    PtrStack_uninit(&obj->ptr_stack_);
}
