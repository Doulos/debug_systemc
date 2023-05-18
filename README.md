# About this example

This example investigates debugging SystemC under GDB.

## Building

To use this code you will need:

1. g++ or clang++ with a version that supports C++17 or better
2. gdb version 9.2 or better (default for Ubuntu 20.04)
3. cmake version 3.21 or better
4. SystemC version 2.3.3 or better.

SystemC needs to be built and installed. You can get and install it from GitHub with:

```bash
# Note: You modify the following; however, they must be distinct (i.e., not one contained within the other).
SYSTEMC_SRC="${HOME}/.local/src" #< where it will be built
SYSTEMC_HOME="${HOME}/.local/apps/systemc" #< where it will be installed
CXX="g++"
CC="gcc"
BUILD_DIR="build/debug-gcc"

# Setup
rm -fr "${SYSTEMC_SRC}"
mkdir -p "${SYSTEMC_SRC}"
cd "${SYSTEMC_SRC}"

# Fetch source
git clone https://github.com/accellera-official/systemc.git .

# Configure
cmake -S "${SYSTEMC_SRC}" -B "${BUILD_DIR}"  \
    -DCMAKE_BUILD_TYPE="RelWithDebInfo"      \
    -DCMAKE_INSTALL_PREFIX="${SYSTEMC_HOME}" \
    -DCMAKE_CXX_STANDARD="17"                \
    -DBUILD_SOURCE_DOCUMENTATION=off
if false; then #< optionally change this to true
  ccmake -B "${BUILD_DIR}" #< interacively examine and change configuration options
fi

# Compile
cmake --build "${BUILD_DIR}"

# Install
rm -fr "${SYSTEMC_HOME}"
mkdir -p "${SYSTEMC_HOME}" 
cmake --install "${BUILD_DIR}"

# Optional - you may remove the "${SYSTEMC_SRC}" if disk space is at a premium
```

In `setup.profile`, we assume it's installed under `$HOME/.local/apps/systemc`; however, you can alter that by setting the `SYSTEMC_HOME` environment variable.

Start by examining and then sourcing `setup.profile` under `bash` or `zsh`.

```bash
source setup.profile
```

```bash
cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug --config Debug
ctest --test-dir build/debug -C Debug -VV
```

## Debugging

Note that building and running the ctests will indicate all tests passed, but that is because we intentionally test for the "Simulation FAILED" string.  There is a .gdbinit file in this directory. To enable automatic loading you will need to enable this in `$HOME/.gdbinit`  by inserting the following:

```gdb
add-auto-load-safe-path /PATH/TO/THIS/DIRECTORY
```

Alternately, you may simply type the following from the GDB command-prompt:

```gdb
source .gdbinit
```

To invoke GDB (and get to the command-prompt) use either of the following approaches:

```bash
gdb build/debug/navigate/app
-or-
gdb build/debug/trivial/app
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
