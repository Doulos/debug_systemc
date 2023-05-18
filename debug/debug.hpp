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
 *
 *  During debug (i.e, GDB) you will find useful:
 *  1. Debug::info() -- displays simulator information (state, time, verbosity)
 *  2. Debug::show( cpp_string ) -- displays std::string
 *  3. Debug::opts() -- display options in effect
 */

#include <systemc>
#include <string>
#include <vector>
#include <cstdio>
#include <map>
#include <cmath>
using namespace std::literals;

#ifndef NOCOLOR
  #define COLOR_STR(cstr) cstr
#else
  #define COLOR_STR(cstr) ""
#endif

// Handy shortcuts
#define REPORT_WARNING(message) SC_REPORT_WARNING( mesgType, (message).c_str() )
#define REPORT_ERROR(message) SC_REPORT_ERROR( mesgType, (message).c_str() )
#define REPORT_FATAL(message) SC_REPORT_FATAL( mesgType, (message).c_str() )
#define REPORT_VERB(message,verbosity) SC_REPORT_INFO_VERB( mesgType, (message).c_str(), sc_core::verbosity )
#define REPORT_ALWAYS(message) SC_REPORT_INFO_VERB( mesgType, (message).c_str(), sc_core::SC_NONE )
#define REPORT_DEBUG(message) SC_REPORT_INFO_VERB( mesgType,\
  ( std::string{Debug::red} + message + "\nFile:"s + std::string{__FILE__}                \
    + "Line:"s + std::to_string(__LINE__) +                                               \
    " at "s + sc_core::sc_time_stamp().to_string()                                        \
    + std::string{Debug::none} ).c_str(),                                                 \
  sc_core::SC_DEBUG )
#define REPORT_STR(var) SC_REPORT_INFO_VERB( "DEBUG",                                     \
  ( std::string{Debug::red} + std::string{#var} + std::string{"="} + var                  \
    + std::string{Debug::none} ).c_str(),                                                 \
  sc_core::SC_NONE )
#define REPORT_OBJ(var) SC_REPORT_INFO_VERB( "DEBUG",                                     \
  ( std::string{Debug::red} + std::string{#var} + std::string{"="} + var.to_string()      \
    + std::string{Debug::none} ).c_str(),                                                 \
  sc_core::SC_NONE )
#define REPORT_INT(var) SC_REPORT_INFO_VERB( "DEBUG",                                     \
  ( std::string{Debug::red} + std::string{#var} + std::string{"="} + std::to_string(var)  \
    + std::string{Debug::none} ).c_str(),                                                 \
  sc_core::SC_NONE )

struct Debug {

  using string = std::string;
  using args_t = std::vector<string>;
  using mask_t = sc_dt::sc_bv<64>;
  using cstr_t = const char*;
  using sc_trace_file = sc_core::sc_trace_file;
  using sc_time = sc_core::sc_time;
  using sc_object = sc_core::sc_object;

  // Member methods
  explicit Debug( const std::string& s = "" );
  cstr_t context( const std::string& s = "" );
  void   mark( const std::string& s, const std::string& func, sc_object* obj = nullptr, const std::string& what = "itds"  );
  void   executed( const std::string& func, sc_object* obj = nullptr, const std::string& what = "itd"  );
  void   entering( const std::string& func, sc_object* obj = nullptr, const std::string& what = "itd"  );
  void   yielding( const std::string& func, sc_object* obj = nullptr, const std::string& what = "itd"  );
  void   resuming( const std::string& func, sc_object* obj = nullptr, const std::string& what = "itd"  );
  void   leaving ( const std::string& func, sc_object* obj = nullptr, const std::string& what = "itd"  );

