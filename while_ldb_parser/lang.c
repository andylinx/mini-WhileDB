#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lang.h"
#include "parser.h"

// expression constructors

struct expr * new_expr_ptr() {
  struct expr * res = (struct expr *) malloc(sizeof(struct expr));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct cmd * new_cmd_ptr() {
  struct cmd * res = (struct cmd *) malloc(sizeof(struct cmd));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct expr * TConst(unsigned int value) {
  struct expr * res = new_expr_ptr();
  res -> t = T_CONST;
  res -> d.CONST.value = value;
  return res;
}

struct expr * TVar(char * name) {
  struct expr * res = new_expr_ptr();
  res -> t = T_VAR;
  res -> d.VAR.name = name;
  return res;
}

struct expr * TBinOp(enum BinOpType op, struct expr * left, struct expr * right) {
  struct expr * res = new_expr_ptr();
  res -> t = T_BINOP;
  res -> d.BINOP.op = op;
  res -> d.BINOP.left = left;
  res -> d.BINOP.right = right;
  return res;
}

struct expr * TUnOp(enum UnOpType op, struct expr * arg) {
  struct expr * res = new_expr_ptr();
  res -> t = T_UNOP;
  res -> d.UNOP.op = op;
  res -> d.UNOP.arg = arg;
  return res;
}

struct expr * TDeref(struct expr * arg) {
  struct expr * res = new_expr_ptr();
  res -> t = T_DEREF;
  res -> d.DEREF.arg = arg;
  return res;
}

struct expr * TMalloc(struct expr * arg) {
  struct expr * res = new_expr_ptr();
  res -> t = T_MALLOC;
  res -> d.MALLOC.arg = arg;
  return res;
}

struct expr * TReadInt() {
  struct expr * res = new_expr_ptr();
  res -> t = T_RI;
  return res;
}

struct expr * TReadChar() {
  struct expr * res = new_expr_ptr();
  res -> t = T_RC;
  return res;
}

// new expressions
struct expr * TLen(struct expr * arg) {
  struct expr * res = new_expr_ptr();
  res -> t = T_LEN;
  res -> d.LEN.arg = arg;
  return res;
}

struct expr * TReadString() {
  struct expr * res = new_expr_ptr();
  res -> t = T_RS;
  return res;
}

struct expr * TSubscriptAccess(struct expr * array_arg, struct expr * index_arg) {
  struct expr * res = new_expr_ptr();
  res -> t = T_SA;
  res -> d.SA.array_arg = array_arg;
  res -> d.SA.index_arg = index_arg;
  return res;
}




// command constructors


struct cmd * TDecl(char * name) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_DECL;
  res -> d.DECL.name = name;
  return res;
}

struct cmd * TAsgn(struct expr * left, struct expr * right) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_ASGN;
  res -> d.ASGN.left = left;
  res -> d.ASGN.right = right;
  return res;
}

struct cmd * TSeq(struct cmd * left, struct cmd * right) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_SEQ;
  res -> d.SEQ.left = left;
  res -> d.SEQ.right = right;
  return res;
}

struct cmd * TIf(struct expr * cond, struct cmd * left, struct cmd * right) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_IF;
  res -> d.IF.cond = cond;
  res -> d.IF.left = left;
  res -> d.IF.right = right;
  return res;
}

struct cmd * TWhile(struct expr * cond, struct cmd * body) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_WHILE;
  res -> d.WHILE.cond = cond;
  res -> d.WHILE.body = body;
  return res;
}

struct cmd * TWriteInt(struct expr * arg) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_WI;
  res -> d.WI.arg = arg;
  return res;
}

struct cmd * TWriteChar(struct expr * arg) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_WC;
  res -> d.WC.arg = arg;
  return res;
}

// new commands
struct cmd * TWriteString(struct expr * arg) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_WS;
  res -> d.WS.arg = arg;
  return res;
}



// helper functions

void print_binop(enum BinOpType op) {
  switch (op) {
  case T_PLUS:
    printf("PLUS");
    break;
  case T_MINUS:
    printf("MINUS");
    break;
  case T_MUL:
    printf("MUL");
    break;
  case T_DIV:
    printf("DIV");
    break;
  case T_MOD:
    printf("MOD");
    break;
  case T_LT:
    printf("LT");
    break;
  case T_GT:
    printf("GT");
    break;
  case T_LE:
    printf("LE");
    break;
  case T_GE:
    printf("GE");
    break;
  case T_EQ:
    printf("EQ");
    break;
  case T_NE:
    printf("NE");
    break;
  case T_AND:
    printf("AND");
    break;
  case T_OR:
    printf("OR");
    break;
  }
}

void print_unop(enum UnOpType op) {
  switch (op) {
  case T_UMINUS:
    printf("UMINUS");
    break;
  case T_NOT:
    printf("NOT");
    break;
  }
}

