# jsonexpr for Javascript

This document describes using jsonexpr with Javascript.
For the language overview, see the [main page](https://github.com/markuskimius/jsonexpr).


## Example

```html
<script type="module">
    import je from "//cdn.cbreak.org/jsonexpr/je-latest.min.js";

    const compiled = await je.compile(`
        PRINT("I have " + LEN(grades) + " students");
        PRINT("Alice's grade is " + grades.alice);
    `);

    compiled.setSymbols({
        grades : {
            alice : "A",
            bob   : "B",
        }
    });

    const result = await compiled.eval();
</script>
```

Output:

```
I have 2 students
Alice's grade is A
```


## License

[Apache 2.0](https://github.com/markuskimius/jsonexpr/blob/main/LICENSE)

