## Purpose

Provide a basic set of support methods to aid debugging SystemC code.

## Overview

Although a struct (class) is used for these methods, it is effectively only a namespace. A namespace was avoided because at the back of my mind there may be a reason in the future to extend this differently. At any rate, you do not need to instantiate this class.

## API

Most methods are inline and all are currently static.

| METHOD                                                         | DESCRIPTION                                                                                                                                    |
| -------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| `sc_trace_file* trace_file()`                                  | returns a pointer to the currently open tracefile                                                                                              |
| `bool tracing()`                                               | returns true if tracing is enabled                                                                                                             |
| `bool debugging( const mask_t& mask = ~0u )`                   | returns true if verbosity is `SC_DEBUG` is enabled                                                                                             |
| `bool injecting( const mask_t& mask = ~0u )`                   | returns true if injecting errors                                                                                                               |
| `bool stopping()`                                              | returns true if there is a request to stop before simulation begins in ernest                                                                  |
| `bool verbose()`                                               | returns true if verbose selected - `SC_HIGH` verbosity or better                                                                               |
| `bool quiet()`                                                 | returns true if quiet selected                                                                                                                 |
| `args_t& config()`                                             | returns a reference to the configuration list (vector<string>)                                                                                 |
| `size_t count(const string& name)`                             | returns the named count                                                                                                                        |
| `sc_time time(const string& name)`                             | returns the named time                                                                                                                         |
| `bool flag(const string& name)`                                | returns the named flag                                                                                                                         |
| `void close_trace_file()`                                      | closes the tracefile                                                                                                                           |
| `void read_configuration( args_t& args, string file = "" )`    | reads a configuration file (default is APPNAME.cfg)                                                                                            |
| `void parse_command_line()`                                    | parses the command-line and configuration files                                                                                                |
| `void breakpoint()`                                            | subroutine for setting a breakpoint explicitly in your source code                                                                             |
| `void stop_if_requested()`                                     | issues `sc_top()` if requested via `s_stop()`                                                                                                  |
| `void set_trace_file( const string& filename )`                | sets the trace file                                                                                                                            |
| `void set_quiet( bool flag = true )`                           | selects quiet output                                                                                                                           |
| `void set_verbose( bool flag = true )`                         | selects verbose output                                                                                                                         |
| `void set_debugging( const mask_t& mask = 1 )`                 | enables debugging                                                                                                                              |
| `void clr_debugging( const mask_t& mask = 0 )`                 | clears debugging                                                                                                                               |
| `void set_injecting( const mask_t& mask = 1 )`                 | enables injection                                                                                                                              |
| `void set_count( const string& name, size_t count = 1 )`       | sets the named count                                                                                                                           |
| `void set_time( const string& name, const sc_time& time = 0 )` | sets the named time                                                                                                                            |
| `void set_flag( const string& name, bool flag = true )`        | sets the named flag                                                                                                                            |
| `void status()`                                                | displays systemc status (for use in GDB)                                                                                                       |
| `void name(const sc_core::sc_object* m)`                       | displays the object path (for use in GDB)                                                                                                      |
| `string get_status()`                                          | returns the simulation status as a string                                                                                                      |
| `string get_verbosity()`                                       | returns the current verbosity level as a string                                                                                                |
| `string command_options()`                                     | returns the currently set command-line options as a string                                                                                     |
| `const char* none`                                             | Character sequence for xterm to turn off all attributes.                                                                                       |
| `const char* bold`                                             | Character sequence for xterm to use **bold** highlighting.                                                                                     |
| `const char* black`                                            | Character sequence for xterm to display the indicated color.                                                                                   |
| `const char* red`                                              | Character sequence for xterm to display the indicated color.                                                                                   |
| `const char* green`                                            | Character sequence for xterm to display the indicated color.                                                                                   |
| `const char* yellow`                                           | Character sequence for xterm to display the indicated color.                                                                                   |
| `const char* blue`                                             | Character sequence for xterm to display the indicated color.                                                                                   |
| `const char* magenta`                                          | Character sequence for xterm to display the indicated color.                                                                                   |
| `const char* cyan`                                             | Character sequence for xterm to display the indicated color.                                                                                   |
| `const char* white`                                            | Character sequence for xterm to display the indicated color.                                                                                   |

The command-line parser supports the following:

| Option           | Description                                            |
| --------------   | ------------------------------------------------------ |
| --config FILE    | Set verbosity to SC_DEBUG                              |
| --nNAME=VALUE    | Set NAME (case-sensitive) to size_t VALUE              |
| --tNAME=TIME     | Set NAME (case-sensitive) to TIME (e.g., 10_ns)        |
| --debug          | Set verbosity to SC_DEBUG                              |
| --quiet          | Set verbosity to SC_LOW                                |
| --warn           | Warn on any unrecognized command-line switches         |
| --Werror         | Treat warnings as errors (stop after parsing)          |
| --verbose | -v   | Set verbosity to SC_HIGH if not debugging              |
| --inject [MASK]  | Intentionally inject errors                            |
| --trace [FILE]   | Trace signals to dump FILE (default: dump)             |
| --no-debug       | Set verbosity to SC_MEDIUM                             |
| --no-verbose     | Set verbosity to SC_MEDIUM                             |
| --no-inject      | Turn off injection if set                              |
| --no-trace       | Turn off trace if set                                  |
| --no-config      | Do not read default configuration file (must be first) |
| --help           | This text                                              |
| --NAME=VALUE     | Set flag true or false (e.g., --test=true)             |

## Example

```c++
#include "debug.hpp"

```
