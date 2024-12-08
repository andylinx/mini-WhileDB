#include <stdlib.h>
#include <string.h>
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
    exit(1);
  }
  return res;
}

struct cont_list *new_cont_list_ptr()
{
  struct cont_list *res = (struct cont_list *)malloc(sizeof(struct cont_list));
  if (res == NULL)
  {
    printf("Failure in malloc.\n");
    exit(1);
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

struct value_type eval(struct expr *e)
{
  struct value_type result, tmp;
  switch (e->t)
  {
  case T_CONST:
  {
    result.is_array = 0;
    result.data.single_value = (long long)e->d.CONST.value;
    return result;
  }
  case T_VAR:
  {
    // switch case by data type
    int opt = SLL_hash_var_type(var_state, e->d.VAR.name);
    if (opt == 0)
    {
      result.is_array = 0;
      result.data.single_value = SLL_hash_get_var(var_state, e->d.VAR.name);
      return result;
    }
    else if (opt == 1)
    {
      // get array and return element by pos
      long long *arr = SLL_hash_get_array(var_state, e->d.VAR.name);
      int len = SLL_hash_get_array_len(var_state, e->d.VAR.name);
      if (len == -1)
      {
        printf("invalid array\n");
        exit(1);
      }
      result.is_array = 1;
      result.data.array_value.length = len;
      result.data.array_value.array = arr;
      return result;
    }
    else if (opt == -1)
    {
      printf("Error: variable not found.\n");
      exit(1);
    }
  }
  case T_BINOP:
    if (e->d.BINOP.op == T_AND)
    {
      result = eval(e->d.BINOP.left);
      if (result.is_array == 0)
      {
        if (result.data.single_value)
        {
          result = eval(e->d.BINOP.right);
          if (result.is_array == 0)
            return result;
          else
          {
            // such as "t&&s" where t is an array
            printf("invalid right expression!");
            exit(1);
          }
        }
        else
        {
          result.data.single_value = 0;
          return result;
        }
      }
      else
      {
        // such as "t&&s" where t is an array
        printf("invalid left expression!");
        exit(1);
      }
      return result;
    }
    else if (e->d.BINOP.op == T_OR)
    {
      result = eval(e->d.BINOP.left);
      if (result.is_array == 0)
      {
        if (result.data.single_value)
          return result;
        else
        {
          result = eval(e->d.BINOP.right);
          if (result.is_array == 0)
            return result;
          else
          {
            // such as "t&&s" where t is an array
            printf("invalid right expression!");
            exit(1);
          }
        }
      }
      else
      {
        // such as "t&&s" where t is an array
        printf("invalid left expression!");
        exit(1);
      }
      return result;
    }
    else
    {
      struct value_type lef_val, rig_val;
      lef_val = eval(e->d.BINOP.left);
      rig_val = eval(e->d.BINOP.right);
      switch (e->d.BINOP.op)
      {
      case T_PLUS:
      {
        if (lef_val.is_array == 1 && rig_val.is_array == 0)
        {
          result.is_array = 1;
          result.data.array_value.array = lef_val.data.array_value.array + rig_val.data.single_value;
          result.data.array_value.length = lef_val.data.array_value.length;
        }
        else if (lef_val.is_array == 0 && rig_val.is_array == 1)
        {
          result.is_array = 1;
          result.data.array_value.array = rig_val.data.array_value.array + lef_val.data.single_value;
          result.data.array_value.length = rig_val.data.array_value.length;
        }
        else if (lef_val.is_array == 1 && rig_val.is_array == 1)
        {
          printf("invalid plus!");
          exit(1);
        }
        else
        {
          result.is_array = 0;
          result.data.single_value = lef_val.data.single_value + rig_val.data.single_value;
        }
        return result;
      }
      case T_MINUS:
      {
        if (lef_val.is_array == 1 && rig_val.is_array == 0)
        {
          result.is_array = 1;
          result.data.array_value.array = lef_val.data.array_value.array - rig_val.data.single_value;
          result.data.array_value.length = lef_val.data.array_value.length;
        }
        else if (lef_val.is_array == 0 && rig_val.is_array == 0)
        {
          result.is_array = 0;
          result.data.single_value = lef_val.data.single_value - rig_val.data.single_value;
        }
        else
        {
          printf("invalid minus!");
          exit(1);
        }
        return result;
      }
      case T_MUL:
      {
        if (lef_val.is_array == 0 && rig_val.is_array == 0)
        {
          result.is_array = 0;
          result.data.single_value = lef_val.data.single_value * rig_val.data.single_value;
        }
        else
        {
          printf("invalid multiply!");
          exit(1);
        }
        return result;
      }
      case T_DIV:
      {
        if (lef_val.is_array == 0 && rig_val.is_array == 0)
        {
          result.is_array = 0;
          result.data.single_value = lef_val.data.single_value / rig_val.data.single_value;
        }
        else
        {
          printf("invalid divide!");
          exit(1);
        }
        return result;
      }
      case T_MOD:
      {
        if (lef_val.is_array == 0 && rig_val.is_array == 0)
        {
          result.is_array = 0;
          result.data.single_value = lef_val.data.single_value % rig_val.data.single_value;
        }
        else
        {
          printf("invalid mod!");
          exit(1);
        }
        return result;
      }
      case T_LT:
      {
        if (lef_val.is_array == 0 && rig_val.is_array == 0)
        {
          result.is_array = 0;
          result.data.single_value = lef_val.data.single_value < rig_val.data.single_value;
        }
        else
        {
          printf("invalid less than!");
          exit(1);
        }
        return result;
      }
      case T_GT:
      {
        if (lef_val.is_array == 0 && rig_val.is_array == 0)
        {
          result.is_array = 0;
          result.data.single_value = lef_val.data.single_value > rig_val.data.single_value;
        }
        else
        {
          printf("invalid greater than!");
          exit(1);
        }
        return result;
      }
      case T_LE:
      {
        if (lef_val.is_array == 0 && rig_val.is_array == 0)
        {
          result.is_array = 0;
          result.data.single_value = lef_val.data.single_value <= rig_val.data.single_value;
        }
        else
        {
          printf("invalid less equal!");
          exit(1);
        }
        return result;
      }
      case T_GE:
      {
        if (lef_val.is_array == 0 && rig_val.is_array == 0)
        {
          result.is_array = 0;
          result.data.single_value = lef_val.data.single_value >= rig_val.data.single_value;
        }
        else
        {
          printf("invalid greater equal!");
          exit(1);
        }
        return result;
      }
      case T_EQ:
      {
        if (lef_val.is_array == 0 && rig_val.is_array == 0)
        {
          result.is_array = 0;
          result.data.single_value = lef_val.data.single_value == rig_val.data.single_value;
        }
        else
        {
          printf("invalid equal!");
          exit(1);
        }
        return result;
      }
      case T_NE:
      {
        if (lef_val.is_array == 0 && rig_val.is_array == 0)
        {
          result.is_array = 0;
          result.data.single_value = lef_val.data.single_value != rig_val.data.single_value;
        }
        else
        {
          printf("invalid unequal!");
          exit(1);
        }
        return result;
      }
      default:
      {
        printf("invalid operation!");
        exit(1);
      }; // impossible case
      }
    }
  case T_UNOP:
  {
    result = eval(e->d.UNOP.arg);
    if (result.is_array)
    {
      printf("invalid unop!");
      exit(1);
    }
    if (e->d.UNOP.op == T_NOT)
      result.data.single_value = !result.data.single_value;
    else
      result.data.single_value = -result.data.single_value;
    return result;
  }
  case T_DEREF:
  {
    tmp = eval(e->d.DEREF.arg);
    result.is_array = 0;
    if (tmp.is_array)
      result.data.single_value = *tmp.data.array_value.array;
    else
      result.data.single_value = *(long long *)tmp.data.single_value;
    return result;
  }
  case T_MALLOC:
  {
    tmp = eval(e->d.MALLOC.arg);
    if (tmp.is_array)
    {
      printf("invalid malloc");
      exit(1);
    }
    long long arg_val = tmp.data.single_value;
    if (arg_val % 8 != 0)
    {
      arg_val = (arg_val | 7) + 1;
    }
    result.is_array = 0;
    result.data.single_value = (long long)malloc(arg_val);
    return result;
  }
  case T_RI:
  {
    long long res;
    scanf("%lld", &res);
    result.is_array = 0;
    result.data.single_value = res;
    return result;
  }
  case T_RC:
  {
    char res;
    scanf("%c", &res);
    result.is_array = 0;
    result.data.single_value = (long long)res;
    return result;
  }
  case T_RS:
  {
    // read string first, then convert to array
    char str[50000];
    scanf("%49999s", str);
    long long *arr = (long long *)malloc(strlen(str) * sizeof(long long));
    for (int i = 0; i < strlen(str); i++)
      arr[i] = (long long)str[i];
    result.is_array = 2;
    result.data.array_value.array = arr;
    result.data.array_value.length = strlen(str);
    return result;
  }
  case T_LEN:
  {
    long long len = SLL_hash_get_array_len(var_state, e->d.LEN.arg->d.VAR.name);
    if (len == -1)
    {
      printf("invalid array\n");
      exit(1);
    }
    result.is_array = 0;
    result.data.single_value = len;
    return result;
  }
  case T_SA:
  {
    long long *arr = SLL_hash_get_array(var_state, e->d.SA.array_arg->d.VAR.name);
    tmp = eval(e->d.SA.index_arg);
    if (tmp.is_array)
    {
      printf("arr lenth is not valid const");
      exit(1);
    }
    long long index = tmp.data.single_value;
    if (arr == NULL)
    {
      printf("Error: array not found.\n");
      exit(1);
    }
    if (index < 0)
    { // modified to check array bounds
      printf("Error: Array index is negative!\n");
      exit(1);
    }
    result.is_array = 0;
    result.data.single_value = arr[index];
    return result;
  }
  }
}

void step(struct res_prog *r)
{
  struct value_type tmp;
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
        // printf("DECLVAR(%s)\n", c->d.DECL.declaration->d.DECLVAR.name);
        SLL_hash_set_var(var_state, c->d.DECL.declaration->d.DECLVAR.name, 0, 1); // undeclared initialize with 0
        break;
      }
      case T_DECLARRAY:
      {
        tmp = eval(c->d.DECL.declaration->d.DECLARRAY.size);
        if (tmp.is_array)
        {
          printf("invalid array length");
          exit(1);
        }
        long long size = tmp.data.single_value;
        long long *arr = (long long *)malloc(size * sizeof(long long));
        for (long long i = 0; i < size; i++)
          arr[i] = 0;
        SLL_hash_set_array(var_state, c->d.DECL.declaration->d.DECLARRAY.name, size, arr);
        break;
      }
      case T_DECLVARINIT:
      {
        tmp = eval(c->d.DECL.declaration->d.DECLVARINIT.init);
        if (tmp.is_array)
        {
          printf("invalid var value");
          exit(1);
        }
        long long init = tmp.data.single_value;
        SLL_hash_set_var(var_state, c->d.DECL.declaration->d.DECLVARINIT.name, init, 1);
        break;
      }
      case T_DECLARARRAYINIT:
      {
        tmp = eval(c->d.DECL.declaration->d.DECLARARRAYINIT.size);
        if (tmp.is_array)
        {
          printf("invalid array length");
          exit(1);
        }
        long long size = tmp.data.single_value;
        if (size <= 0)
        {
          printf("Error: array size is negative\n");
          exit(1);
        }
        long long *arr = (long long *)malloc(size * sizeof(long long));
        struct expr_list *init = c->d.DECL.declaration->d.DECLARARRAYINIT.init;
        for (long long i = 0; i < size; i++)
        {
          if (init != NULL)
          {
            // check if it is a name of other array, if so error
            if (init->head->t == T_VAR)
            {
              if (SLL_hash_var_type(var_state, init->head->d.VAR.name) == 1)
              {
                printf("Error: array initializer is an array\n");
                exit(1);
              }
            }
            tmp = eval(init->head);
            if (tmp.is_array)
            {
              printf("invalid initialize value");
              exit(1);
            }
            arr[i] = tmp.data.single_value;
            init = init->tail;
          }
          else
            arr[i] = 0;
        }
        if (init != NULL)
        {
          printf("Error: too many initializers\n");
          exit(1);
        }
        SLL_hash_set_array(var_state, c->d.DECL.declaration->d.DECLARARRAYINIT.name, size, arr);
        break;
      }
      case T_DECLSEQ:
      {
        // printf("DECLSEQ\n");
        struct decl *left = c->d.DECL.declaration->d.DECLSEQ.left;
        struct decl *right = c->d.DECL.declaration->d.DECLSEQ.right;
        struct cmd *new_cmd = TDecl(left);
        r->foc = new_cmd;
        r->ectx = CL_Cons(TDecl(right), r->ectx);
        step(r);
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
        tmp = eval(c->d.ASGN.right);
        // is_array = 0, single_value = value
        // is_array = 1, error
        // is_array = 2, string assignment
        if (tmp.is_array == 0)
        {
          SLL_hash_set_var(var_state, c->d.ASGN.left->d.VAR.name, tmp.data.single_value, 0);
        }
        else if (tmp.is_array == 2)
        {
          // do not call SLL_hash_set_array, handle here, do the assignment, no need to malloc
          long long *arr = SLL_hash_get_array(var_state, c->d.ASGN.left->d.VAR.name);
          long long len = SLL_hash_get_array_len(var_state, c->d.ASGN.left->d.VAR.name);
          if (len < tmp.data.array_value.length)
          {
            printf("Error: too many initializers\n");
            exit(1);
          }
          for (long long i = 0; i < len; i++)
          {
            if (i < tmp.data.array_value.length)
              arr[i] = tmp.data.array_value.array[i];
            else
              arr[i] = 0;
          }
          free(tmp.data.array_value.array);
        }
        else
        {
          printf("invalid var\n");
          exit(1);
        }
        break;
      }
      case T_DEREF:
      {
        long long *lhs;
        long long rhs;
        tmp = eval(c->d.ASGN.left->d.DEREF.arg);
        if (tmp.is_array)
          lhs = tmp.data.array_value.array;
        else
          lhs = (long long *)tmp.data.single_value;
        tmp = eval(c->d.ASGN.right);
        if (tmp.is_array)
        {
          printf("invalid assign right value");
          exit(1);
        }
        rhs = tmp.data.single_value;
        *lhs = rhs;
        break;
      }
      case T_SA:
      {
        long long *arr = SLL_hash_get_array(var_state, c->d.ASGN.left->d.SA.array_arg->d.VAR.name);
        tmp = eval(c->d.ASGN.left->d.SA.index_arg);
        if (tmp.is_array)
        {
          printf("invalid array index");
          exit(1);
        }
        long long index = tmp.data.single_value;
        tmp = eval(c->d.ASGN.right);
        if (tmp.is_array)
        {
          printf("invalid right value");
          exit(1);
        }
        long long rhs = tmp.data.single_value;
        arr[index] = rhs;
        break;
      }
      default:
        printf("invalid assign left value!\n");
        exit(1);
      }
      r->foc = NULL;
      break;
    case T_ASGNLIST:
      switch (c->d.ASGNLIST.left->t)
      {
      case T_VAR:
      {
        struct value_type string_array = eval(c->d.ASGNLIST.left);
        struct expr_list *str_list = c->d.ASGNLIST.right;
        struct expr_list *str_list_copy = c->d.ASGNLIST.right;
        if (!string_array.is_array)
        {
          printf("invalid assignlist left value\n");
          exit(1);
        }
        long long *arr = string_array.data.array_value.array;
        for (int i = 0; i < string_array.data.array_value.length; i++)
          arr[i] = 0;
        int len = 0;
        while (str_list != NULL)
        {
          tmp = eval(str_list->head);
          if (tmp.is_array)
          {
            printf("invalid assignlist value\n");
            exit(1);
          }
          len++;
          str_list = str_list->tail;
        }
        if (len > string_array.data.array_value.length)
        {
          printf("Error: too many initializers\n");
          exit(1);
        }
        str_list = str_list_copy;
        len = 0;
        while (str_list != NULL)
        {
          tmp = eval(str_list->head);
          arr[len] = tmp.data.single_value;
          len++;
          str_list = str_list->tail;
        }
        break;
      }
      case T_DEREF:
      {
        printf("Error: variable is not an array!\n");
        exit(1);
        break;
      }
      case T_SA:
      {
        printf("Error: variable is not an array!\n");
        exit(1);
        break;
      }
      default:
      {
        printf("Error: variable is not an array!\n");
        exit(1);
      }
      }
      r->foc = NULL;
      break;
    case T_SEQ:
      r->foc = c->d.SEQ.left;
      r->ectx = CL_Cons(c->d.SEQ.right, r->ectx);
      break;
    case T_IF:
    {
      tmp = eval(c->d.IF.cond);
      if (tmp.is_array)
      {
        printf("invalid cond!");
        exit(1);
      }
      if (tmp.data.single_value)
      {
        r->foc = c->d.IF.left;
      }
      else
      {
        r->foc = c->d.IF.right;
      }
      break;
    }
    case T_WHILE:
    {
      tmp = eval(c->d.WHILE.cond);
      if (tmp.is_array)
      {
        printf("invalid cond!");
        exit(1);
      }
      if (tmp.data.single_value)
      {
        r->foc = c->d.WHILE.body;
        r->ectx = CL_Cons(c, r->ectx);
      }
      else
      {
        r->foc = NULL;
      }
      break;
    }
    case T_WI:
    {
      tmp = eval(c->d.WI.arg);
      if (tmp.is_array)
      {
        printf("invalid output value\n");
        exit(1);
      }
      long long rhs = tmp.data.single_value;
      printf("%lld\n", rhs);
      r->foc = NULL;
      break;
    }
    case T_WC:
    {
      tmp = eval(c->d.WC.arg);
      if (tmp.is_array)
      {
        printf("invalid output value\n");
        exit(1);
      }
      char rhs = (char)tmp.data.single_value;
      printf("%c", rhs);
      r->foc = NULL;
      break;
    }
    case T_WS:
    {
      tmp = eval(c->d.WS.arg);
      if (tmp.is_array == 0)
      {
        printf("invalid output value\n");
        exit(1);
      }
      long long *arr = tmp.data.array_value.array;
      for (int i = 0; i < tmp.data.array_value.length; i++)
      {
        // only output printable characters
        if (arr[i] <= 0 || arr[i] > 127)
          break;
        printf("%c", (char)arr[i]);
      }
      printf("\n");
      r->foc = NULL;
      break;
    }
    case T_WSL:
    {
      struct expr_list *str_list = c->d.WSL.arg;
      while (str_list != NULL)
      {
        tmp = eval(str_list->head);
        if (tmp.is_array)
        {
          printf("invalid output value\n");
          exit(1);
        }
        char rhs = (char)tmp.data.single_value;
        printf("%c", rhs);
        str_list = str_list->tail;
      }
      printf("\n");
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