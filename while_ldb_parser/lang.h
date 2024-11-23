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
  
  // new types
  T_WS // write string
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
    struct {char * name; } DECL;
    struct {struct expr * left; struct expr * right; } ASGN;
    struct {struct cmd * left; struct cmd * right; } SEQ;
    struct {struct expr * cond; struct cmd * left; struct cmd * right; } IF;
    struct {struct expr * cond; struct cmd * body; } WHILE;
    struct {struct expr * arg; } WI;
    struct {struct expr * arg; } WC;

    // new commands
    struct {struct expr * arg; } WS;
  } d;
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

// command constructors
struct cmd * TDecl(char * name);
struct cmd * TAsgn(struct expr * left, struct expr * right);
struct cmd * TSeq(struct cmd * left, struct cmd * right);
struct cmd * TIf(struct expr * cond, struct cmd * left, struct cmd * right);
struct cmd * TWhile(struct expr * cond, struct cmd * body);
struct cmd * TWriteInt(struct expr * arg);
struct cmd * TWriteChar(struct expr * arg);

//new commands
struct cmd * TWriteString(struct expr * arg);

// helper functions
void print_binop(enum BinOpType op);
void print_unop(enum UnOpType op);
void print_expr(struct expr * e);
void print_cmd(struct cmd * c);

unsigned int build_nat(char * c, int len);
char * new_str(char * str, int len);

#endif // LANG_H_INCLUDED
