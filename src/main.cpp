#include "top.hpp"
#include "timer.hpp"
#include <string>
using namespace std::literals;

std::unique_ptr<Timer> timer;

[[maybe_unused]] int sc_main( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] )
{
  constexpr const char* mesgType = "/Doulos/debugging_systemc/main";
  timer = std::make_unique<Timer>("app");

  Top top { "top" };
  timer->report( "Construction" );
  sc_core::sc_start();

  if ( not sc_core::sc_end_of_simulation_invoked() ) {
    sc_core::sc_stop();  // triggers end_of_simulation() callback
  }

  auto errors = sc_core::sc_report_handler::get_count( sc_core::SC_ERROR )
              + sc_core::sc_report_handler::get_count( sc_core::SC_FATAL );

  auto message_count = sc_core::sc_report_handler::get_count( sc_core::SC_INFO )
              + sc_core::sc_report_handler::get_count( sc_core::SC_WARNING )
              + errors + 1;
  SC_REPORT_INFO_VERB( mesgType, ( "Total messages: "s + std::to_string(message_count) ).c_str(), SC_NONE );


  return ( errors == 0 ? 0 : 1 );
}