void print_expr(struct expr * e) {
  switch (e -> t) {
  case T_CONST:
    printf("CONST(%d)", e -> d.CONST.value);
    break;
  case T_VAR:
    printf("VAR(%s)", e -> d.VAR.name);
    break;
  case T_BINOP:
    print_binop(e -> d.BINOP.op);
    printf("(");
    print_expr(e -> d.BINOP.left);
    printf(",");
    print_expr(e -> d.BINOP.right);
    printf(")");
    break;
  case T_UNOP:
    print_unop(e -> d.UNOP.op);
    printf("(");
    print_expr(e -> d.UNOP.arg);
    printf(")");
    break;
  case T_DEREF:
    printf("DEREF(");
    print_expr(e -> d.DEREF.arg);
    printf(")");
    break;
  case T_MALLOC:
    printf("MALLOC(");
    print_expr(e -> d.MALLOC.arg);
    printf(")");
    break;
  case T_RI:
    printf("READ_INT()");
    break;
  case T_RC:
    printf("READ_CHAR()");
    break;

  // new types
  case T_LEN:
    printf("LEN(");
    print_expr(e -> d.LEN.arg);
    printf(")");
    break;
  case T_RS:
    printf("READ_STRING()");
    break;
  case T_SA:
    print_expr(e -> d.SA.array_arg);
    printf("[");
    print_expr(e -> d.SA.index_arg);
    printf("]");
    break;
  }
}

void print_cmd(struct cmd * c) {
  switch (c -> t) {
  case T_DECL:
    printf("DECL(%s)", c -> d.DECL.name);
    break;
  case T_ASGN:
    printf("ASGN(");
    print_expr(c -> d.ASGN.left);
    printf(",");
    print_expr(c -> d.ASGN.right);
    printf(")");
    break;
  case T_SEQ:
    printf("SEQ(");
    print_cmd(c -> d.SEQ.left);
    printf(",");
    print_cmd(c -> d.SEQ.right);
    printf(")");
    break;
  case T_IF:
    printf("IF(");
    print_expr(c -> d.IF.cond);
    printf(",");
    print_cmd(c -> d.IF.left);
    printf(",");
    print_cmd(c -> d.IF.right);
    printf(")");
    break;
  case T_WHILE:
    printf("WHILE(");
    print_expr(c -> d.WHILE.cond);
    printf(",");
    print_cmd(c -> d.WHILE.body);
    printf(")");
    break;
  case T_WI:
    printf("WRITE_INT(");
    print_expr(c -> d.WI.arg);
    printf(")");
    break;
  case T_WC:
    printf("WRITE_CHAR(");
    print_expr(c -> d.WC.arg);
    printf(")");
    break;

  // new types
  case T_WS:
    printf("WRITE_CHAR(");
    print_expr(c -> d.WC.arg);
    printf(")");
    break;
  }
}

unsigned int build_nat(char * c, int len) {
  int s = 0, i = 0;
  for (i = 0; i < len; ++i) {
    if (s > 429496729) {
      printf("We cannot handle natural numbers greater than 4294967295.\n");
      exit(0);
    }
    if (s == 429496729 && c[i] > '5') {
      printf("We cannot handle natural numbers greater than 4294967295.\n");
      exit(0);
    }
    s = s * 10 + (c[i] - '0');
  }
  return s;
}

char * new_str(char * str, int len) {
  char * res = (char *) malloc(sizeof(char) * (len + 1));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  strcpy(res, str);
  return res;
}

const char* get_token_name(int token) {
    switch (token) {
        case YYEOF: return "YYEOF";
        case YYerror: return "YYerror";
        case YYUNDEF: return "YYUNDEF";
        case TM_NAT: return "TM_NAT";
        case TM_IDENT: return "TM_IDENT";
        case TM_LEFT_BRACE: return "TM_LEFT_BRACE";
        case TM_RIGHT_BRACE: return "TM_RIGHT_BRACE";
        case TM_LEFT_PAREN: return "TM_LEFT_PAREN";
        case TM_RIGHT_PAREN: return "TM_RIGHT_PAREN";
        case TM_SEMICOL: return "TM_SEMICOL";
        case TM_MALLOC: return "TM_MALLOC";
        case TM_RI: return "TM_RI";
        case TM_RC: return "TM_RC";
        case TM_WI: return "TM_WI";
        case TM_WC: return "TM_WC";
        case TM_VAR: return "TM_VAR";
        case TM_IF: return "TM_IF";
        case TM_THEN: return "TM_THEN";
        case TM_ELSE: return "TM_ELSE";
        case TM_WHILE: return "TM_WHILE";
        case TM_DO: return "TM_DO";
        case TM_ASGNOP: return "TM_ASGNOP";
        case TM_OR: return "TM_OR";
        case TM_AND: return "TM_AND";
        case TM_NOT: return "TM_NOT";
        case TM_LT: return "TM_LT";
        case TM_LE: return "TM_LE";
        case TM_GT: return "TM_GT";
        case TM_GE: return "TM_GE";
        case TM_EQ: return "TM_EQ";
        case TM_NE: return "TM_NE";
        case TM_PLUS: return "TM_PLUS";
        case TM_MINUS: return "TM_MINUS";
        case TM_MUL: return "TM_MUL";
        case TM_DIV: return "TM_DIV";
        case TM_MOD: return "TM_MOD";
        case TM_COMMA: return "TM_COMMA";
        case TM_LSB: return "TM_LSB";
        case TM_RSB: return "TM_RSB";
        case TM_CL: return "TM_CL";
        case TM_SL: return "TM_SL";
        case TM_LEN: return "TM_LEN";
        case TM_RS: return "TM_RS";
        case TM_WS: return "TM_WS";
        default: return "UNKNOWN_TOKEN";
    }
}

void print_token(int token) {
    const char* token_name = get_token_name(token);
    
    if (token == TM_NAT) {
        printf("%s(%d)\n", token_name, yylval.n);
    }
    else if (token == TM_IDENT) {
        printf("%s(%s)\n", token_name, yylval.i);
    }
    else {
        printf("%s\n", token_name);
    }
}
