#pragma once

#include <systemc>
#include <string>

// Fix efficiency so that message will be evaluated only if verbosity meets requirements
#ifdef  SC_REPORT_INFO_VERB
  #define REPORT_INFO_VERB(mesgType,message,verbosity) do {\
    if( verbosity <= ::sc_core::sc_report_handler::get_verbosity_level() ) \
      SC_REPORT_INFO_VERB(mesgType,message,verbosity);\
  } while(false)
#else
  #error "Must include SystemC before " __FILE__
#endif

// Macro to support std::format or fmt::format if present
#if __cplus_plus__ > 202003ul
  #define REPORT_VERB_FORMAT(mesgType,verbosity,fmt,...) do {\
    if( verbosity <= ::sc_core::sc_report_handler::get_verbosity_level() ) \
      SC_REPORT_INFO_VERB(mesgType,::std::format(fmt,__VA_ARGS__).c_str(),verbosity);\
  } while(false)
#else
  #ifdef FMT_FORMAT_H_
    #define REPORT_VERB_FORMAT(mesgType,verbosity,fmt,...) do {\
      if( verbosity <= ::sc_core::sc_report_handler::get_verbosity_level() ) \
        SC_REPORT_INFO_VERB(mesgType,::fmt::format(fmt,__VA_ARGS__).c_str(),verbosity);\
    } while(false)
  #else
    #define REPORT_VERB_FORMAT(mesgType,verbosity,fmt,...) do {\
       SC_REPORT_ERROR( "/Doulos/reporting_enhancement", "Missing support for REPORT_VERB_FORMAT" );\
       SC_REPORT_INFO_VERB( mesgType, fmt, verbosity );\
    } while(false)
  #endif
#endif

#ifndef NOCOLOR
  #define COLOR_STR(cstr) cstr
#else
  #define COLOR_STR(cstr) ""
#endif

#define COLOR_BOLD    std::string{ COLOR_STR( "\033[0m"  ) }
#define COLOR_NONE    std::string{ COLOR_STR( "\033[1m"  ) }
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

#define REPORT_INFO(mesg)       SC_REPORT_INFO(    Doulos::text(msg_type), Doulos::text(mesg, ::sc_core::SC_INFO) )
#define REPORT_WARNING(mesg)    SC_REPORT_WARNING( Doulos::text(msg_type), Doulos::text(mesg, ::sc_core::SC_WARNING) )
#define REPORT_ERROR(mesg)      SC_REPORT_ERROR(   Doulos::text(msg_type), Doulos::text(mesg, ::sc_core::SC_ERROR) )
#define REPORT_FATAL(mesg)      SC_REPORT_FATAL(   Doulos::text(msg_type), Doulos::text(mesg, ::sc_core::SC_FATAL) )
#define REPORT_VERB(mesg,level) REPORT_INFO_VERB(  Doulos::text(msg_type), Doulos::text(mesg, ::sc_core::SC_INFO, level ) )
#define REPORT_ALWAYS(mesg)     REPORT_INFO_VERB(  Doulos::text(msg_type), Doulos::text(mesg, ::sc_core::SC_INFO, ::sc_core::SC_NONE ), ::sc_core::SC_NONE )
#define REPORT_DEBUG(mesg)      REPORT_INFO_VERB(  Doulos::text(msg_type),\
  Doulos::text( std::string{"Debug: "} + std::string{mesg} \
  + "\nFile:"s + std::string{__FILE__} + " Line:"s + std::to_string(__LINE__)\
  + " at "s + ::sc_core::sc_time_stamp().to_string()\
  , ::sc_core::SC_INFO, ::sc_core::SC_DEBUG), ::sc_core::SC_DEBUG )
#define REPORT_NUM(var) REPORT_DEBUG( std::string{#var} + std::string{"="} + std::to_string(var) )
#define REPORT_STR(var) REPORT_DEBUG( std::string{#var} + std::string{"="} + var                 )
#define REPORT_OBJ(var) REPORT_DEBUG( std::string{#var} + std::string{"="} + var.to_string()     )

namespace Doulos {
// Convert a string to const char*
static inline const char* text( const std::string& s ) {
  return s.c_str();
}

static inline const char* text( const std::string& s
                       , ::sc_core::sc_severity severity
                       , int verbosity = ::sc_core::SC_MEDIUM ) {
  static std::string result{};
  result.clear();
  switch( severity ) {
    case ::sc_core::SC_INFO:    result += (verbosity >= ::sc_core::SC_DEBUG)? COLOR_DEBUG : COLOR_INFO; break;
    case ::sc_core::SC_WARNING: result += COLOR_WARN;  break;
    case ::sc_core::SC_ERROR:   result += COLOR_ERROR; break;
    case ::sc_core::SC_FATAL:   result += COLOR_FATAL; break;
    default: break;
  }
  result += s + COLOR_NONE;
  return result.c_str();
}

}//endnamespace Doulos
