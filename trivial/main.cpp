#include "top.hpp"
#include "timer.hpp"
#include "debug.hpp"
#include <string>

using namespace std::literals;
using namespace sc_core;

[[maybe_unused]] int sc_main( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] )
{
  constexpr const char* msg_type = "/Doulos/debugging_systemc/main";

  Top_module top { "top" };
  Timer::global().report( "Construction" );
  sc_start();

  if ( not sc_end_of_simulation_invoked() ) {
    sc_stop();  // triggers end_of_simulation() callback
  }

  return Debug::exit_status( msg_type );
}
