# JSONexpr for Javascript

This document describes using JSONexpr with Javascript.
For the language overview, see the [main page](../README.md).


## Example

```html
<script type="module">
    import je from "//cdn.cbreak.org/jsonexpr/je-latest.min.js";

    const instance = await je.instance();
    const parsed = instance.parse(`
        PRINT("I have " + LEN(grades) + " students");
        PRINT("Alice's grade is " + grades.alice);
        grades
    `);
    const symmap = instance.symmap({
        "grades" : {
            "alice"   : "A",
            "bob"     : "B",
            "charlie" : "B",
        },
    });
    const result = parsed.eval(symmap);
</script>
```

Output to the console:
```
I have 2 students
Alice's grade is A
```


## License

[Apache 2.0](../LICENSE)
