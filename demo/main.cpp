#include "top.hpp"
#include <systemc>
#include <memory>
using namespace sc_core;
using namespace std::literals;

// Entry point called externally
int sc_main( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] )
{
  static constexpr const char* msg_type = "/Doulos/navigation/main";
  // Place most of code on the heap -- sc_start will use this indirectly
  [[maybe_unused]] auto top = std::make_unique<Top_module>( "top" );
  sc_start();

  if ( not sc_end_of_simulation_invoked() ) {
    sc_stop();  // triggers end_of_simulation() callback
  }

  return Debug::exit_status( msg_type );
}
