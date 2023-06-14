#include "debug.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <array>
using namespace sc_core;
using namespace sc_dt;
using namespace std::literals;
using namespace std;

namespace {
  const char* syntax = R"(
Synopsis
--------

EXECUTABLE [--no-config] [OPTIONS]
EXECUTABLE --help

| Option            | Description                                               |
| ----------------- | --------------------------------------------------------- |
| `--config FILE`   | Set verbosity to `SC_DEBUG`                               |
| `--dNAME=DOUBLE`  | Set NAMEd double to DOUBLE (e.g., -dPi=3.14159 )          |
| `--debug [MASK]`  | Set verbosity to `SC_DEBUG`                               |
| `--expect=N`      | Expect N errors                                           |
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

API
---

Many methods are inline and static.

| Method                                                                | Description                                                       |
| --------------------------------------------------------------------- | ----------------------------------------------------------------- |
| `sc_trace_file* Debug::trace_file()`                                  | returns a pointer to the currently open tracefile                 |
| `bool Debug::tracing()`                                               | returns true if tracing is enabled                                |
| `bool Debug::debugging( const mask_t& mask = ~0u )`                   | returns true if verbosity is `SC_DEBUG` is enabled                |
| `bool Debug::injecting( const mask_t& mask = ~0u )`                   | returns true if injecting errors                                  |
| `bool Debug::stopping()`                                              | returns true if stop requested before SC_RUNNING                  |
| `bool Debug::verbose()`                                               | returns true if verbose selected - `SC_HIGH` verbosity or better  |
| `bool Debug::quiet()`                                                 | returns true if quiet selected                                    |
| `args_t& Debug::config()`                                             | returns a reference to the configuration list (vector<string>)    |
| `size_t  Debug::get_count( const string& name )`                      | returns the named count                                           |
| `sc_time Debug::get_time( const string& name )`                       | returns the named time                                            |
| `bool    Debug::get_flag( const string& name )`                       | returns the named flag                                            |
| `string  Debug::get_text(const string& name)`                         | returns the named text                                            |
| `void Debug::close_trace_file()`                                      | closes the tracefile                                              |
| `void Debug::read_config(args_t& args, string file)`                  | reads a configuration file (default is APPNAME.cfg)               |
| `void Debug::parse_command_line()`                                    | parses the command-line and configuration files                   |
| `void Debug::breakpoint( const string& tag )`                         | subroutine to set breakpoint explicitly (for use in GDB)          |
| `void Debug::stop_if_requested()`                                     | issues `sc_top()` if requested via `s_stop()`                     |
| `void Debug::set_trace_file( const string& filename )`                | sets the trace file                                               |
| `void Debug::set_quiet( bool flag = true )`                           | selects quiet output                                              |
| `void Debug::set_verbose( bool flag = true )`                         | selects verbose output                                            |
| `void Debug::set_debugging( const mask_t& mask = 1 )`                 | enables debugging                                                 |
| `void Debug::clr_debugging( const mask_t& mask = 0 )`                 | clears debugging                                                  |
| `void Debug::set_injecting( const mask_t& mask = 1 )`                 | enables injection                                                 |
| `void Debug::set_count( const string& name, size_t count = 1 )`       | sets the named count                                              |
| `void Debug::set_time( const string& name, const sc_time& time = 0 )` | sets the named time                                               |
| `void Debug::set_flag( const string& name, bool flag = true )`        | sets the named flag                                               |
| `void add_expected( sc_severity level, string msg_type, ssize_t n)`   | sets expectations for `n` messages of specified `severity`        |
| `void see_expected( sc_severity level, string msg_type_)`             | use this in `report_handler` if checking msg_type of expected     |
| `ssize_t get_expected( sc_severity level = max_severity )`            | returns total number of expected messages of severity or all      |
| `ssize_t get_observed( sc_severity level = max_severity )`            | returns total number of observed messages of severity or all      |
| `void Debug::help()`                                                  | displays this text (for use in GDB)                               |
| `void Debug::info()`                                                  | displays systemc status (for use in GDB)                          |
| `void Debug::show( const string& s)`                                  | displays a string (for use in GDB)                                |
| `void Debug::opts()`                                                  | displays information about selected options (for use in GDB)      |
| `cstr_t Debug::name( const sc_core::sc_object* obj )`                 | returns the object path (for use in GDB)                          |
| `cstr_t Debug::process()`                                             | returns the current process name (for use in GDB)                 |
| `cstr_t Debug::text(const string& s)`                                 | returns a `std::string` as a `const char*` (for use in GDB)       |
| `string Debug::get_simulation_status()`                               | returns the simulation status as a string                         |
| `string Debug::verbosity_str()`                                       | returns the current verbosity level as a string                   |
| `string Debug::command_options()`                                     | returns the currently set command-line options as a string        |

Some handy macros useful in source code (these all come from `report.hpp`):

| Macro                       | Description                                                                               |
| --------------------------- | ----------------------------------------------------------------------------------------- |
| `COLOR_BOLD`                | ANSI sequence for bold text unless NOCOLOR is defined                                     |
| `COLOR_NONE`                | ANSI sequence turn turn off colors unless NOCOLOR is defined                              |
| `COLOR_BLACK`               | ANSI sequence for black text unless NOCOLOR is defined                                    |
| `COLOR_RED`                 | ANSI sequence for red text unless NOCOLOR is defined                                      |
| `COLOR_GREEN`               | ANSI sequence for green text unless NOCOLOR is defined                                    |
| `COLOR_YELLOW`              | ANSI sequence for yellow text unless NOCOLOR is defined                                   |
| `COLOR_BLUE`                | ANSI sequence for blue text unless NOCOLOR is defined                                     |
| `COLOR_MAGENTA`             | ANSI sequence for magenta text unless NOCOLOR is defined                                  |
| `COLOR_CYAN`                | ANSI sequence for cyan text unless NOCOLOR is defined                                     |
| `COLOR_WHITE`               | ANSI sequence for white text unless NOCOLOR is defined                                    |
| `COLOR_INFO`                | color for informational messages                                                          |
| `COLOR_WARN`                | color for warning messages                                                                |
| `COLOR_ERROR`               | color for error messages                                                                  |
| `COLOR_FATAL`               | color for fatal messages                                                                  |
| `COLOR_DEBUG`               | color for debug messages                                                                  |
| `NDEBUG`                    | If you define this, `DBG_WAIT(...)` becomes `wait(...)`                                   |
| `NOCOLOR`                   | If you define this, color is suppressed.                                                  |
| `REPORT_WARNING(mesg)`      | Effectively `SC_REPORT_WARNING  ( msg_type, mesg )`, but allows for std::string           |
| `REPORT_ERROR(mesg)`        | Effectively `SC_REPORT_ERROR    ( msg_type, mesg )`, but allows for std::string           |
| `REPORT_FATAL(mesg)`        | Effectively `SC_REPORT_FATAL    ( msg_type, mesg )`, but allows for std::string           |
| `REPORT_VERB(mesg,level)`   | Effectively `SC_REPORT_INFO_VERB( msg_type, mesg, level )`, but allows for std::string    |
| `REPORT_ALWAYS(mesg)`       | Effectively `SC_REPORT_INFO_VERB( msg_type, mesg, SC_NONE )`, but allows for std::string  |
| `REPORT_DEBUG(mesg)`        | Effectively `SC_REPORT_INFO_VERB( msg_type, mesg, SC_DEBUG )`, but allows for std::string |
| `REPORT_NUM(var)`           | Effectively `REPORT_DEBUG( #var "=" std::to_string(var) )`                                |
| `REPORT_STR(var)`           | Effectively `REPORT_DEBUG( #var "=" var )`                                                |
| `REPORT_OBJ(var)`           | Effectively `REPORT_DEBUG( #var "=" var.to_string() )`                                    |

The `REPORT_*` macros assume you have defined an identifier `const char* msg_type = "/COMPANY/PROJECT/MODULE"` at the point of compilation where used.

Examples
--------

```bash
EXECUTABLE -nReps=1'000
EXECUTABLE -nReps=20 --trace --debug --inject
```

```c++
// TBS
```

)";

  const auto npos = string::npos;

  string lowercase( string s )
  {
    std::transform( s.begin(), s.end(), s.begin(),
                    [](unsigned char c){ return std::tolower(c); } );
    return s;
  }

  void replace_all(string& inout, string_view what, string_view with)
  {
    for (auto pos = size_t{};
         npos != (pos = inout.find(what.data(), pos, what.length()));
         pos += with.length())
        inout.replace(pos, what.length(), with.data(), with.length());
  }

  string verbosity_offset( int base )
  {
    auto result = ""s;
    auto difference = sc_report_handler::get_verbosity_level() - base;
    if ( difference != 0 ) { result += "+"s + std::to_string( difference ); }
    return result;
  }

}//endnamespace

