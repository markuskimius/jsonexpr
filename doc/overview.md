# JSONexpr Language Overview


## Syntax

A JSONexpr expression consists of one or more expressions separted by a semicolon `;`.
The result of many expressions is the value returned by the last expression.
The semicolon after the last expressions is optional.

An expression may be a JSON literal, a variable, a function, an operation, or
some combination of the same.

JSON array and object literals may include expressions.
They may also include a trailing comma after the last element.
The key of an object must be a string expression (including a string literal).

JSON number literal must fit into either a 64-bit signed integer or a 64-bit
"double" precision floating point number, otherwise may result in an overflow
or an underflow.
Integer literals may be expressed as a hexadecimal, octal, or binary literal
using the prefixes `0x`, `0`, or `0b`, respectively.

Line comments are allowed, prefixed by `#`.
Multiline comments are allowed, enclosed between `/*` and `*/`.


## Data Types

JSONexpr supports most data types native to JSON and introduces a few new ones.
One data type native to JSON not supported by JSONexpr is the _number_ type,
replaced by the types _integer_ and _float_ in JSONexpr.
They are both 64-bit types; the integer type is signed.
Conversion from a number to an integer or a float is inferred from the number
literal's formatting; specifically, a number literal containing a decimal point
or an exponent part will be treated as a floating point number, otherwise an
integer.

Here is a summary of the supported data types:

* Null
* Boolean
* String
* Array
* Object
* Integer  : 64-bit signed.
* Float    : 64-bit IEEE 754.
* Function : For lambda functions, among other uses.
* Node     : For passing code blocks, among other uses.

Attempt to output a piece of data whose type is not native to JSON, such as
function and node, will result in a non-JSON compliant output.
Also, an attempt to output a floating point number whose value is `nan`, `inf`,
or `-inf`, valid values in the IEEE 754 standard but not in JSON, will result
in a non-JSON compliant output.

See the [Data Types](datatypes.md) section for more information.


## Variables & Scope

Variables are untyped and instantiated upon first use.
Variable names in ALL CAPITAL LETTERS are reserved, so please create variable
names that contain at least one lower case letter.
These names are also reserved for use with the JSON and the IEEE 754 standards:
`null`, `true`, `false`, `nan`, `inf`.

A new scope is created when:
* Inside a user-defined function.
* Inside `{}`.
* Inside `[]`.

When inside a subscope, reading/writing a variable whose name was already
instantiated in a superscope reads from and writes to that variable.
If the variable name is not already instantiated in the superscope,
it will be instantiated in the subscope (if writing) or result in `null` (if reading).
Attempt to read an uninstantiated variable will also produce an error message to stderr.

Superscope can be accessed explicitly using the special variable objects
`UPSCOPE` (1 level up) and `GLOBAL` (root level scope).
Another special variable object, `LOCAL` can be used to explicitly instantiate
variables in the current scope.

The application calling JSONexpr may pass a value to JSONexpr, or read a value
from JSONexpr.
Please see the JSONexpr documentation specific to the programming language of
your application for more information.

Variables are copied by value, except for the container types array and object
that are copied by reference.
Function and node types are also copied by reference.


## Special Variables

* `GLOBAL`  : object representing the root-level scope.  Available in a subscope.
* `UPSCOPE` : object representing the scope 1 level above the current scope.  Available in a subscope.
* `LOCAL`   : object representing the current scope.  Available in a subscope.
* `ARGS`    : array of arguments passed to a function.  Available in a user-defined function.
* `THIS`    : array or object representing the array or object being created.  Available inside `[]` or `{}`.


## Operators

JSONexpr supports most operators used in a typical programming language, using
the C's syntax operator precedence.
Two operators in JSONexpr but not in C are `**` (the power operator)
and `>>>` (logical right shift);
their precedences are approximately same as Python and Javascript, respectively.

Attempt to perform any "undefined" operation results in a `null`,
a paradoxical statement given that the statement itself defines a result
hence the reason the word "undefined" is in quotes.
Floating point operations may result in `inf` (positive infinity), `-inf`
(negative infinity), or `nan` (not-a-number).

See the [Operators](operators.md) section for more information.


## Functions

JSONexpr comes with several built-in functions.
The user may also create their own function using the `FUNCTION` function.

See the [Functions](functions.md) section for more information.


## Flow Control

Flow controls are supported using functions.

See the [Functions](functions.md) section for more information.
Look for `IF`, `FOR`, `FOREACH`, and `WHILE`.

