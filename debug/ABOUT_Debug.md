## Summary and Purpose

The `Debug` class provides a basic set of support methods to aid debugging SystemC code. These address the following aspects:

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

4. Options to provide values at runtime
   + `--nCount=UINT`
   + `--dValue=DOUBLE`
   + `--tDelay=TIME`
   + `--sName=TEXT`

5. Options to inject data for the purpose validation
   + `--inject MASK`

6. Provide a summary of warnings, errors, and fatal messages.
   + `return exit_status(mesgType)` // Reports and then returns non-zero if error or fatal messages occur

7. Methods to query simulation status from a debugger (specifically GDB)
   + `call Debug::help()`
   + `call Debug::info()`
   + `call Debug::opts("idtv")`// instance, timestamp, delta, state, verbosity

8. Simplify reporting syntax
   + allow for `std::string` parameters
   + reduce issues with tags

## Overview

Although a struct (class) is used for these methods, it is effectively only a namespace. A namespace was avoided because at the back of my mind there may be a reason in the future to extend this differently. At any rate, you do not need to instantiate this class.

## Command-line options supported

The command-line parser supports the following:

| Option            | Description                                               |
| ----------------- | --------------------------------------------------------- |
| `--config FILE`   | Set verbosity to `SC_DEBUG`                               |
| `--dNAME=DOUBLE`  | Set NAMEd double to DOUBLE (e.g., -dPi=3.14159 )          |
| `--debug [MASK]`  | Set verbosity to `SC_DEBUG`                               |
| `--fNAME=BOOLEAN` | Set NAMEd flag true or false (e.g., --fTest=true)         |
| `--help`          | This text                                                 |
| `--inject [MASK]` | Intentionally inject errors                               |
| `--nNAME=COUNT`   | Set NAMEd count to COUNT (`size_t`)                       |
| `--no-config`     | Do not read default configuration file (must be first)    |
| `--no-debug`      | Set verbosity to `SC_MEDIUM`                              |
| `--no-inject`     | Turn off injection if set                                 |
| `--no-trace`      | Turn off trace if set                                     |
| `--no-verbose`    | Set verbosity to `SC_MEDIUM`                              |
| `--quiet`         | Set verbosity to `SC_LOW`                                 |
| `--sNAME=TEXT`    | Set NAMEd string to TEXT (e.g., -sFile="data.txt")        |
| `--tNAME=TIME`    | Set NAMEd time to TIME value (e.g., `10_ns`)              |
| `--trace [FILE]`  | Trace signals to dump FILE (default: dump)                |
| `--verbose`| `-v` | Set verbosity to `SC_HIGH` if not debugging               |
| `--warn`          | Warn on any unrecognized command-line switches            |
| `--werror`        | Treat warnings as errors (stop after parsing)             |

In above:

- `--no-config` must be the first option specified
- Trace FILE's will have .vcd appended automatically.
- NAMEd items retain the prefix in the internal name. Thus `--nReps` maps to `count("nReps")`.
- COUNT is unsigned (use a small DOUBLE if you need signed).
- MASK is a numeric and different bits can be used.
- Names should be capitalized as indicate in the examples.

\pagebreak
## API

Many methods are inline and static.

