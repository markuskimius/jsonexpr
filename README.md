# jsonexpr
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![Release](https://img.shields.io/github/v/release/markuskimius/jsonexpr?include_prereleases&label=Pre-release)](https://github.com/markuskimius/jsonexpr/releases)
![clang17](https://img.shields.io/badge/clang-17-blue.svg)
![python38](https://img.shields.io/badge/python-3.8-blue.svg)
![javascript](https://img.shields.io/badge/javascript-grey.svg)

A light, JSON-based expression language.


## Overview

jsonexpr is a general-purpose expression language that uses JSON as its data type.
It can manipulate JSON, use JSON as its input, and output JSON.
As matter of a fact, a vanilla JSON is a valid expression in jsonexpr.

A jsonexpr expression is evaluted in a sandbox,
so one common use case for jsonexpr is to evaluate a user-submitted expression safely
from a *host language*.
Currently supported host languages include C, Python, and Javascript;
a library is offered in each of the host languages to be able to evaluate
jsonexpr expression.
A stand-alone binary is also offered so a user can evaluate a jsonexpr
expression from the command line.

Another use case for jsonexpr is to share a common language between the client
and the server written in two different languages.
For example, the user may be asked to enter some expression for evaluation,
and the application can choose to execute the expression on the client side in
Javascript, or on the server side in Python.


## Example

```
[
  {
    "name": "Alice",
    "score": (95 + 98 + 94) / 3.0
  },
  {
    "name": "Bob",
    "score": (85 + 88 + 84) / 3.0
  },
  {
    "name": "Charlie",
    "score": (75 + 78 + 74) / 3.0
  }
]
```


## Documentation

* [Installation](doc/install.md)
* Usage
  * [C](src/README.md)
  * [Python](py/README.md)
  * [Javascript](js/README.md)
* Language Reference
  * [Data Types](doc/datatype.md)
  * [Operators](doc/op.md)
  * [Functions](doc/fn.md)


## License

[Apache 2.0](LICENSE)