// Global constant for use with opts within debugger
[[maybe_unused]] volatile const char* Debug::all = "itdsv"; // instance, timestamp, delta, state, verbosity
[[maybe_unused]] volatile Debug::mask_t mask1{1};
[[maybe_unused]] volatile Debug::mask_t mask0{0};

namespace Doulos {

std::string version() {
  auto result = "Doulos debug "s;
  result += std::to_string(DOULOS_DEBUG_VERSION_MAJOR);
  result += '.';
  result += std::to_string(DOULOS_DEBUG_VERSION_MINOR);
  result += '.';
  result += std::to_string(DOULOS_DEBUG_VERSION_PATCH);
#if DOULOS_DEBUG_IS_PRERELEASE != 0
  result += " prerelease";
#endif
  return result;
}

//------------------------------------------------------------------------------
// Introspection
//..............................................................................
// Constructor
Info::Info( std::string context_name )
 : m_context{std::move(context_name)}
{}

const char* Info::context( const std::string& new_context_name ) {
  if( not new_context_name.empty() ) m_context = new_context_name;
  return m_context.c_str();
}

void Info::mark( const std::string& action_name
               , const std::string& func
               , sc_object* obj
               , const std::string& what
               )
{
  REPORT_INFO_VERB( context()
                  , Doulos::text( action_name + " "s + func + " "s
                                  + Debug::get_simulation_info( obj, what )
                                )
                  , Debug::message_level
                  );
}

void Info::executed( const std::string& func, sc_object* obj, const std::string& what  )
   { mark( "Executed"s, func + "()", obj, what ); }

void Info::entering( const std::string& func, sc_object* obj, const std::string& what )
   { mark( "Entering"s, func + "()", obj, what ); }

void Info::yielding( const std::string& func, sc_object* obj, const std::string& what )
   { mark( "Yielding"s, func + "()", obj, what ); }

void Info::resuming( const std::string& func, sc_object* obj, const std::string& what )
   { mark( "Resuming"s, func + "()", obj, what ); }

void Info::leaving( const std::string& func, sc_object* obj, const std::string& what )
   { mark( "Leaving"s, func + "()", obj, what ); }

}//endnamespace Doulos

