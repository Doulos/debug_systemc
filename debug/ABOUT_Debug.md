## Purpose

Provide a basic set of support methods to aid debugging SystemC code. These address the following aspects:

1. Ability to provide command-line options or a configuration file to select options.
   + Parse the command-line `Debug::parse_command_line()`
   + Detect and warn or error out of mistyped options
   + Determine and use a file to pass options `--config FILENAME`
2. Options to control the verbosity of `REPORT_INFO_VERB` reports
   + `--debug`
   + `--verbose`
   + `--quiet`
3. Options to control waveform tracing
   + `--trace [FILE]`
5. Options to provide values at runtime
   + `--nCount=UINT`
   + `--dValue=DOUBLE`
   + `--tDelay=TIME`
   + `--sName=TEXT`
6. Options to inject data for the purpose validation
   + `--inject MASK
7. Provide a summary of warnings, errors, and fatal messages.
   + Exit non-zero if error or fatal messages occur.
8. Methods to query simulation status from a debugger (specifically GDB)
   + call Debug::info("idtv")
   + call Debug::opts()
9. Simplify reporting syntax
   + allow for std::string parmeters
   + reduce issues with tags

## Overview

Although a struct (class) is used for these methods, it is effectively only a namespace. A namespace was avoided because at the back of my mind there may be a reason in the future to extend this differently. At any rate, you do not need to instantiate this class.

## API

Most methods are inline and all are currently static.

| METHOD                                                       | DESCRIPTION                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| `sc_trace_file* trace_file()`                                | returns a pointer to the currently open tracefile            |
| `bool tracing()`                                             | returns true if tracing is enabled                           |
| `bool debugging( const mask_t& mask = ~0u )`                 | returns true if verbosity is `SC_DEBUG` is enabled           |
| `bool injecting( const mask_t& mask = ~0u )`                 | returns true if injecting errors                             |
| `bool stopping()`                                            | returns true if there is a request to stop before simulation begins in ernest |
| `bool verbose()`                                             | returns true if verbose selected - `SC_HIGH` verbosity or better |
| `bool quiet()`                                               | returns true if quiet selected                               |
| `args_t& config()`                                           | returns a reference to the configuration list (vector<string>) |
| `size_t count(const string& name)`                           | returns the named count                                      |
| `sc_time time(const string& name)`                           | returns the named time                                       |
| `bool flag(const string& name)`                              | returns the named flag                                       |
| `void close_trace_file()`                                    | closes the tracefile                                         |
| `void read_configuration( args_t& args, string file = "" )`  | reads a configuration file (default is APPNAME.cfg)          |
| `void parse_command_line()`                                  | parses the command-line and configuration files              |
| `void breakpoint()`                                          | subroutine for setting a breakpoint explicitly in your source code |
| `void stop_if_requested()`                                   | issues `sc_top()` if requested via `s_stop()`                |
| `void set_trace_file( const string& filename )`              | sets the trace file                                          |
| `void set_quiet( bool flag = true )`                         | selects quiet output                                         |
| `void set_verbose( bool flag = true )`                       | selects verbose output                                       |
| `void set_debugging( const mask_t& mask = 1 )`               | enables debugging                                            |
| `void clr_debugging( const mask_t& mask = 0 )`               | clears debugging                                             |
| `void set_injecting( const mask_t& mask = 1 )`               | enables injection                                            |
| `void set_count( const string& name, size_t count = 1 )`     | sets the named count                                         |
| `void set_time( const string& name, const sc_time& time = 0 )` | sets the named time                                          |
| `void set_flag( const string& name, bool flag = true )`      | sets the named flag                                          |
| `void info()`                                                | displays systemc status (for use in GDB)                     |
| `void name(const sc_core::sc_object* m)`                     | displays the object path (for use in GDB)                    |
| `string get_status()`                                        | returns the simulation status as a string                    |
| `string get_verbosity()`                                     | returns the current verbosity level as a string              |
| `string command_options()`                                   | returns the currently set command-line options as a string   |
| `const char* none`                                           | Character sequence for xterm to turn off all attributes.     |
| `const char* bold`                                           | Character sequence for xterm to use **bold** highlighting.   |
| `const char* black`                                          | Character sequence for xterm to display the indicated color. |
| `const char* red`                                            | Character sequence for xterm to display the indicated color. |
| `const char* green`                                          | Character sequence for xterm to display the indicated color. |
| `const char* yellow`                                         | Character sequence for xterm to display the indicated color. |
| `const char* blue`                                           | Character sequence for xterm to display the indicated color. |
| `const char* magenta`                                        | Character sequence for xterm to display the indicated color. |
| `const char* cyan`                                           | Character sequence for xterm to display the indicated color. |
| `const char* white`                                          | Character sequence for xterm to display the indicated color. |

The command-line parser supports the following:

| Option            | Description                                               |
| ----------------- | --------------------------------------------------------- |
| `--config FILE`   | Set verbosity to `SC_DEBUG`                               |
| `--nNAME=COUNT`   | Set NAMEd count to COUNT (`size_t`)                       |
| `--tNAME=TIME`    | Set NAMEd time to TIME value (e.g., `10_ns`)              |
| `--debug`         | Set verbosity to `SC_DEBUG`                               |
| `--quiet`         | Set verbosity to `SC_LOW`                                 |
| `--warn`          | Warn on any unrecognized command-line switches            |
| `--Werror`        | Treat warnings as errors (stop after parsing)             |
| `--verbose`| `-v` | Set verbosity to `SC_HIGH` if not debugging               |
| `--inject [MASK]` | Intentionally inject errors                               |
| `--trace [FILE]   | Trace signals to dump FILE (default: dump)                |
| `--no-debug`      | Set verbosity to `SC_MEDIUM`                              |
| `--no-verbose`    | Set verbosity to `SC_MEDIUM`                              |
| `--no-inject`     | Turn off injection if set                                 |
| `--no-trace`      | Turn off trace if set                                     |
| `--no-config`     | Do not read default configuration file (must be first)    |
| `--help`          | This text                                                 |
| `--fNAME=BOOLEAN` | Set NAMEd flag true or false (e.g., --fTest=true)  (^NYI) |
| `--sNAME=TEXT`    | Set NAMEd string to TEXT (e.g., -sFile="data.txt") (^NYI) |
| `--dNAME=DOUBLE`  | Set NAMEd double to DOUBLE (e.g., -dPi=3.14159 )   (^NYI) |

