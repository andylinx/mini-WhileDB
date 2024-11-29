#include <stdlib.h>
#include "lang.h"
#include "interpreter.h"
#include "lib.h"

struct SLL_hash_table *var_state;

struct res_prog *new_res_prog_ptr()
{
  struct res_prog *res = (struct res_prog *)malloc(sizeof(struct res_prog));
  if (res == NULL)
  {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct cont_list *new_cont_list_ptr()
{
  struct cont_list *res = (struct cont_list *)malloc(sizeof(struct cont_list));
  if (res == NULL)
  {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct cont_list *CL_Nil()
{
  return NULL;
}

struct cont_list *CL_Cons(struct cmd *c, struct cont_list *l)
{
  struct cont_list *res = new_cont_list_ptr();
  res->c = c;
  res->link = l;
  return res;
}

struct res_prog *init_res_prog(struct cmd *c)
{
  struct res_prog *res = new_res_prog_ptr();
  res->foc = c;
  res->ectx = CL_Nil();
  var_state = init_SLL_hash();
  return res;
}

long long eval(struct expr *e)
{
  switch (e->t)
  {
  case T_CONST:
    return (long long)e->d.CONST.value;
  case T_VAR:
  {
    // switch case by data type
    int opt = SLL_hash_var_type(var_state, e->d.VAR.name);
    if (opt == 0)
      return SLL_hash_get_var(var_state, e->d.VAR.name);
    else if (opt == 1)
    {
      // get array and return element by pos
      long long *arr = SLL_hash_get_array(var_state, e->d.VAR.name);
      return arr[0];
    }
    else if (opt == -1)
    {
      printf("Error: variable not found.\n");
      exit(0);
    }
  }
  case T_BINOP:
    if (e->d.BINOP.op == T_AND)
    {
      if (eval(e->d.BINOP.left))
      {
        return eval(e->d.BINOP.right);
      }
      else
      {
        return 0;
      }
    }
    else if (e->d.BINOP.op == T_OR)
    {
      if (eval(e->d.BINOP.left))
      {
        return 1;
      }
      else
      {
        return eval(e->d.BINOP.right);
      }
    }
    else
    {
      long long left_val = eval(e->d.BINOP.left);
      long long right_val = eval(e->d.BINOP.right);
      switch (e->d.BINOP.op)
      {
      case T_PLUS:
        return left_val + right_val;
      case T_MINUS:
        return left_val - right_val;
      case T_MUL:
        return left_val * right_val;
      case T_DIV:
        return left_val / right_val;
      case T_MOD:
        return left_val % right_val;
      case T_LT:
        return left_val < right_val;
      case T_GT:
        return left_val > right_val;
      case T_LE:
        return left_val <= right_val;
      case T_GE:
        return left_val >= right_val;
      case T_EQ:
        return left_val == right_val;
      case T_NE:
        return left_val != right_val;
      default:
        return 0; // impossible case
      }
    }
  case T_UNOP:
    if (e->d.UNOP.op == T_NOT)
    {
      return !eval(e->d.UNOP.arg);
    }
    else
    {
      return -eval(e->d.UNOP.arg);
    }
  case T_DEREF:
    return *(long long *)eval(e->d.DEREF.arg);
  case T_MALLOC:
  {
    long long arg_val = eval(e->d.MALLOC.arg);
    if (arg_val % 8 != 0)
    {
      arg_val = (arg_val | 7) + 1;
    }
    return (long long)malloc(arg_val);
  }
  case T_RI:
  {
    long long res;
    scanf("%lld", &res);
    return res;
  }
  case T_RC:
  {
    char res;
    scanf("%c", &res);
    return (long long)res;
  }
  case T_LEN:
  {
    long long len = SLL_hash_get_array_len(var_state, e->d.LEN.arg->d.VAR.name);
    if (len == -1)
    {
      printf("invalid array\n");
      exit(0);
    }
    return len;
  }
  }
}

void step(struct res_prog *r)
{
  if (r->foc == NULL)
  {
    struct cont_list *cl = r->ectx;
    r->foc = cl->c;
    r->ectx = cl->link;
    free(cl);
  }
  else
  {
    struct cmd *c = r->foc;
    switch (c->t)
    {
    case T_DECL:
      // modified to apply initialize with value
      switch (c->d.DECL.declaration->t)
      {
      case T_DECLVAR:
      {
        SLL_hash_set_var(var_state, c->d.DECL.declaration->d.DECLVAR.name, 0); // undeclared initialize with 0
        break;
      }
      case T_DECLARRAY:
      {
        long long size = eval(c->d.DECL.declaration->d.DECLARRAY.size);
        long long *arr = (long long *)malloc(size * sizeof(long long));
        for (long long i = 0; i < size; i++)
          arr[i] = 0;
        SLL_hash_set_array(var_state, c->d.DECL.declaration->d.DECLARRAY.name, size, arr);
        break;
      }
      case T_DECLVARINIT:
      {
        long long init = eval(c->d.DECL.declaration->d.DECLVARINIT.init);
        SLL_hash_set_var(var_state, c->d.DECL.declaration->d.DECLVARINIT.name, init);
        break;
      }
      case T_DECLARARRAYINIT:
      {
        long long size = eval(c->d.DECL.declaration->d.DECLARARRAYINIT.size);
        long long *arr = (long long *)malloc(size * sizeof(long long));
        struct expr_list *init = c->d.DECL.declaration->d.DECLARARRAYINIT.init;
        for (long long i = 0; i < size; i++)
        {
          arr[i] = eval(init->head);
          init = init->tail;
        }
        SLL_hash_set_array(var_state, c->d.DECL.declaration->d.DECLARARRAYINIT.name, size, arr);
        break;
      }
      }
      r->foc = NULL;
      break;
    case T_ASGN:
      switch (c->d.ASGN.left->t)
      {
      case T_VAR:
      {
        long long rhs = eval(c->d.ASGN.right);
        SLL_hash_set_var(var_state, c->d.ASGN.left->d.VAR.name, rhs);
        break;
      }
      case T_DEREF:
      {
        long long *lhs = (long long *)eval(c->d.ASGN.left->d.DEREF.arg);
        long long rhs = eval(c->d.ASGN.right);
        *lhs = rhs;
        break;
      }
      default:
        printf("error!\n");
        exit(0);
      }
      r->foc = NULL;
      break;
    case T_SEQ:
      r->foc = c->d.SEQ.left;
      r->ectx = CL_Cons(c->d.SEQ.right, r->ectx);
      break;
    case T_IF:
      if (eval(c->d.IF.cond))
      {
        r->foc = c->d.IF.left;
      }
      else
      {
        r->foc = c->d.IF.right;
      }
      break;
    case T_WHILE:
      if (eval(c->d.WHILE.cond))
      {
        r->foc = c->d.WHILE.body;
        r->ectx = CL_Cons(c, r->ectx);
      }
      else
      {
        r->foc = NULL;
      }
      break;
    case T_WI:
    {
      long long rhs = eval(c->d.WI.arg);
      printf("%lld", rhs);
      r->foc = NULL;
      break;
    }
    case T_WC:
    {
      char rhs = (char)eval(c->d.WC.arg);
      printf("%c", rhs);
      r->foc = NULL;
      break;
    }
    }
  }
}

int test_end(struct res_prog *r)
{
  if (r->foc == NULL && r->ectx == NULL)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