//------------------------------------------------------------------------------
// Debug support
//..............................................................................
std::string Debug::get_simulation_info( sc_object* obj, const std::string& what )
{
  auto result = ""s;
  auto status = sc_get_status();
  auto time_is_valid = (status == SC_RUNNING or status == SC_PAUSED or status == SC_STOPPED or status == SC_END_OF_SIMULATION );
  // instance
  if ( what.find_first_of("iI") != npos and obj != nullptr ) { 
    result +=obj->name();
  }
  if ( time_is_valid ) {
    // time
    if ( what.find_first_of("tT") != npos  ) {
      result += " at "s + sc_time_stamp().to_string();
    }
    // delta cycle
    if ( what.find_first_of("dD") != npos ) {
      static auto last_delta = sc_delta_count() - 1;
      if( last_delta != sc_delta_count() ) {
        result += " : "s + std::to_string( sc_delta_count() );
      }
      last_delta = sc_delta_count();
    }
  }
  // simulator state
  if ( what.find_first_of("sS") != npos ) {
    result += " during "s + Debug::get_simulation_status();
  }
  // verbosity
  if ( what.find_first_of("vV") != npos ) {
    result += " with verbosity level "s + verbosity_str();
  }
  return result;
}

void Debug::read_configuration( args_t& args, string filename ) {
  if( filename.empty() ) {
    // Default filename is executable name with .cfg extension
    filename = sc_argv()[0];
    // Remove directory
    auto pos = filename.find_last_of("/\\:");
    if( pos != npos ) {
      filename.erase(0,pos+1);
    }
    // Replace/add extension
    pos = filename.find_last_of('.', pos);
    if( pos != npos ) filename.erase(pos);
    filename += ".cfg";
  }
  std::ifstream cs{ filename };
  if( not cs ) {
    SC_REPORT_INFO_VERB( msg_type, ( "No default configuration file "s + filename ).c_str(), SC_HIGH );
    return;
  }
  SC_REPORT_INFO_VERB( msg_type, ( "Reading configuration from "s + filename ).c_str(), SC_NONE );
  string line;
  while( std::getline( cs, line ) ) {
    auto pos = line.find_first_of('#');
    if( pos != npos ) line.erase( pos ); // remove comments
    pos = line.find_first_not_of( " \t\n\r" );
    if( pos == npos ) continue; // skip empty line
    if( pos > 0 ) line.erase(0,pos); // remove leading
    pos = line.find_last_not_of( " \t\n\r" );
    if( pos < line.size() - 1 ) line.erase( pos + 1 ); // remove trailing
    std::istringstream is{ line };
    string token;
    while( is >> token ) {
      args.push_back( token );
    }
  }
}

