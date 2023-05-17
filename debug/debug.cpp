#include "debug.hpp"
#include <fstream>
#include <sstream>
using namespace sc_core;
using namespace sc_dt;
using namespace std::literals;
using namespace std;

namespace {
  const char* SYNTAX = R"(
Synopsis
--------

EXECUTABLE [--no-config] [OPTIONS]
EXECUTABLE --help

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

Examples
--------

EXECUTABLE -nreps=1'000
EXECUTABLE -nreps=20 --trace --debug --inject

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
}

//------------------------------------------------------------------------------
// Member methods
//..............................................................................
// Constructor
Debug::Debug( const std::string& s )
 : m_context{std::move(s)}
{}

const char* Debug::context( const std::string& s ) {
  if( not s.empty() ) m_context = s;
  return m_context.c_str();
}

void Debug::mark( const std::string& s, const std::string& func, sc_object* obj, const std::string& what  ) {
  SC_REPORT_INFO_VERB( context(), text( s + func + " "s + get_simulation_info( obj, what ) ), message_level );
}

void Debug::executed( const std::string& func, sc_object* obj, const std::string& what  )
   { mark( "Executed "s, func, obj, what ); }

void Debug::entering( const std::string& func, sc_object* obj, const std::string& what )
   { mark( "Entering "s, func, obj, what ); }

void Debug::yielding( const std::string& func, sc_object* obj, const std::string& what )
   { mark( "Yielding "s, func, obj, what ); }

void Debug::resuming( const std::string& func, sc_object* obj, const std::string& what )
   { mark( "Resuming "s, func, obj, what ); }

void Debug::leaving( const std::string& func, sc_object* obj, const std::string& what )
   { mark( "Leaving "s, func, obj, what ); }


//------------------------------------------------------------------------------
// Static methods
//..............................................................................
// Convert a string to const char*
const char* Debug::text( const std::string& s ) {
  return s.c_str();
}

