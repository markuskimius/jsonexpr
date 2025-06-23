# JSONexpr

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![Release](https://img.shields.io/github/v/release/markuskimius/jsonexpr?include_prereleases&label=Pre-release)](https://github.com/markuskimius/jsonexpr/releases)
![clang17](https://img.shields.io/badge/clang-17-blue.svg)
![python38](https://img.shields.io/badge/python-3.8-blue.svg)
![javascript](https://img.shields.io/badge/javascript-grey.svg)

An expression language built on JSON.


## Motivation

Wouldn't it be great if...

* You could embed comments and expressions in JSON?
* You could evaluate such "JSON expression" from any programming language?

So that you can:

* Add comments and expressions to JSON files used by your application?
* Ask the user for an expression and evaluate it in a safe sandbox?
* And do that evaluation from either the client or the server, regardless of what language they're written in?
* Have the option to change the programming language your application is written in without changing the expresion language used by the JSON file or the users?

JSONexpr executes in an WASM runtime so it executes in a safe sandbox and is
portable to any language that supports WASM runtime.


## Example

```bash
$ cat students.je
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
$ je students.je | jq .
[
  {
    "name": "Alice",
    "score": 95.66666666666667
  },
  {
    "name": "Bob",
    "score": 85.66666666666667
  },
  {
    "name": "Charlie",
    "score": 75.66666666666667
  }
]
$
```


## Documentation

* [Overview](doc/overview.md)
* [Installation](doc/install.md)
* Usage
  * [C](src/README.md)
  * [C++](cpp/README.md)
  * [Python](py/README.md)
  * [Javascript](js/README.md)
* Language Reference
  * [Data Types](doc/datatypes.md)
  * [Operators](doc/operators.md)
  * [Functions](doc/functions.md)


## License

[Apache 2.0](LICENSE)