//..............................................................................
void Debug::parse_command_line() {
  auto& args{ config() };
  if( ( sc_argc() == 1 ) or ( ( sc_argc() > 1 ) and ( string{ sc_argv()[1] } != "--no-config" ) ) ) {
    // Read default configuration file if it exists
    read_configuration( args );
  }
  // Add command-line args
  for ( int i = 1; i < sc_argc(); ++i ) {
    args.emplace_back( sc_argv()[i] );
  }
  // Parse args
  auto pos = size_t{};
  for ( auto i = 0u; i < args.size(); ++i ) {
    auto arg = args[i];
    SC_REPORT_INFO_VERB( msg_type, ("Processing "s + arg).c_str(), SC_HIGH );
    //--------------------------------------------------------------------------
    // Handle --help
    //..........................................................................
    if ( arg == "--help"  or arg == "-h" ) {
      auto executable_name = string{ sc_argv()[0] };
      pos = executable_name.find_last_of("/\\:");
      if( pos != npos ) {
        executable_name.erase(0,pos+1);
      }
      auto message = string{syntax};
      replace_all( message, "EXECUTABLE", executable_name );
      REPORT_ALWAYS( message );
      s_stop() = true;
    }
    //--------------------------------------------------------------------------
    // Handle --config
    //..........................................................................
    else if ( arg == "--config"  ) {
      auto config_name = string{};
      if( i+1 < args.size() and args[i+1][0] != '-' ) {
        config_name = args[++i];
      }
      s_parsed("config");
      read_configuration( s_config(), config_name );
    }
    //--------------------------------------------------------------------------
    // Handle -n (no execution)
    //..........................................................................
    else if ( arg == "-n"  ) {
      s_stop() = true; // parse-only
      SC_REPORT_INFO_VERB( msg_type, "Requested stop", SC_NONE );
    }
    //--------------------------------------------------------------------------
    // Handle --nName=COUNT
    //..........................................................................
    else if ( ( arg.substr(0,3) == "--n" )
            and ( (pos=arg.find_first_of('=')) != npos )
            and ( pos > 3 )
            and ( pos + 1 < arg.length() )
            )
    {
      auto name = arg.substr( 2, pos - 2 );
      auto value = arg.substr( pos+1 );
      replace_all( value, "_", "" );
      replace_all( value, "'", "" );
      if ( ( value.find_first_of("0123456789") == 0 )
         and ( value.find_first_not_of("0123456789") == npos )
         )
      {
        s_parsed(name);
        REPORT_STR(value);
        s_count(name) = std::stoul(value);
      }
      else {
        if( s_warn() )
          REPORT_WARNING( "Ignoring incorrectly specified command-line argument "s + arg );
        continue;
      }
    }
    //--------------------------------------------------------------------------
    // Handle --tName=TIME
    //..........................................................................
    else if ( ( arg.substr(0,3) == "--t" )
            and ( (pos=arg.find_first_of('=')) != npos )
            and ( pos > 3 )
            and ( pos + 1 < arg.length() )
            )
    {
      auto name = arg.substr( 2, pos - 2 );
      auto value = lowercase( arg.substr( pos+1 ) );
      replace_all( value, "_", "" );
      replace_all( value, "'", "" );
      if ( value.find_first_of("0123456789") != 0 ) {
        if( s_warn() )
          REPORT_WARNING( "Ignoring incorrectly specified command-line argument "s + arg );
        continue;
      }
      auto magnitude = std::stod(value);
      if ( (pos = value.find_first_not_of(".0123456789")) == npos ) {
        if( s_warn() )
          REPORT_WARNING( "Ignoring incorrectly specified command-line argument "s + arg );
        continue;
      }
      value.erase( 0, pos ); //< remove magnitude
      auto units = SC_NS;
      if      ( value == "s"  ) units = SC_SEC;
      else if ( value == "ms" ) units = SC_MS;
      else if ( value == "us" ) units = SC_US;
      else if ( value == "ns" ) units = SC_NS;
      else if ( value == "ps" ) units = SC_PS;
      else if ( value == "fs" ) units = SC_FS;
      else {
        if( s_warn() ) 
          REPORT_WARNING( "Ignoring incorrectly specified command-line argument "s + arg );
        continue;
      }
      s_parsed(name);
      s_time(name) = sc_time{ magnitude, units };
    }
    //--------------------------------------------------------------------------
    // Handle --sName=TEXT
    //..........................................................................
    else if ( ( arg.substr(0,3) == "--s" )
            and ( (pos=arg.find_first_of('=')) != npos )
            and ( pos > 3 )
            and ( pos + 1 < arg.length() )
            )
    {
      auto name = arg.substr( 2, pos - 2 );
      s_parsed(name);
      s_text(name) = arg.substr(pos+1);
    }
    //--------------------------------------------------------------------------
    // Handle --fName=BOOLEAN
    //..........................................................................
    else if ( ( arg.substr(0,3) == "--f" )
              and ( (pos=arg.find_first_of('=')) != npos )
              and ( pos > 3 )
              and ( pos + 1 < arg.length() )
            )
    {
      auto name = arg.substr( 2, pos - 2 );
      auto value = lowercase( arg.substr( pos+1 ) );
      if ( value == "true" or value == "yes" or value == "on" or value == "1" ) {
        s_parsed(name);
        s_flag(name) = true;
      }
      else if ( value == "false" or value == "no" or value == "off" or value == "0" ) {
        s_parsed(name);
        s_flag(name) = false;
      }
      else {
        REPORT_WARNING( "Ignoring incorrectly specified flag "s + arg + ". Must be one of: false, true, yes, no, 0, 1, on, off"s );
      }
    }
    //--------------------------------------------------------------------------
    // Handle --fName
    //..........................................................................
    else if ( ( arg.substr(0,3) == "--f" )
            and ( arg.length() > 3 )
            )
    {
      auto name = arg.substr( 2 );
      s_parsed(name);
      s_flag(name) = true;
    }
    //--------------------------------------------------------------------------
    // Handle --tName=TIME
    //..........................................................................
    else if ( ( arg.substr(0,3) == "--t" )
            and ( (pos=arg.find_first_of('=')) != npos )
            and ( pos > 3 )
            and ( pos + 1 < arg.length() )
            )
    {
      auto name = arg.substr( 2, pos - 2 );
      auto value = lowercase( arg.substr( pos+1 ) );
      replace_all( value, "_", "" );
      replace_all( value, "'", "" );
      if ( value.find_first_of("0123456789") != 0 ) {
        if( s_warn() )
          REPORT_WARNING( "Ignoring incorrectly specified command-line argument "s + arg );
        continue;
      }
      auto magnitude = std::stod(value);
      if ( (pos = value.find_first_not_of(".0123456789")) == npos ) {
        if( s_warn() )
          REPORT_WARNING( "Ignoring incorrectly specified command-line argument "s + arg );
        continue;
      }
      value.erase( 0, pos ); //< remove magnitude
      auto units = SC_NS;
      if      ( value == "s"  ) units = SC_SEC;
      else if ( value == "ms" ) units = SC_MS;
      else if ( value == "us" ) units = SC_US;
      else if ( value == "ns" ) units = SC_NS;
      else if ( value == "ps" ) units = SC_PS;
      else if ( value == "fs" ) units = SC_FS;
      else {
        if( s_warn() ) 
          REPORT_WARNING( "Ignoring incorrectly specified command-line argument "s + arg );
        continue;
      }
      s_parsed(name);
      s_time(name) = sc_time{ magnitude, units };
    }
    //--------------------------------------------------------------------------
    // Handle --trace
    //..........................................................................
    else if ( arg == "--trace"  ) {
      auto dump_name = "dump"s; // Compatible with https://www.EDAplayground.com
      if( i+1 < args.size() and args[i+1][0] != '-' ) {
        dump_name = args[++i];
        pos = dump_name.find_first_of("/\\:");
        if( pos == npos ) {
          pos = 0;
        }
        pos = dump_name.find_first_of( '.', pos );
        if( pos != npos ) {
          dump_name.erase( pos ); // Remove extension
        }
      }
      s_parsed("trace");
      set_trace_file( dump_name );
    }
    //--------------------------------------------------------------------------
    // Handle --quiet
    //..........................................................................
    else if ( arg == "--quiet" ) {
      s_parsed("quiet");
      set_quiet();
    }
    //--------------------------------------------------------------------------
    // Handle --verbose
    //..........................................................................
    else if ( arg == "--verbose" ) {
      s_parsed("verbose");
      set_verbose();
    }
    //--------------------------------------------------------------------------
    // Handle --no-verbose
    //..........................................................................
    else if ( arg == "--no-verbose" ) {
      s_parsed("no-verbose");
      set_verbose(false);
    }
    //--------------------------------------------------------------------------
    // Handle --debug
    //..........................................................................
    else if ( arg == "--debug" ) {
      auto mask = mask_t{1};
      if( i+1 < args.size() and args[i+1][0] != '-' ) {
        mask = mask_t{args[i+1].c_str()};
      }
      s_parsed("debug");
      set_debugging( mask );
    }
    //--------------------------------------------------------------------------
    // Handle --no-debug
    //..........................................................................
    else if ( arg == "--no-debug" ) {
      s_parsed("no-debug");
      set_debugging(0);
    }
    //--------------------------------------------------------------------------
    // Handle --nName=COUNT
    //..........................................................................
    else if ( ( arg.substr(0,8) == "--expect" )
              and ( (pos=arg.find_first_of('=')) != npos )
              and ( pos > 7 )
              and ( pos + 1 < arg.length() )
            )
    {
      auto expected = arg.substr( pos+1 );
      replace_all( expected, "_", "" );
      replace_all( expected, "'", "" );
      if ( ( expected.find_first_of("0123456789") == 0 )
           and ( expected.find_first_not_of("0123456789") == npos )
              )
      {
        s_parsed("expect");
        REPORT_STR(expected);
        add_expected(SC_ERROR,"",std::stoi(expected));
      }
      else {
        if( s_warn() )
          REPORT_WARNING( "Ignoring incorrectly specified command-line argument "s + arg );
        continue;
      }
    }
    //--------------------------------------------------------------------------
    // Handle --inject
    //..........................................................................
    else if ( arg == "--inject" ) {
      auto mask = mask_t{1};
      if( i+1 < args.size() and args[i+1][0] != '-' ) {
        mask = mask_t{args[i+1].c_str()};
      }
      s_parsed("inject");
      set_injecting( mask );
    }
    //--------------------------------------------------------------------------
    // Handle --no-inject
    //..........................................................................
    else if ( arg == "--no-inject" ) {
      s_parsed("no-inject");
      set_injecting( 0 );
    }
    //--------------------------------------------------------------------------
    // Handle --warn
    //..........................................................................
    else if ( arg == "--warn" ) {
      s_parsed("warn");
      s_warn() = true;
    }
    //--------------------------------------------------------------------------
    // Handle --no-warn
    //..........................................................................
    else if ( arg == "--no-warn" ) {
      s_parsed("no-warn");
      s_warn() = false;
    }
    //--------------------------------------------------------------------------
    // Handle --werror
    //..........................................................................
    else if ( lowercase(arg) == "--werror" ) {
      s_parsed("werror");
      s_werror() = true;
    }
    //--------------------------------------------------------------------------
    // Handle -fName
    //..........................................................................
    else if ( ( arg.substr(0,6) == "--no-f" )
              and ( arg.length() > 6 )
            )
    {
      auto name = arg.substr( 5 );
      s_parsed(name);
      s_flag(name) = false;
    }
    //--------------------------------------------------------------------------
    // Handle unknown options if --warn requested
    //..........................................................................
    else if( s_warn() ) {
      REPORT_WARNING( "Ignoring unknown command-line argument "s + arg );
    }
  }
  //----------------------------------------------------------------------------
  // Abort if --werror requested and warnings encountered
  //............................................................................
  if( s_werror() and sc_report_handler::get_count( SC_WARNING ) != 0 ) {
    REPORT_ERROR( "Please fix all warnings and retry."s );
    s_stop() = true;
  }
}

