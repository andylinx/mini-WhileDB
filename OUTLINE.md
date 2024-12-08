# Goal

In addition to basic whileDB language features, we support the following features:

<!-- - Strongly typed features: -->

- Uniformly typed features:
  - both int and char are considered nature numbers,
    the difference only occurs at function intepretation level.
  - pointers are also considered nature numbers.

```cpp
  var a, c;
  a = 1;
  c = 'c';
  *a; // valid dereference, but no range check.
```

- Array class (fixed length).
  - build-in function: len(a: array): -> ⟦length of array a⟧.
  - writable subscript access with range check.
    - deemed as new expression type, not binary operator.
  <!-- - '=' assignment. (copy first elements till one array reaches range limit, remaining elements of receiver are set to 0) -->
    <!-- - '<=>' lexicographical comparison. -->
    <!-- - '+' concatenation. -->
    <!-- - build-in function: to_array(x, n) -> n-element array containing x's. -->
```cpp
var a[10];
*(a + 1) = 1; // a[1], no range check
a[1] = 1;
var b = 1;
```
- Pointer support.
  - for normal pointer from malloc, ptr+n is byte level.
  - for array pointer, arr+n is integer level (= ptr + n * 8 byte on 64-bit machine).

```cpp
  var p = malloc(80), a[10], b;
  b = *(p + 8); // access 2nd int in p
  b = a[1]; // access 2nd int in a

```

- String support.
  - char literal (single quote) -> var
  - string literal (double quote) -> var[]
    + can be used as array initializer
    + can be assigned to array
      - in case of overflow(too many elements), raise exception.
    + can be passed to write_string()

```cpp
var str[4];
str = "0123"; // an '\0' will be added
a = '0'; // '0' = 49

var str[4]="012";
```

- Variable declaration extension
  - Variable initialization at creation (including array).
    - Initializer list literal.
      - only applicable for array initialize
      - in case of overflow (too many elements), raise exception.

  - Multiple variables' declaration in one statement.

```cpp
var a = 1;
var b[2] = {0, 1};
var c[2] = {1}; // c[] = {1, 0}, comply with C style
var s[3] = "ab";
```

```cpp
var a, b = 3, c; // declare a, b, c. initialize b = 3. (not a = 3, b = c; not python)
```

- Helper function:
  - EXPR: read_string(): read in a string deliminated by '[ \n\t\rEOF]', and return it as (equivalent to) string literal. 
    - buffer is automatically managed to avoid overflow while reading. (but may overflow at assignment after return).
  - CMD: write_string(S: array/string_literal): interpret S's elements as char and write sequentially till '\0' or end of S. 
    - if non-ASCII element found, raise exception.

# Lexer:

new symbols:

- ,
- [
- ]
- '
- "
<!-- + to_array -->
- len
- read_string
- write_string

deprecated symbols:

- none

new tokens:

- COMMA: ,
- LSB: [
- RSB: ]
- CL: char literal
- SL: string literal
<!-- + ILL: initializer list literal -->
- LEN: len
- RS: read_string
- WS: write_string

# Syntax:

<!-- - <=>: comparison should support array. -->

- =: should support array.
<!-- - \+: should support array. -->
- []: array subscript access.
- '': char literal.
- "": string literal.
- {}: initializer list literal.
<!-- - to_array: convert single element to n-element array. -->
- len: length of array.
- read_string: read in chars to a given array.
- write_string: write a given array as chars.

new expression types:

- LEN: len
- RS: read_string
- SA: [] subscript access

new command types:

- WS: write_string

# Interpreter:

## edition:

12.5 zzc

- In our grammer framework, there exist two type of pointers (created by malloc, array name), their arithmetic operation rule is different. For convenience, I change the type of `eval` func, whose return value at present is the type of `value_type`

- add several type checking via new type of return value

**! ! !** feel free to ask, if there exist anything unproper or incorrect.

## currently implemented func

- initialize with value (var, array)
- get the len of array
- support for multiple declarations
- support for array subscript access (Error when subscript is negative, but not when subscript is out of bound, just like the feature in C language)
- support for array assignment statements (i.e. `a[i] = j`)
- `write_string()`
- support pointer arithmetic operation including array name as pointer
- support two types of dereference

## details about array initialization

- the length of the array is not required to be a CONST (e.g. an integer `A` is also accepted)
- the logical sequence of the array initialization:

  - check if the initialization length is legal
  - check if the initializers are legal
  - array assignment
  - check if there are too many initializers

## file description

the `var_state` is stored in a hash-table, where each var is in the form of `(key, value_type)`. detailed implementation is in the `lib.c`

```cpp
struct value_type
{
  int is_array;
  int is_pointer;
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
```

in the `lib.c` we implemented func to get and set the value in the hash-table

```cpp
long long SLL_hash_get_var(struct SLL_hash_table *t, char *key);
void SLL_hash_set_var(struct SLL_hash_table *t, char *key, long long value);
long long *SLL_hash_get_array(struct SLL_hash_table *t, char *key);
void SLL_hash_set_array(struct SLL_hash_table *t, char *key, int len, long long *arr);
long long SLL_hash_get_array_len(struct SLL_hash_table *t, char *key);
int SLL_hash_var_type(struct SLL_hash_table *t, char *key); // 0->var ; 1-> array ; -1 -> not exist
void SLL_hash_delete(struct SLL_hash_table *t, char *key);
```
