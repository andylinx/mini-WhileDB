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
  - '=' assignment. (copy first elements till one array reaches range limit, remaining elements of receiver are set to 0)
  <!-- - '<=>' lexicographical comparison. -->
  <!-- - '+' concatenation. -->
  <!-- - build-in function: to_array(x, n) -> n-element array containing x's. -->
```cpp
var a[10];
*(a + 1) = 1; // a[1], no range check
a[1] = 1;
var b = 1;
```

- Char literal. (C style, using single quote, string are considered array of char)
```cpp
c = 'a'
```

- String support.
  - char literal (single quote) -> var
  - string literal (double quote) -> var[]
```cpp
var str[4];
str = "0123"; // var[3] = {48, 49, 50, 51}
a = '0'; // '0' = 49
```

- Initializer list literal.
```cpp
{1, 2} // var[2]
```

- Variable initialization at creation (including array).
```cpp
var a = 1;
var b[2] = {0, 1};
var c[2] = {1}; // c[] = {1, 0}, comply with = syntax for array.
var s[3] = "abc";
```
- Multiple variables' declaration in one statement.
```cpp
var a, b = 3, c; // declare a, b, c. initialize b = 3. (not a = 3, b = c !!! not python)
```

- Helper function:
  - EXPR: read_string(): read in a string deliminated by '[ \n\t\rEOF]', and return it. (buffer is automatically managed to avoid overflow.)
  - CMD: write_string(a: array): interpret a's elements as char and write sequentially. 

# Lexer:

new symbols:
+ , // to support initalizer list only
+ [
+ ]
+ '
+ "
<!-- + to_array -->
+ len
+ read_string
+ write_string

deprecated symbols:
+ none

new tokens:
+ COMMA: ,
+ LSB: [
+ RSB: ]
+ CL: char literal
+ SL: string literal
<!-- + ILL: initializer list literal -->
+ LEN: len
+ RS: read_string
+ WS: write_string

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

not determined yet.
