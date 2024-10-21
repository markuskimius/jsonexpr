# Data Types

jsonexpr uses JSON as its base data type, and adds some useful enhancements.


## JSON data types

These data types are identical to JSON:

* `null`
* Boolean: `true` and `false`
* string
* array
* object

The number type in JSON is supported as 2 distinct data types in jsonexpr:

* int: A 64-bit signed integer.
* double: A 64-bit floating point number. Literals `inf`, `-inf`, and `nan` are supported.


## New data type

In addition to the data types found in JSON, jsonexpr adds the following data type:

* function: This data type allows a function to be referenced and called using a variable.
