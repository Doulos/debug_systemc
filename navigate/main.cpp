#include "top.hpp"
#include <systemc>
#include <memory>
using namespace sc_core;
using namespace std::literals;

// Entry point called externally
int sc_main( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] )
{
  static constexpr const char* mesgType = "/Doulos/navigation/main";
  // Place most of code on the heap -- sc_start will use this indirectly
  [[maybe_unused]] auto top = std::make_unique<Top_module>( "top" );
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
