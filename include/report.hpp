#pragma once

#include <systemc>
#include <string>
#if __has_include(<fmt/format.h>)
  #define HAS_FMT_FORMAT
  #include <fmt/format.h>
#endif

// This version is more efficient that SC_REPORT_INFO_VERB because it
// does not evaluate the arguments until and unless the verbosity level
// is appropriate.
//
// Note: SC_REPORT_WARNING, SC_REPORT_ERROR, and SC_REPORT_FATAL do not
//       need this because they unconditionally execute anyhow.

// Fix efficiency so that message will be evaluated only if verbosity meets requirements.
#define REPORT_INFO_VERB(msg_type,message,level) do {\
  if( level <= ::sc_core::sc_report_handler::get_verbosity_level() ) \
    SC_REPORT_INFO_VERB(msg_type,message,level);\
} while(false)

// Macro to support std::format or fmt::format if present
#ifdef HAS_FMT_FORMAT
  #define REPORT_FORMAT(...) ::fmt::format(__VA_ARGS__)
#elif __cplus_plus__ > 202003ul 
    #define REPORT_FORMAT(...) ::std::format(__VA_ARGS__)
#endif
#ifdef REPORT_FORMAT
  #define REPORT_VERB_FORMAT(msg_type,level,...) do {\
    if( level <= ::sc_core::sc_report_handler::get_verbosity_level() ) \
      SC_REPORT_INFO_VERB(msg_type,REPORT_FORMAT(__VA_ARGS__).c_str(),level);\
  } while(false)
#endif
// Do not define REPORT_FORMAT otherwise...

#ifndef NOCOLOR
  #define COLOR_STR(cstr) cstr
#else
  #define COLOR_STR(cstr) ""
#endif

#define COLOR_BOLD    std::string{ COLOR_STR( "\033[1m"  ) }
#define COLOR_NONE    std::string{ COLOR_STR( "\033[0m"  ) }
#define COLOR_BLACK   std::string{ COLOR_STR( "\033[90m" ) }
#define COLOR_RED     std::string{ COLOR_STR( "\033[91m" ) }
#define COLOR_GREEN   std::string{ COLOR_STR( "\033[92m" ) }
#define COLOR_YELLOW  std::string{ COLOR_STR( "\033[93m" ) }
#define COLOR_BLUE    std::string{ COLOR_STR( "\033[94m" ) }
#define COLOR_MAGENTA std::string{ COLOR_STR( "\033[95m" ) }
#define COLOR_CYAN    std::string{ COLOR_STR( "\033[96m" ) }
#define COLOR_WHITE   std::string{ COLOR_STR( "\033[97m" ) }
#define COLOR_INFO    COLOR_CYAN
#define COLOR_WARN    COLOR_YELLOW
#define COLOR_ERROR   COLOR_RED
#define COLOR_FATAL   (COLOR_RED + COLOR_BOLD)
#define COLOR_DEBUG   (COLOR_CYAN + COLOR_BOLD)

#ifdef NEVER_ENABLE_JUST_HIGHLIGHT
// Most of the following macros assume a predefined variable: msg_type
// Usually, this should be declared in your class as:
  static constexpr const char* msg_type = "/COMPANY/PROJECT/MODULE/etc";
#endif

#ifdef REPORT_FORMAT
  #define REPORT_INFO(...)       REPORT_VERB_FORMAT ( msg_type, ::sc_core::SC_MEDIUM, __VA_ARGS__ )
  #define REPORT_WARNING(...)    SC_REPORT_WARNING(   msg_type, REPORT_FORMAT(__VA_ARGS__).c_str() )
  #define REPORT_ERROR(...)      SC_REPORT_ERROR(     msg_type, REPORT_FORMAT(__VA_ARGS__).c_str() )
  #define REPORT_FATAL(...)      SC_REPORT_FATAL(     msg_type, REPORT_FORMAT(__VA_ARGS__).c_str() )
  #define REPORT_VERB(level,...) REPORT_VERB_FORMAT(  msg_type, level,               __VA_ARGS__ )
  #define REPORT_ALWAYS(...)     REPORT_VERB_FORMAT(  msg_type, ::sc_core::SC_NONE,  __VA_ARGS__ )
  #define REPORT_DEBUG(...)      REPORT_VERB_FORMAT(  msg_type, ::sc_core::SC_DEBUG, __VA_ARGS__ )
  #define REPORT_NUM(var) REPORT_DEBUG( "{} = {}", #var, var )
  #define REPORT_STR(var) REPORT_DEBUG( "{} = {}", #var, var ) // use this if OBJ supports std::format
  #define REPORT_OBJ(var) REPORT_DEBUG( "{} = {}", #var, var.to_string()     )
#else
  #define REPORT_INFO(mesg)       SC_REPORT_INFO(    msg_type, Doulos::text(mesg, ::sc_core::SC_INFO) )
  #define REPORT_WARNING(mesg)    SC_REPORT_WARNING( msg_type, Doulos::text(mesg, ::sc_core::SC_WARNING) )
  #define REPORT_ERROR(mesg)      SC_REPORT_ERROR(   msg_type, Doulos::text(mesg, ::sc_core::SC_ERROR) )
  #define REPORT_FATAL(mesg)      SC_REPORT_FATAL(   msg_type, Doulos::text(mesg, ::sc_core::SC_FATAL) )
  #define REPORT_VERB(mesg,level) REPORT_INFO_VERB(  msg_type, Doulos::text(mesg, ::sc_core::SC_INFO, level ) )
  #define REPORT_ALWAYS(mesg)     REPORT_INFO_VERB(  msg_type, Doulos::text(mesg, ::sc_core::SC_INFO, ::sc_core::SC_NONE ), ::sc_core::SC_NONE )
  #define REPORT_DEBUG(mesg)      REPORT_INFO_VERB(  msg_type,\
    Doulos::text( std::string{"Debug: "} + std::string{mesg} \
    + std::string{"\nFile:"} + std::string{__FILE__} + std::string{" Line:"} + std::to_string(__LINE__)\
    + std::string{" at "} + ::sc_core::sc_time_stamp().to_string()\
    , ::sc_core::SC_INFO, ::sc_core::SC_DEBUG), ::sc_core::SC_DEBUG )
  #define REPORT_NUM(var) REPORT_DEBUG( std::string{#var} + std::string{"="} + std::to_string(var) )
  #define REPORT_STR(var) REPORT_DEBUG( std::string{#var} + std::string{"="} + var                 )
  #define REPORT_OBJ(var) REPORT_DEBUG( std::string{#var} + std::string{"="} + var.to_string()     )
#endif

namespace Doulos {
// Convert a string to const char*
static inline const char* text( const std::string& the_string ) {
  return the_string.c_str();
}

static inline const char* text( const std::string& the_string
                       , ::sc_core::sc_severity severity
                       , int level = ::sc_core::SC_MEDIUM ) {
  static std::string result{};
  result.clear();
  switch( severity ) {
    case ::sc_core::SC_INFO:    result += (level >= ::sc_core::SC_DEBUG)? COLOR_DEBUG : COLOR_INFO; break;
    case ::sc_core::SC_WARNING: result += COLOR_WARN;  break;
    case ::sc_core::SC_ERROR:   result += COLOR_ERROR; break;
    case ::sc_core::SC_FATAL:   result += COLOR_FATAL; break;
    default: break;
  }
  result += the_string + COLOR_NONE;
  return result.c_str();
}

}//endnamespace Doulos
