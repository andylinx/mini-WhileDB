#ifndef LIB_H_INCLUDED
#define LIB_H_INCLUDED

#define NONE 4294967295

struct value_type;
struct SLL_hash_table;
struct SLL_hash_table *init_SLL_hash();
long long SLL_hash_get_var(struct SLL_hash_table *t, char *key);
void SLL_hash_set_var(struct SLL_hash_table *t, char *key, long long value);
void SLL_hash_delete(struct SLL_hash_table *t, char *key);
int SLL_hash_var_type(struct SLL_hash_table *t, char *key);
long long *SLL_hash_get_array(struct SLL_hash_table *t, char *key);
void SLL_hash_set_array(struct SLL_hash_table *t, char *key, int len, long long *arr);
long long SLL_hash_get_array_len(struct SLL_hash_table *t, char *key);

#endif
