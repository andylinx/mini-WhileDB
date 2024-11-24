%option noyywrap yylineno
%option outfile="lexer.c" header-file="lexer.h"
%{
#include "lang.h"
#include "parser.h"
%}

%%

0|[1-9][0-9]* {
    yylval.n = build_nat(yytext, yyleng);
    return TM_NAT;
}

"var" {
    return TM_VAR;
}

"if" {
    return TM_IF;
}

"then" {
    return TM_THEN;
}

"else" {
    return TM_ELSE;
}

"while" {
    return TM_WHILE;
}

"do" {
    return TM_DO;
}

"malloc" {
    return TM_MALLOC;
}

"read_int" {
    return TM_RI;
}

"read_char" {
    return TM_RC;
}

"write_int" {
    return TM_WI;
}

"write_char" {
    return TM_WC;
}



"len" { // len
    // new tokens for key words and CL, SL
    return TM_LEN;
}

"read_string" { // read string
    return TM_RS;
}

"write_string" { // write string
    return TM_WS;
}

\"([^"\\\n]|\\.)*\" { // string literal 
    char *text = yytext + 1;        // skip opening quote
    text[yyleng-2] = '\0';          // remove closing quote
    yylval.sl = new_str(text, yyleng-2); 
    return TM_SL;
}

'([^'\\\n]|\\.)' { // char literal 
    char *text = yytext + 1;        // skip opening quote
    text[yyleng-2] = '\0';          // remove closing quote
    yylval.cl = text[0];            // store the character value
    return TM_CL;
}



[_A-Za-z][_A-Za-z0-9]* {
    yylval.i = new_str(yytext, yyleng);
    return TM_IDENT;
}

";" {
    return TM_SEMICOL;
    }

"(" {
    return TM_LEFT_PAREN;
    }

")" {
    return TM_RIGHT_PAREN;
    }

"{" {
    return TM_LEFT_BRACE;
    }

"}" {
    return TM_RIGHT_BRACE;
    }

"+" {
    return TM_PLUS;
    }

"-" {
    return TM_MINUS;
    }

"*" {
    return TM_MUL;
    }

"/" {
    return TM_DIV;
    }

"%" {
    return TM_MOD;
    }

"<" {
    return TM_LT;
    }

">" {
    return TM_GT;
    }

"<=" {
    return TM_LE;
    }

">=" {
    return TM_GE;
    }

"==" {
    return TM_EQ;
    }

"!=" {
    return TM_NE;
    }

"=" {
    return TM_ASGNOP;
    }

"&&" {
    return TM_AND;
    }

"||" {
    return TM_OR;
    }

"!" {
    return TM_NOT;
    }



"[" { // new tokens for simple symbols [] ,
    return TM_LSB;
    }

"]" {
    return TM_RSB;
    }

"," {
    return TM_COMMA;
    }

[ \t\n\r]    { };

.   {printf("%s",yytext);
     return -1; }
%%
