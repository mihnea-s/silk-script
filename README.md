# Silk Script Programming language

## Introduction

Silk Script is a small little inefficient language written as an
exercise for myself. It is written in *mostly* modern C++, it makes liberal use of `std::variant`, `std::shared_ptr`s, and dynamic casts.

## Table of Contents

- [Silk Script Programming language](#silk-script-programming-language)
  - [Introduction](#introduction)
  - [Table of Contents](#table-of-contents)
  - [Documentation](#documentation)
    - [Variables](#variables)
    - [Primitive types](#primitive-types)
    - [Operation support](#operation-support)
    - [Functions](#functions)
    - [Structs](#structs)
    - [Strings](#strings)
    - [Standard Library](#standard-library)
      - [Library `'std/io'`](#library-stdio)
      - [Library `'std/str'`](#library-stdstr)
      - [Library `'std/data'`](#library-stddata)
  - [License](#license)

## Documentation

### Variables

```py
let x;          # dynamic type, initialized to vid
let z = 2;      # dynamic type, initialized to 2
let y: int;     # static type, initialized to vid
let w: int = 2; # static type, initialized to 2
```

Variables in silk are lexically scoped, meaning that they are
available only in the scope of the surrounding braces.

```py
{
  let x = 2.0;
}

x = 3.0; # invalid, will throw runtime error
```

### Primitive types

Silk supports seven different primitive types including
integers, real numbers, strings, booleans, vid, callable,
and gettables. All types have a truthy or falsy value
which is used by logical statements (e.g. `if`, `for`).

* #### Integers `(int)`

  Integers can contain 64 bit (8 byte) signed values, integers
  are truthy if they are not equal to zero.

* #### Real numbers `(real)`

  Real numbers can contain 64 bit floating point values of
  arbitrary precision, reals are always truthy.

* #### Strings `(str)`

  Strings contain an immutable buffer of `UTF-8` encoded
  stream of characters. A string is truthy if it is not empty.
  ([Go to Strings section](###strings))

* #### Booleans `(bool)`

  Booleans contain a `true` or `false` logical value.

* #### Vid `(vid)`

  Vids are used for uninitialized or invalid data, they
  are always falsy.

* #### Callable `(callable)`

  Variables of callable type can be invoked with a call
  expression `myCallable()`. Callables include constructors,
  named functions and variable bound lambda expressions.
  They are always truthy.

* #### Gettable `(gettable)`

  Gettables are types which support accessing internal state
  by chaining a dot after them, mainly struct instances.

### Operation support

|               | Integer | Real  | String | Boolean |  Vid  | Callable | Gettable |
| ------------- | :-----: | :---: | :----: | :-----: | :---: | :------: | :------: |
| `==`          |    ✓    |   ✓   |   ✓    |    ✓    |   ✓   |    ✓     |    ✓     |
| `+ *`         |    ✓    |   ✓   |   ✓*   |    ✗    |   ✗   |    ✗     |    ✗     |
| `- / // * **` |    ✓    |   ✓   |   ✗    |    ✗    |   ✗   |    ✗     |    ✗     |
| `~ & | ^ %`   |    ✓    |   ✗   |   ✗    |    ✗    |   ✗   |    ✗     |    ✗     |
| `< > <= >=`   |    ✓    |   ✓   |   ✓    |    ✗    |   ✗   |    ✗     |    ✗     |
| `()`          |    ✗    |   ✗   |   ✗    |    ✗    |   ✗   |    ✓     |    ✗     |
| `.`           |    ✗    |   ✗   |   ✗    |    ✗    |   ✗   |    ✗     |    ✓     |

> <sup>\*</sup>if you need to concatenate multiple strings together, 
>   the `+` operator is slow, use `SBuilder` instead.
>   ([Go to Strings section](###strings))

### Functions

Functions is silk are first class citizens just like variables.
They also support closures. There are multiple ways to declare
a function:

1. Named functions

    ```py
    fct namedFunction(x, y) {
      return x + y;
    }
    ```

    > Note: Function parameters are optional, but if there are
      none you are required to omit the parenthesis.

    > Note: You can also specify a static type for function parameters
      and return type: `fct iTakeTwoInts(x: int, a: int): real`;


1. Methods

    ```py
    struct MyStruct {
      fct myMethod {
        println('struct stuff');
      }

      fct myVirtual(thing, stuff) virt;
    }
    ```

2. Lambda syntax

    ```py
    let myFunction = fct {
      println('my function was called!');
    };
    ```

3. Arrow functions

    ```py
    fct version -> '2.3.4';
    fct square(x: real) -> x ** 2.0;
    ```

    > Note: Arrow functions do not work as lambdas!

### Structs

### Strings

Strings are immutable data types, unlike in languages like C
you cannot modify the underlying data directly, instead you must
rely on helper functions provided by external packages or the
[standard library](####library-`'std/io'`) to do so.

String literals are interpolated at runtime:

```py
let greeting = 'hello';
println('$greeting$ world!'); # will print 'hello world!'
```

You can use any variable name between `$...$` and its textual
representation will be replaced in the string. You can escape
the interpolation expression by adding another `$` at the beginning (e.g. `$$not_interpolated$`).

### Standard Library

#### Library `'std/io'`

The `io` library provides functions to interface with the operating
system's file and terminal system. It includes the following functions:

* `print(v: ?): vid`

  Prints the textual representation of `v` to stdout.

* `println(v: ?): vid`

  Prints the textual representation of `v` to stdout after
  appending a newline to it.

* `input(): str`

  Gets the next line of input from stdin.

* `create(name: str): File`

  Creates a file with the given name and returns a `File` object
  accessing it.

  > Warning: Creating a file with an already existing name will
  delete all of it's content.

* `open(name: str): File`

  Opens a file with the given name for reading and writing.

And the following struct:

* `File`

  * `write(w: str): vid`

    Writes the string `w` to the file.

  * `rword(): str`

    Reads the next word from the file.

  * `rline(): str`

    Reads the next line from the file.

  * `eof(): bool`

    Returns `true` if the end of file has been reached,
    `false` otherwise.

#### Library `'std/str'`

The `str` library includes most of the string focused
functionality of the language provided standard library.
It exposes the following functions:

* `searchs(src: str, find: str): str`

  Returns the index of the first occurrence of the string `find`
  in `src`, if no such string is found it returns `-1`.

* `cuts(s: str, i: int, j: int): str`

  Returns a new string containing the data from `s` starting at
  index `i` and ending at `j`.

* `lens(s: str): int`

  Returns the length in bytes of the string `s`. This value can
  be used with `cuts` to cut up until the end of the string.

* `intConverts(s: str): int`

  Converts the given string into an `int` if such is found or
  returns `vid` if not.

* `realConverts(s: str): real`

  Converts the given string into a `real` if such is found or
  returns `vid` if not.

* `sbuilder(): SBuilder`

  Creates and returns a [`SBuilder`]() object.

And the following struct:

* `SBuilder`

  * `add(...): int`

    Adds all arguments into the string builder and returns
    the number of strings added.

  * `addln(...): int`

    Adds all arguments into the string builder appending a
    newline to each one, then returns the number of lines added.

  * `clear(): vid`

    Clears the contents of the string builder.

  * `str(): str`

    Returns the content of the builder in a string object.

#### Library `'std/data'`

The data library exposes commonly used data structures:

* `List`

  * `at(i: int): ?`

    Returns the value at index `i`.

  * `in(i: int, v: ?): ?`

    Replaces the value at index `i` with `v` and returns
    the old value.

  * `add(v: ?): ?`

    Adds a new value `v` to the list and returns it.

  * `addAll(...): int`

    Adds all arguments to the list and returns the number of
    values added.

  * `len(): int`

    Returns the length of the list.

  * `rem(i: int): ?`

    Removes the element at index `i` and returns it.

  * `each(c: callable): vid`

    Calls `c` for every element in the list as the argument.

  * `eachi(c: callable): vid`

    Calls `c` for every element in the list along with it's index
    as the arguments. The index goes first.

  * `sort(): vid`

    Sorts all orderable values in the list.

  * `reduce(init: ?, c: callable): ?`

    Reduces the list to a single value by calling `c` with `init` and
    every element in the array then setting `init` to `c`'s return value.
    Afterwards it returns `init`.

* `Map`

  * `at(s: str): ?`

    Returns the value at `s`.

  * `in(s: str, v: ?): ?`

    Replaces the value at `s` with `v` and returns
    the old value.

  * `add(s: str, v: ?): ?`

    Adds a new value `v` to the map at `s` and returns it.

  * `len(): int`

    Returns the number of elements in the map.

  * `rem(s: str): ?`

    Removes the element at `s` and returns it.

  * `each(c: callable): vid`

    Calls `c` for every key, value pair in the map as the 
    arguments.

Along with functions to construct them:

* `list(...): List`

  Returns a new `List` with every argument.

* `map(): Map`

  Returns a new `Map`,

## License

Silk is licensed under the MIT license.