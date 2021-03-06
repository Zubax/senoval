[![Forum](https://img.shields.io/discourse/https/forum.zubax.com/users.svg)](https://forum.zubax.com)
[![Travis CI build status](https://travis-ci.org/Zubax/senoval.svg?branch=master)](https://travis-ci.org/Zubax/senoval)

# Senoval

A simple dependency-free header-only library of C++17 classes for real-time embedded systems.
It is mostly intended for use as a git submodule.

## Usage

Either copy the code into your source tree, or (better)
add this repository as a submodule to your version control system.
For example:

```bash
git submodule add https://github.com/Zubax/senoval
```

For more information, refer to the Git docs.

Having added the library to your codebase, configure your build system so that
its root directory is located within your include search paths.
Then include the library as follows (example):

```c++
#include <senoval/string.hpp>
```

## Development

Contributors, please follow the [Zubax Coding Conventions](https://kb.zubax.com/x/84Ah).

Build and run the tests like this: `cd test/ && cmake . && make && ./senoval_test`.
See the CI build script for additional details.
