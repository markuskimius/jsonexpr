# Installation

jsonexpr may be built from scratch or installed for a specific language.


## Building from Scratch

To build jsonexpr from scratch,
first install [libc-wasm](https://github.com/markuskimius/libc-wasm) then run:

```bash
$ make
$ sudo make PREFIX=/usr/local install
```


## C

Build from scratch to install the C library and header files into
`/usr/local/lib` and `/usr/local/include`, respectively.


## Python

```bash
pip3 install jsonexpr
```


## Javascript

Build from scratch, then copy `js/lib/je.wasm` and `js/lib/jsonexpr.js` into
your application.
