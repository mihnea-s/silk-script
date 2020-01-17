# Silk Script Programming language

## Documentation

### Variable Declaration

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
  ([Go to Strings section](###Strings))

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

|                   | Integer | Real  | String | Boolean |  Vid  | Callable | Gettable |
| ----------------- | :-----: | :---: | :----: | :-----: | :---: | :------: | :------: |
| `==`              |    ✓    |   ✓   |   ✓    |    ✓    |   ✓   |    ✓     |    ✓     |
| `+, *`            |    ✓    |   ✓   |   ✓*   |    ✗    |   ✗   |    ✗     |    ✗     |
| `-, /, //, *, **` |    ✓    |   ✓   |   ✗    |    ✗    |   ✗   |    ✗     |    ✗     |
| `~, &, |, ^, %`   |    ✓    |   ✗   |   ✗    |    ✗    |   ✗   |    ✗     |    ✗     |
| `<, >, <=, >=`    |    ✓    |   ✓   |   ✓    |    ✗    |   ✗   |    ✗     |    ✗     |
| `()`              |    ✗    |   ✗   |   ✗    |    ✗    |   ✗   |    ✓     |    ✗     |
| `.`               |    ✗    |   ✗   |   ✗    |    ✗    |   ✗   |    ✗     |    ✓     |

> <sup>\*</sup>if you need to concatenate multiple strings together, 
>   the `+` operator is slow, use `SBuilder` instead.
>   ([Go to Strings section](###Strings))

### Function declarations

Functions is silk are first class citizens just like variables. They also support closures.
There are multiple ways to declare a function:

1. Named functions

    ```py
    fct namedFunction(x, y) {
      return x + y;
    }
    ```

    > Note: Function parameters are optional, but if there are none you are required to omit the parenthesis.

    > You can also specify a static type for function parameters: `fct iTakeTwoInts(x: int, a: int)`;


1. Methods

    ```py
    struct ... {
      fct myMethod {
        println('struct stuff');
      }

      fct myVirtual(thing, stuff) virt;
    }
    ```

1. Lambda syntax

    ```py
    let myFunction = fct {
      println('my function was called!');
    };
    ```

1. Arrow functions

    ```py
    fct version -> '2.3.4';
    fct square(x: real) -> x ** 2.0;
    ```

    > Note: Arrow functions do not work as lambdas!


### Strings