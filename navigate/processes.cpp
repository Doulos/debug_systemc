#include "processes.hpp"
#include "objection.hpp"
#include <random>
using namespace sc_core;
using namespace std::literals;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
Processes_module::Processes_module( const sc_module_name& instance )
  : sc_module{ instance }, gen{rd()}, dist{ { 35, 35, 20, 7, 3 } }
{
   SC_HAS_PROCESS( Processes_module );
   SC_THREAD( thread1 );
   SC_THREAD( thread2 );
   SC_THREAD( thread3 );
   SC_METHOD( method4 );
   SC_METHOD( method5 );
}

//------------------------------------------------------------------------------
// Overrides
//------------------------------------------------------------------------------
void Processes_module::before_end_of_elaboration() {
  debug.executed( __func__, this );
}

void Processes_module::end_of_elaboration() {
  debug.executed( __func__, this );
}
void Processes_module::start_of_simulation() {
  Objection::set_maxTimeout( sc_core::sc_time{100, sc_core::SC_MS} );
  Objection::set_drainTime( sc_core::sc_time{100, sc_core::SC_NS} );
  debug.executed( __func__, this );
}
void Processes_module::end_of_simulation() {
  SC_REPORT_INFO_VERB( mesgType, Debug::message( std::to_string( Debug::context_switch(false) ) + " context switches" ), SC_NONE );
  debug.executed( __func__, this );
}

//------------------------------------------------------------------------------
// Processes
//------------------------------------------------------------------------------
void Processes_module::thread1() {
  debug.entering( __func__, this );
  random_delays( __func__ );
  debug.leaving( __func__, this );
}

void Processes_module::thread2() {
  debug.entering( __func__, this );
  random_delays( __func__ );
  debug.leaving( __func__, this );
}

void Processes_module::thread3() {
  debug.entering( __func__, this );
  random_delays( __func__ );
  debug.leaving( __func__, this );
}

void Processes_module::method4() {
  debug.entering( __func__, this );
  next_trigger( random_time() );
  debug.leaving( __func__, this );
  debug.context_switch();
}

void Processes_module::method5() {
  debug.entering( __func__, this );
  next_trigger( random_time() );
  debug.leaving( __func__, this );
  debug.context_switch();
}

//------------------------------------------------------------------------------
// Helpers
//------------------------------------------------------------------------------
sc_time Processes_module::random_time() {
  return sc_time{ 10.0*dist(gen), SC_NS };
}

void Processes_module::random_delays( const std::string& func, size_t n )
{
  Objection objection{ func, SC_DEBUG, /*quiet*/true };
  while ( n-- ) {
    debug.yielding( func, this );
    wait( random_time());
    debug.resuming( func, this );
    debug.context_switch();
  }
}

// EOF