void   Debug::breakpoint( const string& tag )
{
  SC_REPORT_INFO_VERB( msg_type, tag.c_str(), SC_DEBUG + 1 );
}

void   Debug::resume()
{
  SC_REPORT_INFO_VERB( msg_type, "GDB hint: use finish", SC_DEBUG + 1 );
}

void Debug::stop_if_requested() {
  if( s_stop() ) {
    sc_stop();
  }
}

//..............................................................................
void Debug::set_trace_file( const string& filename ) {
  if( ( not filename.empty() ) and ( filename == s_trace_name() ) ) return;
  if( s_trace_file() != nullptr ) {
    sc_close_vcd_trace_file( s_trace_file() );
    SC_REPORT_INFO_VERB( msg_type,
                         ( "Closed trace file '"s + s_trace_name() + ".vcd'"s ).c_str(),
                         SC_NONE
                       );
  }
  if( filename.length() != 0 ) {
    s_trace_name() = filename;
    s_trace_file() = sc_create_vcd_trace_file( filename.c_str() );
    SC_REPORT_INFO_VERB( msg_type,
                         ( "Tracing to '"s + s_trace_name() + ".vcd'"s ).c_str(),
                         SC_NONE
                       );
  }
}

//..............................................................................
void Debug::set_quiet( bool flag ) {
  s_quiet() = flag;
  if( flag ) {
    sc_report_handler::set_verbosity_level( SC_LOW );
    SC_REPORT_INFO_VERB( msg_type, "Quiet", SC_NONE );
  }
  else if( s_verbose() ) {
    sc_report_handler::set_verbosity_level( SC_HIGH );
    SC_REPORT_INFO_VERB( msg_type, "Normal", SC_NONE );
  }
  else {
    sc_report_handler::set_verbosity_level( SC_MEDIUM );
    SC_REPORT_INFO_VERB( msg_type, "Normal", SC_NONE );
  }
}

//..............................................................................
void Debug::set_verbose( bool flag ) {
  s_verbose() = flag;
  auto current = sc_report_handler::get_verbosity_level();
  if( flag and current < SC_HIGH ) {
    sc_report_handler::set_verbosity_level( SC_HIGH );
    SC_REPORT_INFO_VERB( msg_type, "Verbose", SC_NONE );
  }
  else if ( not flag and not debugging() ) {
    sc_report_handler::set_verbosity_level( SC_MEDIUM );
    SC_REPORT_INFO_VERB( msg_type, "Normal", SC_NONE );
  }
}

