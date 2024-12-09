%{
	#define YYDEBUG 1
	#include <stdio.h>
	#include "lang.h"
	#include "lexer.h"
	void yyerror(char *);
	int yylex(void);
	struct cmd * root;
%}

%union {
	unsigned int n;
	char * i;
	struct expr * e;
	struct cmd * c;
	struct decl * d;
	void * none;
	char * sl;
	char * cl;
	struct expr_list * el;
}

// Terminals
%token <n> TM_NAT
%token <i> TM_IDENT
%token <none> TM_LEFT_BRACE TM_RIGHT_BRACE
%token <none> TM_LEFT_PAREN TM_RIGHT_PAREN
%token <none> TM_SEMICOL
%token <none> TM_MALLOC TM_RI TM_RC TM_WI TM_WC
%token <none> TM_VAR TM_IF TM_THEN TM_ELSE TM_WHILE TM_DO
%token <none> TM_ASGNOP
%token <none> TM_OR
%token <none> TM_AND
%token <none> TM_NOT
%token <none> TM_LT TM_LE TM_GT TM_GE TM_EQ TM_NE
%token <none> TM_PLUS TM_MINUS
%token <none> TM_MUL TM_DIV TM_MOD
// added tokens
%token <none> TM_LEN TM_RS TM_WS
%token <none> TM_LSB TM_RSB TM_COMMA
%token <sl> TM_SL
%token <cl> TM_CL

// Nonterminals
%type <c> NT_WHOLE
%type <c> NT_CMD
%type <e> NT_EXPR_2
%type <e> NT_EXPR
%type <el> NT_EXPR_LIST
%type <d> NT_DECL

// Priority
%nonassoc TM_VAR
%left TM_SEMICOL
%left TM_LSB TM_RSB
%right TM_NOT
%nonassoc TM_ASGNOP
%left TM_OR
%left TM_AND
%left TM_LT TM_LE TM_GT TM_GE TM_EQ TM_NE
%left TM_PLUS TM_MINUS
%left TM_MUL TM_DIV TM_MOD
%left TM_LEFT_PAREN TM_RIGHT_PAREN

%%

// indicating the whole program (begin symbol)
NT_WHOLE:
  NT_CMD
  {
    $$ = ($1);
    root = $$;
  }
;

// added non-terminal for declaration expression
// indicating the declaration expression
// including variable declaration, array declaration, and support comma

// furthermore, we should support array initialization and variable initialization
// thus we add NT_EXPR_LIST
NT_DECL:
  TM_IDENT
  {
    $$ = TDeclVar($1);
  }
| TM_IDENT TM_ASGNOP NT_EXPR
  {
    $$ = TDeclVarInit($1, $3);
  }
| TM_IDENT TM_LSB NT_EXPR TM_RSB
  {
    $$ = TDeclArray($1, $3);
  }
| TM_IDENT TM_LSB NT_EXPR TM_RSB TM_ASGNOP TM_LEFT_BRACE NT_EXPR_LIST TM_RIGHT_BRACE
  {
    $$ = TDeclArrayInit($1, $3, $7);
  }
| TM_IDENT TM_COMMA NT_DECL
  {
    $$ = TDeclSeq(TDeclVar($1), $3);
  }
| TM_IDENT TM_ASGNOP NT_EXPR TM_COMMA NT_DECL
  {
    $$ = TDeclSeq(TDeclVarInit($1, $3), $5);
  }
| TM_IDENT TM_LSB NT_EXPR TM_RSB TM_COMMA NT_DECL
  {
    $$ = TDeclSeq(TDeclArray($1, $3), $6);
  }
| TM_IDENT TM_LSB NT_EXPR TM_RSB TM_ASGNOP TM_LEFT_BRACE NT_EXPR_LIST TM_RIGHT_BRACE TM_COMMA NT_DECL
  {
    $$ = TDeclSeq(TDeclArrayInit($1, $3, $7), $10);
  }
| TM_IDENT TM_LSB NT_EXPR TM_RSB TM_ASGNOP TM_SL
  {
    $$ = TDeclArrayInit($1, $3, TStringToExprList($6));
  }
| TM_IDENT TM_LSB NT_EXPR TM_RSB TM_ASGNOP TM_SL TM_COMMA NT_DECL
  {
    $$ = TDeclSeq(TDeclArrayInit($1, $3, TStringToExprList($6)), $8);
  }
;

// Add new non-terminal for expression lists (array initialization)
NT_EXPR_LIST:
  NT_EXPR
  {
    $$ = TExprList($1, NULL);
  }
| NT_EXPR TM_COMMA NT_EXPR_LIST
  {
    $$ = TExprList($1, $3);
  }
;

// indicating the command
NT_CMD:
  TM_VAR NT_DECL
  {
    $$ = TDecl($2);
  }
