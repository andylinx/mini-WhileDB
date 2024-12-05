#ifndef LANG_H_INCLUDED
#define LANG_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

enum BinOpType {
  T_PLUS,
  T_MINUS,
  T_MUL,
  T_DIV,
  T_MOD,
  T_LT,
  T_GT,
  T_LE,
  T_GE,
  T_EQ,
  T_NE,
  T_AND,
  T_OR
};

enum UnOpType {
  T_UMINUS, // -x
  T_NOT // !x
};

enum ExprType {
  T_CONST = 0, // constant
  T_VAR, // variable
  T_BINOP, // binary operator
  T_UNOP, // uniary operator
  T_DEREF, // *ptr
  T_MALLOC, // malloc
  T_RI, // read int
  T_RC, // read char

  // new types
  T_LEN, // len
  T_RS, // read string
  T_SA // subscript access []
};

enum CmdType {
  T_DECL = 0,
  T_ASGN, // assignment = 
  T_SEQ, // sequential execution ; 
  T_IF, // if
  T_WHILE, // while
  T_WI, // write int
  T_WC, // write char
  T_WS, // write string
  T_WSL, // write string list
  T_ASGNDEREF,  // *left = right
  T_ASGNLIST // left = right
};

enum DeclType {
  T_DECLVAR,
  T_DECLARRAY,
  T_DECLSEQ,
  T_DECLVARINIT,
  T_DECLARARRAYINIT
};

struct expr {
  enum ExprType t;
  union {
    struct {unsigned int value; } CONST;
    struct {char * name; } VAR;
    struct {enum BinOpType op; struct expr * left; struct expr * right; } BINOP;
    struct {enum UnOpType op; struct expr * arg; } UNOP;
    struct {struct expr * arg; } DEREF;
    struct {struct expr * arg; } MALLOC;
    struct {void * none; } RI;
    struct {void * none; } RC;

    // new expressions
    struct {struct expr * arg; } LEN;
    struct {void * none; } RS;
    struct {struct expr * array_arg, * index_arg; } SA;
  } d;
};

struct cmd {
  enum CmdType t;
  union {
    struct {struct decl * declaration; } DECL;
    struct {struct expr * left; struct expr * right; } ASGN;
    struct {struct cmd * left; struct cmd * right; } SEQ;
    struct {struct expr * cond; struct cmd * left; struct cmd * right; } IF;
    struct {struct expr * cond; struct cmd * body; } WHILE;
    struct {struct expr * arg; } WI;
    struct {struct expr * arg; } WC;

    // new commands
    struct {struct expr * arg; } WS;
    struct {struct expr_list * arg; } WSL;
    struct {struct expr * left; struct expr * right; } ASGNDEREF;
    struct {struct expr * left; struct expr_list * right; } ASGNLIST;
  } d;
};

struct decl {
  enum DeclType t;
  union {
    struct {
      char * name;
    } DECLVAR;
    struct {
      char * name;
      struct expr * size;
    } DECLARRAY;
    struct {
      struct decl * left;
      struct decl * right;
    } DECLSEQ;
    struct {
      char * name;
      struct expr * init;
    } DECLVARINIT;
    struct {
      char * name;
      struct expr * size;
      struct expr_list * init;
    } DECLARARRAYINIT;
  } d;
};

struct expr_list {
  struct expr * head;
  struct expr_list * tail;
};

// expression constructors
struct expr * TConst(unsigned int value);

struct expr * TVar(char * name);
struct expr * TBinOp(enum BinOpType op, struct expr * left, struct expr * right);
struct expr * TUnOp(enum UnOpType op, struct expr * arg);
struct expr * TDeref(struct expr * arg);
struct expr * TMalloc(struct expr * arg);
struct expr * TReadInt();
struct expr * TReadChar();

// new expressions
struct expr * TLen(struct expr * arg);
struct expr * TReadString();
struct expr * TSubscriptAccess(struct expr * array_arg, struct expr * index_arg);
struct expr * TConstChar(char c);
struct expr_list * TStringToExprList(char * str);

// command constructors
struct cmd * TDecl(struct decl * declaration);
struct cmd * TAsgn(struct expr * left, struct expr * right);
struct cmd * TSeq(struct cmd * left, struct cmd * right);
struct cmd * TIf(struct expr * cond, struct cmd * left, struct cmd * right);
struct cmd * TWhile(struct expr * cond, struct cmd * body);
struct cmd * TWriteInt(struct expr * arg);
struct cmd * TWriteChar(struct expr * arg);

//new commands
struct cmd * TWriteString(struct expr * arg);
struct cmd * TWriteStringList(struct expr_list * str_list);
struct cmd * TAsgnDeref(struct expr * left, struct expr * right);
struct cmd * TAsgnList(struct expr * left, struct expr_list * right);

// helper functions
// const char* get_token_name(int token);
// void print_token(int token);
void print_binop(enum BinOpType op);
void print_unop(enum UnOpType op);
void print_expr(struct expr * e);
void print_cmd(struct cmd * c);
void print_decl(struct decl * d);
void print_expr_list(struct expr_list * l);
unsigned int build_nat(char * c, int len);
char * new_str(char * str, int len);

struct decl * TDeclVar(char * name);
struct decl * TDeclArray(char * name, struct expr * size);
struct decl * TDeclSeq(struct decl * left, struct decl * right);
struct decl * TDeclVarInit(char * name, struct expr * init);
struct decl * TDeclArrayInit(char * name, struct expr * size, struct expr_list * init);
struct expr_list * TExprList(struct expr * head, struct expr_list * tail);

#endif // LANG_H_INCLUDED