std::string Debug::get_simulation_info( sc_object* obj, const std::string& what )
{
  auto result = ""s;
  // instance
  if ( what.find_first_of("iI") != npos and obj != nullptr ) { 
    result +=obj->name() + " "s;
  };
  // time
  if ( what.find_first_of("tT") != npos  ) {
    result += "at "s + sc_time_stamp().to_string();
  }
  // delta cycle
  if ( what.find_first_of("dD") != npos ) {
    static auto last_delta = sc_delta_count() - 1;
    if( last_delta != sc_delta_count() ) {
      result += ":"s + std::to_string( sc_delta_count() );
    }
    result += " "s;
    last_delta = sc_delta_count();
  }
  // simulator state
  if ( what.find_first_of("sS") != npos ) {
    result += "during "s;
    auto status = sc_get_status();
    switch( status ) {
      case SC_UNITIALIZED /*PoC typo*/  : result += "SC_UNINITIALIZED";  break;
      case SC_ELABORATION               : result += "SC_ELABORATION"; break;
      case SC_BEFORE_END_OF_ELABORATION : result += "SC_BEFORE_END_OF_ELABORATION"; break;
      case SC_END_OF_ELABORATION        : result += "SC_END_OF_ELABORATION"; break;
      case SC_END_OF_INITIALIZATION     : result += "SC_END_OF_INITIALIZATION"; break;
      case SC_START_OF_SIMULATION       : result += "SC_START_OF_SIMULATION"; break;
      case SC_BEFORE_TIMESTEP           : result += "SC_BEFORE_TIMESTEP"; break;
      case SC_END_OF_UPDATE             : result += "SC_END_OF_UPDATE"; break;
      case SC_STATUS_ANY                : result += "SC_STATUS_ANY"; break;
      case SC_RUNNING                   : result += "SC_RUNNING"; break;
      case SC_PAUSED                    : result += "SC_PAUSED"; break;
      case SC_STOPPED                   : result += "SC_STOPPED"; break;
      case SC_END_OF_SIMULATION         : result += "SC_END_OF_SIMULATION"; break;
      default                           : result += std::string{"*** UNKNOWN STATUS "} + std::to_string( status ) + " ***"; break;
    }
  }
  // verbosity
  if ( what.find_first_of("vV") != npos ) {
    result += " with verbosity level "s + get_verbosity();
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
    SC_REPORT_INFO_VERB( mesgType, ( string{"No default configuration file "} + filename ).c_str(), SC_HIGH );
    return;
  }
  SC_REPORT_INFO( mesgType, ( string{"Reading configuration from "} + filename ).c_str() );
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
    SC_REPORT_INFO_VERB( mesgType, (string{"Processing "} + arg).c_str(), SC_HIGH );
    //--------------------------------------------------------------------------
    if ( arg == "--help"  or arg == "-h" ) {
      auto executable_name = string{ sc_argv()[0] };
      pos = executable_name.find_last_of("/\\:");
      if( pos != npos ) {
        executable_name.erase(0,pos+1);
      }
      auto syntax = string{SYNTAX};
      replace_all( syntax, "EXECUTABLE", executable_name );
      SC_REPORT_INFO_VERB( mesgType, syntax.c_str(), SC_NONE );
      s_stop() = true;
    }
    //--------------------------------------------------------------------------
    else if ( arg == "--config"  ) {
      auto config_name = string{};
      if( i+1 < args.size() and args[i+1][0] != '-' ) {
        config_name = args[++i];
      }
      read_configuration( s_config(), config_name );
    }
    //--------------------------------------------------------------------------
    else if ( arg == "-n"  ) {
      s_stop() = true; // parse-only
      SC_REPORT_INFO_VERB( mesgType, "Requested stop", SC_NONE );
    }
    //--------------------------------------------------------------------------
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
        s_count(name) = std::stoul(value);
      }
      else {
        if( s_warn() )
          SC_REPORT_WARNING( mesgType, ( "Ignoring incorrectly specified command-line argument " + arg ).c_str() );
        continue;
      }
    }
    //--------------------------------------------------------------------------
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
          SC_REPORT_WARNING( mesgType, ( "Ignoring incorrectly specified command-line argument " + arg ).c_str() );
        continue;
      }
      auto magnitude = std::stod(value);
      if ( (pos = value.find_first_not_of(".0123456789")) == npos ) {
        if( s_warn() )
          SC_REPORT_WARNING( mesgType, ( "Ignoring incorrectly specified command-line argument " + arg ).c_str() );
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
          SC_REPORT_WARNING( mesgType, ( "Ignoring incorrectly specified command-line argument " + arg ).c_str() );
        continue;
      }
      s_time(name) = sc_time{ magnitude, units };
    }
    //--------------------------------------------------------------------------
    else if ( arg == "--trace"  ) {
      auto dump_name = string{"dump"}; // Compatible with https://www.EDAplayground.com
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
      set_trace_file( dump_name );
    }
    //--------------------------------------------------------------------------
    else if ( arg == "--quiet" ) {
      set_quiet();
    }
    //--------------------------------------------------------------------------
    else if ( arg == "--verbose" ) {
      set_verbose();
    }
    //--------------------------------------------------------------------------
    else if ( arg == "--no-verbose" ) {
      set_verbose(false);
    }
    //--------------------------------------------------------------------------
    else if ( arg == "--debug" ) {
      auto mask = mask_t{1};
      if( i+1 < args.size() and args[i+1][0] != '-' ) {
        mask = mask_t{args[i+1].c_str()};
      }
      set_debugging( mask );
    }
    //--------------------------------------------------------------------------
    else if ( arg == "--no-debug" ) {
      set_debugging(0);
    }
    //--------------------------------------------------------------------------
    else if ( arg == "--inject" ) {
      auto mask = mask_t{1};
      if( i+1 < args.size() and args[i+1][0] != '-' ) {
        mask = mask_t{args[i+1].c_str()};
      }
      set_injecting( mask );
    }
    //--------------------------------------------------------------------------
    else if ( arg == "--no-inject" ) {
      set_injecting( 0 );
    }
    //--------------------------------------------------------------------------
    else if ( arg == "--warn" ) {
      s_warn() = true;
    }
    //--------------------------------------------------------------------------
    else if ( arg == "--no-warn" ) {
      s_warn() = false;
    }
    //--------------------------------------------------------------------------
    else if ( lowercase(arg) == "--werror" ) {
      s_werror() = true;
    }
    //--------------------------------------------------------------------------
    else if( s_warn() ) {
      SC_REPORT_WARNING( mesgType, ( "Ignoring unknown command-line argument " + arg ).c_str() );
    }
  }
  if( s_werror() and sc_report_handler::get_count( sc_core::SC_WARNING ) != 0 ) {
    SC_REPORT_ERROR( mesgType, "Please fix all warnings and retry." );
    s_stop() = true;
  }
}

