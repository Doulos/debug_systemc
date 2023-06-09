#pragma once

#include "producer.hpp"
#include "consumer.hpp"
#include "debug.hpp"
#include "timer.hpp"
#include "objection.hpp"
#include <systemc>
#include <string>
using namespace std::literals;

SC_MODULE( Top_module ) {
  using report_handler = sc_core::sc_report_handler;

  static constexpr const char *msg_type = "/Doulos/debugging_systemc/top";

  // Constants for the simulation
  const sc_core::sc_time period{ 10, sc_core::SC_NS };

  // Sub-modules
  Producer_module producer{ "producer" };
  Consumer_module consumer{ "consumer" };

  // Constructor
  explicit Top_module( const sc_core::sc_module_name& instance ) : sc_module( instance )
  {
    SC_HAS_PROCESS( Top_module );
    SC_THREAD( terminate_thread );

    // Connectivity
    consumer.data_in.bind( producer.data_out );
  }

  void before_end_of_elaboration() override
  {

    report_handler::set_actions( sc_core::SC_WARNING, sc_core::SC_DISPLAY | sc_core::SC_LOG | sc_core::SC_INTERRUPT );
    report_handler::set_actions( sc_core::SC_ERROR, sc_core::SC_DISPLAY   | sc_core::SC_LOG | sc_core::SC_INTERRUPT );
    report_handler::set_actions( "producer", sc_core::SC_INFO, sc_core::SC_DO_NOTHING );
    report_handler::set_actions( "consumer", sc_core::SC_INFO, sc_core::SC_LOG );
    report_handler::set_log_file_name( "consumer.log" );

    Debug::parse_command_line();
  }

  void end_of_elaboration() override
  {
    Timer::global().report("Elaboration took");
    Timer::global().reset();
  }

  void start_of_simulation() override
  {
    Objection::set_drainTime( sc_core::sc_time{100, sc_core::SC_NS} );
    Objection::set_maxTimeout( sc_core::sc_time{100, sc_core::SC_MS} );

    Timer::global().reset();
    Debug::stop_if_requested();
    SC_REPORT_INFO_VERB(
      msg_type,
      ( std::string( 80, '-' ) + "\n"s
      + "Configuration\n"s
      + "-------------\n"s
      + "        Verbosity: "s + ( Debug::debugging() ? "DEBUG"s : "medium"s ) + "\n"s
      + "        Injecting: "s + ( Debug::injecting() ? "true"s : "false"s ) + "\n"s
      + "          Tracing: "s + ( Debug::tracing() ? "true"s : "false"s ) + "\n"s
      + std::string( 80, '-' )
      ).c_str(),
      sc_core::SC_NONE
    );
  }

  void end_of_simulation() override
  {
    Timer::global().report("Simulation took");
    if( consumer.count() == producer.count() ) {
      SC_REPORT_INFO_VERB( msg_type, "Transmit & receive transaction counts match", sc_core::SC_NONE );
    }
    else {
      SC_REPORT_ERROR( msg_type, "Transmit & receive transaction count mismatch" );
    }
  }

  void terminate_thread()
  {
    // Use this to stop --help gracefully and/or pre-simulation time errors.
    wait( sc_core::SC_ZERO_TIME );
    Debug::stop_if_requested();
  }

  // Destructor
  ~Top_module() override {
    // Report results of simulation
    if( Debug::tracing() ) {
      Debug::close_trace_file();
    }
  }

};