  // Static methods
  static constexpr cstr_t mesgType = "/Doulos/Debug";
  static sc_trace_file* trace_file()                          { return s_trace_file(); }
  static           bool tracing()                             { return s_trace_file() != nullptr; }
  static           bool debugging( const mask_t& mask = ~0u ) { return (s_debug() & mask) != 0u; }
  static           bool injecting( const mask_t& mask = ~0u ) { return (s_inject() & mask) != 0u; }
  static           bool stopping()                            { return s_stop(); }
  static           bool verbose()                             { return s_verbose(); }
  static           bool quiet()                               { return s_quiet(); }
  static        args_t& config()                              { return s_config(); }
  static         size_t get_count(const string& name)         { return s_count(name, false); }
  static        sc_time get_time(const string& name)          { return s_time(name, false); }
  static           bool get_flag(const string& name)          { return s_flag(name, false); }
  static         string get_text(const string& name)          { return s_text(name, false); }
  static         double get_value(const string& name)         { return s_value(name, false); }
  static           void close_trace_file()                    { Debug::set_trace_file(""); }

  static void   read_configuration( args_t& args, string filename = "" );
  static void   parse_command_line();
  static void   breakpoint( const string& tag = "" );
  static void   stop_if_requested();
  static void   set_trace_file( const string& filename );
  static void   set_quiet( bool flag = true );
  static void   set_verbose( bool flag = true );
  static void   set_debugging( const mask_t& mask = 1 ); // 0 => no-change
  static void   clr_debugging( const mask_t& mask = 0 ); // 0 => all cleared
  static void   set_injecting( const mask_t& mask = 1 );
  static void   set_count( const string& name, size_t count = 1 );
  static void   set_time( const string& name, const sc_time& time = sc_core::SC_ZERO_TIME );
  static void   set_flag( const string& name, bool flag = true );
  static void   set_text( const string& name, string text = "" );
  static void   set_value( const string& name, double value = 0.0 );
  static void   info( cstr_t what = "itdsv" ); // Display information about situation
  static void   opts(); // Display various flags, counts & times
  static void   name(const sc_core::sc_object* m); // Display information about the object
  static void   show(const string& s);
  static cstr_t text( const string& s );
  static size_t context_switch( bool increment = true ) { static size_t count{0}; if(increment) ++count; return count; }
  static string get_simulation_info( sc_object* obj = nullptr, const string& what = "itd" );
  static string get_simulation_status();
  static string get_verbosity();
  static string command_options(); // returns command-line options including config
  static int exit_status( const string& project );

  // Constants
  static volatile const char* all;
  static constexpr const int message_level = sc_core::SC_NONE;
  static constexpr cstr_t none    = COLOR_STR( "\033[0m"  );
  static constexpr cstr_t bold    = COLOR_STR( "\033[1m"  );
  static constexpr cstr_t black   = COLOR_STR( "\033[90m" );
  static constexpr cstr_t red     = COLOR_STR( "\033[91m" );
  static constexpr cstr_t green   = COLOR_STR( "\033[92m" );
  static constexpr cstr_t yellow  = COLOR_STR( "\033[93m" );
  static constexpr cstr_t blue    = COLOR_STR( "\033[94m" );
  static constexpr cstr_t magenta = COLOR_STR( "\033[95m" );
  static constexpr cstr_t cyan    = COLOR_STR( "\033[96m" );
  static constexpr cstr_t white   = COLOR_STR( "\033[97m" );

private:
  string m_context{"DEBUG"};

  static mask_t&  s_inject();
  static mask_t&  s_debug();
  static string&  s_trace_name();
  static args_t&  s_config();
  static bool&    s_stop();
  static bool&    s_quiet();
  static bool&    s_verbose();
  static bool&    s_warn();
  static bool&    s_werror();
  static std::map<string,size_t>&  s_count_map();
  static std::map<string,sc_time>& s_time_map();
  static std::map<string,bool>&    s_flag_map();
  static std::map<string,string>&  s_text_map();
  static std::map<string,double>&  s_value_map();
  static size_t&  s_count ( const string& name, bool modify = true );
  static sc_time& s_time  ( const string& name, bool modify = true );
  static bool&    s_flag  ( const string& name, bool modify = true );
  static string&  s_text  ( const string& name, bool modify = true );
  static double&  s_value ( const string& name, bool modify = true );
  static sc_trace_file*& s_trace_file();

};