| Method                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| `sc_trace_file* trace_file()`                                | returns a pointer to the currently open tracefile            |
| `bool tracing()`                                             | returns true if tracing is enabled                           |
| `bool debugging( const mask_t& mask = ~0u )`                 | returns true if verbosity is `SC_DEBUG` is enabled           |
| `bool injecting( const mask_t& mask = ~0u )`                 | returns true if injecting errors                             |
| `bool stopping()`                                            | returns true if there is a request to stop before simulation begins in ernest |
| `bool verbose()`                                             | returns true if verbose selected - `SC_HIGH` verbosity or better |
| `bool quiet()`                                               | returns true if quiet selected                               |
| `args_t& config()`                                           | returns a reference to the configuration list (vector<string>) |
| `size_t get_count( const string& name )`                     | returns the named count                                      |
| `sc_time get_time( const string& name )`                     | returns the named time                                       |
| `bool get_flag( const string& name )`                        | returns the named flag                                       |
| `string get_text(const string& name)`                        | returns the named text                                       |
| `void close_trace_file()`                                    | closes the tracefile                                         |
| `void read_config(args_t& args, string file)`                | reads a configuration file (default is APPNAME.cfg)          |
| `void parse_command_line()`                                  | parses the command-line and configuration files              |
| `void breakpoint( const string& tag )`                       | subroutine to set breakpoint explicitly (for use in GDB)     |
| `void stop_if_requested()`                                   | issues `sc_top()` if requested via `s_stop()`                |
| `void set_trace_file( const string& filename )`              | sets the trace file                                          |
| `void set_quiet( bool flag = true )`                         | selects quiet output                                         |
| `void set_verbose( bool flag = true )`                       | selects verbose output                                       |
| `void set_debugging( const mask_t& mask = 1 )`               | enables debugging                                            |
| `void clr_debugging( const mask_t& mask = 0 )`               | clears debugging                                             |
| `void set_injecting( const mask_t& mask = 1 )`               | enables injection                                            |
| `void set_count( const string& name, size_t count = 1 )`     | sets the named count                                         |
| `void set_time( const string& name, const sc_time& time = 0 )` | sets the named time                                        |
| `void set_flag( const string& name, bool flag = true )`      | sets the named flag                                          |
| `void help()`                                                | displays this text (for use in GDB)                          |
| `void info()`                                                | displays systemc status (for use in GDB)                     |
| `void show( const string& s)`                                | displays a string (for use in GDB)                           |
| `void name( const sc_core::sc_object* obj )`                 | displays the object path (for use in GDB)                    |
| `void opts()`                                                | displays information about selected options (for use in GDB) |
| `cstr_t process()`                                           | returns the current process name (for use in GDB)            |
| `cstr_t text(const string& s)`                               | returns a `std::string` as a `const char*` (for use in GDB)  |
| `string get_simulation_status()`                             | returns the simulation status as a string                    |
| `string get_verbosity()`                                     | returns the current verbosity level as a string              |
| `string command_options()`                                   | returns the currently set command-line options as a string   |

| Variable                                                     | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| `#define NOCOLOR`                                            | Optionally define to suppress color messages                 |
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

Some handy macros useful in source code:

| Macro                       | Description                                                                               |
| --------------------------- | ----------------------------------------------------------------------------------------- |
| `NDEBUG`                    | If you define this, `DBG_WAIT(...)` becomes `wait(...)`                                   |
| `NOCOLOR`                   | If you define this, color is suppressed.                                                  |
| `REPORT_WARNING(mesg)`      | Effectively `SC_REPORT_WARNING  ( mesgType, mesg )`, but allows for std::string           |
| `REPORT_ERROR(mesg)`        | Effectively `SC_REPORT_ERROR    ( mesgType, mesg )`, but allows for std::string           |
| `REPORT_FATAL(mesg)`        | Effectively `SC_REPORT_FATAL    ( mesgType, mesg )`, but allows for std::string           |
| `REPORT_VERB(mesg,level)`   | Effectively `SC_REPORT_INFO_VERB( mesgType, mesg, level )`, but allows for std::string    |
| `REPORT_ALWAYS(mesg)`       | Effectively `SC_REPORT_INFO_VERB( mesgType, mesg, SC_NONE )`, but allows for std::string  |
| `REPORT_DEBUG(mesg)`        | Effectively `SC_REPORT_INFO_VERB( mesgType, mesg, SC_DEBUG )`, but allows for std::string |
| `REPORT_NUM(var)`           | Effectively `REPORT_DEBUG( #var "=" std::to_string(var) )`                                |
| `REPORT_STR(var)`           | Effectively `REPORT_DEBUG( #var "=" var )`                                                |
| `REPORT_OBJ(var)`           | Effectively `REPORT_DEBUG( #var "=" var.to_string() )`                                    |

The `REPORT_*` macros assume you have defined an identifier `const char* mesgType = "/COMPANY/PROJECT/MODULE"` at the point of compilation where used.

\pagebreak
## Example

```cpp
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
    auto nGrade = Debug::get_count("nGrade");
    auto tDelay = Debug::get_time("tDelay");
    auto sName = Debug::get_text("sName");
    auto fEnable = Debug::get_flag("fEnable");
    ...
    if( Debug::injecting() ) { ... }
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
