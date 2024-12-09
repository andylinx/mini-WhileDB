# mini-WhileDB
This is the repo for CS2205 Fall Project, mini-WhileDB with string and array.

## Usage
Besides the basic usage of WhileDB language, we also add some features to the language.

1. String
We have supported declaration of char and string.
```cpp
var a = 'a';
var b[10] = "hello";
```
In our language, we use `var` to declare a variable, and `[10]` to declare the length of the array.

We also support I/O of char/string.
``` cpp
var a[10], b;
a = read_string();
b = read_char();
write_string(a);
write_char(a[5]);
```
Some more details:
We have supported escape character in string, like `\n`, `\t`, `\\`.

2. Array

For array, we support declaration, assignment, dereference, and I/O.

```cpp
  var p = malloc(80), a[10], b;
  b = *(p + 8); // access 2nd int in p
  b = a[1]; // access 2nd int in a
```
Some more details:
    1. Initializer list literal is supported for array declaration.
    2. Multiple variables' declaration in one statement.
    3. support for array subscript access (Error when subscript is negative, but not when subscript is out of bound, just like the feature in C language)
    


## Verification
We have designed a comprehensive test suite to evaluate the correctness of our implementation.
Our test suite includes various test cases, including but not limited to:
1. Basic usage of WhileDB language.
2. String and array usage.
3. Declaration test
4. Pointer and dereference test

We use the corresponding cpp file to test the correctness of our implementation.

To run the test, you can use the following command:
``` bash
cd verification
python test_cases.py all_cases.json
```
