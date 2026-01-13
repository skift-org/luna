# Luna

Luna is an interpreted, dynamically typed scripting language implemented as a C++20 module. It uses a recursive descent parser and an AST-walking interpreter.

## Type System

Luna supports the following value types:

| Type        | Description                     | Example            |
| ----------- | ------------------------------- | ------------------ |
| **None**    | Represents the absence of value | `none`             |
| **Boolean** | Logical true or false           | `true`, `false`    |
| **Integer** | 64-bit signed integer           | `42`               |
| **Number**  | 64-bit floating point           | `3.14159`          |
| **Symbol**  | Interned identifier string      | `#identifier`      |
| **String**  | Text sequence                   | `"hello"`          |
| **List**    | Ordered generic collection      | `[1, 2, "a"]`      |
| **Table**   | Key-value associative array     | `{ x: 10, y: 20 }` |

## Syntax

### Variable Declaration

Variables are declared using the `var` keyword.

```javascript
var x = 100;
var message = "Hello";

```

### Control Flow

Luna provides standard branching and looping constructs.

```javascript
// Conditional
if (x > 50) {
    x = x - 1;
} else {
    x = x + 1;
}

// Loop
while (x > 0) {
    if (x == 10) break;
    if (x == 20) continue;
    x = x - 1;
}

```

### Functions

Functions are first-class citizens. They support optional parameters with default values.

```javascript
fn add(a, b: 1) {
    return a + b;
}

// Usage
var result = add(10); // b defaults to 1

```

### Error Handling

Errors are structural exceptions that propagate up the call stack.

```javascript
try {
    throw "An error occurred";
} catch (e) {
    // 'e' contains the thrown value
}

```

### Reflection and Casting

The language includes operators for runtime type inspection and conversion.

```javascript
var x = 10;

// Type Check
if (x is integer) {
    // Conversion
    var s = x as string;
}

// Type Query
var t = typeof(x); // returns #integer

```
