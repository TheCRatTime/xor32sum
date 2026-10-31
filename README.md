# xor32sum
**xor32sum** - small control sum checker.

## Description
This util checks file for corruptions and calculate 32-bit hash sum
for it. Have small API for calcutale same sums. Sums based on
Xorshift32 algorithm.

### Features
- **C89/ANSI C** code standard.
- **Small API**: you can add hash sums to your project
- **Minimal memory usage**: only for file calls **malloc**.

### Build
Needed:
- C Compiler
- CMake 3.10 and greater
- GNU Make

```bash
cmake -B build
cd build/
make
```

### Documentation
You can found documentation in `doc/` directory.

### Authors
TheCRatTime
