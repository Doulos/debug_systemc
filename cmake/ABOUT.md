# About this `cmake/` directory

This directory contains some `cmake` cruft to set up policy and simplify the contents of `CMakeLists.txt` files.

# How to use

To use the files in this directory, several things must be done:

1. Use a directory layout as below:
```
PROJECT_NAME <--------- Or course exercise directory name
├── bin/ <------------- Optional local scripts used to compile
│   ├── build <-------- Simplifies use of cmake slightly
│   └── Essential-IO <- Used by `build` script
├── cmake/ <----------- This directory
├── extern/ <--------- For external dependencies such a GoogleTest or {fmt} lib
│   ├── bin/ <--------- Executables if any
│   ├── include/ <----- For includes
│   ├── lib/ <--------- For compiled libraries
│   :
:
```

2. Setup some environment variables. Perhaps using a file named setup.profile

  + `SYSTEMC_HOME` refers to the installation directory for SystemC.

  + `LD_LIBRARY_PATH` is needed to reference the SystemC library.

  + `DYLD_LIBRARY_PATH` is identical to `LD_LIBRARY_PATH` for macOS.

  Typically, I create a file at the top-level called `setup.profile` marked as executable (to remind me) as follows:

  ```bash
  #!/usr/bin/env bash
  (return 0 2>/dev/null) && SOURCED=1 || SOURCED=0
  if [[ ${SOURCED} == 0 ]]; then
    echo "Don't run $0, source it" >&2
    exit 1
  fi
  export SYSTEMC_HOME LD_LIBRARY_PATH DYLD_LIBRARY_PATH
  SYSTEMC_HOME="${APPS}/systemc"
  LD_LIBRARY_PATH="${SYSTEMC_HOME}/lib"
  DYLD_LIBRARY_PATH="${LD_LIBRARY_PATH}"
  ```

3. Create your `CMakeLists.txt` file minimally as follows. Replacing elements enclosed in {:bace-colon brackets:} appropriately.

```cmake
#!cmake .

cmake_minimum_required( VERSION 3.18 )

project( {:Name of project:} VERSION {:Major:}.{:Minor:} DESCRIPTION "{:Description of project:}" LANGUAGES CXX )

#-------------------------------------------------------------------------------
# Find project working tree directory containing extern/ and cmake/)
#-------------------------------------------------------------------------------
set( _dir "${CMAKE_CURRENT_SOURCE_DIR}" )
cmake_path( GET _dir ROOT_PATH _root )
while( NOT IS_DIRECTORY "${_dir}/extern" AND NOT IS_DIRECTORY "${_dir}/cmake" )
  cmake_path( GET _dir PARENT_PATH _dir )
  if( "${_dir}" STREQUAL "${_root}" )
    message( FATAL_ERROR "Unable to find project workintg tree directory!" )
  endif()
endwhile()
set( WORKTREE_DIR "${_dir}" CACHE PATH "Points to directory containing cmake/ and extern/ subdirectories for the project." )
list( PREPEND CMAKE_MODULE_PATH "${WORKTREE_DIR}/cmake" )
include( sc-project_defaults )

#-------------------------------------------------------------------------------
# Target specific
#-------------------------------------------------------------------------------
set_target( {:Name of executable:} )
add_executable( "${Target}" )
target_sources( "${Target}" PRIVATE
  {:List of source files including headers:}
)
add_test( NAME "${Target}_test" COMMAND "${Target}" )

```

4. Compile and run using the following commands:

```bash
# Setup environment
source "$(git rev-parse --show-toplevel)/setup.profile"
DEFS="CMAKE_PREFIX_PATH='$(pwd);${HOME}/.local/apps'"
cmake -B build -D"${DEFS}" -S .        ;# Configure
cmake --build build                    ;# Compile & link
ctest --test-dir build -V              ;# Execute
cmake --install build --prefix {:Dir:} ;# Optional
```

or simplify the above by using the script `bin/build`:

```bash
bin/build --help # for details
bin/build        # to configure, compile, and link only
bin/build -test  # to configure, compile, link and test
```

For more information, contact me (david.black@doulos.com)
