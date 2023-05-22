#pragma once

#include "test.hpp"
#include "debug.hpp"
#include "timer.hpp"
#include <systemc>
#include <string>
using namespace std::literals;

SC_MODULE( Top_module ) {
  using report_handler = sc_core::sc_report_handler;

  static constexpr const char *msg_type = "/Doulos/debugging_systemc/top";

  Test_module test{ "test" };
  sc_core::sc_clock clk{ "clock", 10, sc_core::SC_NS };

  explicit Top_module( const sc_core::sc_module_name& instance ) : sc_module( instance )
  {
    test.clock.bind( clk );
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

  void end_of_simulation() override
  {
    Timer::global().report("Simulation took");
  }

  void start_of_simulation() override
  {
    Timer::global().reset();
    Debug::stop_if_requested();
  }

  ~Top_module() override {
    if( Debug::tracing() ) {
      Debug::close_trace_file();
    }
  }

};
