#include "processes.hpp"
#include "objection.hpp"
#include <random>
using namespace sc_core;
using namespace std::literals;
using std::string;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
Processes_module::Processes_module( const sc_module_name& instance, unsigned seed )
  : sc_module{ instance }
  , random_generator{ (seed == 0) ? true_random() : seed }
  , time_distribution{ { 35, 35, 20, 7, 3 } }
{
   SC_HAS_PROCESS( Processes_module );
   SC_THREAD( p1_thread );
   SC_THREAD( p2_thread );
   SC_THREAD( p3_thread );
   SC_METHOD( p4_method );
   SC_METHOD( p5_method );
}

//------------------------------------------------------------------------------
// Overrides
//------------------------------------------------------------------------------
void Processes_module::before_end_of_elaboration() {
  info.executed( __func__, this );
}

void Processes_module::end_of_elaboration() {
  info.executed( __func__, this );
}

void Processes_module::start_of_simulation()
{
  Objection::set_drainTime( sc_core::sc_time{1, sc_core::SC_PS} );
  Objection::set_maxTimeout( sc_core::sc_time{100, sc_core::SC_MS} );
  auto t = Debug::get_count( "nSamples" );
  if (t > 0 ) {
    nSamples = t;
  }
  if( Debug::tracing() ) {
    sc_trace( Debug::trace_file(), m_level, "m_level" );
  }
  info.executed( __func__, this );
}

void Processes_module::end_of_simulation() {
  info.executed( __func__, this );
}

//------------------------------------------------------------------------------
// Processes
//------------------------------------------------------------------------------
void Processes_module::p1_thread() {
  random_delays( __func__, nSamples );
}

void Processes_module::p2_thread() {
  random_delays( __func__, nSamples );
}

void Processes_module::p3_thread() {
  try {
    random_delays( __func__, nSamples );
  } catch (int& e) {
    REPORT_INFO( "Caught int{"s + std::to_string(e) + "}" );
  }
  info.leaving( __func__, this );
}

void Processes_module::p4_method()
{
  next_trigger( random_time() );
  ++m_level;
  info.executed( __func__, this );
}

void Processes_module::p5_method() {
  next_trigger( random_time() );
  m_level -= 2;
  info.executed( __func__, this );
}

//------------------------------------------------------------------------------
// Helpers
//------------------------------------------------------------------------------
sc_time Processes_module::random_time() {
  auto result = sc_time{1,SC_NS};
  if ( nSamples > 1 ) {
    result = sc_time{ 10, SC_NS } * time_distribution(random_generator);
  }
  return result;
}

void Processes_module::random_delays( const string& threadName, size_t loopCount )
{
  Objection objection{ string{name()} +"."s + threadName, SC_DEBUG, /*quiet*/true };
  while ( loopCount-- ) {
    computation( threadName );
    wait( random_time() );
    ++m_level;
  }
}

volatile int bug = 0;

void Processes_module::computation( const string& threadName ) {
  // Do something fancy, but in the middle of it...
  if( "top.m1.p2_thread"s == ( string{name()} + "."s + threadName ) ) {
    switch ( bug ) {
      case 1: REPORT_WARNING( "Detected possible bug: "s + std::to_string( bug ) ); break;
      case 2: REPORT_ERROR( "Detected bug: "s + std::to_string( bug ) ); break;
      case 3: REPORT_FATAL( "Detected really bad bug: "s + std::to_string( bug ) ); break;
      case 4: throw bug;
      default: /* No problems detected */ break;
    }
  }
}
// EOF
