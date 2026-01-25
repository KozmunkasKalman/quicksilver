# Quicksilver

Quicksilver is an esoteric programming language with the primary goals of speed and simplicity: fast to compile and at runtime, while having pretty simple - although unorthodox - syntax.

> [!WARNING]
> The Quicksilver compiler (qsc) is currently very early in development (0.0.2), as of the latest commit its able to do basic mathematical operations on 8-bit integers, and handle exit codes.

## Language concept:

Inspired by multiple languages, such as Haskell, HolyC, and Python

The Quicksilver programming language is designed to be simultaneously low and high level: it will use semi-direct types (int8, flt64, null0, etc), while also having simple and readable syntax, that is unorthodox for the sake of it. The primary goals of the language are are compile and runtime speed.

My final goal with this language is be able to compile the compiler with the compiler (self-host qsc).

## Goals

[x] Exit codes
[x] Variables
[x] Basic mathematical operations (+, -, *, /, %)
[ ] Strings
[ ] Printing with ANSI escape code support and interpolation
[ ] Conditional statements (`if`, `elsif`, `else`, `while`, `select` `case` `from`)
[ ] Bitwise operations
[ ] Arrays
[ ] Named functions
[ ] Constants
[ ] Semi-direct data types (bool1, int8-int64, flt64, str8-str64, null0, etc.)
[ ] Loading headers/libraries (similar to #include in C)
[ ] Multi-pass compilation
[ ] Optimizations
[ ] Decently verbose error messages
[ ] **Raylib**
[ ] Self-hosting