| NT_EXPR TM_ASGNOP NT_EXPR
  {
    $$ = TAsgn($1, $3);
  }
| NT_EXPR TM_ASGNOP TM_SL
  {
    $$ = TAsgnList($1, TStringToExprList($3));
  }
| NT_CMD TM_SEMICOL NT_CMD
  {
    $$ = (TSeq($1,$3));
  }
| TM_IF NT_EXPR TM_THEN TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE TM_ELSE TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE
  {
    $$ = (TIf($2,$5,$9));
  }
| TM_WHILE NT_EXPR TM_DO TM_LEFT_BRACE NT_CMD TM_RIGHT_BRACE
  {
    $$ = (TWhile($2,$5));
  }
| TM_WI TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN
  {
    $$ = (TWriteInt($3));
  }
| TM_WC TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN
  {
    $$ = (TWriteChar($3));
  }
  // added rules for write string
| TM_WS TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN
  {
    $$ = (TWriteString($3));
  }
| TM_WS TM_LEFT_PAREN TM_SL TM_RIGHT_PAREN
  {
    $$ = (TWriteStringList(TStringToExprList($3)));
  }
;

// indicating the basic expression
NT_EXPR_2:
  TM_NAT
  {
    $$ = (TConst($1));
  }
| TM_CL
  {
    char *processed = (char *)malloc(strlen($1) + 1);
    process_escape_characters($1, processed);
    struct expr * e = TConstChar(processed[0]);
    free(processed);
    $$ = e;
  }
| TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN
  {
    $$ = ($2);
  }
| TM_IDENT
  {
    $$ = (TVar($1));
  }
| TM_RI TM_LEFT_PAREN TM_RIGHT_PAREN
  {
    $$ = (TReadInt());
  }
| TM_RC TM_LEFT_PAREN TM_RIGHT_PAREN
  {
    $$ = (TReadChar());
  }
| TM_MALLOC TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN
  {
    $$ = (TMalloc($3));
  }
| TM_NOT NT_EXPR
  {
    $$ = (TUnOp(T_NOT,$2));
  }
| TM_MINUS NT_EXPR
  {
    $$ = TUnOp(T_UMINUS,$2);
  }
  // added rules for len and read string
| TM_LEN TM_LEFT_PAREN TM_SL TM_RIGHT_PAREN
  {
    $$ = TLenList(TStringToExprList($3));
  }
| TM_LEN TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN
  {
    $$ = TLen($3);
  }
| TM_RS TM_LEFT_PAREN TM_RIGHT_PAREN
  {
    $$ = TReadString();
  }
| TM_IDENT TM_LSB NT_EXPR TM_RSB
  {
    $$ = TSubscriptAccess(TVar($1), $3);
  }
| TM_MUL NT_EXPR
  {
    $$ = TDeref($2);
  }
;

// indicating the expression
NT_EXPR:
  NT_EXPR_2
  {
    $$ = ($1);
  }
| NT_EXPR TM_MUL NT_EXPR
  {
    $$ = (TBinOp(T_MUL,$1,$3));
  }
| NT_EXPR TM_PLUS NT_EXPR
  {
    $$ = (TBinOp(T_PLUS,$1,$3));
  }
| NT_EXPR TM_MINUS NT_EXPR
  {
    $$ = (TBinOp(T_MINUS,$1,$3));
  }
| NT_EXPR TM_DIV NT_EXPR
  {
    $$ = (TBinOp(T_DIV,$1,$3));
  }
| NT_EXPR TM_MOD NT_EXPR
  {
    $$ = (TBinOp(T_MOD,$1,$3));
  }
| NT_EXPR TM_LT NT_EXPR
  {
    $$ = (TBinOp(T_LT,$1,$3));
  }
| NT_EXPR TM_GT NT_EXPR
  {
    $$ = (TBinOp(T_GT,$1,$3));
  }
| NT_EXPR TM_LE NT_EXPR
  {
    $$ = (TBinOp(T_LE,$1,$3));
  }
| NT_EXPR TM_GE NT_EXPR
  {
    $$ = (TBinOp(T_GE,$1,$3));
  }
| NT_EXPR TM_EQ NT_EXPR
  {
    $$ = (TBinOp(T_EQ,$1,$3));
  }
| NT_EXPR TM_NE NT_EXPR
  {
    $$ = (TBinOp(T_NE,$1,$3));
  }
| NT_EXPR TM_AND NT_EXPR
  {
    $$ = (TBinOp(T_AND,$1,$3));
  }
| NT_EXPR TM_OR NT_EXPR
  {
    $$ = (TBinOp(T_OR,$1,$3));
  }
;


%%

void yyerror(char* s)
{
    fprintf(stderr , "%s\n",s);
}
