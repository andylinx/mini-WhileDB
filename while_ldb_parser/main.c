#include <stdio.h>
#include "lang.h"
#include "lexer.h"
#include "parser.h"

extern int yydebug;
extern struct cmd *root;
int yyparse();

const char *get_token_name(int token)
{
    switch (token)
    {
    case YYEOF:
        return "YYEOF";
    case YYerror:
        return "YYerror";
    case YYUNDEF:
        return "YYUNDEF";
    case TM_NAT:
        return "TM_NAT";
    case TM_IDENT:
        return "TM_IDENT";
    case TM_LEFT_BRACE:
        return "TM_LEFT_BRACE";
    case TM_RIGHT_BRACE:
        return "TM_RIGHT_BRACE";
    case TM_LEFT_PAREN:
        return "TM_LEFT_PAREN";
    case TM_RIGHT_PAREN:
        return "TM_RIGHT_PAREN";
    case TM_SEMICOL:
        return "TM_SEMICOL";
    case TM_MALLOC:
        return "TM_MALLOC";
    case TM_RI:
        return "TM_RI";
    case TM_RC:
        return "TM_RC";
    case TM_WI:
        return "TM_WI";
    case TM_WC:
        return "TM_WC";
    case TM_VAR:
        return "TM_VAR";
    case TM_IF:
        return "TM_IF";
    case TM_THEN:
        return "TM_THEN";
    case TM_ELSE:
        return "TM_ELSE";
    case TM_WHILE:
        return "TM_WHILE";
    case TM_DO:
        return "TM_DO";
    case TM_ASGNOP:
        return "TM_ASGNOP";
    case TM_OR:
        return "TM_OR";
    case TM_AND:
        return "TM_AND";
    case TM_NOT:
        return "TM_NOT";
    case TM_LT:
        return "TM_LT";
    case TM_LE:
        return "TM_LE";
    case TM_GT:
        return "TM_GT";
    case TM_GE:
        return "TM_GE";
    case TM_EQ:
        return "TM_EQ";
    case TM_NE:
        return "TM_NE";
    case TM_PLUS:
        return "TM_PLUS";
    case TM_MINUS:
        return "TM_MINUS";
    case TM_MUL:
        return "TM_MUL";
    case TM_DIV:
        return "TM_DIV";
    case TM_MOD:
        return "TM_MOD";
    case TM_COMMA:
        return "TM_COMMA";
    case TM_LSB:
        return "TM_LSB";
    case TM_RSB:
        return "TM_RSB";
    case TM_CL:
        return "TM_CL";
    case TM_SL:
        return "TM_SL";
    case TM_LEN:
        return "TM_LEN";
    case TM_RS:
        return "TM_RS";
    case TM_WS:
        return "TM_WS";
    default:
        return "UNKNOWN_TOKEN";
    }
}

int main(int argc, char **argv)
{
    yyin = stdin;

    // you can set yydebug to 1 to see the debug information from the parser
    yydebug = 0;

    // you can uncomment this to see the result from the lexer
    // int token;
    // while (1)
    // {
    //     token = yylex();
    //     if (token != 0)
    //     {
    //         // printf("token: %d\n", token);
    //         printf("token name: %s\n", get_token_name(token));
    //     }
    //     else
    //         break;
    // }

    yyparse();
    // printf("parse success\n");
    fclose(stdin);
    // print_cmd(root);
    struct res_prog *r = init_res_prog(root);
    while (!test_end(r))
    {
        step(r);
    }
}
