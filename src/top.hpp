#pragma once

#include "test.hpp"
#include "debug.hpp"
#include "timer.hpp"
#include <systemc>
#include <string>
using namespace std::literals;

extern std::unique_ptr<Timer> timer;

SC_MODULE( Top ) {
  using report_handler = sc_core::sc_report_handler;

  static constexpr const char *mesgType = "/Doulos/ex05/top";

  Test test{ "test" };
  sc_core::sc_clock clk{ "clock", 10, sc_core::SC_NS };

  explicit Top( const sc_core::sc_module_name& instance ) : sc_module( instance )
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
    timer->report("Elaboration took");
    timer->reset();
  }

  void end_of_simulation() override
  {
    timer->report("Simulation took");
  }

  void start_of_simulation() override
  {
    timer->reset();
    Debug::stop_if_requested();
  }

  ~Top() override {
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
