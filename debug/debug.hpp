#pragma once

// See ABOUT_Debug.md for details

#include <systemc>
#include <string>
#include <vector>
#include <cstdio>
#include <map>
#include <cmath>
#include "report.hpp"
using namespace std::literals;

//------------------------------------------------------------------------------
// Introspection
//------------------------------------------------------------------------------

namespace Doulos {

#define DOULOS_DEBUG_VERSION_MAJOR 1
#define DOULOS_DEBUG_VERSION_MINOR 0
#define DOULOS_DEBUG_VERSION_PATCH 0
#define DOULOS_DEBUG_IS_PRERELEASE 1

std::string version();

struct Info {
  using cstr_t = const char*;
  using sc_object = ::sc_core::sc_object;

  explicit Info( std::string context_name = "" );
  cstr_t context( const std::string& new_context_name = "" );
  void   mark( const std::string& action_name, const std::string& func, sc_object* obj = nullptr, const std::string& what = "itds"  );
  void   executed( const std::string& func, sc_object* obj = nullptr, const std::string& what = "itd"  );
  void   entering( const std::string& func, sc_object* obj = nullptr, const std::string& what = "itd"  );
  void   yielding( const std::string& func, sc_object* obj = nullptr, const std::string& what = "itd"  );
  void   resuming( const std::string& func, sc_object* obj = nullptr, const std::string& what = "itd"  );
  void   leaving ( const std::string& func, sc_object* obj = nullptr, const std::string& what = "itd"  );

private:
  std::string m_context{"DEBUG"};

};

}//endnamespace Doulos

//------------------------------------------------------------------------------
// Debug support
//------------------------------------------------------------------------------

struct Debug {

  using string = std::string;
  using args_t = std::vector<string>;
  using mask_t = sc_dt::sc_bv<64>;
  using cstr_t = const char*;
  using sc_severity   = ::sc_core::sc_severity;
  using sc_trace_file = ::sc_core::sc_trace_file;
  using sc_time       = ::sc_core::sc_time;
  using sc_object     = ::sc_core::sc_object;

  // Static methods
  static constexpr cstr_t msg_type = "/Doulos/Debug";
  static constexpr sc_severity max_severity = sc_core::SC_MAX_SEVERITY;
  static sc_trace_file* trace_file()                          { return s_trace_file(); }
  static           bool tracing()                             { return s_trace_file() != nullptr; }
  static           bool debugging( const mask_t& mask = ~0u ) { return (s_debug() & mask) != 0u; }
  static           bool injecting( const mask_t& mask = ~0u ) { return (s_inject() & mask) != 0u; }
  static           bool stopping()                            { return s_stop(); }
  static           bool verbose()                             { return s_verbose(); }
  static           bool quiet()                               { return s_quiet(); }
  static        args_t& config()                              { return s_config(); }
  static           bool stop_requested()                      { return s_stop(); }
  static         size_t get_count(const string& name)         { return s_count(name, false); }
  static        sc_time get_time(const string& name)          { return s_time(name, false); }
  static           bool get_flag(const string& name)          { return s_flag(name, false); }
  static         string get_text(const string& name)          { return s_text(name, false); }
  static         double get_value(const string& name)         { return s_value(name, false); }
  static           void close_trace_file()                    { set_trace_file(""); }

  static           void add_expected( sc_severity severity, const string& msg_type_ = "", ssize_t n = 1 );
  static           void see_expected( sc_severity severity, const string& msg_type_ = "" );
  static        ssize_t get_expected( sc_severity severity = max_severity );
  static        ssize_t get_observed( sc_severity severity = max_severity );

  static void   read_configuration( args_t& args, string filename = "" );
  static void   parse_command_line();
  static void   breakpoint( const string& tag = "" );
  static void   resume();
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
  static void   help();
  static void   info( cstr_t what = "itsv" ); // Display information about situation
  static void   opts(); // Display various flags, counts & times
  static size_t parsed( const string& opt ); // indicates if an option was parsed successfully
  static cstr_t name(const sc_core::sc_object* m); // Return information about the object
  static void   show( const string& theText );
  static cstr_t process();
  static string get_simulation_info( sc_object* obj = nullptr, const string& what = "itd" );
  static string get_simulation_status();
  static string severity_str( sc_severity severity );
  static string verbosity_str( int level );
  static string verbosity_str();
  static string command_options(); // returns command-line options including config
  static int exit_status( const string& project );
  static size_t context_switch( bool increment = true )
  {
    static size_t count{0};
    if(increment and sc_core::sc_time_stamp() > sc_core::SC_ZERO_TIME) ++count;
    return count;
  }

  // Constants
  static volatile const char* all;
  static volatile mask_t mask1;
  static volatile mask_t mask0;
  static constexpr const int message_level = sc_core::SC_NONE;

private:

  static mask_t&  s_inject();
  static mask_t&  s_debug();
  static string&  s_trace_name();
  static args_t&  s_config();
  static bool&    s_stop();
  static bool&    s_quiet();
  static bool&    s_verbose();
  static bool&    s_warn();
  static bool&    s_werror();
  using severity_n_type = std::pair<sc_severity,string>;
  static std::map<severity_n_type,ssize_t>& s_expected_map();
  static std::map<severity_n_type,ssize_t>& s_observed_map();
  static std::map<string,size_t>&  s_count_map();
  static std::map<string,sc_time>& s_time_map();
  static std::map<string,bool>&    s_flag_map();
  static std::map<string,string>&  s_text_map();
  static std::map<string,double>&  s_value_map();
  static std::map<string,size_t>&  s_parsed_map();
  static size_t&  s_count  ( const string& name, bool modify = true );
  static sc_time& s_time   ( const string& name, bool modify = true );
  static bool&    s_flag   ( const string& name, bool modify = true );
  static string&  s_text   ( const string& name, bool modify = true );
  static double&  s_value  ( const string& name, bool modify = true );
  static void     s_parsed ( const string& name );
  static string   get_opts ( const string& prefix = "" );
  static sc_trace_file*& s_trace_file();

};
