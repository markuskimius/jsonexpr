# Built-In Functions

## FOR

`FOR(expr1, expr2, expr3, expr4) -> any`[^1]

`FOR()` evaluates `expr1`,
then evalutes `expr4` for as long as `expr2` is true.
`expr3` is evaluated after each iteration.

Its return value is the value of the last expression evaluated.
If no expression was evaluated, it returns `null`.


## FUNCTION

`FUNCTION(string, expr) -> function`[^1]

`FUNCTION()` creates, and returns, a custom function from `expr`.

Its first argument is a character sequence that describes the argument(s) accepted by `expr`.
The following characters are recognized:

* `B`: A Boolean argument.
* `I`: An integer argument.
* `D`: A double-precision floating point argument.
* `#`: Any numeric argument (an integer or a double).
* `S`: A string argument.
* `A`: An array argument.
* `O`: An object argument.
* `F`: A function argument.
* `?`: An argument of any type.
* `*`: Any number of arguments of any type. Must be the last character in the sequence.

The following special variables are accessible within `expr`:

* `ARGS`: Values passed to `expr` may be accessed via this special array variable.
* `GLOBAL`: Global variables may be accessed via this special object variable.
* `UPSCOPE`: Variables above the current scope can be read from, or written to, explicitly through this special object variable.
  Typically they are accessed automatically if already defined in a scope above or higher, otherwise accessed locally.


## IF

`IF(expr1, then1, [expr2, then2, [expr3, then3, ...]], [else]) -> any`[^1]

Evaluate `then1` if `expr1` is true,
else evaluate `then2` if `expr2` is true,
etc.
If none of the `exprN` are true, evaluate `else`.

The returned value is the value of the last expression evaluated.
If no expression was evaluated, it returns `null`.


## LEN

`LEN(string or array or object) -> int`

Return the length of a string,
the number of items in an array,
or the number of keys in an object.


## PRINT

`PRINT(any, [any, [any, ...]]) -> any`

Print one or more expressions to the standard output.
Its return value is the last expression printed,
or `null` otherwise.


## SQRT

`SQRT(int or double) -> double`

Return the square root of an integer or double.