In above:

- `--no-config` must be the first option specified
- Trace FILE's will have .vcd appended automatically.
- COUNT is unsigned (use a small DOUBLE if you need signed).
- MASK is a numeric and different bits can be used.
- Names should be capitalized as indicate in the examples.

[^NYI]: Not yet implemented

## Example

```c++
#include "debug.hpp"
#include <systemc>


struct Top_module : sc_core::sc_module
{
  using report_handler = sc_core::sc_report_handler;

  static constexpr const char *mesgType = "/Doulos/debugging_systemc/top";

  // Constructor
  explicit Top_module( const sc_core::sc_module_name& instance )
    : sc_module( instance )
  {
    // No interconnect for this design
    SC_HAS_PROCESS(Top_module);
    SC_THREAD( test_thread );
  }

  void before_end_of_elaboration() override
  {

    report_handler::set_actions( sc_core::SC_WARNING, sc_core::SC_DISPLAY | sc_core::SC_INTERRUPT );
    report_handler::set_actions( sc_core::SC_ERROR, sc_core::SC_DISPLAY   | sc_core::SC_INTERRUPT );

    Debug::parse_command_line();
  }

  void start_of_simulation() override
  {
    Debug::stop_if_requested();
  }

  void test_thread() {
    auto nGrade = Debug::Count("nGrade");
    SC_REPORT_INFO_VERB( mesgType, "Starting report...", sc_core::SC_DEBUG );
    if ( nGrade < 70 ) SC_REPORT_FATAL( mesgType, "You failed the exam!" );
    else if ( nGrade < 80 ) SC_REPORT_ERROR( mesgType, "You barely passed" );
    else if ( nGrade < 90 ) SC_REPORT_WARNING( mesgType, "You did fairly well, but you can do better" );
    else if ( nGrade < 100 ) SC_REPORT_INFO( mesgType, "You are an A student!" );
    else if ( nGrade > 100 ) SC_REPORT_INFO_VERB( mesgType, "This looks fishy!", sc_core::SC_NONE );
    sc_core::sc_stop();
  }

};


int sc_main( int argc, char* argv[] )
{
  constexpr const char* mesgType = "/Doulos/debugging_systemc/sc_main";

  Top_module top { "top" };
  sc_start();

  if ( not sc_end_of_simulation_invoked() ) {
    sc_stop();  // triggers end_of_simulation() callback
  }

  return Debug::exit_status( mesgType );
}
```
