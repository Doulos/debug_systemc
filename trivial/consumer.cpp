#include "consumer.hpp"
#include "debug.hpp"
#include "objection.hpp"
using namespace sc_core;
using namespace std;
using namespace std::literals;

//------------------------------------------------------------------------------
// Constructor
Consumer_module::Consumer_module( const sc_core::sc_module_name& instance )
        : sc_module{ instance }
{
  SC_HAS_PROCESS( Consumer_module );
  SC_THREAD( consumer_thread );
}

//------------------------------------------------------------------------------
// Consume data
void Consumer_module::consumer_thread()
{
  auto dump = std::max( Debug::get_count("nDump"), size_t{0} );
  if( Debug::tracing() ) {
    sc_trace( Debug::trace_file(), m_rx, "m_rx" );
  }

  for(;;) {
    auto rx = data_in->read();
    m_rx.write( rx ); // for tracing
    {
      Objection consumer_objection{ name() };
      ++ m_received_count;

      // Dump a few transactions at the start 
      auto dump_level = ( dump > 0 and not Debug::debugging() ) ? SC_NONE : SC_DEBUG;
      dump -= ( dump > 0 ) ? 1 : 0;
      SC_REPORT_INFO_VERB(
          mesgType,
          ( "At "s + sc_time_stamp().to_string()
            + " received " + rx.to_string()
          ).c_str(),
          dump_level
      );
    }
  }
}
