# JSONexpr for C++

This document describes using JSONexpr with C++.
For the language overview, see the [main page](../README.md).


## Installation

Build the C libraries first, then run:

```bash
$ make
$ sudo make PREFIX=/usr/local install
```

Once built, the C++ library does not require the C library.


## Example

Code:
```c++
#include <iostream>
#include "jepp/jepp.h"

int main() {
    auto instance = je::instance();
    auto parsed = instance.parse(R"je(
        PRINT("I have " + LEN(grades) + " students");
        PRINT("Alice's grade is " + grades.alice);
        grades
    )je");
    auto symmap = instance.symmap(R"je({
        "grades" : {
            "alice"   : "A",
            "bob"     : "B",
            "charlie" : "B",
        }
    })je");
    auto result = parsed.eval(symmap);

    return 0;
}
```

Compilation:
```bash
$ clang++ -I/usr/local/include example.cpp -c
$ clang++ -L/usr/local/lib example.o -ljepp -o example
```

Output:
```
I have 2 students
Alice's grade is A
```


## License

[Apache 2.0](../LICENSE)
