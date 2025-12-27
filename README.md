# JSONexpr

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![Release](https://img.shields.io/github/v/release/markuskimius/jsonexpr?include_prereleases&label=Pre-release)](https://github.com/markuskimius/jsonexpr/releases)
![clang17](https://img.shields.io/badge/clang-17-blue.svg)
![python38](https://img.shields.io/badge/python-3.8-blue.svg)
![javascript](https://img.shields.io/badge/javascript-grey.svg)

A safe, language-neutral expression language built on JSON.


## Motivation

Quite simply,
I needed a way to pass between processes a code snippet that can be evaluated
safely, by any process regardless of the language the process was writen in,
with an identical behavior across the languages.

So I made one.

And it just so happened that using JSON as the datatype of this language was very useful.


## Basic Examples

Mathematical expressions are valid:

```bash
$ je -e '1 + 2 + 3 + 4 + 5 + 6 + 7 + 8'
36
$
```

You can use variables:

```bash
$ je -e 'n=8; n * (n+1) / 2'
36
$
```

A variable can store any valid JSON value:

```bash
$ je -e 'myvar = { "n" : 8 }; myvar.n * (myvar.n+1) / 2'
36
$
```

You can have loops:

```bash
$ je -e 'sum=0; FOR(i=0, i<9, i++, sum+=i); sum'
36
$
```

You can have functions:

```bash
$ je -e 'SQRT(36)'
6.0
$
```

Above examples evaluate JSONexpr expression from the command line using the
`je` command that comes with the C and Python installations of JSONexpr, but
JSONexpr is primarily designed to be evaluated programmatically.
See [documentation](#documentation) for the list of supported programming
languages that can evaluate JSONexpr expressions today;
additional language support can be added with a WASM runtime.


## More Examples

Since a value is a valid expression in an expression language,
JSON is a valid expression in JSONexpr.
And you can embed expressions in the JSON.
For example:

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


## Demo

Interactive demo [here](https://cdn-test.cbreak.org/jsonexpr/je-latest.html)


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
