/* vim: set filetype=c: */

#include "lex.h"
#include "ast.h"

#include <stdlib.h>
#include <stdio.h>

#define INPUT_SIZE 512

static void print_tokens(const Lex* lex) {
    const TokenListItem* curr;
    for(curr = lex->tokens.front; curr != NULL; curr = curr->next) {
        Token_print(&curr->val, stdout);
        fputc('\n', stdout);
    }
}

static void handle_normal_err(const Result res, const char* domain) {
    Result_print_err_or(res, domain, NULL, stderr, stdout);
    if(res.has_err)
        exit(EXIT_FAILURE);
}

int main() {
    size_t i;
    Result res;
    char in[INPUT_SIZE];

    Lex lex;
    Lex_init(&lex);

    printf("Enter an expression:\n");
    for(i = 0;;i++) {
        const char c = fgetc(stdin);
        if(i >= INPUT_SIZE - 1 || c == '\n' || c == EOF) {
            in[i] = '\0'; /* String terminator */
            break;
        }
        in[i] = c;
    }
    for(i = 0; in[i] != '\0'; i++) {
        res = Lex_lex_char(&lex, in[i]);
            handle_normal_err(res, "Lexer error: ");
    }
    res = Lex_finish_lex(&lex);
        handle_normal_err(res, "Lexer error: ");
    printf("\n**Tokens**\n");
    print_tokens(&lex);
    res = Lex_apply_op_precedence(&lex, "^", true);
        handle_normal_err(res, "Lexer error: ");
    res = Lex_apply_op_precedence(&lex, "*/", false);
        handle_normal_err(res, "Lexer error: ");
    res = Lex_apply_op_precedence(&lex, "+-", false);
        handle_normal_err(res, "Lexer error: ");
    printf("\n**Tokens (with operator precedence)**\n");
    print_tokens(&lex);

    AST ast;
    AST_init(&ast);
    res = AST_parse_from_TokenList(&ast, &lex.tokens);
        handle_normal_err(res, "Parser error: ");
    printf("\n**AST**\n");
    Token_print_as_tree(ast.root, stdout);

    long double result = AST_evaluate(&ast);
    printf("Result: %Lf\n", result);

    AST_uninit(&ast);
    Lex_uninit(&lex);
    return EXIT_SUCCESS;
}
