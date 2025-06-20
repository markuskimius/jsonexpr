# Data Types

JSONexpr supports the following data types:

* Null
* Boolean
* String
* Array
* Object
* Integer  : 64-bit signed.
* Float    : 64-bit IEEE 754.
* Function : For lambda functions, among other uses.
* Node     : For passing code blocks, among other uses.


## Null

Null has just one value, `null`.
JSONexpr uses null as the default return value of any "undefined" expression.

Null is a JSON standard type.

### Null Literal

Null has just one literal form, `null`.


## Boolean

Boolean has two possible values, either a `true` or `false`.

Although JSONexpr explicitly supports the Boolean type,
all types have a Boolean equivalent when used in a Boolean context,
such as in a conditional or with a logical operator:

* Null: `null` is always equivalent to `false`.
* String: An empty string is equivalent to `false`, otherwise `true`.
* Array: An array with no element is equivalent to `false`, otherwise `true`.
* Object: An object with no element is equivalent to `false`, otherwise `true`.
* Integer: 0 is equivalent is `false`, otherwise `true`.
* Float: 0.0 is equivalent to `false`, otherwise `true` including `nan`, `inf`, and `-inf`.
* Function: A function is always `true`.
* Node: A node is always `true`.

Boolean is a JSON standard type.

### Boolean Literal

Boolean has two literal forms, `true` and `false`.


## String

String is a JSON standard type.
JSONexpr adds some enhancements to its literal form for convenience.

### String Literal

As is with the JSON standard, a string literal is always between two double quotes `"`
and may contain any of the following escape sequences:

* `\"` for a quotation mark,
* `\\` for a backslash,
* `\/` for a forward slash,
* `\b` for a backspace,
* `\f` for a formfeed,
* `\n` for a newline,
* `\r` for a carriage return,
* `\t` for a tab,
* `\uNNNN` for a 16-bit Unicode character,

JSONexpr version of the string also allows following escape sequences:

* `\xNN` for an 8-bit Unicode,
* `\UNNNNNNNN` for a 32-bit Unicode,

Additionally, the JSONexpr version of string allows a newline inside a string.
The newline may be prefixed with a backslash to escape it from the resulting
string.


## Array

Array is a JSON standard type.
JSONexpr adds some enhancements to its literal form for convenience.

### Array Literal

Like the JSON standard,
an array literal begins with a `[` and ends with a `]`
with zero or more elements inside separated by commas `,`.

JSONexpr version of the array allows for a trailing comma
and for each element to be a JSONexpr expression.

### Special Variable

Within the array literal, the special array variable `THIS` may be accessed to
read or modify the array itself mid-construction.
Adding a new index position to the array mid-construction is an undefined
behavior.


## Object

Object is a JSON standard type.
JSONexpr adds some enhancements to its literal form for convenience.

### Object Literal

Like the JSON standard,
an object literal begins with a `{` and ends with a `}`
with zero or more key-value pairs inside separated by commas `,`.
A kkey-value pair is separated by a colon `:`.

JSONexpr version of the the object allows for a trailing comma,
and for each key or value to be a JSONexpr expression.
The key need not be a string literal, but it must evaluate to a string.

### Special Variable

Within the object literal, the special object variable `THIS` may be accessed
to read or modify the object itself mid-construction.


## Integer

Integer in JSONexpr is a 64-bit signed integer.

Integer is one of the two replacements to JSON's _number_ type.
The other is the _float_ type (see below.)

### Integer Literal

An integer literal may be any of the following:

* A sequence of one or more decimal digits starting with a nonzero digit, or `0`.
* A sequence of one or more hexadecimal digits prefixed by `0x`.
* A sequence of one or more octal digits starting with `0`.
* A sequence of one or more binary digits prefixed by `0b`.

### Integer I/O

When reading JSON data containing a number literal, JSONexpr interprets the
number as an integer if it satisfies the integer literal format.

When outputting an integer value, JSONexpr by default outputs it as a sequence
of one or more decimal digits starting with a nonzero digit, prefixed by `-`
for a negative number.


## Float

Float in JSONexpr is 64-bit double precision floating point number.

Float is one of the two replacements to JSON's _number_ type.
The other is the _integer_ type (see below.)

### Float Literal

A float literal may be any of the following:

* A sequence of one or more decimal digits starting with a nonzero digit,
  or `0`, followed by a decimal point and 1 or more decimal digits.
* Same, followed by `e` or `E`, an optional sign, and one or more decimal
  digits.
* A sequence of one or more decimal digits starting with a nonzero digit,
  followed by `e` or `E`, an optional sign, and one or more decimal digits.

### Float I/O

When reading JSON data containing a number literal, JSONexpr interprets the
number as a float if it satisfies the float literal format.

When outputting a float value in a numerically displayble range, JSONexpr by
default outputs it as a sequence of one or more decimal digits starting with a
nonzero digit, or `0`, followed by a decimal point and at least one decimal
digit.
As many digits are output to convey the complete precision of the float.
The output is prefixed by `-` for a negative value.

For a float value not in a numerically disable range, JSONexpr outputs `nan`
for a not-a-number, `inf` for a positive infinity, and `-inf` for a negative
infinity.


## Function

Function is a data type in JSONexpr:
A function may be stored in a variable, returned from another function, or
passed to a function.

A function value suffixed by `()` calls the function.
One or more expressions may be specified between the parenthesis,
separated by commas `,` to pass them as arguments to the function.
Depending on the function signature, the arguments may be evaluated before
passed into the function, or the expression may be passed into the function as
a node to be conditionally evaluated.

User-defined functions may be created by calling the built-in function
appropriately named `FUNCTION`.
The function value returned by `FUNCTION` may be called immediately, or stored
in a variable or passed to another function for a later execution.
User-defined functions are lambda functions; that is, executing the function
gives it access to the superscopes in which it was instantiated.

### Special Variable

Within a user-defined function, the special array variable `ARGS` may be
accessed to read the arguments passed to it by the caller.
Most arguments are passed by value,
except for the container types arrays and objects are passed by reference.
Functions and nodes are also passed by reference but they are immutable so
their passing by reference is just an implementation detail.

### Miscellaneous

* Recursion is supported.
* Variable argument list is supported.
* Overloading is not supported.

See the [Functions](functions.md) section for more information.


## Node

This is an advanced data type.

Node represents a branch in the Abstract Syntax Tree (AST), the tree data
structure for representing the syntactic sturecture of the expresion being
evaluated.

A user-defined function may request to receive one or more arguments to its
call as nodes instead of as values resulting from evaluating the arguments,
so they can be conditionally or repeated evaluated using the `EVAL` function.
This is useful for creating a custom control structure.
