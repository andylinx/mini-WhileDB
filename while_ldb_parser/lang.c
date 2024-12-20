#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lang.h"
// #include "parser.h"

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

// new expression
struct expr * TConstChar(char c) {
  struct expr * e = new_expr_ptr();
  e -> t = T_CONST;
  e -> d.CONST.value = (int)c;
  return e;
}

struct expr * TVar(char * name) {
  struct expr * res = new_expr_ptr();
  res -> t = T_VAR;
  res -> d.VAR.name = name;
  return res;
}
struct expr_list * TStringToExprList(char * str) {
    if (str == NULL || *str == '\0') {
        return NULL;
    }

    // Process escape sequences
    char *processed = (char *)malloc(strlen(str) + 1);
    process_escape_characters(str, processed);
    
    struct expr * e = TConstChar(*processed);
    struct expr_list * result = TExprList(e, TStringToExprList(processed + 1));
    
    free(processed);
    return result;
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
struct expr * TLenList(struct expr_list * list) {
    struct expr * res = new_expr_ptr();
    res->t = T_CONST;
    
    // Count the length of the expression list
    int length = 0;
    struct expr_list * current = list;
    while (current != NULL) {
        length++;
        current = current->tail;
    }
    
    res->d.CONST.value = length;
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


struct cmd * TDecl(struct decl * declaration) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_DECL;
  res -> d.DECL.declaration = declaration;
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
struct cmd * TWriteStringList(struct expr_list * str_list) {
    struct cmd * res = new_cmd_ptr();
    res->t = T_WSL;
    res->d.WSL.arg = str_list;
    return res;
}

struct cmd * TAsgnDeref(struct expr * left, struct expr * right) {
  struct cmd * res = new_cmd_ptr();
  res -> t = T_ASGNDEREF;
  res -> d.ASGN.left = left;
  res -> d.ASGN.right = right;
  return res;
}

struct cmd * TAsgnList(struct expr * left, struct expr_list * right) {
    // printf("Is here!\n");
    // print_expr(left);
    // printf("\n");
    // print_expr_list(right);
    // printf("\n");
    struct cmd * res = new_cmd_ptr();
    res->t = T_ASGNLIST;
    res->d.ASGNLIST.left = left;
    res->d.ASGNLIST.right = right;
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
    print_decl(c -> d.DECL.declaration);
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
  case T_WSL:
    printf("WRITE_STRING_LIST(");
    print_expr_list(c -> d.WSL.arg);
    printf(")");
    break;
  case T_ASGNDEREF:
    printf("ASGNDEREF(");
    print_expr(c -> d.ASGN.left);
    printf(",");
    print_expr(c -> d.ASGN.right);
    printf(")");
    break;
  case T_ASGNLIST:
    printf("ASGNLIST(");
    print_expr(c -> d.ASGNLIST.left);
    printf(",");
    print_expr_list(c -> d.ASGNLIST.right);
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

struct decl * new_decl_ptr() {
  struct decl * res = (struct decl *) malloc(sizeof(struct decl));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct decl * TDeclVar(char * name) {
  struct decl * res = new_decl_ptr();
  res -> t = T_DECLVAR;
  res -> d.DECLVAR.name = name;
  return res;
}

struct decl * TDeclArray(char * name, struct expr * size) {
  struct decl * res = new_decl_ptr();
  res -> t = T_DECLARRAY;
  res -> d.DECLARRAY.name = name;
  res -> d.DECLARRAY.size = size;
  return res;
}

struct decl * TDeclSeq(struct decl * left, struct decl * right) {
  struct decl * res = new_decl_ptr();
  res -> t = T_DECLSEQ;
  res -> d.DECLSEQ.left = left;
  res -> d.DECLSEQ.right = right;
  return res;
}

struct expr_list * new_expr_list_ptr() {
  struct expr_list * res = (struct expr_list *) malloc(sizeof(struct expr_list));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct decl * TDeclVarInit(char * name, struct expr * init) {
  struct decl * res = new_decl_ptr();
  res -> t = T_DECLVARINIT;
  res -> d.DECLVARINIT.name = name;
  res -> d.DECLVARINIT.init = init;
  return res;
}

struct decl * TDeclArrayInit(char * name, struct expr * size, struct expr_list * init) {
  struct decl * res = new_decl_ptr();
  res -> t = T_DECLARARRAYINIT;
  res -> d.DECLARARRAYINIT.name = name;
  res -> d.DECLARARRAYINIT.size = size;
  res -> d.DECLARARRAYINIT.init = init;
  return res;
}

struct expr_list * TExprList(struct expr * head, struct expr_list * tail) {
  struct expr_list * res = new_expr_list_ptr();
  res -> head = head;
  res -> tail = tail;
  return res;
}

// Move print_expr_list implementation before print_decl
void print_expr_list(struct expr_list * l) {
  if (l == NULL) return;
  print_expr(l -> head);
  if (l -> tail != NULL) {
    printf(",");
    print_expr_list(l -> tail);
  }
}

void print_decl(struct decl * d) {
  switch (d -> t) {
  case T_DECLVAR:
    printf("DECLVAR(%s)", d -> d.DECLVAR.name);
    break;
  case T_DECLARRAY:
    printf("DECLARRAY(%s,", d -> d.DECLARRAY.name);
    print_expr(d -> d.DECLARRAY.size);
    printf(")");
    break;
  case T_DECLSEQ:
    printf("DECLSEQ(");
    print_decl(d -> d.DECLSEQ.left);
    printf(",");
    print_decl(d -> d.DECLSEQ.right);
    printf(")");
    break;
  case T_DECLVARINIT:
    printf("DECLVARINIT(%s,", d -> d.DECLVARINIT.name);
    print_expr(d -> d.DECLVARINIT.init);
    printf(")");
    break;
  case T_DECLARARRAYINIT:
    printf("DECLARARRAYINIT(%s,", d -> d.DECLARARRAYINIT.name);
    print_expr(d -> d.DECLARARRAYINIT.size);
    printf(",");
    print_expr_list(d -> d.DECLARARRAYINIT.init);
    printf(")");
    break;
  }
}

// Helper function to convert octal digits
int parse_octal(const char **src) {
    int value = 0;
    int count = 0;
    while (count < 3 && **src >= '0' && **src <= '7') {
        value = value * 8 + (**src - '0');
        (*src)++;
        count++;
    }
    (*src)--; // Step back one position for consistency
    return value;
}

// Helper function to convert hexadecimal digits
int parse_hex(const char **src) {
    int value = 0;
    while (isxdigit(**src)) {
        if (**src >= '0' && **src <= '9')
            value = value * 16 + (**src - '0');
        else if (**src >= 'a' && **src <= 'f')
            value = value * 16 + (**src - 'a' + 10);
        else if (**src >= 'A' && **src <= 'F')
            value = value * 16 + (**src - 'A' + 10);
        (*src)++;
    }
    (*src)--; // Step back one position for consistency
    return value;
}

void process_escape_characters(const char *A, char *B) {
    const char *src = A;  // Pointer to traverse the source string
    char *dst = B;        // Pointer to construct the destination string

    while (*src) {
        if (*src == '\\' && *(src + 1)) {
            src++; // Move to the character after '\'
            switch (*src) {
                case 'n': *dst = '\n'; break;
                case 't': *dst = '\t'; break;
                case 'r': *dst = '\r'; break;
                case 'v': *dst = '\v'; break;
                case 'f': *dst = '\f'; break;
                case 'a': *dst = '\a'; break;
                case 'b': *dst = '\b'; break;
                case '\\': *dst = '\\'; break;
                case '\'': *dst = '\''; break;
                case '\"': *dst = '\"'; break;
                case '\?': *dst = '\?'; break;
                case '0':  // Null character or octal values
                    src++;
                    *dst = parse_octal(&src);
                    break;
                case 'x':  // Hexadecimal values
                    src++;
                    *dst = parse_hex(&src);
                    break;
                default:
                    // Unrecognized escape, keep the original `\` and the next character
                    *(dst++) = '\\';
                    *dst = *src;
                    break;
            }
        } else {
            // Copy regular characters
            *dst = *src;
        }
        src++;
        dst++;
    }
    *dst = '\0'; // Null-terminate the result string
}