//..............................................................................
void Debug::set_debugging( const mask_t& mask ) {
  s_debug() |= mask;
  if( s_debug() != 0 ) {
    sc_report_handler::set_verbosity_level( SC_DEBUG );
    SC_REPORT_INFO_VERB( msg_type, ( "Debugging ENABLED "s + s_debug().to_string(SC_BIN,true) ).c_str(), SC_NONE );
  }
  else {
    s_debug() = 0;
    if( verbose() ) {
      sc_report_handler::set_verbosity_level( SC_HIGH );
      SC_REPORT_INFO_VERB( msg_type, "Debugging disabled", SC_NONE );
    }
    else if( quiet() ) {
      sc_report_handler::set_verbosity_level( SC_LOW );
      SC_REPORT_INFO_VERB( msg_type, "Debugging disabled", SC_NONE );
    }
    else {
      sc_report_handler::set_verbosity_level( SC_MEDIUM );
      SC_REPORT_INFO_VERB( msg_type, "Debugging disabled", SC_NONE );
    }
  }
}
void Debug::clr_debugging( const mask_t& mask ) {
    if( mask == 0 ) {
      s_debug() = 0;
    }
    else {
      s_debug() &= ~mask;
    }
    set_debugging( 0 );
}

//..............................................................................
void Debug::set_injecting( const mask_t& mask ) {
  s_inject() = mask;
  if( mask != 0 ) {
    SC_REPORT_INFO_VERB( msg_type,
                         ( "Injecting  ENABLED "s + mask.to_string(SC_BIN,true) ).c_str(),
                         SC_NONE
    );
  }
  else {
    SC_REPORT_INFO_VERB( msg_type, "Injecting  DISABLED", SC_NONE );
  }
}

//..............................................................................
void Debug::set_count( const string& name, size_t count ) {
  s_count(name) = count;
  SC_REPORT_INFO_VERB( msg_type,
                       ( name + " = "s + std::to_string(count) ).c_str(),
                       SC_NONE
  );
}

//..............................................................................
void Debug::set_time( const string& name, const sc_time& time ) {
  s_time(name) = time;
  SC_REPORT_INFO_VERB( msg_type,
                       ( name + " = "s + time.to_string() ).c_str(),
                       SC_NONE
  );
}

//..............................................................................
void Debug::set_flag( const string& name, bool flag ) {
  s_flag(name) = flag;
  SC_REPORT_INFO_VERB( msg_type,
                       ( name + " = "s + (flag?"true":"false") ).c_str(),
                       SC_NONE
  );
}

//..............................................................................
void Debug::help()
{
      REPORT_ALWAYS( syntax);
}

//..............................................................................
void Debug::info( const char* what )
{
  SC_REPORT_INFO_VERB( msg_type,
                       ( COLOR_INFO
                         + get_simulation_info(nullptr, what)
                         + COLOR_NONE
                       ).c_str(),
                       SC_NONE
  );
}

const char* Debug::process() { //< return hierarchical process name
  auto h = sc_get_current_process_handle();
  return h.name();
}

//..............................................................................
void Debug::add_expected( sc_severity severity, const string& msg_type_, ssize_t n )
{
  sc_assert( severity > SC_INFO and severity < max_severity );
  auto key = std::pair<sc_severity, string>{severity,msg_type_};
  if ( s_expected_map().count(key) == 0 ) {
    s_expected_map()[key] = n;
  }
  else {
    s_expected_map()[key] += n;
  }
}

//..............................................................................
void Debug::see_expected( sc_severity severity, const std::string& msg_type_ )
{
  auto key = std::pair<sc_severity, string>{severity,msg_type_};
  if ( s_expected_map().count(key) == 0 ) {
    // Allow for anonymous expectations
    key.second = "";
    if ( s_expected_map().count(key) == 0 ) return;
  }
  ++s_observed_map()[key];
}

//..............................................................................
ssize_t Debug::get_expected( sc_severity severity )
{
  auto count = ssize_t{};
  for( auto [key,n] : s_expected_map() ) {
    if ( severity == max_severity or key.first == severity ) {
      count += n;
    }
  }
  return count;
}

//..............................................................................
ssize_t Debug::get_observed( sc_severity severity )
{
  auto count = ssize_t{};
  for( auto [key,n] : s_observed_map() ) {
    if ( severity == max_severity or key.first == severity ) {
      count += n;
    }
  }
  return count;
}

//..............................................................................
string Debug::get_opts( const string& prefix )
{
  auto result = prefix;
  result += "Command-line: "s + command_options() + "\n"s;
  result += prefix + "\nRun-time options\n----------------\n"s;
  if( not s_count_map().empty() ) {
    result += prefix + "  Counts:\n"s;
    for( auto [k,v]: s_count_map() ) {
      result += prefix;
      result += "    --"s + k + " = "s + std::to_string(v) + "\n"s;
    }
  }
  if( not s_time_map().empty() ) {
    result += prefix;
    result += "  Times:\n"s;
    for( auto [k,v]: s_time_map() ) {
      result += prefix;
      result += "    --"s + k + " = "s + v.to_string() + "\n"s;
    }
  }
  if( not s_flag_map().empty() ) {
    result += prefix;
    result += "  Flags:\n"s;
    for( auto [k,v]: s_flag_map() ) {
      result += prefix;
      result += "    --"s + k + " = "s + (v?"true"s:"false"s) + "\n"s;
    }
  }
  if( not s_text_map().empty() ) {
    result += prefix;
    result += "  Texts:\n"s;
    for( auto [k,v]: s_text_map() ) {
      result += "    --"s + k;
      result += " = '"s + v + "'\n"s;
    }
  }
  if( not s_value_map().empty() ) {
    result += prefix;
    result += "  Doubles:\n"s;
    for( auto [k,v]: s_value_map() ) {
      result += prefix;
      result += "    --"s + k + " = "s + std::to_string(v) + "\n"s;
    }
  }
  return result;
}

