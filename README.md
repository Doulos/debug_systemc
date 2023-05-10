# About this example

This example investigates debugging SystemC under GDB.

## Building

```bash
cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug --config Debug
ctest --test-dir build/debug -C Debug -VV
```

## Debugging

Note that building and running the ctests will indicate all tests passed, but that is because we intentionally test for the "simulation FAILED" string.  There is a .gdbinit file in this directory. To enable automatic loading you will need to enable this in `$HOME/.gdbinit`  by inserting the following:

```gdb
add-auto-load-safe-path /PATH/TO/THIS/DIRECTORY
```

Alternately, you may simply type the following from the GDB command-prompt:

```gdb
source .gdbinit
```

To invoke GDB (and get to the command-prompt) use either of the following approaches:

```bash
build -gdb
# -OR-
gdb -tui build/debug/debug_systemc
```

## Requirements

Note: This project was tested on:

1. macos Ventura 13.3.1a running on MacBook Pro with Apple M1 Max
2. Ubuntu 20.04.6 LTS running on `x86_64` (Intel Xeon CPU) with 1 core & 2G RAM
3. WSL2 running Ubuntu 20.04.5 LTS running on a Dell XPS 15 9510 with i9-11900H (x86_64)

You should have the following tools installed.

- C++ compiler that support C++17 or better (e.g., g++ version 9.4 or clang++ 10.0)
- SystemC version 2.3.3 or better
- `cmake` version 3.21 or better
- `gdb` version 9.2 or better

There may be more requirements, but this description should be sufficient.


#### The end
