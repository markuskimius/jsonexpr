# Installation

JSONexpr may be built from scratch or installed from a package.

The command-line version of JSONexpr, `je`, comes with the C version and the
Python version of JSONexpr.


## Building from Scratch

To build JSONexpr from scratch,
first install [libc-wasm](https://github.com/markuskimius/libc-wasm) then run:

```bash
$ make
$ sudo make PREFIX=/usr/local install
```


### C

Build from scratch then the C libraries will be installed
to `/usr/local/lib` and `/usr/local/include`.


### Python

Build from scratch then run `pip3 install .` from `py/`.


### Javascript

Build from scratch then copy `js/lib/je.js` and `js/lib/je.wasm` into your project.


## Installing from a Package

### C

C libraries are not available for installation from a package at this time.

Build from scratch to install the C library and header files into
`/usr/local/lib` and `/usr/local/include`, respectively.


### Python

```bash
pip3 install jsonexpr
```


### Javascript

```javascript
import je from "//cdn.cbreak.org/jsonexpr/je-latest.min.js";
```
