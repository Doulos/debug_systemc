#pragma once

#include "processes.hpp"
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
    auto message  = std::string{"\n"}
                  + "Status report\n"s
                  + "-------------\n"s
                  ;
    if( warnings > 0 ) {
      message += std::string{Debug::yellow} + std::string{Debug::bold}
        + "  "s + std::to_string( warnings ) + " Warnings\n"s
        + std::string{Debug::none}
        ;
    }
    if( errors > 0 ) {
      message += std::string{Debug::red} + std::string{Debug::bold}
        + "  "s + std::to_string( errors ) + " ERRORS - simulation FAILED"s
        + std::string{Debug::none}
        ;
    }
    else {
      message += std::string{Debug::green} + std::string{Debug::bold}
        + "  No errors - simulation SUCCESS."s
        + std::string{Debug::none}
        ;
    }
    SC_REPORT_INFO_VERB( mesgType, message.c_str(), sc_core::SC_NONE);
  }

};
