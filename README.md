![](./logo.png)

# pfederc

*pfederc* is a [Feder](https://federlang.github.io/feder-ref) compiler.

## Build

Clone to your drive and build project:

```
git clone https://github.com/federlang/pfederc
cd pfederc
mkdir build
cd build
cmake ..
cmake --build . -j $(nproc)
```

## Dependencies

*pfederc* depends on:

- C++ compiler (build)
- cmake (build)
- llvm
