# jsonexpr
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![Release](https://img.shields.io/github/v/release/markuskimius/jsonexpr?include_prereleases&label=Pre-release)](https://github.com/markuskimius/jsonexpr/releases)
[![C](https://img.shields.io/badge/C-blue.svg)](c)
[![Python3](https://img.shields.io/badge/Python-blue.svg)](python)
[![Javascript](https://img.shields.io/badge/Javascript-blue.svg)](javascript)

A light, JSON-based expression language.


## Overview

jsonexpr is a general-purpose expression language for running simple
calculations.  It may be called from a host language, such as C, Python, or Javascript to run
calculations inside a sandboxed environment.  The host language may pass
variables and functions to the evaluation environment to enrich its
capabilities.


## Example

```
# The host may alternatively pass students into the evaluation environment
students = [ "Alice", "Bob", "Charlie" ];

FOR(i = 0, i < LEN(students), i++,
    note = "";

    IF(students[i] == "Alice",
        note = " (she's my favorite)"
    );

    PRINT("Student #" + (i+1) + "'s name is " + students[i] + note)
);

# Return the number of students (last expression always returns its value)
LEN(students)
```

Like many expression languages, jsonexpr uses functions for flow control, which
is the reason the syntax of `FOR` and `IF` resemble functions in the above
example.


## Supported Host Languages

* [C](src)
* [Python](py)
* [Javascript](js)


## Data Types

* null
* boolean
* int - 64-bit signed integer
* double - 64-bit floating point number
* string
* array
* object - for storing key-value pair data
* function

Data types are based on the JSON syntax, with some extensions.

The null literal is `null`.

A boolean literal may be `true` or `false`.

An integer literal may be a decimal integer or a hexadecimal integer.
A decimal integer may not start with a 0.
A hexadecimal integer starts with the chracter sequence `0x`.

A double literal must start with one or more valid decimal digits, followed by a period, followed by one or more valid decimal digits.

A string literal begins and ends with the double quote character (`"`).
Any double quote chracter or backslash character must be prefixed (escaped) by the backslash character.
Other commonly used escape characters are supported, including the tab, (`\t`), the carriage return (`\r`), and the newline (`\n`).
An 8-bit unicode character may be embedded using its 2-digit hexadecimal code prefixed by `\x`,
a 16-bit unicode character may be embedded using its 4-digit hexadecimal code prefixed by `\u`,
and a 32-bit unicode character may be embedded using its 8-digit hexadecimal code prefixed by `\U`.

An array may contain elements of mixed type; for example, one array may contain one element that is an integer, and another that is a string.
An array may contain elements of any type, including arrays and objects.
An array literal is encapsulated in brackets (`[ ... ]`), with its elements separated by commas.
An array index must be contiguous.
The index of the first element in the array is 0.

An object stores key-value pairs.
An object literal is encapsulated in braces (`{ ... }`), with its elements separated by commas,
with each element's key-value pair separated by the colon (`:`).
The key must be a valid identifier.
The value may be of any data type, including arrays and objects.

Function may be a built-in or created using the built-in `FUNCTION()` function.


## Variables

The name of a variable must be a valid identifier.
A valid identifier consists of one or more alphanumeric character and/or the underscore, except it may not start with a digit.

Variable names are case-sensitive.
By default, built-in variables are in all-caps except the literals `null`, `true`, and `false`;
it is recommended custom variable names include at least one lower case letter to avoid compatibility with future built-in variables.

A variable is not typed or declared.
The variable is instantiated the first time it is assigned a value.

If the variable contains an array,
its element may be accessed by the variable name followed by the array index in brackets (`[...]`).

If the variable contains an object,
the object's element may be accessed by the object's name followed by the period (`.`) and the key, or the key in brackets (`[...]`).


## Operations

Following are valid identifiers, in the order of highest to lowest precedence:

| Operator                                                                                              | Associativity[^1]         | Description                                                     |
| ----------------------------------------------------------------------------------------------------- | ------------------------- | --------------------------------------------------------------- |
| `( ... )`                                                                                             |                           | Grouping                                                        |
| `.`                                                                                                   |                           | Object member accessor                                          |
| `[ ... ]`                                                                                             |                           | Array element accessor                                          |
| `( ... )`                                                                                             |                           | Function call                                                   |
| `++` `--`                                                                                             |                           | Postfix increment, decrement                                    |
| `++` `--`                                                                                             |                           | Prefix increment, decrement                                     |
| `!`<br>`~`<br>`+` `-`                                                                                 |                           | Logical not<br>Bitwise not<br>Positive, negative                |
| `**`                                                                                                  | right-to-left             | Power                                                           |
| `*` `/` `%`                                                                                           |                           | Multiply, divide, modulus                                       |
| `+` `-`                                                                                               |                           | Add, subtract                                                   |
| `<<`<br>`>>`<br>`>>>`                                                                                 |                           | Left shift<br>Right shift sign-extended<br>Right shift unsigned |
| `<` `>` `<=` `>=`                                                                                     |                           | Inequality                                                      |
| `==` `!=`                                                                                             |                           | Equal to, not equal to                                          |
| `&`                                                                                                   |                           | Bitwise and                                                     |
| `^`                                                                                                   |                           | Bitwise xor                                                     |
| `\|`                                                                                                  |                           | Bitwise or                                                      |
| `&&`                                                                                                  |                           | Logical and                                                     |
| `\|\|`                                                                                                |                           | Logical or                                                      |
| `=`<br>`**=`<br>`*=` `/=` `%=`<br>`+=` `-=`<br>`<<=` `>>=` `>>>=`<br>`&=` `^=` `\|=`<br>`&&=` `\|\|=` | right-to-left             | Assignment                                                      |
| `? ... :`                                                                                             | right-to-left             | Ternary conditional                                             |
| `;`                                                                                                   |                           | Expression separator / terminator                               |

[^1]: Associativity is left-to-right unless noted otherwise.


## Built-In Functions

Following are functions built into jsonexpr, arguments they accept, and their return values.

* `FOR(expr1, expr2, expr3, expr4) -> any`[^2]
* `FUNCTION(string, expr) -> function`[^2]
* `IF(expr1, then1, [expr2, then2, [expr3, then3, ...]], [else]) -> any`[^2]
* `LEN(string or array or object) -> int`
* `PRINT(any, [any, [any, ...]]) -> int`
* `SQRT(int or double) -> double`

[^2]: These functions' arguments are delay-evaluated.
Typically, arguments to a function are evaluated before the function is called in order to pass the results of the evaluation to the function.
However, flow-control functions such as `IF` and `FOR` need to be able to control when to evaluate its arguments;
in order to support this behavior, its arguments are passed to these functions, and the functions themselves choose when to evaluate the arguments.

The `FUNCTION()` function may be used to create a custom function within the expression;
it returns a function object which may be assigned to a variable to be called in a later part of the expression.

`FUNCTION()` takes two arguments:
The first argument is a character sequence (a string) that describes the number of arguments the custom function can accept:
the number of `?` characters in the string describes the [minimum] number of arguments expected by the function.
The characters `B`, `I, `D`, `#`, `S`, `A`, `O`, or `F` may be used instead to test the argument's data type is
Boolean, integer, double, number, string, array, object, or function, respectively.
If the string ends with a `*`, the function can accept any number of additional arguments.

The second argument to `FUNCTION()` is the custom function's body.
The arguments passed to the function may be accessed through the automatic array variable `ARG`.

The custom function body has its own scope.
Assigning to a variable within the function scope instantiates a new variable within the function scope.
A variable instantiated outside of the function scope may only be read directly;
To write to variables instantiated outside of the function scope, use the following automatic variables:

* `GLOBAL` - An object that can be used to access global variables.
* `UPSCOPE` - An object that can be used to access variables one scope level above the function scope.


## License

[Apache 2.0](LICENSE)