//..............................................................................
void Debug::opts()
{
  auto result = COLOR_DEBUG + "\n"s;
  result += get_opts();
  result += COLOR_NONE;
  SC_REPORT_INFO_VERB( msg_type, result.c_str(), SC_NONE );
}

//..............................................................................
size_t Debug::parsed( const string& name )
{
  return s_parsed_map().count(name);
}

//..............................................................................
const char* Debug::name(const sc_object* m)
{
  return m->name();
}

void Debug::show(const string& s)
{
  SC_REPORT_INFO_VERB( msg_type, s.c_str(), SC_NONE );
}

//..............................................................................
string Debug::command_options() // returns command-line options including config
{
  auto result = string{};
  for( const auto& elt : s_config() ) {
    result += elt + ' ';
  }
  if( not result.empty() ) { // remove trailing space
    result.erase( result.size() - 1 );
  }
  return result;
}

//..............................................................................
string Debug::get_simulation_status()
{
  auto status = sc_get_status();
  switch( status ) {
    case SC_UNITIALIZED               : return "SC_UNINITIALIZED"; //< typo in PoC
    case SC_ELABORATION               : return "SC_ELABORATION";
    case SC_BEFORE_END_OF_ELABORATION : return "SC_BEFORE_END_OF_ELABORATION";
    case SC_END_OF_ELABORATION        : return "SC_END_OF_ELABORATION";
    case SC_END_OF_INITIALIZATION     : return "SC_END_OF_INITIALIZATION";
    case SC_START_OF_SIMULATION       : return "SC_START_OF_SIMULATION";
    case SC_BEFORE_TIMESTEP           : return "SC_BEFORE_TIMESTEP";
    case SC_END_OF_UPDATE             : return "SC_END_OF_UPDATE";
    case SC_STATUS_ANY                : return "SC_STATUS_ANY";
    case SC_RUNNING                   : return "SC_RUNNING";
    case SC_PAUSED                    : return "SC_PAUSED";
    case SC_STOPPED                   : return "SC_STOPPED";
    case SC_END_OF_SIMULATION         : return "SC_END_OF_SIMULATION";
    default                           : return "*** UNKNOWN STATUS "s + std::to_string(status) + " ***";
  }
}

//..............................................................................
string Debug::severity_str( sc_severity severity )
{
  static const std::array<std::string, 4> severities =
    { "information", "warning", "error", "fatal" };
  return severities[severity];
}

//..............................................................................
string Debug::verbosity_str( int level )
{
  auto result = string{};
  if      ( level >= SC_DEBUG  ) { result = "SC_DEBUG"s  + verbosity_offset( SC_DEBUG  ); }
  else if ( level >= SC_FULL   ) { result = "SC_FULL"s   + verbosity_offset( SC_FULL   ); }
  else if ( level >= SC_HIGH   ) { result = "SC_HIGH"s   + verbosity_offset( SC_HIGH   ); }
  else if ( level >= SC_MEDIUM ) { result = "SC_MEDIUM"s + verbosity_offset( SC_MEDIUM ); }
  else if ( level >= SC_LOW    ) { result = "SC_LOW"s    + verbosity_offset( SC_LOW    ); }
  else                           { result = "SC_NONE"s   + verbosity_offset( SC_NONE   ); }
  return result;
}

string Debug::verbosity_str()
{
  return Debug::verbosity_str( sc_report_handler::get_verbosity_level() );
}

int Debug::exit_status( const string& project )
{
  auto message  = "\n"s
      + Debug::get_opts("")
      + "\n"s
      + "Simulation results\n"s
      + "------------------\n"s
      ;

  auto expected_total = get_expected( max_severity );
  auto surprise_total = ssize_t{0};
  auto observed_total = get_observed( max_severity );
  auto severity_color = std::array<string,max_severity>{ COLOR_INFO, COLOR_WARN, COLOR_ERROR, COLOR_FATAL };
  auto severity_count = std::array<ssize_t,max_severity>{};
  for( auto severity = SC_INFO; severity < max_severity; severity = static_cast<sc_severity>(severity + 1) ) {
    severity_count[severity] = ssize_t{sc_report_handler::get_count( severity )};
    auto& the_count{severity_count[severity]};

    // Deal with expected messages if any
    if ( expected_total > 0 ) {
      if ( observed_total == 0 ) {
        auto expected_count = get_expected(severity);
        if ( the_count < expected_count ) {
          surprise_total += expected_count - the_count;
          auto missed_message = "Missed "s;
          missed_message += std::to_string( expected_count - the_count );
          missed_message += " expected ";
          missed_message += severity_str(severity);
          missed_message += " reports.";
          REPORT_ERROR( missed_message );
          the_count = 0;
          ++severity_count[SC_ERROR];
        }
        else /* the_count >= expected_count */ {
          the_count -= expected_count;
        }
      } 
      else /* detailed observations */ {
        for( auto [expected_key,expected_count] : s_expected_map() ) {
          if ( expected_key.first != severity ) continue;
          for( auto [observed_key,observed_count] : s_observed_map() ) {
            if( observed_key == expected_key ) {
              if( observed_count >= expected_count ) {
                // Expectations met
                the_count -= expected_count;
              }
              else {
                surprise_total += expected_count - observed_count;
                auto where = expected_key.second.empty() ? ""s : (" from "s + expected_key.second);
                auto missed_message = "Missed "s;
                missed_message += std::to_string( expected_count - observed_count );
                missed_message += " expected ";
                missed_message += severity_str(severity);
                missed_message += " reports.";
                missed_message += where;
                REPORT_ERROR( missed_message );
                ++severity_count[SC_ERROR];
                the_count -= observed_count;
              }
            }
          }
        }//end for
      }
    }//end if expected

    // Add the severity count information to status
    if( severity_count[severity] > 0 ) {
      message += severity_color[severity]
        + "  "s + std::to_string( severity_count[severity] ) + " "s + severity_str(severity) + " messages\n"s
        + COLOR_NONE
        ;
    }
  }//end for severity

  if ( expected_total > 0 ) {
    message += "  Expected "s + std::to_string( expected_total ) + " problems\n"s;
  }
  if ( (expected_total - surprise_total) > 0 ) {
    message += "  Observed "s + std::to_string( expected_total - surprise_total ) + " expected problems\n"s;
  }
  if ( surprise_total > 0 ) {
    message += "  Surprised by "s + std::to_string( surprise_total ) + " missed expectations\n"s;
  }

  auto ok =  (severity_count[SC_ERROR] + severity_count[SC_FATAL]) == 0 ;
  if( ok ) {
    message += COLOR_GREEN + COLOR_BOLD
      + "\nNo major problems - Simulation PASSED."s
      + COLOR_NONE
      ;
  }
  else {
    message += COLOR_FATAL
      + "\nSimulation FAILED."s
      + COLOR_NONE
      ;
  }
  SC_REPORT_INFO_VERB( project.c_str(), message.c_str(), sc_core::SC_NONE );

  breakpoint( project );

  return ok?0:1;
}

