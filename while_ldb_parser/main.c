#include <stdio.h>
#include "lang.h"
#include "lexer.h"
#include "parser.h"

extern struct cmd * root;
int yyparse();

int main(int argc, char **argv) {
    yyin = stdin;
    int token;
    while (1)
    {
        token = yylex();
        if (token != 0)
        {
            // printf("token: %d\n", token);
            printf("token name: %s\n", get_token_name(token));
        }
        else
            break;
    }
    yyparse();
    printf("parse success\n");
    fclose(stdin);
    print_cmd(root);
    printf("\n");
}
