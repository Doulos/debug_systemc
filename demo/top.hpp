#pragma once

#include "processes.hpp"
#include <systemc>
#include <string>
using namespace std::literals;

struct Top_module : sc_core::sc_module
{
  using report_handler = sc_core::sc_report_handler;
  using string = std::string;

  static constexpr const char *msg_type = "/Doulos/debugging_systemc/top";

  // Constructor
  explicit Top_module( const sc_core::sc_module_name& instance )
    : sc_module( instance )
  {
    SC_HAS_PROCESS( Top_module );
    SC_THREAD( die_if_requested );
  }

  Processes_module m1{"m1", 1u};
  Processes_module m2{"m2", 2u};

  void before_end_of_elaboration() override
  {

    report_handler::set_actions( sc_core::SC_WARNING, sc_core::SC_DISPLAY | sc_core::SC_LOG | sc_core::SC_INTERRUPT );
    report_handler::set_actions( sc_core::SC_ERROR, sc_core::SC_DISPLAY   | sc_core::SC_LOG | sc_core::SC_INTERRUPT );

    Debug::parse_command_line();
  }

  void die_if_requested()
  {
    wait( sc_core::SC_ZERO_TIME );
    Debug::stop_if_requested();
  }

};
