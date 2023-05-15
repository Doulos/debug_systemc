#include "producer.hpp"
#include "debug.hpp"
#include "objection.hpp"
#include <algorithm>
using namespace sc_core;
using namespace std;
using namespace std::literals;

//------------------------------------------------------------------------------
// Constructor
Producer_module::Producer_module( const sc_module_name& instance )
: sc_module{ instance }
{
  SC_HAS_PROCESS( Producer_module );
  SC_THREAD( producer_thread );
  data_out.bind( fifo );
}

//------------------------------------------------------------------------------
// Produce specified quantify of data at specified rate
void Producer_module::producer_thread()
{
  auto reps = std::max( Debug::count("nReps"), size_t{10} );
  auto dump = std::max( Debug::count("nDump"), 0ul );
  auto period = std::max( Debug::time("tPeriod"), sc_time{ 1, SC_NS } );
  auto tx = Transaction{};

  Objection producer_objection{ name() };

  for( auto i=0u; i != reps; ++i ) {
    wait( period );
    tx.randomize();
    fifo.write( tx );
    ++transmit_count;

    // Dump a few transactions at the start 
    auto dump_level = ( dump > 0 and not Debug::debugging() ) ? SC_NONE : SC_DEBUG;
    dump -= ( dump > 0 ) ? 1 : 0;
    SC_REPORT_INFO_VERB(
        mesgType,
        ( "At "s + sc_time_stamp().to_string()
          + " sent " + tx.to_string()
        ).c_str(),
        dump_level
    );
  }

  wait( period );
  sc_stop();
}
