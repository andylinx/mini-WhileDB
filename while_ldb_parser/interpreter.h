#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include "lang.h"

struct cont_list
{
    struct cmd *c;
    struct cont_list *link;
};

struct res_prog
{
    struct cmd *foc;
    struct cont_list *ectx;
};

struct res_prog *init_res_prog(struct cmd *c);
void step(struct res_prog *r);
int test_end(struct res_prog *r);

#endif