void Debug::stop_if_requested() {
  if( s_stop() ) {
    sc_set_stop_mode(SC_STOP_IMMEDIATE);
    sc_stop();
  }
}

//..............................................................................
void Debug::set_trace_file( const string& filename ) {
  if( ( not filename.empty() ) and ( filename == s_trace_name() ) ) return;
  if( s_trace_file() != nullptr ) {
    sc_close_vcd_trace_file( s_trace_file() );
    SC_REPORT_INFO_VERB( mesgType,
                         ( string{"Closed trace file '"}
                           + s_trace_name() + string{".vcd'"} 
                         ).c_str(),
                         SC_NONE
                       );
  }
  if( filename.length() != 0 ) {
    s_trace_name() = filename;
    s_trace_file() = sc_create_vcd_trace_file( filename.c_str() );
    SC_REPORT_INFO_VERB( mesgType,
                         ( string{"Tracing to '"}
                           + s_trace_name() + string{".vcd'"} 
                         ).c_str(),
                         SC_NONE
                       );
  }
}

//..............................................................................
void Debug::set_quiet( bool flag ) {
  s_quiet() = flag;
  if( flag ) {
    sc_report_handler::set_verbosity_level( SC_LOW );
    SC_REPORT_INFO_VERB( mesgType, "Quiet", SC_NONE );
  }
  else if( s_verbose() ) {
    sc_report_handler::set_verbosity_level( SC_HIGH );
    SC_REPORT_INFO_VERB( mesgType, "Normal", SC_NONE );
  }
  else {
    sc_report_handler::set_verbosity_level( SC_MEDIUM );
    SC_REPORT_INFO_VERB( mesgType, "Normal", SC_NONE );
  }
}

//..............................................................................
void Debug::set_verbose( bool flag ) {
  s_verbose() = flag;
  auto current = sc_report_handler::get_verbosity_level();
  if( flag and current < SC_HIGH ) {
    sc_report_handler::set_verbosity_level( SC_HIGH );
    SC_REPORT_INFO_VERB( mesgType, "Verbose", SC_NONE );
  }
  else if ( not flag and not debugging() ) {
    sc_report_handler::set_verbosity_level( SC_MEDIUM );
    SC_REPORT_INFO_VERB( mesgType, "Normal", SC_NONE );
  }
}

