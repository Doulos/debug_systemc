#pragma once

#include "processes.hpp"
#include <systemc>
#include <string>
using namespace std::literals;

struct Top_module : sc_core::sc_module
{
  using sc_report_handler = sc_core::sc_report_handler;
  using string = std::string;

  static constexpr const char *msg_type = "/Doulos/debugging_systemc/top";

  // Sub-modules
  Processes_module m1{"m1", 1u};
  Processes_module m2{"m2", 2u};

  // Constructor
  explicit Top_module( const sc_core::sc_module_name& instance )
    : sc_module( instance )
  {
    SC_HAS_PROCESS( Top_module );
    SC_THREAD( early_terminate_thread );
  }

  void before_end_of_elaboration() override
  {
    sc_report_handler::set_actions( sc_core::SC_WARNING, sc_core::SC_DISPLAY | sc_core::SC_LOG | sc_core::SC_INTERRUPT );
    sc_report_handler::set_actions( sc_core::SC_ERROR, sc_core::SC_DISPLAY   | sc_core::SC_LOG | sc_core::SC_INTERRUPT );
    sc_report_handler::set_actions( sc_core::SC_FATAL, sc_core::SC_DISPLAY   | sc_core::SC_LOG | sc_core::SC_STOP );

    Debug::parse_command_line();
  }

  void early_terminate_thread()
  {
    // Use this to stop --help gracefully and/or pre-simulation time errors.
    wait( sc_core::SC_ZERO_TIME );
    Debug::stop_if_requested();
  }

  void end_of_simulation() override {
    REPORT_ALWAYS( std::to_string( Debug::context_switch(false) ) + " context switches" );
  }

  // Destructor
  ~Top_module() override {
    // Report results of simulation
    if( Debug::tracing() ) {
      Debug::close_trace_file();
    }
  }

};
