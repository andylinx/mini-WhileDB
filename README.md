# mini-WhileDB
This is the repo for CS2205 Fall Project, mini-WhileDB with string and array.

## Usage

To get started, navigate to root dirctory.
```bash
cd while_ldb_parser 
make
```
To run a certain source file `a.jtl`
```bash
cd while_ldb_parser 
./main a.jtl
```

## Introduction
Besides the basic usage of WhileDB language, we some features below:

1. Array

For array, we support array declaration, initialization via C-style initializer list, subscript access, built-in length function, and dereference.

```cpp
  var p = malloc(80), a[10], b;
  b = *(p + 8); // access 2nd int in p
  b = a[1]; // access 2nd int in a
  b = len(a) // b = 10
```

2. String
Our string is built on top of array.
We support string literal, char literal (including C-style escape characters), and string I/O.
```cpp
var a = 'a';
var b[10] = "hello";
var c = '\n';
```

``` cpp
var a[10], b;
a = read_string();
b = read_char();
write_string(a);
write_char(a[5]);
```

### Detailed Standard

#### 1. Type Specification
  - Allint, pointer and char are considered nature numbers, the difference only occurs at function intepretation level.
  - Each identifier (and intermediate result) holds an attribute `is_array` to indicate whether it's an array pointer.

#### 2. Array class (fixed length)
  - Writable subscript access with range check.
  - Initializer list for array initialization.
    - curly brackets + comma (C style)
    - copy initializer list to array, and remaining part of array (if any) is filled with zeros.
    - if initializer list is longer than array, raise error.
```cpp
  var a[10];
  a[1] = 1; // correct
  a[10] = 1; // raise error, out of range
  var b[10] = {0, 1, 2, 3, 4, 5}
```

#### 3. Pointer Arithmetic
  - For array pointer ptr (`is_array = true`), ptr + n is interpreted at integer level.
  - For non-array pointer ptr (`is_array = false`), ptr + n is interpreted at byte level.
```cpp
  var p = malloc(80), a[10], b;
  b = *(p + 8); // access 2nd int in p
  b = a[1] // access 2nd int in a
```

#### 4. String
  - String Literal & Char Literal
    - double quote: string literal.
    - single quote: char literal.
    - support C-style escape characters.
  - String literal can be assigned to array or used at initialization.
    - copy string to array, and remaining part of array(if any) is filled with zeros.
    - if string is longer then array, raise error.
    - no '\0' at the end of string literal by our design.
  - Char literal is directly interpreted as a constant, and all its behavior comply with constant.
```cpp
var a[10] = "abcd", b = '\n';
```
  
#### 5. Extension of Variable Declaration
  - Mulitple variable/array declaration in one line, separated by comma.
  - Vairalbe/array initialization at declaration, via equal sign.

```cpp
var a, b, c=1;
var d[10] = {0, 1, 2, 3, 4}, e[10] = "abcde";
var f = 0, g[5]
```
  
#### 6. More Built-in Functions.
  - EXPR **len**: 
    - len(S : string literal) -> var
    - len(A : array) -> var
      - return the length of argument.
      - note that even if array A carries an string, len(A) returns the fixed length of array rather than return the first position of '\0'.
  - EXPR **read_string**:
    - read_sring() -> string literal
      - read in a string deliminated by any white space, and return as string literal.
      - buffer will be automatically managed to avoid overflow while reading.
  - CMD **write_string** :
    - write_string(S : string literal)
    - write_string(A : array)
      - interpret the argument as a sequence of ASCII char and write sequentially. 
      - if any non-ASCII character encountered, raise error.

---

## Verification
We have devised a comprehensive test suite to evaluate the correctness of our implementation.

Our test extensively examines every listed feature, including correctness and error capturing.

Testcases can be found at /verfication/cases with 53 cases in total, including following sub-modules:

- array
  - len
  - subscript access
  - write int
- basic
  - 9 general cases used as initial correctness test at development stage.
- decl
  - 4 cases for variable declaration initialization extension.
- porinter
  - 4 cases for pointer arithmetic and dereference.
- string
  - char literal
  - len
  - read string
  - string literal
  - write string
- tester_self_test
  - 4 cases to verify the correctness of tester itself.

The tester uses three methods to justify the correctness of our impelmentation:
  1. `method=cpp`, compare output with equivalent C++ program on multiple inputs.
  1. `method=out`, compare output with manually verified output.
  1. `method=error`, expected to receive exception.


To run full test:
``` bash
cd verification
./test_cases.py all_cases.json
```
> if you encoutered "python not found", please change the shebang (first line) of test_cases.py and test_onecase.py to "!/usr/bin/python"

To run a subset of cases
```bash
cd verification
./test_cases.py cases/<sub-module>/<sub-module>.json
```

To run a single case:
```bash
cd verification
./test_onecase.py -n <case_path>
```



For detailed arguments and case setup, please refer to comments in `test_cases.py` and `test_onecase.py`.
