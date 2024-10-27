# Operators

Following are operators supported by jsonexpr, in the order of highest to lowest precedence:

| Operator                                                                                              | Associativity[^1]         | Description                                                     |
| ----------------------------------------------------------------------------------------------------- | ------------------------- | --------------------------------------------------------------- |
| `( ... )`                                                                                             |                           | Grouping                                                        |
| `.`                                                                                                   |                           | Object member accessor                                          |
| `[ ... ]`                                                                                             |                           | Array element / object member accessor                          |
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
