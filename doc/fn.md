# Built-In Functions

## CEIL

`CEIL(int or double) -> int`

Return a number rounded up to the nearest integer.


## EVAL

`EVAL(string) -> any`

Evaluate a jsonexpr expression and return its result.


## FLOOR

`FLOOR(int or double) -> int`

Return a number rounded down to the nearest integer.


## FOR

`FOR(expr1, expr2, expr3, expr4) -> any`

`FOR()` evaluates `expr1`,
then evalutes `expr4` for as long as `expr2` is true.
`expr3` is evaluated after each iteration.

Its return value is the value of the last `expr4` expression evaluated.
If no `expr4` expression was evaluated, it returns `null`.


## FOREACH

`FOREACH(variable, iterable, expr) -> any`

`FOREACH()` evaluates `expr` `n` times
where `n` is the number of elements in `iterable`
and `iterable` is either an array or an object.

At the beginning of each iteration,
the variable named `variable` will be set
to the value of the `n`th item in `iterable`.
If `iterable` is an object,
`variable` will be set to an array of two elements,
where the item at index 0 is the key
and the item at index 1 is the value.


## FUNCTION

`FUNCTION(string, expr) -> function`

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
* `@`: Any container argument (an array or an object).
* `F`: A function argument.
* `?`: An argument of any type.
* `.`: An argument of any type as a node.
* `*`: Any number of arguments of any type. Must be the last character in the sequence.
* `**`: Any number of arguments of any type as nodes. Must be the last character in the sequence.
  (Mutually exclusive with `*`.)

The following special variables are accessible within `expr`:

* `ARG`: Values passed to `expr` may be accessed via this special array variable.
* `GLOBAL`: Global variables may be accessed via this special object variable.
* `UPSCOPE`: Variables above the current scope can be read from, or written to, explicitly through this special object variable.
  Typically they are accessed automatically if already defined in a scope above or higher, otherwise accessed locally.


## IF

`IF(expr1, then1, [expr2, then2, [expr3, then3, ...]], [else]) -> any`

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

`PRINT(any, [any, [any, ...]]) -> int`

Print one or more expressions to the standard output.
Its return value is the number of items printed.


## ROUND

`ROUND(int or double) -> int`

Return a number rounded to the nearest integer.
x.5 is rounded up.


## SQRT

`SQRT(int or double) -> double`

Return the square root of an integer or double.


## WHILE

`WHILE(expr1, expr2) -> any`

Evaluate `expr2` for as long as `expr1` is true.

Its return value is the value of the last `expr2` expression evaluated.
If no `expr2` expression was evaluated, it returns `null`.
