<!-- omit in toc -->
# The Silk scripting language

## Introduction

Silk Script is a small little inefficient language written as an
exercise for myself. It is written in modern C++, it makes use
of C++17 features like `std::variant` and `std::visit`.

<!-- omit in toc -->
## Table of Contents

- [Introduction](#introduction)
- [Roadmap](#roadmap)
- [Current status](#current-status)
- [License](#license)

## Roadmap

- [ ] Parser implementation
- [ ] Packager implementation
- [ ] Initial compiler implementation (moth vm)
- [ ] Initial VM implementation
- [ ] Repl implementation
- [ ] Initial compiler implementation (web asm)
- [ ] Optimizer implementation
- [ ] Enums
- [ ] Objects
- [ ] Macros
- [ ] Generational GC
- [ ] Static typing
- [ ] Debugger

## Current status

The implementation of the language has been restart three times:

  - Commit `e9d717bdb94919c209e2aa24384af80728425d2f`
    - Initial implementation as a tree walk interpreter, fully
      working (with probably a lot of bugs)

  - Commit `47ec24f15a10c09df7b56468947069aafdec7622`
    - Second implementation as a VM language, somewhat working
      (functions work but are buggy, closures do not work)

  - Latest commit
    - Current implementation as a VM language, currently nothing
      works and it's full of bugs 


## License

Silk is licensed under the MIT license.
