# Function Reference

## BINARY

`BINARY(int) -> string`

Return the binary representation of an integer, with leading zeros.
Always returns a 64 character string.


## CEIL

`CEIL(int or double) -> int`

Return a number rounded up to the nearest integer.


## EVAL

`EVAL(any) -> any`

Evaluate a JSONexpr expression and return its result.


## FLOOR

`FLOOR(int or double) -> int`

Return a number rounded down to the nearest integer.


## FOR

`FOR(expr1, expr2, expr3, expr4) -> any`

`FOR()` evaluates `expr1`, then evalutes `expr4` for as long as `expr2` is
true.  `expr3` is evaluated after each iteration.

Its return value is the value of the last `expr4` expression evaluated.  If no
`expr4` expression was evaluated, it returns `null`.


## FOREACH

`FOREACH(variable, iterable, expr) -> any`

`FOREACH()` evaluates `expr` `n` times where `n` is the number of elements in
`iterable` and `iterable` is either an array or an object.

At the beginning of each iteration, the variable named `variable` will be set
to the value of the `n`th item in `iterable`.
If `iterable` is an object, `variable` will be set to an array of two elements,
where the item at index 0 is the key and the item at index 1 is the value.


## FUNCTION

`FUNCTION(argspec, expr) -> function`

`FUNCTION()` instantiates and returns a user-defined function from `expr`.

`argspec` is a string of character sequence that describes the argument(s)
accepted by `expr`.  The following characters are recognized:

* `0`: A null argument.
* `B`: A Boolean argument.
* `I`: An integer argument.
* `F`: A float argument.
* `#`: Any numeric argument (an integer or a double).
* `S`: A string argument.
* `A`: An array argument.
* `O`: An object argument.
* `C`: A function argument.
* `?`: An argument of any type.
* `.`: An argument of any type as a node.
* `*`: Any number of arguments of any type. Must be the last character in the sequence.
* `**`: Any number of arguments of any type as nodes. Must be the last character in the sequence.
  (Mutually exclusive with `*`.)

Arguments are validated according to `argspec` before `expr` is called.
If the validation fails, an error message is output to stderr and `null` is
returned.

The following special variables are accessible from `expr`:

* `ARGS`    : The array of arguments passed to the function call.
* `LOCAL`   : The object of variables in the current scope.
* `UPSCOPE` : The object of variables in the scope 1 level above the current scope.
* `GLOBAL`  : The object of variables in the global scope.

The return value of a function is the last expression evaluated.


## HEX

`HEX(int) -> string`

Return the hexadecimal representation of an integer, with leading zeros.
Always returns a 16 character string.


## IF

`IF(expr1, then1, [expr2, then2, [expr3, then3, ...]], [else]) -> any`

Evaluate `then1` if `expr1` is true, else evaluate `then2` if `expr2` is true, etc.
If none of the `exprN` are true, evaluate `else`.

The returned value is the value of the last expression evaluated.
If no expression was evaluated, it returns `null`.


## LEN

`LEN(string or array or object) -> int`

Return the length of a string, the number of items in an array, or the number
of keys in an object.


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
