#include "top.hpp"
#include "timer.hpp"
#include <string>

using namespace std::literals;
using namespace sc_core;

[[maybe_unused]] int sc_main( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] )
{
  constexpr const char* mesgType = "/Doulos/debugging_systemc/main";

  Top_module top { "top" };
  Timer::global().report( "Construction" );
  sc_start();

  if ( not sc_end_of_simulation_invoked() ) {
    sc_stop();  // triggers end_of_simulation() callback
  }

  auto errors = sc_report_handler::get_count( SC_ERROR )
              + sc_report_handler::get_count( SC_FATAL );

  auto message_count = sc_report_handler::get_count( SC_INFO )
              + sc_report_handler::get_count( SC_WARNING )
              + errors + 1;
  SC_REPORT_INFO_VERB( mesgType, ( "Total messages: "s + std::to_string(message_count) ).c_str(), SC_NONE );


  return ( errors == 0 ? 0 : 1 );
}
