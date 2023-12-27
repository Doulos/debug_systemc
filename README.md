# About this project

This project investigates debugging SystemC models under GDB. A discussion of this topic and a demonstration will be available in the Doulos webinar: "Debugging SystemC with GDB". Contact [Doulos](https://www.doulos.com) for more information.

There are two primary results of this project:

1. Some simple SystemC projects to play with while learning how to use GDB. See the directories: `demo/`, `simple/`, and `trivial/`.
2. Scripts and code to support debugging in SystemC. See the directories: `debug/`, and `gdb/` .

You should also read the various markdown documents. Best viewed with GFM flavored viewers such as [GitHub](https://github.com) or [Typora](https://typora.io).

If you enjoy this project and its associated webinar why not take a look at our full SystemC course offerings at <https://www.doulos.com/training/soc-design-and-verification/systemc-tlm-20/>.

## Building

To use this code you will need:

1. **g++** or **clang++** with a version that supports C++17 or better
2. **gdb** version 9.2 or better (default for Ubuntu 20.04)
3. **cmake** version 3.21 or better
4. **SystemC** version 2.3.3 or better.

Caveat: The "minimum" versions specified above are based on how I built this project. It is possible that earlier versions of CMake might suffice; however, you would need to modify all the appropriate `cmake` files since they call out a minimum version. Likewise, other versions of `gdb` might work. However, do
not expect any support for requests of this nature.

### Installing SystemC

SystemC needs to be built and installed. You can get and install it from GitHub with:

```bash
#!bash

# Note: You may modify the following directory paths; however, they should be distinct
# (i.e., not one directory contained within the other). The default
# is to install within your home directory under $HOME/.local/.
SYSTEMC_SRC="${HOME}/.local/src/systemc" #< where it will be built
SYSTEMC_HOME="${HOME}/.local/apps/systemc" #< where it will be installed
export SYSTEMC_HOME

# Note: clang++ works as well. Just be sure versions support C++17 or later.
export CC CXX
CC="gcc"
CXX="g++"
CPPSTD="17"
BUILD_DIR="build/debug-gcc"
GIT_URL="https://github.com/accellera-official/systemc.git"

# Setup directory for source
rm -fr "${SYSTEMC_SRC}"
mkdir -p "${SYSTEMC_SRC}"
cd "${SYSTEMC_SRC}"

# Fetch source
git clone "${GIT_URL}" .

# Select the version you wish to install by checking out the appropriate tag
git tag # list tags
LATEST="$(git tag | tail -1)" # select the latest (last one listed)
git checkout "${LATEST}"

# Configure
cmake -S "${SYSTEMC_SRC}" -B "${BUILD_DIR}"  \
    -DCMAKE_BUILD_TYPE="RelWithDebInfo"      \
    -DCMAKE_INSTALL_PREFIX="${SYSTEMC_HOME}" \
    -DCMAKE_CXX_STANDARD="${CPPSTD}"                \
    -DBUILD_SOURCE_DOCUMENTATION=off
if false; then #< OPTIONAL: change this to true
  ccmake -B "${BUILD_DIR}" #< interactively examine and change configuration options
fi

# Compile
cmake --build "${BUILD_DIR}"

# Install
rm -fr "${SYSTEMC_HOME}" # Precautionary: remove previous version
mkdir -p "${SYSTEMC_HOME}" 
cmake --install "${BUILD_DIR}"

#rm -fr "${SYSTEMC_SRC}" ;# OPTIONAL: uncomment if disk space is at a premium

# OPTIONAL: Test the installation
cat >hello_systemc.cpp <<EOT
#include <systemc>
using namespace sc_core;
using namespace std::literals;
SC_MODULE( Top_module ) {
  SC_CTOR( Top_module ) {
    SC_THREAD( top_thread );
  }
  void top_thread() {
    wait(10,SC_NS);
    SC_REPORT_INFO( "/Doulos/Hello_SystemC/top", ("Hello SystemC at "s + sc_time_stamp().to_string()).c_str() );
    sc_stop();
  }
};
int sc_main( int argc, char* argv[] )
{
  Top_module top{"top"};
  sc_start();
  return sc_report_handler::get_count(SC_ERROR);
}
EOT
"${CXX}" -std="c++${CPPSTD}" -o hello_systemc hello_systemc.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc
```

### Compiling the project

In `setup.profile`, we assume it's installed under `$HOME/.local/apps/systemc`; however, you can alter that by setting the `SYSTEMC_HOME` environment variable.

Start by examining and then sourcing `setup.profile` under `bash` or `zsh`.

```bash
source setup.profile
```

Note: There are four sub-projects: three executables and one "library." The library is not treated as a full class library as the executables recompile it (very small). If you drop into one of the sub-directories, you can build just that portion with the same instructions. It depends on which directory you invoke the following commands. The smallest executable is in the `demo/` directory.

```bash
cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug --config Debug
ctest --test-dir build/debug -C Debug -VV
```

## Debugging

Note that building and running the `ctests` will indicate all tests passed, but that is because we intentionally test for the "Simulation FAILED" string.  There is a .gdbinit file in this directory. To enable automatic loading, you will need to enable this in `$HOME/.gdbinit`  by inserting the following:

```gdb
add-auto-load-safe-path /PATH/TO/THIS/DIRECTORY
```

Alternatively, you may simply type the following from the GDB command prompt:

```gdb
source .gdbinit
```

You may also like to support for `vi` style command-line editing by providing `${HOME}/.inputrc` with: 

```
set editing-mode vi
```

To invoke GDB (and get to the command prompt) any either of the following approaches:

```bash
gdb --args build/debug/demo/app --nSamples=5 --debug
-or-
gdb --args build/debug/trivial/app --nReps=10
-or-
gdb --args build/debug/simple/app --help
```

## Requirements

Note: This project was tested on:

1. macos Ventura 13.3.1a running on MacBook Pro with Apple M1 Max (Arm64) **without GDB**
2. Ubuntu 20.04.6 LTS running on `x86_64` (Intel Xeon CPU) with 1 core & 2G RAM
3. WSL2 running Ubuntu 20.04.5 LTS running on a Dell XPS 15 9510 with i9-11900H (x86_64)
4. WSL2 running Ubuntu 22.04.2 LTS running on a Dell XPS 15 9510 with i9-11900H (x86_64)

You should have the following tools installed.

- C++ compiler that support C++17 or better (e.g., g++ version 9.4 or clang++ 10.0)
- SystemC version 2.3.3 or better
- `cmake` version 3.21 or better
- `gdb` version 9.2 or better
- `make` or `ninja`
- `perl` and `bash` if using any of the scripts

There may be more requirements we've overlooked, but this description should be sufficient. You can use the `tool-versions` bash script under extern/bin to see what your configuration is.

<!--
# vim:nospell
-->
#### The end
