# JSONexpr Operators

## Precedence

Following are operators supported by JSONexpr, in the order of highest to lowest precedence:

| Operator                                                                                              | Associativity[^1]         | Description                                                     |
| ----------------------------------------------------------------------------------------------------- | ------------------------- | --------------------------------------------------------------- |
| `( ... )`<br>`{ ... }`<br>`[ ... ]`<br>`.`<br>`++` `--`                                               |                           | Grouping, function call<br>Object constructor<br>Array constructor, array subscript, object member accessor<br>Object member accessor<br>Postfix increment, decrement |
| `++` `--`<br>`!`<br>`~`<br>`+` `-`                                                                    |                           | Prefix increment, decrement<br>Logical not<br>Bitwise not<br>Positive, negative                |
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
| `? ... :`<br>`:`                                                                                      | right-to-left             | Ternary conditional<br>Key-value separator                      |
| `=`<br>`**=`<br>`*=` `/=` `%=`<br>`+=` `-=`<br>`<<=` `>>=` `>>>=`<br>`&=` `^=` `\|=`<br>`&&=` `\|\|=` | right-to-left             | Assignment                                                      |
| `,`                                                                                                   |                           | List separator                                                  |
| `;`                                                                                                   |                           | Expression separator / terminator                               |

[^1]: Associativity is left-to-right unless noted otherwise.