//------------------------------------------------------------------------------
// Internal methods
//..............................................................................
// By C++ rules, static variable initialization inside functions is thread-safe.

Debug::mask_t& Debug::s_debug()
{
  static mask_t debug{0};
  return debug;
}

Debug::mask_t& Debug::s_inject() {
  static mask_t inject{0};
  return inject;
}

sc_trace_file*& Debug::s_trace_file() {
  static sc_trace_file* trace_file{nullptr};
  return trace_file;
}

string& Debug::s_trace_name() {
  static string trace_name{};
  return trace_name;
}

Debug::args_t& Debug::s_config() {
  static args_t config;
  return config;
}

bool& Debug::s_stop() {
  static bool stop_requested{false};
  return stop_requested;
}

bool& Debug::s_quiet() {
  static bool quiet{false};
  return quiet;
}

bool& Debug::s_verbose() {
  static bool verbose{false};
  return verbose;
}

bool& Debug::s_warn() {
  static bool warn{false};
  return warn;
}

bool& Debug::s_werror() {
  static bool werror{false};
  return werror;
}

// Maps
std::map<string,size_t>& Debug::s_count_map() {
  static std::map<string,size_t>  the_map;
  return the_map;
}
std::map<string,sc_time>& Debug::s_time_map() {
  static std::map<string,sc_time> the_map;
  return the_map;
}
std::map<string,bool>& Debug::s_flag_map() {
  static std::map<string,bool> the_map;
  return the_map;
}
std::map<string,string>& Debug::s_text_map() {
  static std::map<string,string> the_map;
  return the_map;
}
std::map<string,double>& Debug::s_value_map() {
  static std::map<string,double> the_map;
  return the_map;
}
std::map<string,size_t>& Debug::s_parsed_map() {
  static std::map<string,size_t> the_map;
  return the_map;
}
std::map<Debug::severity_n_type,ssize_t>& Debug::s_expected_map() {
  static std::map<severity_n_type,ssize_t> the_map;
  return the_map;
}
std::map<Debug::severity_n_type,ssize_t>& Debug::s_observed_map() {
  static std::map<severity_n_type,ssize_t> the_map;
  return the_map;
}

// Options
size_t& Debug::s_count(const string& name, bool modify) {
  if( s_count_map().count(name) == 0 and not modify and s_warn() ) REPORT_WARNING( "No count named: "s + name );
  if( s_count_map().count(name) == 0 and modify ) s_count_map()[name] = size_t{}; // default value
  return s_count_map()[name];
}
sc_time& Debug::s_time(const string& name, bool modify) {
  if( s_time_map().count(name) == 0 and not modify and s_warn() ) REPORT_WARNING( "No time named: "s + name );
  if( s_time_map().count(name) == 0 and modify ) s_time_map()[name] = sc_time{}; // default value
  return s_time_map()[name];
}
bool& Debug::s_flag(const string& name, bool modify) {
  if( s_flag_map().count(name) == 0 and not modify and s_warn() ) REPORT_WARNING( "No such flag -"s + name );
  if( s_flag_map().count(name) == 0 and modify ) s_flag_map()[name] = false; // default value
  return s_flag_map()[name];
}
string& Debug::s_text(const string& name, bool modify) {
  if( s_text_map().count(name) == 0 and not modify and s_warn() ) REPORT_WARNING( "No such text -"s + name );
  if( s_text_map().count(name) == 0 and modify ) s_text_map()[name] = ""; // default value
  return s_text_map()[name];
}
double& Debug::s_value(const string& name, bool modify) {
  if( s_value_map().count(name) == 0 and not modify and s_warn() ) REPORT_WARNING( "No such double value -"s + name );
  if( s_value_map().count(name) == 0 and modify ) s_value_map()[name] = 0.0; // default value
  return s_value_map()[name];
}
void Debug::s_parsed(const string& name) {
  if( s_parsed_map().count(name) == 0 )
    s_parsed_map()[name] = 1;
  else
    ++s_parsed_map()[name];
}

// vim:nospell
