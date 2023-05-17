#pragma once

#include "processes.hpp"
#include <systemc>
#include <string>
using namespace std::literals;

struct Top_module : sc_core::sc_module
{
  using report_handler = sc_core::sc_report_handler;
  using string = std::string;

  static constexpr const char *mesgType = "/Doulos/debugging_systemc/top";

  // Constructor
  explicit Top_module( const sc_core::sc_module_name& instance )
    : sc_module( instance )
  {
    // No interconnect for this design
  }

  Processes_module processes{"processes"};

  void before_end_of_elaboration() override
  {

    report_handler::set_actions( sc_core::SC_WARNING, sc_core::SC_DISPLAY | sc_core::SC_LOG | sc_core::SC_INTERRUPT );
    report_handler::set_actions( sc_core::SC_ERROR, sc_core::SC_DISPLAY   | sc_core::SC_LOG | sc_core::SC_INTERRUPT );

    Debug::parse_command_line();
  }

  void start_of_simulation() override
  {
    Debug::stop_if_requested();
  }

  ~Top_module() override {
    auto warnings = report_handler::get_count( sc_core::SC_WARNING );
    auto errors   = report_handler::get_count( sc_core::SC_ERROR );
    auto fatals   = report_handler::get_count( sc_core::SC_FATAL );
    auto message  = "\n\n"s
                  + "Status report\n"s
                  + "-------------\n"s
                  ;
    if( warnings > 0 ) {
      message += string{Debug::yellow} + string{Debug::bold}
        + "  "s + std::to_string( warnings ) + " Warnings\n"s
        + string{Debug::none}
        ;
    }
    if( errors > 0 ) {
      message += string{Debug::red} + string{Debug::bold}
        + "  "s + std::to_string( errors ) + " ERRORS"s
        + string{Debug::none}
        ;
    }
    if( fatals > 0 ) {
      message += string{Debug::red} + string{Debug::bold}
        + "  "s + std::to_string( fatals ) + " FATALS - simulation FAILED"s
        + string{Debug::none}
        ;
    }
    if( errors + fatals == 0 ) {
      message += string{Debug::green} + string{Debug::bold}
        + "\nNo errors - simulation SUCCESS."s
        + string{Debug::none}
        ;
    }
    else {
      message += string{Debug::red} + string{Debug::bold}
        + "\nSimulation FAILED."s
        + string{Debug::none}
        ;
    }
    SC_REPORT_INFO_VERB( mesgType, message.c_str(), sc_core::SC_NONE);
  }

};
