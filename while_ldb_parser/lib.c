#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lib.h"
#include "lang.h"

// two data type : var and array
struct value_type
{
  int is_array;
  union
  {
    long long single_value;
    struct
    {
      long long *array;
      int length;
    } array_value;
  } data;
};

struct SLL_hash_cell
{
  char *key;
  struct value_type value;
  struct SLL_hash_cell *tail;
};

struct SLL_hash_table
{
  struct SLL_hash_cell *(h[2048]);
};

unsigned int hash_fun(char *str)
{
  unsigned int s = 0;
  while (str[0] != 0)
  {
    s = (s * 307 + 97 + str[0]) & 2047;
    str++;
  }
  return s;
}

struct SLL_hash_table *init_SLL_hash()
{
  struct SLL_hash_table *res = (struct SLL_hash_table *)malloc(sizeof(struct SLL_hash_table));
  if (res == NULL)
  {
    printf("Failure in malloc.\n");
    exit(0);
  }
  memset(res, 0, sizeof(struct SLL_hash_table));
  return res;
}

long long SLL_hash_get_var(struct SLL_hash_table *t, char *key)
{
  unsigned int s = hash_fun(key);
  struct SLL_hash_cell *p = t->h[s];
  while (p != NULL)
  {
    if (strcmp(key, p->key) == 0)
    {
      return p->value.data.single_value;
    }
    p = p->tail;
  }
  return NONE;
}

void SLL_hash_set_var(struct SLL_hash_table *t, char *key, long long value, bool is_define)
{
  unsigned int s = hash_fun(key);
  struct SLL_hash_cell **d = &(t->h[s]);
  while ((*d) != NULL)
  {
    if (strcmp(key, (*d)->key) == 0)
    {
      if ((*d)->value.is_array)
        printf("Error: variable is an array.\n");
      (*d)->value.data.single_value = value;
      return;
    }
    d = &((*d)->tail);
  }
  if(!is_define){
    // show the undefined variable
    printf("Error: variable %s is not defined.\n", key);
    exit(1);
  }
  *d = (struct SLL_hash_cell *)malloc(sizeof(struct SLL_hash_cell));
  if (*d == NULL)
  {
    printf("Failure in malloc.\n");
    exit(0);
  }
  (*d)->key = new_str(key, strlen(key));
  (*d)->value.is_array = 0;
  (*d)->value.data.single_value = value;
  (*d)->tail = NULL;
  return;
}

void SLL_hash_set_array(struct SLL_hash_table *t, char *key, int len, long long *arr)
{
  unsigned int s = hash_fun(key);
  struct SLL_hash_cell **d = &(t->h[s]);
  while ((*d) != NULL)
  {
    if (strcmp(key, (*d)->key) == 0)
    {
      if (!(*d)->value.is_array) {
        printf("Error: variable is not an array!\n");
        exit(1);
      }
      free((*d)->value.data.array_value.array);
      (*d)->value.is_array = 1;
      (*d)->value.data.array_value.array = arr;
      (*d)->value.data.array_value.length = len;
      return;
    }
    d = &((*d)->tail);
  }
  *d = (struct SLL_hash_cell *)malloc(sizeof(struct SLL_hash_cell));
  if (*d == NULL)
  {
    printf("Failure in malloc.\n");
    exit(0);
  }
  (*d)->key = new_str(key, strlen(key));
  (*d)->value.is_array = 1;
  (*d)->value.data.array_value.array = arr;
  (*d)->value.data.array_value.length = len;
  (*d)->tail = NULL;
  return;
}

long long *SLL_hash_get_array(struct SLL_hash_table *t, char *key)
{
  unsigned int s = hash_fun(key);
  struct SLL_hash_cell *p = t->h[s];
  while (p != NULL)
  {
    if (strcmp(key, p->key) == 0)
    {
      return p->value.data.array_value.array;
    }
    p = p->tail;
  }
  return NULL;
}

// return 1->array 0->var -1->not found
int SLL_hash_var_type(struct SLL_hash_table *t, char *key)
{
  unsigned int s = hash_fun(key);
  struct SLL_hash_cell *p = t->h[s];
  while (p != NULL)
  {
    if (strcmp(key, p->key) == 0)
    {
      return p->value.is_array;
    }
    p = p->tail;
  }
  return -1;
}

long long SLL_hash_get_array_len(struct SLL_hash_table *t, char *key)
{
  unsigned int s = hash_fun(key);
  struct SLL_hash_cell *p = t->h[s];
  while (p != NULL)
  {
    if (strcmp(key, p->key) == 0)
    {
      if (!p->value.is_array)
        return -1;
      return p->value.data.array_value.length;
    }
    p = p->tail;
  }
  return -1;
}

void SLL_hash_delete(struct SLL_hash_table *t, char *key)
{
  unsigned int s = hash_fun(key);
  struct SLL_hash_cell **d = &(t->h[s]);
  while ((*d) != NULL)
  {
    if (strcmp(key, (*d)->key) == 0)
    {
      struct SLL_hash_cell *p = *d;
      if (p->value.is_array)
      {
        free(p->value.data.array_value.array);
      }
      free(p->key);
      *d = p->tail;
      free(p);
      return;
    }
    d = &((*d)->tail);
  }
}