//..............................................................................
void Debug::set_debugging( const mask_t& mask ) {
  s_debug() |= mask;
  if( s_debug() != 0 ) {
    sc_report_handler::set_verbosity_level( SC_DEBUG );
    SC_REPORT_INFO_VERB( mesgType, ( string{"Debugging ENABLED "} + s_debug().to_string(SC_BIN,true) ).c_str(), SC_NONE );
  }
  else {
    s_debug() = 0;
    if( verbose() ) {
      sc_report_handler::set_verbosity_level( SC_HIGH );
      SC_REPORT_INFO_VERB( mesgType, "Debugging disabled", SC_NONE );
    }
    else if( quiet() ) {
      sc_report_handler::set_verbosity_level( SC_LOW );
      SC_REPORT_INFO_VERB( mesgType, "Debugging disabled", SC_NONE );
    }
    else {
      sc_report_handler::set_verbosity_level( SC_MEDIUM );
      SC_REPORT_INFO_VERB( mesgType, "Debugging disabled", SC_NONE );
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
    SC_REPORT_INFO_VERB( mesgType,
                         ( string{"Injecting  ENABLED "}
                           + mask.to_string(SC_BIN,true)
                         ).c_str(),
                         SC_NONE
    );
  }
  else {
    SC_REPORT_INFO_VERB( mesgType, "Injecting  DISABLED", SC_NONE );
  }
}

//..............................................................................
void Debug::set_count( const string& name, size_t count ) {
  s_count(name) = count;
  SC_REPORT_INFO_VERB( mesgType,
                       ( name + string{" = "}
                         + std::to_string(count)
                       ).c_str(),
                       SC_NONE
  );
}

//..............................................................................
void Debug::set_time( const string& name, const sc_time& time ) {
  s_time(name) = time;
  SC_REPORT_INFO_VERB( mesgType,
                       ( name + string{" = "}
                         + time.to_string()
                       ).c_str(),
                       SC_NONE
  );
}

//..............................................................................
void Debug::set_flag( const string& name, bool flag ) {
  s_flag(name) = flag;
  SC_REPORT_INFO_VERB( mesgType,
                       ( name + string{" = "}
                         + (flag?"true":"false")
                       ).c_str(),
                       SC_NONE
  );
}

//..............................................................................
void Debug::info( cstr_t what )
{
  SC_REPORT_INFO_VERB( mesgType,
                       ( yellow
                         + get_simulation_info(nullptr, what)
                         + none
                       ).c_str(),
                       SC_NONE
  );
}

//..............................................................................
void Debug::opts()
{
  auto result = std::string{magenta} + "\nOptions\n"s;
  if( not s_count_map().empty() ) {
    result += "  Counts:\n"s;
    for( auto [k,v]: s_count_map() ) {
      result += "    "s + k + " = "s + std::to_string(v) + "\n"s;
    }
  }
  if( not s_time_map().empty() ) {
    result += "  Times:\n"s;
    for( auto [k,v]: s_time_map() ) {
      result += "    "s + k + " = "s + v.to_string() + "\n"s;
    }
  }
  if( not s_flag_map().empty() ) {
    result += "  Flags:\n"s;
    for( auto [k,v]: s_flag_map() ) {
      result += "    "s + k + " = "s + (v?"true"s:"false"s) + "\n"s;
    }
  }
  result += none;
  SC_REPORT_INFO_VERB( mesgType, result.c_str(), SC_NONE );
}

//..............................................................................
void Debug::name(const sc_object* m)
{
  SC_REPORT_INFO_VERB( mesgType, m->name(), SC_NONE );
}

void Debug::show(const string& s)
{
  SC_REPORT_INFO_VERB( mesgType, s.c_str(), SC_NONE );
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
    case SC_UNITIALIZED               : return "SC_UNITIALIZED"; //< typo in PoC
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
    default                           : return string{"*** UNKNOWN STATUS "} + std::to_string(status) + " ***";
  }
}

//..............................................................................
string Debug::get_verbosity()
{
  auto result = string{};
  auto level = sc_report_handler::get_verbosity_level();
  if      ( level >= SC_DEBUG  ) { result = "DEBUG"s  + verbosity_offset( SC_DEBUG  ); }
  else if ( level >= SC_FULL   ) { result = "FULL"s   + verbosity_offset( SC_FULL   ); }
  else if ( level >= SC_HIGH   ) { result = "HIGH"s   + verbosity_offset( SC_HIGH   ); }
  else if ( level >= SC_MEDIUM ) { result = "MEDIUM"s + verbosity_offset( SC_MEDIUM ); }
  else if ( level >= SC_LOW    ) { result = "LOW"s    + verbosity_offset( SC_LOW    ); }
  else                           { result = "NONE"s   + verbosity_offset( SC_NONE   ); }
  return result;
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

size_t& Debug::s_count(const string& name, bool modify) {
  if( s_count_map().count(name) == 0 and not modify and s_warn() ) SC_REPORT_WARNING( mesgType, (string{"No count named: "} + name).c_str() );
  if( s_count_map().count(name) == 0 and modify ) s_count_map()[name] = size_t{};
  return s_count_map()[name];
}
sc_time& Debug::s_time(const string& name, bool modify) {
  if( s_time_map().count(name) == 0 and not modify and s_warn() ) SC_REPORT_WARNING( mesgType, (string{"No time named: "} + name).c_str() );
  if( s_time_map().count(name) == 0 and modify ) {
    s_time_map()[name] = sc_time{};
  }
  return s_time_map()[name];
}
bool& Debug::s_flag(const string& name, bool modify) {
  if( s_flag_map().count(name) == 0 and not modify and s_warn() ) SC_REPORT_WARNING( mesgType, (string{"No such flag -"} + name).c_str() );
  if( s_flag_map().count(name) == 0 and modify ) s_flag_map()[name] = false;
  return s_flag_map()[name];
}

//TAF!
