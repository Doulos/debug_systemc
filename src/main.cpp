#include "top.hpp"

[[maybe_unused]] int sc_main( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] )
{
  Top top { "top" };
  sc_core::sc_start();

  if ( not sc_core::sc_end_of_simulation_invoked() ) {
    sc_core::sc_stop();  // triggers end_of_simulation() callback
  }

  auto errors = sc_core::sc_report_handler::get_count( sc_core::SC_ERROR )
              + sc_core::sc_report_handler::get_count( sc_core::SC_FATAL );

  return ( errors == 0 ? 0 : 1 );
}
