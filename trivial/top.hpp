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

  static constexpr const char *mesgType = "/Doulos/debugging_systemc/top";

  // Constants for the simulation
  const sc_core::sc_time period{ 10, sc_core::SC_NS };

  Producer_module producer{ "producer" };
  Consumer_module consumer{ "consumer" };

  explicit Top_module( const sc_core::sc_module_name& instance ) : sc_module( instance )
  {
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
    Objection::set_maxTimeout( sc_core::sc_time{100, sc_core::SC_MS} );
    Objection::set_drainTime( sc_core::sc_time{100, sc_core::SC_NS} );

    Timer::global().reset();
    Debug::stop_if_requested();
    SC_REPORT_INFO_VERB(
      mesgType,
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
      SC_REPORT_INFO_VERB( mesgType, "Transmit & receive transaction counts match", sc_core::SC_NONE );
    }
    else {
      SC_REPORT_ERROR( mesgType, "Transmit & receive transaction count mismatch" );
    }
  }

  ~Top_module() override {
    // Report results of simulation
    if( Debug::tracing() ) {
      Debug::close_trace_file();
    }
    auto warnings = report_handler::get_count( sc_core::SC_WARNING );
    auto errors   = report_handler::get_count( sc_core::SC_ERROR );

    auto message = std::string{"\n"}
        + "Run-time options\n"s
        + "----------------\n"s
        + "  "s + Debug::command_options() + "\n"s
        + "\n"s
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
