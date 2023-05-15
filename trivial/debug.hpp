#pragma once

/** @brief Simple debug support
 *
 *  Features:
 *
 *  0. Parse options
 *  1. Turn up/down verbosity
 *  2. Turn on/off tracing
 *  3. Turn on/off injection of bugs
 *
 *  These may be optionally specified at run-time via
 *  a configuration file or via command-line options
 *  determined at run-time. The configuration file should
 *  be located next to the executable with a filename extension
 *  of ".cfg"
 *
 *  Internal to the code, the program may detect:
 *  1. Debug verbosity by calling Debug::debugging()
 *  2. Trace status by calling Debug::tracing()
 *  3. Injection status by calling Debug::injecting()
 *
 *  Each module should:
 *  1. Use SC_REPORT_INFO_VERB( ..., SC_DEBUG ) strategically
 *  2. Add tracing conditionally at start_of_simulation
 *  3. Conditionally inject faults to test error detection code
 *
 *  Optionally, you may programmatically choose to:
 *  1. Change debug status with set_debugging( true/false )
 *     -or- simply change SystemC's verbosity_level
 *  2. Rarely change or close the trace_file with set_trace_file()
 *  3. Rarely turn injection on/off with set_injecting( true/false )
 */

#include <systemc>
#include <string>
#include <vector>

#ifndef NOCOLOR
  #define COLOR_STR(cstr) cstr
#else
  #define COLOR_STR(cstr) ""
#endif

#define REPORT_WARNING(message) SC_REPORT_WARNING( mesgType, (message).c_str() )
#define REPORT_ERROR(message) SC_REPORT_ERROR( mesgType, (message).c_str() )
#define REPORT_FATAL(message) SC_REPORT_FATAL( mesgType, (message).c_str() )
#define REPORT_VERB(message,verbosity) SC_REPORT_INFO_VERB( mesgType, (message).c_str(), sc_core::verbosity )
#define REPORT_ALWAYS(message) SC_REPORT_INFO_VERB( mesgType, (message).c_str(), sc_core::SC_NONE )
#define REPORT_DEBUG(message) SC_REPORT_INFO_VERB( mesgType,\
  ( message + "\nFile:"s + std::string{__FILE__} + "Line:"s + std::to_string(__LINE__) + " at "s + sc_core::sc_time_stamp().to_string() ).c_str(),\
  sc_core::SC_DEBUG )

struct Debug { //< not using namespace due to future considerations
  using string = std::string;
  using args_t = std::vector<string>;
  using mask_t = sc_dt::sc_bv<64>;
  using sc_trace_file = sc_core::sc_trace_file;
  using sc_time = sc_core::sc_time;
  static constexpr const char* mesgType = "/Doulos/Debug";
  inline static sc_trace_file* trace_file()                          { return s_trace_file(); }
  inline static           bool tracing()                             { return s_trace_file() != nullptr; }
  inline static           bool debugging( const mask_t& mask = ~0u ) { return (s_debug() & mask) != 0u; }
  inline static           bool injecting( const mask_t& mask = ~0u ) { return (s_inject() & mask) != 0u; }
  inline static           bool stopping()                            { return s_stop(); }
  inline static           bool verbose()                             { return s_verbose(); }
  inline static           bool quiet()                               { return s_quiet(); }
  inline static        args_t& config()                              { return s_config(); }
  inline static         size_t count(const string& name)             { return s_count(name, false); }
  inline static        sc_time time(const string& name)              { return s_time(name, false); }
  inline static           bool flag(const string& name)              { return s_flag(name, false); }
  inline static           void close_trace_file()                    { Debug::set_trace_file(""); }

  static void read_configuration( args_t& args, string filename = "" );
  static void parse_command_line();
  static void breakpoint() {}
  static void stop_if_requested();
  static void set_trace_file( const string& filename );
  static void set_quiet( bool flag = true );
  static void set_verbose( bool flag = true );
  static void set_debugging( const mask_t& mask = 1 ); // 0 => no-change
  static void clr_debugging( const mask_t& mask = 0 ); // 0 => all cleared
  static void set_injecting( const mask_t& mask = 1 );
  static void set_count( const string& name, size_t count = 1 );
  static void set_time( const string& name, const sc_time& time = sc_core::SC_ZERO_TIME );
  static void set_flag( const string& name, bool flag = true );
  static void status(); // Display information about situation
  static void name(const sc_core::sc_object* m); // Display information about the object
  static string get_status();
  static string get_verbosity();
  static string command_options(); // returns command-line options including config

  static constexpr const char* none    = COLOR_STR( "\033[0m"  );
  static constexpr const char* bold    = COLOR_STR( "\033[1m"  );
  static constexpr const char* black   = COLOR_STR( "\033[90m" );
  static constexpr const char* red     = COLOR_STR( "\033[91m" );
  static constexpr const char* green   = COLOR_STR( "\033[92m" );
  static constexpr const char* yellow  = COLOR_STR( "\033[93m" );
  static constexpr const char* blue    = COLOR_STR( "\033[94m" );
  static constexpr const char* magenta = COLOR_STR( "\033[95m" );
  static constexpr const char* cyan    = COLOR_STR( "\033[96m" );
  static constexpr const char* white   = COLOR_STR( "\033[97m" );

private:
  static mask_t&  s_inject();
  static mask_t&  s_debug();
  static string&  s_trace_name();
  static args_t&  s_config();
  static bool&    s_stop();
  static bool&    s_quiet();
  static bool&    s_verbose();
  static size_t&  s_count( const string& name, bool modify = true );
  static sc_time& s_time ( const string& name, bool modify = true );
  static bool&    s_flag ( const string& name, bool modify = true );
  static sc_trace_file*& s_trace_file();

};
