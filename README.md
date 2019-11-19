![](./logo.png)

# pfederc

*pfederc* is a [Feder](https://federlang.github.io/feder-ref) compiler.

## Build

### Linux

Clone to your drive and build project:

```sh
git clone https://github.com/federlang/pfederc
cd pfederc
mkdir build
cd build
cmake ..
cmake --build . -j $(nproc)
```

Testing (user is in build directory):

```sh
cd test
ctest -j $(nproc)
```

## Dependencies

*pfederc* depends on:

- C++ compiler (build)
- cmake (build)
- valgrind (optional, testing)
- llvm
